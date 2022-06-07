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

Ud. debe crear el archivo disco.c y programar ahí las funciones solicitadas.
Ya hay una plantilla para disco.c en disco.c.plantilla.  Ud. debe incluir
disco.h, en donde se declaran los encabezados de la funciones pedidas.

Pruebe su tarea bajo Debian 11 de 64 bits.  Estos son los requerimientos
para aprobar su tarea:

+ make run debe felicitarlo por aprobar este modo de ejecución.

+ make run-g debe felicitarlo.

+ make run-san debe felicitarlo y no debe señalar ningún datarace.

+ make run-mem debe felicitarlo y no señalar ningún problema como errores
  de manejo de memoria.

Cuando pruebe su tarea asegúrese de que su computador que está
configurado en modo alto rendimiento y que no se estén corriendo otros
procesos intensivos en uso de CPU al mismo tiempo.  De otro modo podría
no aprobar el test de orden de llegada.

Invoque el comando make zip para ejecutar todos los tests y generar un
archivo disco.zip que contiene disco.c, con su solución, y resultados.txt,
con la salida de make run, make run-g, make run-san y make run-mem.

Para depurar use: make ddd
Tenga en consideración que depurar programas con procesos multi-threaded es
más difícil que depurar programas secuenciales.

Video con ejemplos de uso de ddd: https://youtu.be/FtHZy7UkTT4
Archivos con los ejemplos:
  https://www.u-cursos.cl/ingenieria/2020/2/CC3301/1/novedades/r/demo-ddd.zip

-----------------------------------------------

Entrega de la tarea

Ejecute: make zip

Entregue por U-cursos el archivo disco.zip

A continuación es muy importante que descargue de U-cursos el mismo
archivo que subió.  Luego examine el archivo disco.c revisando que es
su última versión.  Es frecuente que no lo sea producto de un defecto
de U-cursos.

-----------------------------------------------

Limpieza de archivos

make clean

Hace limpieza borrando todos los archivos que se pueden volver
a reconstruir a partir de los fuentes: *.o binarios etc.

-----------------------------------------------

Acerca del comando make

El comando make sirve para automatizar el proceso de compilación asegurando
recompilar el archivo binario ejecutable cuando cambió uno de los archivos
fuentes de los cuales depende.

A veces es útil usar make con la opción -n para que solo muestre
exactamente qué comandos va a ejecutar, sin ejecutarlos de verdad.
Por ejemplo:

   make -n sort-rv-nbits.ddd

También es útil usar make con la opción -B para forzar la recompilación
de los fuentes a pesar de que no han cambiado desde la última compilación.
Por ejemplo:

   make -B sort-rv-nbits

Recompilará para generar el archivo sort-rv-nbits desde cero
