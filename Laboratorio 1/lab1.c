#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "funciones.h"


int main(int argc, char *argv[]){
    // Option para el getopt
    int option;
    int obligatorio1 = 0;
    int obligatorio2 = 0;
    char prefijo[50];
    // Los valores fijados indican valores por defecto, pueden ser cambiados
    int cantidad_filtros = 3;
    float factor_saturacion = 1.3;
    float umbral_binarizacion = 0.5;
    float umbral_clasificacion = 0.5;
    char nombre_carpeta[100];
    char nombre_archivo_clasificacion[100];
    /*
    //Ejemplo sin colocacion de umbrales
        ./lab1 -N imagen -f 2 -p -C Ejemplo_nombre -R Nombre_resultado
    
    //Ejemplo con umbrales (u y v)
        ./lab1 -N imagen -f 2 -p 2 -u 0.7 -v 0.8 -C ImagenesFiltradas -R ImagenesClasificadas
    */
    while((option = getopt(argc, argv, "N:f:p:u:v:C:R:")) != -1){
        switch(option){
            case 'N':
                strcpy(prefijo, optarg);
                break;
            case 'f':
                cantidad_filtros = atoi(optarg);
                break;
            case 'p':
                factor_saturacion = atof(optarg);
                break;
            case 'u':
                umbral_binarizacion = atof(optarg);
                break;
            case 'v':
                umbral_clasificacion = atof(optarg);
                break;
            case 'C':
                strcpy(nombre_carpeta, optarg);
                obligatorio1 = 1;
                break;
            case 'R':
                strcpy(nombre_archivo_clasificacion, optarg);
                obligatorio2 = 1;
                break;
        }
    }

    if(obligatorio1 != 1 || obligatorio2 != 1){
        printf("Error: Se debe ingresar un nombre para la carpeta y para el archivo de clasificacion\n");
        return 1;    
    }
    
    if(!(strcmp(prefijo, "img") == 0 || strcmp(prefijo, "imagen") == 0 || strcmp(prefijo, "photo") == 0)){
     	   printf("Error: No puede ingresar un prefijo distinto a img, imagen o photo\n");
    	   return 1; 
    }
    
    if(umbral_binarizacion < 0.0 || umbral_binarizacion > 1.0){
    	printf("Error: Se debe ingresar un umbral de binarizacion entre 0 y 1\n");
    	return 1; 
    } 
    
    if(umbral_clasificacion < 0.0 || umbral_clasificacion > 1.0){
    	printf("Error: Se debe ingresar un umbral de clasificacion entre 0 y 1\n");
    	return 1; 
    } 

    // Aqui se crea una carpeta --> Si ya existe una carpeta con el mismo nombre no la puede crear
    int carpeta = mkdir(nombre_carpeta,0777); // 0777 para accesibilidad a carpeta
    if(carpeta == 0){
        printf("Carpeta creada: %s\n", nombre_carpeta);
    }
    else{
        printf("Error al crear carpeta\n");
        return 1;
    }

    //Se crea el archivo CSV para dejar almacenadas las imagenes con su respectiva clasificacion nearly black
    char nombre_csv[150];
    sprintf(nombre_csv, "%s.csv", nombre_archivo_clasificacion);
    FILE* csv= fopen(nombre_csv, "w");
    if(csv == NULL) {
        perror("Error al abrir el archvio");
        return 1;
    }
    // Encabezados archivo csv
    fprintf(csv, "NombreImagen,NearlyBlack\n");

    int i;
    char filename[200];
    int nearly_black;
    
    // Punteros a imagenes a ser creadas
    BMPImage* image;
    BMPImage* imagen_saturada;
    BMPImage* imagen_escala_grises;
    BMPImage* imagen_binarizada;
    
    // Se recorren todas las imagenes con un prefijo en cuestion desde la imagen con identificador 1 hasta n
    // Se recorre de corrido si imagen_3 es la imagen con identificador mas alto, recorre imagen_1, imagen_2 y imagen_3
    // Si no existe una imagen entre medio, por ejemplo: no existe imagen_2, solamente aplica filtros a imagen_1
    
    for(i=1; i<=i+1; i++){

        // Nombre de imagen iesima almacenada en filename
        sprintf(filename, "%s_%d.bmp", prefijo, i);
        
        // Leyendo imagen
        image = read_bmp(filename);
        if (!image) {
            break;
        }
        if(cantidad_filtros == 1){
            // Saturando imagen
            imagen_saturada = saturate_bmp(image, factor_saturacion);

            // Verificacion de nearly black
            nearly_black = is_nearly_black(imagen_saturada, umbral_clasificacion);
            fprintf(csv, "%s,%d\n", filename, nearly_black);
            sprintf(filename, "%s/%s_%d.bmp", nombre_carpeta, prefijo, i);
            // Escribiendo imagen en carpeta
            write_bmp(filename, imagen_saturada);

            //Liberando memoria
            free_bmp(image);
            free_bmp(imagen_saturada);
        }
        
        if(cantidad_filtros == 2){
            // Saturando imagen
            imagen_saturada = saturate_bmp(image, factor_saturacion);
        
            // LLevando imagen a escala de grises
            imagen_escala_grises = rgb_escala_de_grises(imagen_saturada);

            // Verificacion de nearly black
            nearly_black = is_nearly_black(imagen_escala_grises, umbral_clasificacion);
            fprintf(csv, "%s,%d\n", filename, nearly_black);

            sprintf(filename, "%s/%s_%d.bmp", nombre_carpeta, prefijo, i);
            // Escribiendo imagen en carpeta
            write_bmp(filename, imagen_escala_grises);

            //Liberando memoria
            free_bmp(image);
            free_bmp(imagen_saturada);
            free_bmp(imagen_escala_grises);
        }
        
        if(cantidad_filtros == 3){

            // Saturando imagen
            imagen_saturada = saturate_bmp(image, factor_saturacion);
        
            // LLevando imagen a escala de grises
            imagen_escala_grises = rgb_escala_de_grises(imagen_saturada);

            // LLevando a binarizacion
            imagen_binarizada = binarizacion_bmp(imagen_escala_grises, umbral_binarizacion);
            
            // Verificacion de nearly black
            nearly_black = is_nearly_black(imagen_binarizada, umbral_clasificacion);
            fprintf(csv, "%s,%d\n", filename, nearly_black);

            sprintf(filename, "%s/%s_%d.bmp", nombre_carpeta, prefijo, i);
            // Escribiendo imagen en carpeta
            write_bmp(filename, imagen_binarizada);

            //Liberando memoria
            free_bmp(image);
            free_bmp(imagen_saturada);
            free_bmp(imagen_escala_grises);
            free_bmp(imagen_binarizada);
        }
    }	
    
    //Cerrando archivo CSV con imagenes clasificadas
    fclose(csv);
    printf("\nFin del programa :)\n");
    return 0;
}
