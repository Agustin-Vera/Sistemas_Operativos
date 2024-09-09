#include <stdio.h>
#include <stdlib.h>
#include "ImageBMP.h"

//Entradas: factor, número de tipo flotante correspondiente al factor de saturación que se le aplica a una imagen, num_pixeles, número entero que corresponde a la cantidad de pixeles que contiene una imagen y columnas, puntero a las columnas de una imagen de tipo RGBPixel. 
//Salidas: void (todos los pixeles son modificados).
//Descripción: Aplica la saturación a todos los pixeles contenidos en las columnas que se le entreguen, aplica mediante un factor de saturación dado.
//   Si la multiplicación entre el factor y un componente (sea el pixel R, G o B) sobrepasa el valor de 255, el pixel R, G o B queda como 255. Si no, el pixel R, G
//   o B queda como factor * componente (R, G o B). 
void aplicarSaturacion(float factor, int num_pixeles, RGBPixel *columnas);

//Entradas: num_pixeles, número entero que corresponde a la cantidad de pixeles que contiene una imagen y columnas, puntero (de tipo RGBPixel) a las columnas de una imagen.
//Salidas: void (todos los pixeles son modificados)
//Descripción: Aplica un filtro para llevar todos los pixeles contenidos en las columnas entregadas a escala de grises según una fórmula.
//  fórmula: escala_grises = pixel_r*0.3 + pixel_g*0.59 + pixel_b*0.11, por ende, modifica todos los pixeles entregados.
void aplicarEscalaGrises(int num_pixeles, RGBPixel *columnas);

//Entradas: umbral, número de tipo flotante correspondiente al umbral de binarización que se le aplica a una imagen, num_pixeles, número entero que corresponde a la cantidad de pixeles que contiene una imagen y columnas, puntero (de tipo RGBPixel) a las columnas de una imagen.
//Salidas: void (todos los pixeles son modificados).
//Descripción: Aplica la binarización a cada pixel contenido en las columnas entregadas. El criterio para modificar un pixel a blanco (valor 255)
//  o modifcarlo a negro (0) dado un umbral de binarizacion, el cual se compara con el valor del pixel RGB.
void aplicarBinarizacion(float umbral, int num_pixeles, RGBPixel *columnas);
