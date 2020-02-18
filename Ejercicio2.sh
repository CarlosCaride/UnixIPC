#!/bin/bash
#/***********************************************************************
#* Alumno: Carlos Caride Santeiro
#* DNI: 44446239-G
#* Centro Asociado: OURENSE 
#* Teléfono de contacto: 690.155.343
#* Email: ccaride5@alumno.uned.es
#* Curso: 2018/2019
#* Fecha: 15/12/2018
#*
#* Script Ejercicio2.sh del Trabajo II: Combate de procesos
#*************************************************************************/

#1 compila los fuentes padre.c e hijo.c con gcc
gcc Trabajo2/padre.c Trabajo2/common.c -o PADRE
gcc Trabajo2/hijo.c Trabajo2/common.c -o HIJO

#2 crea el fihero fifo "resultado"
mkfifo resultado

#3 lanza un cat en segundo plano para leer "resultado"
cat resultado &

#4 lanza el proceso padre con 10 hijos
./PADRE 10

#5 al acabar limpia todos los ficheros que ha creado
rm resultado
rm PADRE
rm HIJO
