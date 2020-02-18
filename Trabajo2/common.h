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
* Estructura del mensaje
*************************************************************************/

//Estructura del mensaje usado
struct MensajeEstado
{
    long tipo;
    int PID;
    char estado[2];
};

//Inicializacion del semaforo
int init_sem(int semid, int valor);

//Soliciud de uso exclusivo
int wait_sem(int semid);

//Libracion del uso exclusivo
int signal_sem(int semid);