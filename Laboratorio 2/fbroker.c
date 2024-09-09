#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fbroker.h"
#include <unistd.h>

// Función para leer una imagen BMP
BMPImage* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        if (errno == ENOENT) {
            fprintf(stderr, "El archivo '%s' no existe.\n", filename);
        } else {
            fprintf(stderr, "Error: No se pudo abrir el archivo.\n");
        }
        return NULL;
    }

    BMPHeader header;
    fread(&header, sizeof(BMPHeader), 1, file);
    if (header.type != 0x4D42) {
        fprintf(stderr, "Error: The file is not a valid BMP.\n");
        fclose(file);
        return NULL;
    }

    BMPInfoHeader info_header;
    fread(&info_header, sizeof(BMPInfoHeader), 1, file);

    BMPImage* image = (BMPImage*)malloc(sizeof(BMPImage));
    image->width = info_header.width;
    image->height = info_header.height;
    image->data = (RGBPixel*)malloc(sizeof(RGBPixel) * info_header.width * info_header.height);

    fseek(file, header.offset, SEEK_SET);

    int padding = (4 - (info_header.width * sizeof(RGBPixel)) % 4) % 4;
    for (int y = info_header.height - 1; y >= 0; y--) {
        for (int x = 0; x < info_header.width; x++) {
            fread(&image->data[y * info_header.width + x], sizeof(RGBPixel), 1, file);
        }
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
    return image;
}

// Funcion para liberar memoria de una imagen BMP
void free_bmp(BMPImage* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

// Función para dividir las columnas de una imagen para los workers
void dividirColumnas(BMPImage* image, int pixeles_por_columna, int num_workers, int pixeles_por_columnas_final, RGBPixel** columnas_por_worker, RGBPixel* ultimas_columnas) {
    int x, y;
    int contador = 0;  // contador de pixeles
    int id_worker = 0; // identifiacador del worker i-esimo
    int i = 0;         // identificador de la posicion para el pixel i-esimo
    RGBPixel pixel;

    // Recorre ancho x alto para ir dividiendo las columnas que le corresponden a cada worker
    for (x = 0; x < image->width; x++) {
        for (y = 0; y < image->height; y++) {
            if (contador == pixeles_por_columna) {
                contador = 0;
                i = 0;
                id_worker++;
                if (id_worker >= num_workers) {
                    id_worker = num_workers - 1;
                }
            }
            // Si no es el ultimo worker asigna pixeles por columnas
            if (id_worker < num_workers - 1) {
                pixel = image->data[x * image->height + y];
                columnas_por_worker[id_worker][i] = pixel;
            } 
            // Si es el ultimo worker asigna los pixeles por columnas y los pixeles de las columnas extra
            else {
                pixel = image->data[x * image->height + y];
                ultimas_columnas[i] = pixel;
            }
            i++;
            contador++;
        }
    }  
}


// Función para reensamblar una imgen, junta las columnas y le da el formato adecuado a estas para retornar la imagen
BMPImage* reensamblarImagen(BMPImage* image, int pixeles_por_columna, int num_workers, int pixeles_por_columna_final, RGBPixel **columnas_por_worker, RGBPixel *ultimas_columnas) {
    BMPImage* new_image = (BMPImage*)malloc(sizeof(BMPImage));
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->data = (RGBPixel*)malloc(sizeof(RGBPixel) * image->width * image->height);
    int x, y;
    int contador = 0;  // contador de pixeles
    int id_worker = 0; // identifiacador del worker i-esimo
    int i = 0;         // identificador de la posicion para el pixel i-esimo
    RGBPixel pixel;
    for (x = 0; x < image->width; x++) {
        for (y = 0; y < image->height; y++) {
            if (contador == pixeles_por_columna) {
                contador = 0;
                i = 0;
                id_worker++;
                if (id_worker >= num_workers) {
                    id_worker = num_workers - 1;
                }
            }
            // Si no es el ultimo worker asigna pixeles por columnas
            if (id_worker < num_workers - 1) {
                pixel = columnas_por_worker[id_worker][i];
                new_image->data[x * new_image->height + y] = pixel;
            } 
            // Si es el ultimo worker asignas los pixeles por columnas y los pixeles de las columnas extra
            else {
                pixel = ultimas_columnas[i];
                new_image->data[x * new_image->height + y] = pixel;
            }
            i++;
            contador++;
        }
    }
    return new_image;
}


// Función que ejecuta los workers y le entrega los parametros necesarios a los procesos worker para su funcionamiento
void ejecutarWorkers(int cantidad_workers, int fd1[][2], int fd2[][2], int cantidad_filtros, float factor_saturacion, float umbral_binarizacion){
    int i;
    pid_t pid;
    
    for(i=0; i<cantidad_workers; i++){
      // Soy padre y creo los pipes
      pipe(fd1[i]);
      pipe(fd2[i]);
    
      pid = fork();
      if (pid == -1) { // Verificar error de fork
        perror("fork");
        exit(EXIT_FAILURE);
      }
      if(pid == 0){ // Soy hijo
        close(fd1[i][1]);
        close(fd2[i][0]);
        dup2(fd1[i][0], STDIN_FILENO);
        dup2(fd2[i][1], STDOUT_FILENO);
        // Se pasan las variables a valores soportados por execv
        char cantidad_filtros_str[50];
        char factor_saturacion_str[50];
        char umbral_binarizacion_str[50];
        sprintf(cantidad_filtros_str, "%d", cantidad_filtros);
        sprintf(factor_saturacion_str, "%f", factor_saturacion);
        sprintf(umbral_binarizacion_str, "%f", umbral_binarizacion);
        char* argvWORKER[]={"./worker", cantidad_filtros_str, factor_saturacion_str, umbral_binarizacion_str, NULL}; // Ejecuto al worker
        execv(argvWORKER[0], argvWORKER);
      }
      
    }
}

//Función que libera la memoria asignada para las columnas de píxeles RGB utilizadas por los workers y sus hijos (brokers).
void free_columnas(RGBPixel **columnas_workers, RGBPixel *columnas_ultimo_worker, RGBPixel **columnas_hijo, RGBPixel *columnas_ultimo_hijo, int cantidad_workers){
    free(columnas_ultimo_worker);
    free(columnas_ultimo_hijo);
    for(int m = 0; m<cantidad_workers-1; m++){
        free(columnas_workers[m]);
        free(columnas_hijo[m]);
      }
    free(columnas_workers);
    free(columnas_hijo);

}

// Función que asigna memoria a todos los punteros que representan columnas a ser distribuidas y recibidas por el broker
// Aqui solo se reserva memoria, se dice "Matriz de columnas trabajadas por workers" pero nos referimos a la memoria asignada para que el broker reciba las columnas
void asignarMemoriaParaColumnasWorkers(int cantidad_workers, int pixeles_por_columna, int pixeles_por_columnasfinales, RGBPixel ***columnas_workers, RGBPixel **columnas_ultimo_worker, RGBPixel ***columnas_hijo, RGBPixel **columnas_ultimo_hijo) {
    int m;

    // Memoria para enviar columnas a los hijos (workers) y estos las trabajen
    *columnas_workers = (RGBPixel**)malloc(sizeof(RGBPixel*) * (cantidad_workers - 1)); // Matriz de columnas por worker
    for (m = 0; m < cantidad_workers - 1; m++) {
        (*columnas_workers)[m] = (RGBPixel*)malloc(sizeof(RGBPixel) * pixeles_por_columna); // Columnas de pixeles de cada worker m-esimo
    }
    *columnas_ultimo_worker = (RGBPixel*)malloc(sizeof(RGBPixel) * pixeles_por_columnasfinales); // Columnas del ultimo worker

    // Memoria para recibir columnas del hijo (columnas ya tratadas)
    *columnas_hijo = (RGBPixel**)malloc(sizeof(RGBPixel*) * (cantidad_workers - 1)); // Matriz de columnas trabajadas por workers
    for (m = 0; m < cantidad_workers - 1; m++) {
        (*columnas_hijo)[m] = (RGBPixel*)malloc(sizeof(RGBPixel) * pixeles_por_columna); // Columnas de pixeles de cada worker m-esimo
    }
    *columnas_ultimo_hijo = (RGBPixel*)malloc(sizeof(RGBPixel) * pixeles_por_columnasfinales); // Columnas del ultimo worker
}
