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

//Entradas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen a ser saturada y un número flotante respectivo al factor de saturación.
//Salidas: new_image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen nueva que fue filtrada mediante saturación. 
//Descripción: Modifica la imagen ingresada como entrada, creando una imagen nueva aplicando el filtro de saturación a todos los pixeles de  
//             dicha imagen dentro de la estructura tipo BMPImage según un factor ingresado. Si la multiplicación entre el factor y un 
//             componente (sea el pixel R, G o B) sobrepasa el valor de 255, el pixel R, G o B queda como 255. Si no el pixel R, G
//             o B queda como factor * componente (R, G o B).
BMPImage* saturate_bmp(BMPImage* image, float factor);

//Entradas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen a ser filtrada en escala de grises.
//Salidas: new_image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen nueva que fue filtrada mediante escala de grises. 
//Descripción: Modifica la imagen ingresada como entrada, creando una imagen nueva aplicando el filtro de la escala de grises de los pixeles de la imagen con la fórmula:
//             escala_grises = pixel_r*0.3 + pixel_g*0.59 + pixel_b*0.11
//             con ello, altera todos los pixeles con escala_grises y la imagen a retornar esta en su escala de grises.
BMPImage* rgb_escala_de_grises(BMPImage* image);

//Entradas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen a ser binarizada y un número flotante respectivo al umbral de binarización.
//Salidas: new_image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen nueva que fue filtrada mediante binarización.
//Descripción: Modifica la imagen ingresada como entrada, creando una imagen nueva alterando cada pixel de la imagen, si el valor del pixel es:
//             valor_pixel = pixel_r + pixel_g + pixel_b
//             si valor_pixel es mayor que umbral*255 (el umbral es llevado a valores rgb), el pixel pasara a ser blanco, en caso contrario pasa a ser negro.
BMPImage* binarizacion_bmp(BMPImage* image, float umbral);

//Entradas: image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen que va a ser puesta en clasificación de nearly black y un número flotante respectivo al umbral de clasificación.
//Salidas: nearly_black, entero que representa si una imagen es casi negra dado un umbral. 1 indica que es casi negra y 0 indica lo contrario.
//Descripción: Calcula la proporcion de cantidad de pixeles negros frente a la cantidad de pixeles totales, si el porcentaje de pixeles
//             negros es mayor al umbral ingresado, se dice que la imagen es nearly black. 
int is_nearly_black(BMPImage* image, float umbral);

//Entradas: filename, puntero a tipo char correspondiente al nombre de la imagen modificada e image, puntero a tipo BMPImage que contiene la dirección de memoria de la imagen modificada mediante filtros.
//Salidas: void (crea una imagen bmp que dependiendo del nombre puede ser almacenada en una carpeta, o en el mismo lugar donde esta el codigo fuente).
//Descripción: Escribe una imagen bmp, por ende, la imagen es creada en una direccion, la imagen tiene las caracteristicas presentes en el puntero de tipo BMPImage a la imagen que se desea crear.
void write_bmp(const char* filename, BMPImage* image);
