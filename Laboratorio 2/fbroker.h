#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "ImageBMP.h"

//Entradas: filename, puntero a tipo char correspondiente al nombre de la imagen.
//Salidas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen leída.
//Descripción: Lee una imagen (encabezado y la informacion del encabezado) para luego almacenar los valores de la imagen en estructuras
//             BMPImage y sus pixeles en RGBPixel.
BMPImage* read_bmp(const char* filename);

//Entradas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen a ser liberada.
//Salidas: void.
//Descripción: Libera la memoria pedida por las estructuras de la imagen (BMPImage), incluyendo la memoria reservada para los datos de la imagen y cualquier otro recurso que haya sido asignado durante la creación  o manipulación de la imagen.
void free_bmp(BMPImage* image);

//Entradas: image es un puntero a la imagen a ser dividida, pixeles_por_columna es la cantidad de pixeles por columna, pixeles_por_columna_final es un entero que representa la cantidad de pixeles en columnas finales (último worker),
//    columnas_por_worker (de tipo RGBPixel) es la matriz de columnas (pixeles) de los workers (menos el último) y ultimas_columnas  es un puntero (de tipo RGBPixel) a las ultimas columnas (trabajadas por el último worker).
//Salidas: void (los punteros a las columnas y matriz quedan llenos de contenido respectivo a los pixeles RGB de la imagen segun corresponda).
//Descripción: Divide la imagen de la forma: ancho imagen % cantidad de workers, con ello se le entregan N columnas de la imagen a cada worker
//  el resto de la división (columnas restantes) se le entregan al último worker, por ende, en el caso de que exista resto != 0 el último worker tendra mas columnas a trabajar.
void dividirColumnas(BMPImage* image, int pixeles_por_columna, int num_workers, int pixeles_por_columnas_final, RGBPixel **columnas_por_worker, RGBPixel *ultimas_columnas);

//Entradas: image es el puntero a la imagen a ser reensamblada, pixeles_por_columna es un entero que representa la cantidad de pixeles por columna, pixeles_por_columna_final es un entero que representa la cantidad de pixeles en columnas finales (último worker),
//    columnas_por_worker (de tipo RGBPixel) es una matriz de columnas (pixeles) de los workers (menos el último), ultimas_columnas puntero (de tipo RGBPixel) a las ultimas columnas (trabajadas por el último worker).
//Salidas: Puntero a BMPImage, el cual representa la imagen reensamblada 
//Descripción: Dadas todas las columnas de pixeles RGB de la imagen, reensambla la imagen juntando las columnas y rellenando la imagen con sus columnas filtradas
//  va reensamblando pixeles columna a columna, manejando las columnas de cada worker, lo cual se va midiendo con num_workers
BMPImage* reensamblarImagen(BMPImage* image, int pixeles_por_columna, int num_workers, int pixeles_por_columna_final, RGBPixel **columnas_por_worker, RGBPixel *ultimas_columnas);

//Entradas: cantidad_workers es un entero correspondiente al número de workers, fd1 (de tipo entero) corresponde a una matriz con descriptores de archivos para comunicación, fd2 (de tipo entero) corresponde a una matriz con descriptores de archivos para comunicación, cantidad_filtros es un entero que representa la cantidad de filtros a aplicar, factor_saturacion es un flotante que representa factor de saturacion a aplicar en una imagen, umbral_binarizacion es un flotante que representa el umbral de binarizacion para binarizar una imagen.
//Salidas:void
//Descripción: Ejecuta la cantidad de workers que se le indique, entregandole los parametros de entrada que le correspondan a los procesos workers, inicializa los canales de comunicación
//  entre padre "broker" e hijo "worker" con pipes, generando comunicación bidireccional
void ejecutarWorkers(int cantidad_workers, int fd1[][2], int fd2[][2], int cantidad_filtros, float factor_saturacion, float umbral_binarizacion);

//Entradas: columnas_workers (de tipo RGBPixel) es la matriz de las columnas divididas para los workers iniciales, columnas_ultimo_worker (de tipo RGBPixel) son las columnas entregadas al último worker
//  columnas_hijo (de tipo RGBPixel) es la matriz de las columnas trabajadas por los workers iniciales, columnas_ultimo_hijo (de tipo RGBPixel) son las columnas trabajadas por el último worker, cantidad_workers es un entero que representa la cantidad de workers.
//Salidas: void.
//Descripción: Los punteros que pidieron memoria y son recibidos como entrada en la funcion, liberan la memoria pedida
void free_columnas(RGBPixel **columnas_workers, RGBPixel *columnas_ultimo_worker, RGBPixel **columnas_hijo, RGBPixel *columnas_ultimo_hijo, int cantidad_workers);

//Entradas: cantidad_workers es un entero correspondiente a la cantidad de workers, pixeles_por_columna es un entero correspondiente a pixeles por columnas para workers iniciales, pixeles_por_columnas_finales es un entero que representa la cantidad de pixeles por columnas del último worker
//  columnas_workers es puntero (de tipo RGBPixel) a la matriz que contiene las columnas de los workers iniciales, columnas_ultimo_worker es puntero (de tipo RGBPixel) que contiene las columnas del último worker
//  columnas_hijo es puntero (de tipo RGBPixel) a la matriz que contiene las columnas ya trabajadas por los workers iniciales, columnas_ultimo_hijo es puntero (de tipo RGBPixel) que contiene las columnas ya trabajadas por el último worker
//Salidas: void
//Descripción: Asigna memoria a los punteros que se le entregan como parámetros, segun corresponda, se basa en la cantidad de workers y los pixeles que le corresponden a cada worker
void asignarMemoriaParaColumnasWorkers(int cantidad_workers, int pixeles_por_columna, int pixeles_por_columnasfinales, RGBPixel ***columnas_workers, RGBPixel **columnas_ultimo_worker, RGBPixel ***columnas_hijo, RGBPixel **columnas_ultimo_hijo);
