#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "flab2.h"

int main(int argc, char *argv[]) {
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
    int cantidad_workers = 1;

    while ((option = getopt(argc, argv, "N:f:p:u:v:C:R:W:")) != -1) {
        switch (option) {
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
            case 'W':
                cantidad_workers = atoi(optarg);
                break;
        }
    }

    if (obligatorio1 != 1 || obligatorio2 != 1) {
        printf("Error: Se debe ingresar un nombre para la carpeta y para el archivo de clasificacion\n");
        return 1;
    }

    if (!(strcmp(prefijo, "img") == 0 || strcmp(prefijo, "imagen") == 0 || strcmp(prefijo, "photo") == 0)) {
        printf("Error: No puede ingresar un prefijo distinto a img, imagen o photo\n");
        return 1;
    }
    
    if(cantidad_filtros <= 0 || cantidad_filtros > 3){
    	printf("Error: Ingrese la cantidad de filtros correcta, puede ingresar 1, 2 o 3 filtros\n");
        return 1;
    }
    
    if (factor_saturacion < 0.0) {
        printf("Error: No puede ingresar un factor de saturación negativo\n");
        return 1;
    }

    if (umbral_binarizacion < 0.0 || umbral_binarizacion > 1.0) {
        printf("Error: Se debe ingresar un umbral de binarizacion entre 0 y 1\n");
        return 1;
    }

    if (umbral_clasificacion < 0.0 || umbral_clasificacion > 1.0) {
        printf("Error: Se debe ingresar un umbral de clasificacion entre 0 y 1\n");
        return 1;
    }
    
    if(cantidad_workers <= 0){
    	printf("Error: Se debe ingresar una cantidad de workers positiva\n");
        return 1;
    }
    
    // Aquí se crea una carpeta --> Si ya existe una carpeta con el mismo nombre se puede crear
    int carpeta = mkdir(nombre_carpeta, 0777); // 0777 para accesibilidad a carpeta
    if (carpeta != 0 && errno != EEXIST) {
        printf("Error al crear carpeta\n");
        return 1;
    }

    printf("Carpeta creada: %s\n", nombre_carpeta);
    
    //Se crea el archivo CSV para dejar almacenadas las imagenes con su respectiva clasificacion nearly black
    char nombre_csv[150];
    sprintf(nombre_csv, "%s.csv", nombre_archivo_clasificacion);
    FILE *csv = fopen(nombre_csv, "w");
    if (csv == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }
    // Encabezados archivo csv
    fprintf(csv, "NombreImagen,NearlyBlack\n");

    int fd1[2], fd2[2];
    
    // Se crean los pipes para comunicar lab2 (proceso padre) con broker (proceso hijo)
    pipe(fd1);
    pipe(fd2);

    pid_t pid = fork();

    if (pid == -1) { // Error al crear al hijo
        perror("fork");
        return 1;
    }

    if (pid == 0) {  // Proceso hijo
        close(fd1[1]);
        close(fd2[0]);

        dup2(fd1[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);
        // Los parametros para el proceso hijo "broker" son pasados a string para que puedan ser recibidos por execv
        char cantidad_filtros_str[50], factor_saturacion_str[50], umbral_binarizacion_str[50], cantidad_workers_str[50];
        sprintf(cantidad_filtros_str, "%d", cantidad_filtros);
        sprintf(factor_saturacion_str, "%f", factor_saturacion);
        sprintf(umbral_binarizacion_str, "%f", umbral_binarizacion);
        sprintf(cantidad_workers_str, "%d", cantidad_workers);

        char *argvEXECV[] = {"./broker", prefijo, cantidad_filtros_str, factor_saturacion_str, umbral_binarizacion_str, cantidad_workers_str, nombre_carpeta, NULL};
        execv(argvEXECV[0], argvEXECV);
    } 
    
    else{  // Proceso padre
        close(fd1[0]);
        close(fd2[1]);
	    int status, i; 
        char filename[200];
        BMPImage* image_filtrada;
        
        for(i = 1; i != -1; i++){
            int width, height;
            image_filtrada = (BMPImage*)malloc(sizeof(BMPImage));

            read(fd2[0], &width, sizeof(int)); // Leyendo el mensaje que representa el ancho o la finalizacion (0 indica que el broker termino)

            if(width == 0){ // El broker termino de trabajar imagenes y ya las envio todas (el borker mando senial de finalizar)
                break;
            }
            else{ // El broker manda el ancho de una imagen, se va a recibir una imagen, no se debe finalizar
                image_filtrada->width = width;
           
                read(fd2[0], &height, sizeof(int)); // Se lee el alto de la imagen
                image_filtrada->height = height;
           
                image_filtrada->data = (RGBPixel*)malloc(image_filtrada->width * image_filtrada->height * sizeof(RGBPixel));
           
                for(int j = 0; j < image_filtrada->width * image_filtrada->height; j++){ // Se lee la imagen pixel por pixel
                    read(fd2[0], &image_filtrada->data[j], sizeof(RGBPixel));	
                }
                sprintf(filename, "%s/%s_%d.bmp", nombre_carpeta, prefijo, i);
                // Escribiendo imagen en carpeta
                write_bmp(filename, image_filtrada);
           	
           	// Verificacion de nearly black
                int nearly_black = is_nearly_black(image_filtrada, umbral_clasificacion);
                fprintf(csv, "%s_%d.bmp,%d\n", prefijo, i, nearly_black);
           	
           	//Liberando memoria de la imagen filtrada
                free_bmp(image_filtrada);		
            }
        }
        
        // Se cierra la comunicación y espera el pid correspondiente.
        close(fd2[0]);
        waitpid(pid, &status, 0);
    }
    
    //Cerrando archivo CSV con imagenes clasificadas
    fclose(csv);
    printf("Fin del programa.\n");
    return 0;
}
