#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "ImageBMP.h"

//Entradas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen a ser liberada.
//Salidas: void.
//Descripción: Libera la memoria pedida por las estructuras de la imagen (BMPImage), incluyendo la memoria reservada para los datos de la imagen y cualquier otro recurso que haya sido asignado durante la creación  o manipulación de la imagen.
void free_bmp(BMPImage* image);

//Entradas: filename, puntero a tipo char correspondiente al nombre de la imagen modificada e image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen modificada mediante filtros.
//Salidas: void (crea una imagen bmp que dependiendo del nombre puede ser almacenada en una carpeta, o en el mismo lugar donde esta el codigo fuente).
//Descripción: Escribe una imagen bmp, por ende, la imagen es creada en una direccion, la imagen tiene las caracteristicas presentes en el puntero de tipo BMPImage a la imagen que se desea crear.
void write_bmp(const char* filename, BMPImage* image);

//Entradas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen que va a ser puesta en clasificación de nearly black y un número flotante respectivo al umbral de clasificación.
//Salidas: nearly_black, entero que representa si una imagen es casi negra dado un umbral. 1 indica que es casi negra y 0 indica lo contrario.
//Descripción: Calcula la proporcion de cantidad de pixeles negros frente a la cantidad de pixeles totales, si el porcentaje de pixeles
//             negros es mayor al umbral ingresado, se dice que la imagen es nearly black. 
int is_nearly_black(BMPImage* image, float umbral);