#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fbroker.h"

int main(int argc, char* argv[]){

    // Variables para argumentos que recibe el proceso broker
    char prefijo[50];
    strcpy(prefijo, argv[1]);
    int cantidad_filtros = atoi(argv[2]);
    float factor_saturacion = atof(argv[3]);
    float umbral_binarizacion = atof(argv[4]);
    int cantidad_workers = atoi(argv[5]);

    // Variables para poder trabajar con hijos (workers) (comunicacion mediante pipes)
    int i, k;
    int fd1[cantidad_workers][2];
    int fd2[cantidad_workers][2];
  
    // Variables para generar imagenes
    char filename[200];
    BMPImage* image;
    BMPImage* image_filtrada;

    // Recorre por la cantidad de imagenes que hayan, recorre las imagenes desde 1 a n, seguidas, es decir, si se tienen 3 imagenes
    // recorre imagen_1, imagen_2, imagen_3, en caso de que no exista la imagen_2, solamente aplicara filtros a la imagen_1
    for(k=1; k != -1; k++){
      sprintf(filename, "%s_%d.bmp", prefijo, k);
      image = read_bmp(filename);
      if (!image) {
        k = -1;
        break;
      }
      else {
      // Se crean los workers y se ejecutan con sus entradas correspondientes
      ejecutarWorkers(cantidad_workers, fd1, fd2, cantidad_filtros, factor_saturacion, umbral_binarizacion);
      
      int columnas_por_worker = image->width / cantidad_workers;        // Cantidad de columnas a analizar por worker
      int resto_columnas = image->width % cantidad_workers;             // Cantidad de columnas respectivas al resto
      int columnas_final = columnas_por_worker + resto_columnas;        // Cantidad de columnas para el worker final
      int pixeles_por_columna = columnas_por_worker * image->height;    // Cantidad de pixeles por columnas
      int pixeles_por_columnasfinales = columnas_final * image->height; // Cantidad de pixeles por columnas considerando las restantes

      RGBPixel** columnas_workers = NULL;       // Matriz con las columnas a dividir para workers iniciales
      RGBPixel* columnas_ultimo_worker = NULL;  // Puntero a las columnas del ultimo worker
      RGBPixel** columnas_hijo = NULL;          // Matriz con las columnas trabajadas por los primeros workers
      RGBPixel* columnas_ultimo_hijo = NULL;    // Puntero a las columnas trabajadas por el ultimo worker
      asignarMemoriaParaColumnasWorkers(cantidad_workers, pixeles_por_columna, pixeles_por_columnasfinales, &columnas_workers, &columnas_ultimo_worker, &columnas_hijo, &columnas_ultimo_hijo);


      // Se dividen las columnas de la imagen entre los workers    
      dividirColumnas(image, pixeles_por_columna, cantidad_workers, pixeles_por_columnasfinales, columnas_workers, columnas_ultimo_worker);

      int j;
      for(i=0; i<cantidad_workers; i++){
        close(fd1[i][0]); // Se cierran las entradas y salidas que no se usaran para padre e hijo de pipes
        close(fd2[i][1]);
        
        char columnas_para_hijo[50];
        sprintf(columnas_para_hijo, "%d", columnas_por_worker);
        char alto_columnas[50];
        sprintf(alto_columnas, "%d", image->height);
        
        if(i < cantidad_workers -1){ // Si no es el ultimo worker envia columnas al hijo

          // Enviando la cantidad de columnas que debe analizar el hijo y la profundidad de estas (cantidad de pixeles por columna)
          write(fd1[i][1], columnas_para_hijo, 50);
          write(fd1[i][1], alto_columnas, 50);

          // Enviando pixeles uno por uno (envia columnas al fin y al cabo)
          for(j=0; j<pixeles_por_columna; j++){
            write(fd1[i][1], &columnas_workers[i][j], sizeof(columnas_workers[i][j]));
          }
        }
        else{ // Es el ultimo worker (se envian columnas extras del "resto")
          sprintf(columnas_para_hijo, "%d", columnas_final);
          write(fd1[i][1], columnas_para_hijo, 50);
          write(fd1[i][1], alto_columnas, 50);

          // Enviando pixeles uno por uno
          for(j=0; j<pixeles_por_columnasfinales; j++){
            write(fd1[i][1], &columnas_ultimo_worker[j], sizeof(columnas_ultimo_worker[j]));
          }
        }

        // Mensaje del hijo al padre (read)
        if(i < cantidad_workers -1){ // Si no recibo del ultimo worker
          // Recibiendo pixeles uno por uno
          for(j=0; j<pixeles_por_columna; j++){
            read(fd2[i][0], &columnas_hijo[i][j], sizeof(columnas_hijo[i][j]));
          }
        }
        else{ // Recibo del ultimo worker
          for(j=0; j<pixeles_por_columnasfinales; j++){
              read(fd2[i][0], &columnas_ultimo_hijo[j], sizeof(columnas_ultimo_hijo[j]));
            }
        }
      }
      // Reensamblando la imagen con las columnas tratadas por workers
      image_filtrada = reensamblarImagen(image, pixeles_por_columna, cantidad_workers, pixeles_por_columnasfinales, columnas_hijo, columnas_ultimo_hijo);

      //Enviando el ancho y alto de la imagen reensamblada al padre
      write(STDOUT_FILENO, &image_filtrada->width, sizeof(int));
      write(STDOUT_FILENO, &image_filtrada->height, sizeof(int));
      
      // Enviando la imagen reensamblada al padre, de la forma pixel por pixel
      for(i = 0; i < image_filtrada->width * image_filtrada->height; i++){
      	  write(STDOUT_FILENO, &image_filtrada->data[i], sizeof(RGBPixel));
      }
      
      free_bmp(image);
      free_bmp(image_filtrada);
      free_columnas(columnas_workers, columnas_ultimo_worker, columnas_hijo, columnas_ultimo_hijo, cantidad_workers);
      }
    }
    int final = 0;
    write(STDOUT_FILENO, &final, sizeof(int)); //Enviando al padre la señal de que broker ya termino de hacer su trabajo (no le quedan imagenes por trabajar)
    return 0;
}
