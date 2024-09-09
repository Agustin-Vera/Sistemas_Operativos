#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "flab2.h"
#include <unistd.h>


// Funcion para escribir una imagen a bmp
void write_bmp(const char* filename, BMPImage* image) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Unable to open the file.\n");
        return;
    }

    BMPHeader header = {
        .type = 0x4D42,
        .size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + image->width * image->height * sizeof(RGBPixel),
        .reserved1 = 0,
        .reserved2 = 0,
        .offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader)
    };

    BMPInfoHeader info_header = {
        .size = sizeof(BMPInfoHeader),
        .width = image->width,
        .height = image->height,
        .planes = 1,
        .bit_count = 24,
        .compression = 0,
        .size_image = image->width * image->height * sizeof(RGBPixel),
        .x_pixels_per_meter = 0,
        .y_pixels_per_meter = 0,
        .colors_used = 0,
        .colors_important = 0
    };

    fwrite(&header, sizeof(BMPHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);

    int padding = (4 - (image->width * sizeof(RGBPixel)) % 4) % 4;
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            fwrite(&image->data[y * image->width + x], sizeof(RGBPixel), 1, file);
        }
        RGBPixel padding_pixel = {0, 0, 0};
        fwrite(&padding_pixel, sizeof(RGBPixel), padding, file);
    }

    fclose(file);
}

// Función que indica si una imagen es casi negra segun un umbral
int is_nearly_black(BMPImage* image, float umbral){
    float cantidad_pixels_negros = 0;
    float cantidad_pixeles_imagen = 0;
    float porcentaje_pixeles_negros;
    int nearly_black;
    
    // Recorre una imagen y calcula el porcentaje de pixeles negros frente a la cantidad de pixeles totales de la imagen
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            RGBPixel pixel = image->data[y * image->width + x];
            if(pixel.r == 0 && pixel.g == 0 && pixel.b == 0){
                cantidad_pixels_negros = cantidad_pixels_negros + 1;
            }
            cantidad_pixeles_imagen = cantidad_pixeles_imagen + 1;
        }
    }
    // Si el porcentaje de pixeles negros es mayor al umbral, la imagen es nearly black
    // Si el porcentaje de pixeles negros no es mayor al umbral, la imagen no es nearly black
    porcentaje_pixeles_negros = cantidad_pixels_negros/cantidad_pixeles_imagen;
    if(porcentaje_pixeles_negros >= umbral) {
        nearly_black = 1;
        return nearly_black;
    }
    nearly_black = 0;
    return nearly_black;
}

// Funcion para liberar memoria de una imagen BMP
void free_bmp(BMPImage* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}