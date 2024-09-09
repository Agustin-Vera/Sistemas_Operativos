#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(push, 1) //cuando se trabaja con uint se usa para leer directamente los bytes de la imagen, push hace que la estructura se lea byte por byte y pop hace que se lea de forma normal
// Estructura que representa el encabezado de una imagen BMP
typedef struct {
    // BMP Header | Tipo de dato por bits | Descripcion
    uint16_t type; // Tipo de dato, tiene 2 bytes y es un numero que indica si el archivo es BMP a traves de las siglas BM
    uint32_t size; // Tamanio del archivo, tiene 4 bytes y es un numero que indica el tamanio del archivo en bytes
    uint16_t reserved1; // Reservado, tiene 2 bytes y es un numero que no se utiliza
    uint16_t reserved2; // Reservado, tiene 2 bytes y es un numero que no se utiliza
    uint32_t offset; // Offset, tiene 4 bytes y es un numero que indica la posicion en bytes donde comienza la informacion de la imagen
} BMPHeader;

// Estructura que representa la informacion contenida en el encabezado de una imagen BMP
typedef struct {
    uint32_t size; // Tamanio de la informacion de la imagen, tiene 4 bytes y es un numero que indica el tamanio de la informacion de la imagen en bytes
    int32_t width; // Ancho de la imagen, tiene 4 bytes y es un numero que indica el ancho de la imagen en pixeles
    int32_t height; // Alto de la imagen, tiene 4 bytes y es un numero que indica el alto de la imagen en pixeles
    uint16_t planes; // Planos, tiene 2 bytes y es un numero que indica la cantidad de planos de color
    uint16_t bit_count; // Bits por pixel, tiene 2 bytes y es un numero que indica la cantidad de bits por pixel
    uint32_t compression; // Compresion, tiene 4 bytes y es un numero que indica el tipo de compresion
    uint32_t size_image; // Tamanio de la imagen, tiene 4 bytes y es un numero que indica el tamanio de la imagen en bytes
    int32_t x_pixels_per_meter; // Pixeles por metro en el eje X, tiene 4 bytes y es un numero que indica la cantidad de pixeles por metro en el eje X
    int32_t y_pixels_per_meter; // Pixeles por metro en el eje Y, tiene 4 bytes y es un numero que indica la cantidad de pixeles por metro en el eje Y
    uint32_t colors_used; // Colores usados, tiene 4 bytes y es un numero que indica la cantidad de colores usados
    uint32_t colors_important; // Colores importantes, tiene 4 bytes y es un numero que indica la cantidad de colores importantes
} BMPInfoHeader;
#pragma pack(pop)

// Estas estructuras se leen de forma normal debido a pop

// Estructura que representa un pixel RGB
typedef struct {
    unsigned char r; // Valor R (rojo) del pixel RGB
    unsigned char g; // Valor G (verde) del pixel RGB
    unsigned char b; // Valor B (azul) del pixel RGB
} RGBPixel;

// Estructura que representa una imagen tipo BMP
typedef struct {
    int width; // Ancho de la imagen
    int height; // Alto de la imagen
    RGBPixel *data; // Puntero a los pixeles de la imagen
} BMPImage;
