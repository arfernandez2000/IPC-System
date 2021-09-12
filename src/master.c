// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _XOPEN_SOURCE 500 //ftruncate warning
#include "master.h"
#define READ 0
#define WRITE 1
#define STDOUT 1
#define STDIN 0
#define USED 1
#define UNUSED 0

#define SLAVES 3
#define SLAVE_PATH "./slave"

sem_t* semaphore;
int currentTask = 1;
int totalTasks;

int main(int argc, char const *argv[]){
    if(argc < 2){
        error("Cantidad incorrecta de argumentos");
    }

    //setvbuf(stdout,NULL,_IONBF,0);
    char * ptr_write;
    //ptr_write = createShm();

    sleep(2);
    
    //semaphore = sem_open(SEM,O_CREAT,SEM_FLAGS);

    int initialTasks =  SLAVES * 2 >= argc ? 1 : 2; 

    FILE * results;
    results = fopen("results.txt", "w");
    if(results == NULL){
        error("No se pudo crear el archivo results");
    }

    int slaveCount = (argc - 1 > SLAVES)? SLAVES : argc - 1;
    slaveinfo slave[slaveCount];
    int remainingTasks = argc - 1;
    totalTasks = argc - 1;

    int created = createSlaves(slaveCount, initialTasks, (char**) argv, slave);
    printf("%d\n",remainingTasks);

    printf("sali de create\n");
    assignTasks(slave, slaveCount, remainingTasks, results, (char**) argv);
    printf("LPM\n");

    //ptr_write = writeShm(ptr_write);
   
    
    if(fclose(results) < 0){
        error("Error al cerrar results.txt");
    }

    //if( sem_close(semaphore) < 0)
    //    error("Semaphore close failed");

    closeSlaves(slave, slaveCount);   
    return 0;
}

int createSlaves(int slaveCount, int initialTasks, char* files[], slaveinfo* slave) {


    // FILE* fdPrueba;
    // fdPrueba = fopen("log.txt", "w+") ;

    int tasks[2];
    int answers[2];

    char* filesToSend[BUF_SIZE] = {0};
    int counter = 1;
    // char readBuf[10000] = {0};    

    printf("initial Tasks: %d\n", initialTasks);
    printf("slaves count: %d\n", slaveCount);

    //for por cada slave -> cortarlo si me quedo sin archivos antes de llenar los 7 slaves
    
    for (int i = 0; i < slaveCount; i++) {       
        if(pipe(tasks) < 0) {
            error("Error al crear pipe task");
        }

        if(pipe(answers) < 0) {
            error("Error al crear pipe answer");
        }

        printf("fdAnsRead: %d, fdTaskWrite: %d\n", answers[READ], tasks[WRITE]);

        int pid;
    
        if ((pid = fork()) == 0) {
            
            for (int j = 0; j < initialTasks; j++) {
                filesToSend[j] = files[counter++];
            }

            if ( dup2(answers[WRITE], STDOUT) < 0) {
                error("Error al hacer el dup2 de STDOUT");
            }

            if ( dup2(tasks[READ], STDIN) < 0) {
                error("Error al hacer el dup2 de STDIN");
            }
            
            if (close(tasks[READ]) < 0) {
                error("Error al cerras el fd de tasks, READ");
            }
            if (close(tasks[WRITE]) < 0) {
                error("Error al cerras el fd de tasks, WRITE");
            }
            if (close(answers[READ]) < 0) {
                error("Error al cerras el fd de answers, READ");
            }
            if (close(answers[WRITE]) < 0) {
                error("Error al cerras el fd de answers, WRITE");
            }
        
            if (execv(SLAVE_PATH, filesToSend) < 0) {
                error("Error no funciona execv");
            }
        } else if (pid < 0) {
            error("Error del fork");
        }
        
        slave[i].fdAnswersRead = answers[READ];
        slave[i].fdTasksWrite = tasks[WRITE];
        slave[i].status = USED; 
        slave[i].tasks = initialTasks;

        if(close(tasks[READ]) < 0 || close(answers[WRITE]) < 0){
            error("Error al cerrar pipes");
        }
        counter += initialTasks;
    }
    
    //TODO-funcion para el select

    // sleep(2);
    
    // for (int i = 0; i < slaveCount; i++) {
    //     if (read(slave[i].fdAnswersRead, readBuf, 10000) < 0) {
    //         error("Error al leer el answers, READ");
    //     }
    //     fprintf(fdPrueba, "%s\n", readBuf);
    //     close(slave[i].fdAnswersRead);
    // }
	
    // fclose(fdPrueba);
    currentTask += slaveCount*initialTasks;
    printf("current task en create: %d\n", currentTask);
    printf("chau create\n");
    return 1;
}

void closeSlaves(slaveinfo* slave, int slaveCount){

    for (int i = 0; i < slaveCount; i++){
        close(slave[i].fdAnswersRead);
        close(slave[i].fdTasksWrite);
    }
    
}

void closeWriteSlaves(slaveinfo* slave, int slaveCount){

    for (int i = 0; i < slaveCount; i++){
        close(slave[i].fdTasksWrite);
    }
    
}

void assignTasks(slaveinfo* slave, int slaveCount, int remainingTasks, FILE* results, char* tasks[]){
    while (remainingTasks > 0) {
        //printf("rt: %d\n", remainingTasks);
        fd_set fdSet;
        FD_ZERO(&fdSet);
        FD_SET(0, &fdSet);

        int fd;
        int maxFd = -1;
        for(int i = 0; i < slaveCount; i++) {
            if( slave[i].status ){
                fd = slave[i].fdAnswersRead;
                FD_SET(fd, &fdSet);
                maxFd = (maxFd > fd)? maxFd : fd;              
            }
        }
        
        int ready = select(maxFd + 1, &fdSet, NULL, NULL, NULL);
        if(ready < 0){
            error("Error al crear el select");
        }

        for(int i = 0; i < slaveCount; i++){
            if(FD_ISSET(slave[i].fdAnswersRead, &fdSet)) {
                //printf("entre con slave: %d\n", i + 1);
                if (writeResult(results, slave[i]) > 0) {
                    if(slave[i].tasks > 1){
                        remainingTasks--;
                    }
                    slave[i].tasks--;
                    if(slave[i].tasks == 0 && remainingTasks != 0){
                        remainingTasks--;
                        if(currentTask <= totalTasks) {
                            newTask(&slave[i], tasks);
                        }   
                    }
                }
            }
        }
    }   
}

char *  createShm(){
    int fd;
    char * ptr;

    fd  = shm_open(SHM_NAME, O_CREAT | O_RDWR, 00700);
    if (-1 == fd)
        error("shm_open failed");

    if(-1 == ftruncate(fd, SHM_SIZE)){
        error("ftruncate failed");
    };

    ptr = mmap(NULL, 50, PROT_WRITE, MAP_SHARED, fd, 0);

    if(ptr == MAP_FAILED){
        error("Map failed");
    }

    return ptr;   
}   

char* writeShm(char *ptr){
    
    int fd;
    char buff[50];

    fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (-1 == fd)
       error("shm_open failed");
    
   
    fgets(buff, sizeof(buff), stdin);
    size_t strLength = strlen(buff);
    memcpy(ptr, buff, strLength);

    ptr+=strLength*sizeof(size_t);

    close(fd);
    if(sem_post(semaphore) < 0 ){
        error("Semaphore Master Error");
    }
    return ptr;
}

int writeResult(FILE* results, slaveinfo slave){
    char readBuff[BUF_SIZE] ={0};
    int charRead;
    charRead = read(slave.fdAnswersRead, readBuff, BUF_SIZE);
    fputs(readBuff, results);
    fflush(results);
    //printf("charRead: %d", charRead);
    printf("%s", readBuff);
    
    return charRead;
}

void newTask(slaveinfo* slave, char* tasks[]) {
    char* file = tasks[currentTask++];
    printf("%s\n", tasks[currentTask - 1]);
    printf("fdTaskWrite: %d\n", slave->fdTasksWrite);
    if(write(slave->fdTasksWrite, file, sizeof(file) + 1) < 0){
        error("Error al escribir en el pipe");
    }
    printf("AAAAAAA\n");
    char readBuff[BUF_SIZE] ={0};
    read(slave->fdTasksWrite, readBuff, BUF_SIZE);
    printf("%s\n", readBuff);
    slave->tasks = 1;
}