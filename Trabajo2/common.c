/***********************************************************************
* Alumno: Carlos Caride Santeiro
* DNI: 44446239-G
* Centro Asociado: OURENSE 
* Teléfono de contacto: 690.155.343
* Email: ccaride5@alumno.uned.es
* Curso: 2018/2019
* Fecha: 15/12/2018
*
* Procedimientos init_sem, wait_sem y signal_sem comunes a padre e hijo
*************************************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

#include "common.h"

//Estructura del mensaje usado
int init_sem(int semid, int valor) {
    if (semctl(semid, 0, SETVAL, valor) == -1) {
        perror("COMMON--init_sem:");
        return -1;
    }

    return 0;
}

//Soliciud de uso exclusivo
int wait_sem(int semid) {

    struct sembuf op[1];

    op[0].sem_num = 0;
    op[0].sem_op = -1;
    op[0].sem_flg = 0;

    if (semop(semid, op, 1) == -1) {
        perror("COMMON--wait_sem:");
        return -1;
    }
    
    return 0;
}

//Libracion del uso exclusivo
int signal_sem(int semid) {

    struct sembuf op[1];

    op[0].sem_num = 0;
    op[0].sem_op = 1;
    op[0].sem_flg = 0;

    if (semop(semid, op, 1) == -1) {
        perror("COMMON--signal_sem\n");
        return -1;
    }

    return 0;
}