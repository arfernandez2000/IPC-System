# TP1-SO
Primer Trabajo Práctico Obligatorio para la materia Sistemas Operativos

# Requerimentos

Se necesita obligatoriamente la librería Minisat . Para análisis estático de código se necesita *PVS-STUDIO* y *cppcheck* y para formatear la librería *Clang-Format*.

sudo apt-get install minisat.

A su vez, se debe contar con archivos .cnf en la carpeta raíz del proyecto(TP1-SO).

# Compilación

Ubicarse en la  carpeta raíz. Para compilar ejecutar: 

$> make all

Los ejecutables que se crean son master, slave y view.

# Correr el programa

## Opcion 1: Utilizando solo una terminal

$> ./master filename1.cnf  filename2.cnf .... | ./view

## Opcion 2: Utilizando dos terminales

En la primer terminal ejecutar 

$> ./master filename1.cnf  filename2.cnf ...
NUMBER
Este proceso va a imprimir en stdout un int. Debemos pasarle este número como argumento al proceso view en la segunda terminal

En la segunda terminal ejecutar:
./view NUMBER


# Static Code Analysis

Para ejecutar la herramienta de análisis de código estático Cppcheck :

$> make check

Para ejecutar PVS-Studio correr:

$> make pvs

Los resultados de pvs se pueden apreciar en el archivo report.tasks. En el informe hay una justificación de la respuesta. 

# Formating

Para ejecutar la herramienta de formateo Clang-format ejecutar:

$> make format

