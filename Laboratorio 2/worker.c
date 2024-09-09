#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fworker.h"

int main(int argc, char* argv[]){

    // Variables para argumentos que recibe el proceso worker
    int cantidad_filtros = atoi(argv[1]);
    float factor_saturacion = atof(argv[2]);
    float umbral_binarizacion = atof(argv[3]);

    // Se lee del pipe la cantidad de columnas a trabajar    
    char columnas[50];
    read(STDIN_FILENO, columnas, sizeof(columnas));

    // Se lee del pipe la cantidad de pixeles por columna (alto de las columnas)
    char alto_columnas[50];
    read(STDIN_FILENO, alto_columnas, sizeof(alto_columnas));

    int cantidad_columnas = atoi(columnas);
    int pixeles_por_columna = atoi(alto_columnas);
    int pixeles_totales = pixeles_por_columna*cantidad_columnas;
    
    RGBPixel *columnas_worker = (RGBPixel*)malloc(sizeof(RGBPixel) * pixeles_totales);
    
    int i;
    // Recibiendo pixeles uno por uno
    for(i = 0; i<pixeles_totales; i++){
        read(STDIN_FILENO, &columnas_worker[i], sizeof(columnas_worker[i]));
    }

    // Se aplican los filtros a las columnas segun la cantidad de filtros corresponda
    if(cantidad_filtros == 1){
        aplicarSaturacion(factor_saturacion, pixeles_totales, columnas_worker);
    }
    if(cantidad_filtros == 2){
        aplicarSaturacion(factor_saturacion, pixeles_totales, columnas_worker);
        aplicarEscalaGrises(pixeles_totales, columnas_worker);
    }
    if(cantidad_filtros == 3){
        aplicarSaturacion(factor_saturacion, pixeles_totales, columnas_worker);
        aplicarEscalaGrises(pixeles_totales, columnas_worker);
        aplicarBinarizacion(umbral_binarizacion, pixeles_totales, columnas_worker);
    }

    // Enviando al padre pixel por pixel
    for(i = 0; i<pixeles_totales; i++){
        write(STDOUT_FILENO, &columnas_worker[i], sizeof(columnas_worker[i]));
    }
    
    free(columnas_worker);
    return 0;
}
