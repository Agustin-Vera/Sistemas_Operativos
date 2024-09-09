#include "funciones.h"

//Funcion que lee una imagen BMP y almacena su contenido en una estructura
BMPImage* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb"); //rb = read binary
    if (!file) {
        
        if(errno == ENOENT){ //Variable global que indica si el archivo existe o no
            fprintf(stderr, "El archivo '%s' no existe.\n", filename);
            return NULL;
        }
        else {
            fprintf(stderr, "Error: No se pudo abrir el archivo.\n");
            return NULL;
        }
    }

    BMPHeader header;
    fread(&header, sizeof(BMPHeader), 1, file);
    if (header.type != 0x4D42) { // 42 = 66 en decimal = B en ASCII y 4D = 77 en decimal = M en ASCII para saber si es una imagen BMP
        fprintf(stderr, "Error: El archivo no es un BMP valido.\n");
        fclose(file);
        return NULL;
    }

    BMPInfoHeader info_header;
    fread(&info_header, sizeof(BMPInfoHeader), 1, file); //fread(puntero a la estructura, tamanio de la estructura, cantidad de estructuras, archivo)

    BMPImage* image = (BMPImage*)malloc(sizeof(BMPImage));
    image->width = info_header.width;
    image->height = info_header.height;
    image->data = (RGBPixel*)malloc(sizeof(RGBPixel) * info_header.width * info_header.height);

    fseek(file, header.offset, SEEK_SET); // fseek(archivo, desplazamiento, origen desde donde se desplaza SEEK_SET = inicio del archivo, SEEK_CUR = posicion actual del archivo, SEEK_END = final del archivo)

    //se hace padding para que la imagen tenga un tamanio multiplo de 4, esto se hace para que la imagen sea mas rapida de leer
    int padding = (4 - (info_header.width * sizeof(RGBPixel)) % 4) % 4; // primero se pasan a bytes los pixeles de la imagen y se calcula el residuo de la division entre 4, si el residuo es 0 no hay padding, si el residuo es 1, 2 o 3 se calcula el padding
    for (int y = info_header.height - 1; y >= 0; y--) {
        for (int x = 0; x < info_header.width; x++) {
            RGBPixel pixel;
            fread(&pixel, sizeof(RGBPixel), 1, file);
            image->data[y * info_header.width + x] = pixel;
        }
        fseek(file, padding, SEEK_CUR);
    }
    fclose(file);
    return image;
}

//Funcion para liberar la memoria pedida por la estructura ingresada (BMP Image)
void free_bmp(BMPImage* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

//Funcion para saturar los colores de una imagen (altera los pixeles de la imagen)
BMPImage* saturate_bmp(BMPImage* image, float factor) {
    BMPImage* new_image = (BMPImage*)malloc(sizeof(BMPImage));
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (RGBPixel*)malloc(sizeof(RGBPixel) * image->width * image->height);

    unsigned char maximo = 255;
    //Recorre la imagen y satura pixel por pixel, si un valor RGB*factor es mayor al numero maximo (255) el valor R, G o B es = 255
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            RGBPixel pixel = image->data[y * image->width + x];
            if((pixel.r * factor) >= maximo){
                pixel.r = maximo;
            }
            else{
                pixel.r = (unsigned char)(pixel.r * factor);
            }
            if((pixel.g * factor) >= maximo){
                pixel.g = maximo;
            }
            else{
                pixel.g = (unsigned char)(pixel.g * factor);
            }
            if((pixel.b * factor) >= maximo){
                pixel.b = maximo;
            }
            else{
                pixel.b = (unsigned char)(pixel.b * factor);
            }
            new_image->data[y * image->width + x] = pixel;
        }
    }
    return new_image;
}

//Funcion para llevar una imagen con pixeles RGB a escala de grises
BMPImage* rgb_escala_de_grises(BMPImage* image) {
    BMPImage* new_image = (BMPImage*)malloc(sizeof(BMPImage));
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (RGBPixel*)malloc(sizeof(RGBPixel) * image->width * image->height);

    // Recorre la imagen y calcula la escala_gris de cada pixel, los pixeles RGB son alterados con la escala de gris calculada
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            RGBPixel pixel = image->data[y * image->width + x];

            unsigned char escala_gris = pixel.r*0.3 + pixel.g*0.59 + pixel.b*0.11;
            pixel.r = escala_gris;
            pixel.g = escala_gris;
            pixel.b = escala_gris;
            new_image->data[y * image->width + x] = pixel;
        }
    }
    return new_image;
}

//Funcion para  Binarizar una imagen BMP
BMPImage* binarizacion_bmp(BMPImage* image, float umbral) {
    BMPImage* new_image = (BMPImage*)malloc(sizeof(BMPImage));
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (RGBPixel*)malloc(sizeof(RGBPixel) * image->width * image->height);

    // Recorre la imagen y calcula el valor de cada pixel, este valor es comparado con un umbral llevado a valores RGB (umbral * 255)
    // si el valor del pixel es mayor, el pixel RGB es lo suficientemente claro y pasa a ser un pixel RGB blanco
    // si el valor del pixel no es mayor, el pixel RGB es lo suficientemente oscuro y pasa a ser un pixel RGB negro
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            RGBPixel pixel = image->data[y * image->width + x];
            unsigned char pixel_valor = pixel.r + pixel.g + pixel.b;

            if(pixel_valor > umbral*255) { //Pixel RGB a blanco
                pixel.r = pixel.g = pixel.b = 255;
            }
            else{ //Pixel RGB a negro
                pixel.r = pixel.g = pixel.b = 0;
            }
            new_image->data[y * image->width + x] = pixel;
        }
    }
    return new_image;
}

//Funcion que indica si una imagen es Nearly Black
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

//Funcion para crear una imagen BMP
void write_bmp(const char* filename, BMPImage* image) {
    FILE* file = fopen(filename, "wb"); //wb = write binary
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo.\n");
        return;
    }
    
    BMPHeader header;
    header.type = 0x4D42; // 42 = 66 en decimal = B en ASCII y 4D = 77 en decimal = M en ASCII para indicar que es una imagen BMP
    header.size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + image->width * image->height * sizeof(RGBPixel); //calcula el tamanio de la imagen
    header.offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);

    BMPInfoHeader info_header;
    info_header.size = sizeof(BMPInfoHeader);
    info_header.width = image->width;
    info_header.height = image->height;
    info_header.planes = 1;
    info_header.bit_count = 24; // 24 bits por pixel
    info_header.size_image = image->width * image->height * sizeof(RGBPixel);

    // Se escribe el encabezado de la imagen mas el contenido del encabezado
    fwrite(&header, sizeof(BMPHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);

    int padding = (4 - (image->width * sizeof(RGBPixel)) % 4) % 4; // bits de relleno al final de cada fila de pixeles (multiplo de 4)
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            RGBPixel pixel = image->data[y * image->width + x];
            fwrite(&pixel, sizeof(RGBPixel), 1, file);
        }

        RGBPixel padding_pixel = {0};
        fwrite(&padding_pixel, sizeof(RGBPixel), padding, file);
    }

    fclose(file);
}