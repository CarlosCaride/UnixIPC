# UNED - Ingeniería Informática - Programación y Estructuras Avanzadas

## PED 1 - Problema de la mochila con objetos no fraccionables

### Introducción
Los procesos deben comunicarse e interactuar entre ellos compartiendo datos y recur-sos. Para ello UNIX pone mecanismos IPCs universales (señales y tuberías). Asimismo, System V ofrece mecanismos de cola de mensajes, memoria compartida y semáforos pa-ra realizar la intercomunicación entre procesos.

En este trabajo consiste en un combate entre varios procesos hijos que será arbitrada por el proceso padre.

El proceso inicial es el padre, que se encarga de crear n hijos dado como argumento. Usando métodos de intercomunicación de procesos (IPCs), se encargará de iniciar ron-das de combate, eliminar los hijos que han sido abatidos y proclamar al campeón o em-pate.

El proceso hijo es iniciado por el proceso padre. Al igual que padre, este realizara la co-municación entre otros hijos y con el padre mediante el uso de mecanismos IPC. El hijo establecerá una postura de ataque, y atacará a otro hijo, o defensa. Al acabar la ronda, informará al proceso padre en qué estado se encuentra.

### Implementación
Para el desarrollo de este, se ha utilizado el editor de texto Sublime Text, así como la máquina virtual de Ubuntu 16.04 LTS aportada por el equipo docente.

El encabezado common.h declara la estructura del mensaje que se usara entre el proceso padre y los hijos. También declara los métodos de inicializar, solicitar y liberar el semá-foro para el recurso compartido de la lista de PID que se usará.

El archivo de código fuente common.c implementa los métodos inicializar, solicitar y liberar el semáforo.

El archivo Ejercicio2.sh es el punto de entrada del ejercicio, ya que este compila tanto padre.c como hijo.c, crea un archivo FIFO, ejecuta un cat en segundo plano al archivo FIFO y ejecuta PADRE con 10 hijos como argumento. Finalmente, elimina todos los ar-chivos creados.

El archivo padre.c implementa el comportamiento de PADRE. Este inicia la llave para los mecanismos IPC. Crea una nueva cola de mensajes, memoria compartida (para la lista de PID de los hijos), un semáforo y una tubería sin nombre. Tras ello, crea n hijos (siendo n pasado como argumento) e inicia una ronda escribiendo por la tubería sin nombre n bytes (siendo n el número de hijos “vivos”). Los hijos le envían el resultado de la ronda y el padre eliminar los que tengan como estado “KO”. Repetirá el proceso hasta que quede uno o ningún hijo, imprimiendo el resultado en el archivo FIFO. Finalmente, elimina todos los IPC que ha creado.

El archivo hijo.c implementa el comportamiento de HIJO. Este obtiene los IPC creados por el proceso padre y espera a que este último mande la señal de inicio de ronda. Tras ello, se pone aleatoriamente en modo ataque o defensa. Si ataca, elige un hijo aleatorio (no a si mismo), y lo ataca enviándole la señal de usuario 1. Tras ello, envía el mensaje de estado al proceso padre y espera una nueva ronda.

Vea la documentación aportada en el repositorio para la solucón completa.