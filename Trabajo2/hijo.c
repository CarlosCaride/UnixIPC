/***********************************************************************
* Alumno: Carlos Caride Santeiro
* DNI: 44446239-G
* Centro Asociado: OURENSE 
* Teléfono de contacto: 690.155.343
* Email: ccaride5@alumno.uned.es
* Curso: 2018/2019
* Fecha: 15/12/2018
*
* Proceso hijo del Trabajo II: Combate de procesos
*************************************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


#include "common.h"

#define TRUE 1

//Estado del hijo "KO" o "OK"
char estado[2];

//Manejador de señal SIGUSR1 cuando el hijo se defiende
void defensa();
//Manejador de señal SIGUSR1 cuando el hijo se defiende
void indefenso();

//Numero de hijo
int hi;

int main(int argc, char *argv[]) {
    //Estructura para el contenido del mensaje recibido y longitud del mismo
    struct MensajeEstado mensaje;
    int longitudMensaje = sizeof(mensaje) - sizeof(mensaje.tipo);

    //buffer auxiliar para la lectura de 1 byte de la barrera
    char data[1];

    //PID del hijo
    int pid;
    //PID del hijo que se va a atacar
    int pidAtaque;
    //Numero de hijos iniciales
    int hijosIniciales;
    //Auxiliar de la funcion rand
    int rndIndex;

    //Llave usada para la cola de mensajes, semáforo y memoria compartida
    key_t llave;
    //Identificador de la cola de mensajes
    int mensajes;
    //Identificador de la memoria compartida
    int listaSHM;
    //Puntero al primer elemento de la memoria compartida
    int *lista;
    //Identidcicador del semáforo
    int sem;
    //Identificador de la tuberia sin nombre (lectura)
    int barrera;

    //Obtenemos el PID del proceso hijo
    pid = getpid();

    //El segundo argumento es el numero del hijo
    hi = atoi(argv[2]);
    //El tercer argumento es el numero total de hijos
    hijosIniciales = atoi(argv[3]);
    //El cuarto argumento es el descriptor de la barrera (lectura)
    barrera = atoi(argv[4]);

    //Se establecen los datos invariables del mensaje: tipo y PID
    mensaje.tipo = 1;
    mensaje.PID = pid;

    //Creamos la llave para la cola de mensajes, semaforo y memoria compartida
    if ((llave = ftok(argv[1], 'C')) == -1) {
        perror("HIJO-ftok");
        exit(-1);
    }

    //Obtenemos la cola de mensajes (ya creada por PADRE)
    if((mensajes = msgget(llave, IPC_CREAT | 0600)) == -1) {
        perror("HIJO--msgget");
            exit(-1);
    }

    //Obtenemos la memoria compartida (creado por PADRE)
    if ((listaSHM = shmget(llave, hijosIniciales * sizeof(int), IPC_CREAT | 0600)) == -1) {
        perror("HIJO--shmget");
            exit(-1);
    }

    //Se obtiene el puntero al array de la memoria compartida
    lista = shmat(listaSHM, 0, 0);  

    //Obtenemos el semaforo (creado por PADRE)
    if((sem = semget(llave, 1, IPC_CREAT | 0600)) == -1) {
        perror("HIJO--semget");
            exit(-1);
    }

    //Establecemos en la lista el PID del proceso
    wait_sem(sem);
    lista[hi] = pid;
    signal_sem(sem);

    //Inicializamos rand con una semilla aleatoria.
    srand(time(NULL)*(hi+1));

    //Bucle de ronda de ataques
    while (TRUE) {

        //Establecemos el estado OK
        strcpy(estado, "OK");

        //Esperamos a la señal de inicio de ataque leyendo un byte de la tuberia sin nombre
        read(barrera, data, 1);

        //Se decide si se actaca o se defiende
        if(rand() % 2 == 0) {
            //Establecemos defensa y esperamos al final de la contienda
            signal(SIGUSR1, defensa);
            printf("El hijo %d decide defender\n", hi + 1);
            usleep(200000);
        } else {
            //Establecemos ataque
            signal(SIGUSR1, indefenso);
            printf("El hijo %d decide atacar\n", hi + 1);
            usleep(100000);
            
            //Decidimos a que hijo atacamos aleatoriamente
            pidAtaque = 0;

            wait_sem(sem);
            while(pidAtaque == 0) {
                rndIndex = rand() % hijosIniciales;
                if (lista[rndIndex] != pid && lista[rndIndex] != 0) {
                    pidAtaque = lista[rndIndex];
                }
            }
            signal_sem(sem);

            printf("El hijo %d ataca al hijo %d\n", hi + 1, rndIndex + 1);

            //Atacamos al hijo seleccionado
            kill(pidAtaque, SIGUSR1);
            usleep(100000);
        }

        //Mandamos un mensaje a PADRE indicando el estado actual del hijo
        strcpy(mensaje.estado, estado);
        if(msgsnd(mensajes, &mensaje, longitudMensaje, 0) == -1) {
            perror("HIJO--msgsend");
            exit(-1);
        }
    }
}

void defensa() {
    printf("El hijo %d ha repelido un ataque\n", hi + 1);
    strcpy(estado, "OK");
}

void indefenso() {
    printf("El hijo %d ha sido emboscado mientras realizaba un ataque\n", hi + 1);
    strcpy(estado, "KO");
}