==========================================================
Esta es la documentación para compilar y ejecutar su tarea
==========================================================

Se está ejecutando el comando: less README.txt

***************************
*** Para salir: tecla q ***
***************************

Para avanzar a una nueva página: tecla <page down>
Para retroceder a la página anterior: tecla <page up>
Para avanzar una sola línea: tecla <enter>
Para buscar un texto: tecla / seguido del texto (/...texto...)
         por ejemplo: /ddd

-----------------------------------------------

Breve introducción a nThreads

(Luego las instrucciones para su tarea.)

Este es nThreads (nano threads).  Es un sistema operativo de
juguete que implementa threads ultra livianos a partir de un numero fijo
de p-threads (no tan livianos).  Su uso es puramente pedagógico.

Para compilar sus tareas previas (T1, T2 y T3) requiere cambiar:

#include <pthread.h>

por:

#define _XOPEN_SOURCE 500
#include <nSystem.h>

No todas las funciones de Unix/Debian se pueden ejecutar.  Además las
funciones tienen nombres distintos.  Al final del archivo nKernel/nthread.h
hay macros del tipo #define pthread_create nThreadCreate que traducen
los nombres oficiales de la API de Unix/Debian a los nombres de nThreads.
Por eso Ud. si puede usar los nombres tradicionales en sus programas.
Pero considere que el debugger le mostrará los nombres traducidos.
Para saber qué funciones puede invocar, vea si hay un #define para su
función al final del archivo nKernel/nthread.h.  Si no lo hay, lo más
probable es que no puede usar esa función.  Están todas las funciones que
requiere para sus tareas.

Para compilar una aplicación que consista por ejemplo de los archivos
pub.c y test-pub.c, ejecute alguno de estos comandos:

make SRCS="pub.c test-pub.c" pub.bin-mem
make SRCS="pub.c test-pub.c" pub.bin-g
make SRCS="pub.c test-pub.c" pub.bin

El primero genera el binario pub.bin-mem con operaciones de memoria o
indefinidas verificadas por sanitize.  El segundo es para hacer debugging con
el comando ddd pub.bin-g.  El tercero esta compilado con todas las
optimizaciones.

Ejecute el comando con la opción -h para recibir explicaciones sobre
las opciones para ejecutar con distintos schedulings.  Por ejemplo:

./pub.bin-mem -h

----------------------------------------------

Instrucciones para la tarea 5

Ud. debe programar en el archivo nKernel/npub.c las funciones solicitadas.
Ya hay una plantilla de lo pedido en nKernel/npub.c.plantilla.

Pruebe su tarea bajo Debian 11 de 64 bits.  Estos son los requerimientos
para aprobar su tarea con nota 5 menos el descuento por día de atraso:

+ make run debe felicitarlo por aprobar este modo de ejecución.
+ make run-g debe felicitarlo por aprobar este modo de ejecución.
+ make run-mem debe felicitarlo por aprobar este modo de ejecución y no
  señalar ningún problema como errores de manejo de memoria, excepto
  el señalado a continuación.

*** No se preocupe si make run-mem reclama con este error: ***

==12434==WARNING: ASan is ignoring requested __asan_handle_no_return:
stack top: 0x7f8efd4fef40; bottom 0x7fff61550000; size: 0xffffff8f9bfaef40
(-482714390720)
False positive error reports may follow
For details see https://github.com/google/sanitizers/issues/189

Es un problema conocido que ocurre con sistemas de threads a nivel usuario
como nThreads.

Para obtener todo el puntaje además debe correr su tarea con scheduling
round robin para tri-core con estos comandos:

make run-tricore
make run-tricore-g
make run-tricore-mem

Detección de dataraces

Lamentablemente los reportes de make run-san no son confiables.  A menudo
make run-san se cae en el código de sanitize (no el de uno) y reporta errores
inexistentes.  A pesar de ello, si tiene dataraces, make run-san sí podría
ayudarlo.  Pero no es requisito aprobar exitosamente make run-san en esta
tarea.

Cuando pruebe su tarea asegúrese de que su computador está
configurado en modo alto rendimiento y que no se estén corriendo otros
procesos intensivos en uso de CPU al mismo tiempo.  De otro modo podría
no aprobar el test de prueba.

Para depurar:

make run-ddd : corre ddd con fcfs para single core
make run-ddd-tricore : corre ddd con round robin para tricore

Tenga en consideración que depurar programas con procesos multi-threaded es
más difícil que depurar programas secuenciales.

Video con ejemplos de uso de ddd: https://youtu.be/FtHZy7UkTT4
Archivos con los ejemplos:
  https://www.u-cursos.cl/ingenieria/2020/2/CC3301/1/novedades/r/demo-ddd.zip

-----------------------------------------------

Entrega de la tarea

Invoque el comando make zip para ejecutar todos los tests y generar un
archivo npub.zip que contiene npub.c, con su solución, y resultados.txt,
con la salida de make run, make run-g, make run-mem y las 3 ejecuciones
para round robin en un tri-core.  ¡Son cores virtuales!  Su máquina no
necesita ser tri-core.

Entregue por U-cursos el archivo npub.zip

A continuación es muy importante que descargue de U-cursos el mismo
archivo que subió.  Luego examine el archivo npub.c revisando que es
su última versión.  Es frecuente que no lo sea producto de un defecto
de U-cursos.

-----------------------------------------------

Limpieza de archivos

make clean

Hace limpieza borrando todos los archivos que se pueden volver
a reconstruir a partir de los fuentes: *.o binarios etc.

-----------------------------------------------

Ejemplos incluidos con nThreads

hello.c
factor.c: determina un factor de un número entero
msgs.c: programa de prueba para los mensajes

Compile y ejecute por ejemplo con:

make hello.bin-mem ; ./hello.bin-mem
make factor.bin ; ./factor.bin-mem -slice 100 -ncores 3 229442531844556801 6
make msgs.bin-g ; ./msgs.bin-g
make SRCS="pub.c test-pub.c" pub.bin ; ./pub.bin
