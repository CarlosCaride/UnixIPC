/***********************************************************************
* Alumno: Carlos Caride Santeiro
* DNI: 44446239-G
* Centro Asociado: OURENSE 
* Teléfono de contacto: 690.155.343
* Email: ccaride5@alumno.uned.es
* Curso: 2018/2019
* Fecha: 15/12/2018
*
* Proceso padre del Trabajo II: Combate de procesos
*************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

#define TRUE 1

int main(int argc, char *argv[]) {
    //Estructura para el contenido del mensaje recibido y longitud del mismo
    struct MensajeEstado mensaje;
    int longitudMensaje = sizeof(mensaje) - sizeof(mensaje.tipo);
    
    //Nombre del archivo FIFO que se usará para escribir el resultado.
    char * resultado = "resultado";

    //Enteros auxiliares para el desarrollo del programa
    int k, hijosIniciales, msgReceived, deletedPID, retornoWait, resultadoFIFO, pid;

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
    //Identificadores de la tuberia sin nombre
    int barrera[2];

    //Mensaje que se envía antes de cada ronda de ataques
    char msgContienda[] = "\nIniciando ronda de ataques";

    //Mensaje de resultado
    char msgResultado[50];

    //Comprobamos que se nos pasara como arguemento el número de hijos
    if (argc < 2) {
        printf("PADRE--Error en el numero de argumentos\n");
        exit(-1);
    }

    //Convertimos el segundo argumento en el número de hijos que realizarán el combate.
    k = atoi(argv[1]);
    hijosIniciales = atoi(argv[1]);

    //Creamos la llave para la cola de mensajes, semaforo y memoria compartida
    if ((llave = ftok(argv[0], 'C')) == (key_t)-1) {
        perror("PADRE--ftok");
        exit(-1);
    }

    //Creamos la cola de mensajes
    if((mensajes = msgget(llave, IPC_CREAT | 0600)) == -1) {
        perror("PADRE--msgget");
        exit(-1);
    }

    //Creamos la memoria compartida y la obtenemos
    if ((listaSHM = shmget(llave, hijosIniciales * sizeof(int), IPC_CREAT | 0600)) == -1) {
        perror("PADRE--shmget");
        exit(-1);
    }

    //Se obtiene el puntero al array de la memoria compartida
    lista = shmat(listaSHM, 0, 0);

    //Se crea el semaforo
    if((sem = semget(llave, 1, IPC_CREAT | 0600)) == -1) {
        perror("PADRE--semget");
        exit(-1);
    }

    //Se inicia el semaforo
    init_sem(sem, 1);

    //Se crear la tubería sin nombre
    if(pipe(barrera) == -1) {
        perror("PADRE--pipe");
        exit(-1);
    }

    //Iniciamos los hijos indicados como argumento
    for (int i = 0; i < hijosIniciales; i++)
    {
        char iStr[30];
        char barreraStr[30];
        sprintf(iStr, "%d", i);
        sprintf(barreraStr, "%d", barrera[0]);
        if ((pid=fork()) == -1) {
            perror("fork");
            exit(7);
        } else if (pid == 0) {
            int ret = execl("HIJO", "HIJO", argv[0], iStr, argv[1], barreraStr, NULL);
            exit(0);
        }
    }

    //Establecemos el numero de hijos "caidos en combate" a cero
    deletedPID = 0;

    //Esperamos 0.1 segundos para que esten todos los hijos esperando a la señal
    printf("Hijos creados. Preparando hijos para ataques...\n");
    usleep(100000);

    //Iniciamos la batalla hasta que solo quede uno o ninguno
    while (TRUE) {

        printf("%s\n", msgContienda);

        //Damos la señal de ataque por la tuberia con k bytes = numero de hijos vivos
        write(barrera[1], msgContienda, k);
        //Esperamos 0.3 segundos a que acaben la ronda
        usleep(300000);

        //Leemos los mensajes que nos envian los hijos
        msgReceived = 0;
        while(msgReceived < k) {
            if(msgrcv(mensajes, &mensaje, longitudMensaje, 1, 0) == -1) {
                perror("HIJO--msgrcv");
                exit(-1);
            }

            //Si el estado del mensajes es "KO" terminamos el proceso.
            if (strcmp(mensaje.estado, "KO") == 0) {
                kill(mensaje.PID, SIGTERM);
                wait(&retornoWait);

                //Asignamos al valor de hijo en la lista como 0 para indicar que ha fallecido el hijo
                wait_sem(sem);
                for (int i = 0; i < hijosIniciales; ++i)
                {
                    if (mensaje.PID == lista[i]) {
                        lista[i] = 0;
                    }
                }
                signal_sem(sem);
                deletedPID++;
            }
            msgReceived++;
        }

        //Comprobamos los hijos que continuan vivos. Si es uno, es el ganador. Si cero, empate. En otro caso,
        //se realiza otra ronda de ataques.
        k = hijosIniciales - deletedPID;
        if(k == 1){
            //Comprobamos que hijo es el ganador e imprimimos el resultado por el fichero FIFO
            wait_sem(sem);
            for (int i = 0; i < hijosIniciales; ++i)
            {
                if (0 != lista[i]) {
                    //Terminamos el hijo ganador.
                    kill(lista[i], SIGTERM);
                    wait(&retornoWait);

                    //Imprimimos el resultado
                    resultadoFIFO = open("resultado", O_WRONLY);
                    sprintf(msgResultado, "El hijo %d ha ganado\n", i+1);
                    write(resultadoFIFO, msgResultado, strlen(msgResultado));
                    close(resultadoFIFO);

                    break;
                }
            }
            signal_sem(sem);
            break;
        }
        if (k == 0) {
            //Imprimimos por el fichero FIFO que hubo un empate
            resultadoFIFO = open("resultado", O_WRONLY);
            sprintf(msgResultado, "Empate\n");
            write(resultadoFIFO, msgResultado, strlen(msgResultado));
            close(resultadoFIFO);
            break;
        }
    }

    //Eliminamos la cola de mensajes
    if (msgctl(mensajes, IPC_RMID, 0) == -1) {
        perror("PADRE--msgctl--remove");
        exit(-1);
    }

    //Desasociamos la lista y eliminamos la memoria compartida
    shmdt(lista);
    if (shmctl(listaSHM, IPC_RMID, 0) == -1) {
        perror("PADRE--shmctl--remove");
        exit(-1);
    }

    //Eliminamos el semaforo
    if (semctl(sem, 0, IPC_RMID, 0) == -1) {
        perror("PADRE--semctl--remove");
        exit(-1);
    }

    //Cerramos la tuberia sin nombre
    close(barrera[0]);
    close(barrera[1]);

    //Mostramos los IPCs del sistema
    system("ipcs");

    return 0;
}