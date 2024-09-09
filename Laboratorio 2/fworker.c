#include "fworker.h"

// Función que aplica saturación a los pixeles RGB contenidos en columnas de una imagen
void aplicarSaturacion(float factor, int num_pixeles, RGBPixel *columnas){
    int i;
    unsigned char maximo = 255;
    
    for(i=0; i<num_pixeles; i++){
        if((columnas[i].r * factor) >= maximo){
            columnas[i].r = maximo;
        }
        else{
            columnas[i].r = (unsigned char)(columnas[i].r * factor);
        }
        if((columnas[i].g * factor) >= maximo){
            columnas[i].g = maximo;
        }
        else{
            columnas[i].g = (unsigned char)(columnas[i].g * factor);
        }
        if((columnas[i].b * factor) >= maximo){
            columnas[i].b = maximo;
        }
        else{
            columnas[i].b = (unsigned char)(columnas[i].b * factor);
        }
    }
}

//Función para llevar los pixeles RGB de columnas a escala de grises
void aplicarEscalaGrises(int num_pixeles, RGBPixel *columnas){
    int i;
    for(i=0; i<num_pixeles; i++){
        unsigned char escala_gris = columnas[i].r*0.3 + columnas[i].g*0.59 + columnas[i].b*0.11;
        columnas[i].r = escala_gris;
        columnas[i].g = escala_gris;
        columnas[i].b = escala_gris;
    }
}

//Función para binarizar los pixeles RGB de columnas entregadas
void aplicarBinarizacion(float umbral, int num_pixeles, RGBPixel *columnas){
    int i;
    for(i=0; i<num_pixeles; i++){
        unsigned char pixel_valor = columnas[i].r + columnas[i].g + columnas[i].b;
        if(pixel_valor > umbral*255) { //Pixel RGB a blanco
            columnas[i].r = columnas[i].g = columnas[i].b = 255;
        }
        else{ //Pixel RGB a negro
            columnas[i].r = columnas[i].g = columnas[i].b = 0;
        }
    }
}
