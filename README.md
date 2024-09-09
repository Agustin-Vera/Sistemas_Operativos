# **Sistemas_Operativos**
Repositorio correspondiente a los laboratorios del ramo sistemas operativos.
Los laboratorios son realizados en el lenguaje de programación C.

## **Contexto Laboratorio**
Se necesita realizar la aplicación de filtros a imagenes de tipo BMP. Los filtros se aplican en un orden, como el siguiente:
* **Saturación**: Satura la imagen intensificando sus colores dad un factor de saturación.

* **Escala de Grises**: Lleva la imagen a una escala de grises según una formula planteada.

* **Binarización**: Dado un umbral, si el valor de un pixel se encuentra por debajo o es igual al umbral el pixel tomar el valor 0 (negro) y en caso de que sea mayor al umbral toma el valor 255 (blanco).

Finalizando, se realiza la clasificación **Nearly Black** de la imagen dado un umbral de clasificación, esta clasificación indica si una imagen tiene una cantidad determinada de pixeles negros, lo cual indica que la imagen es casi negra.

## **Compilación y Ejecución**
Se usa makefile para la compilación.
> **>>> make**

Se puede usar makefile para eliminar archivos creados con make
> **>>> make clear**

### **Parametros de Entrada**
* **N**: Prefijo de imagenes BMP --> img, photo o imagen (**String** donde solo esas tres opciones están permitidas)
* **f**: Cantidad de filtros a aplicar (**int** de 1 a 3)
* **p**: Factor de saturación del filtro (**Float**)
* **u**: Umbral para binarizar la imagen (**Float** de 0.00 a 1.00)
* **v**: Umbral para clasifiación nearly black (**Float** de 0.00 a 1.00)
* **C**: Nombre de carpeta resultante con imágenes, con los filtros aplicados (**String**)
* **R**: Nombre del archivo CSV con las clasificaciones resultantes (**String**)

Parametro agregado correspondiente solo a laboratorio 2:
* **W**: Cantidad de workers a crear (**int** debe ser un número positivo)

### **Ejecución Laboratorio 1**
> **>>> ./lab1 -N imagen -f 2 -p 2 -u 0.7 -v 0.8 -C ImagenesFiltradas -R ImagenesClasificadas**

### **Ejecución Laboratorio 2**
> **>>> ./lab2 -N imagen -f 2 -p 2 -u 0.7 -v 0.8 -C ImagenesFiltradas -R ImagenesClasificadas -W 4**
