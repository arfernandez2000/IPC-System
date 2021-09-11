#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/time.h>
#include <string.h>
#include "shm.h"
#include "errors.h"
#include "master.h"


#define READ 0
#define WRITE 1
#define STDOUT 1
#define STDIN 0
#define USED 1
#define UNUSED 0

#define SLAVES 2
#define SLAVE_PATH "./slave"

sem_t* semaphore;

int main(int argc, char const *argv[]){
    if(argc < 2){
        error("Cantidad incorrecta de argumentos");
    }
    
    sleep(2);
    
    char * ptr_write;
    createShm();
    semaphore = sem_open("/semaphore",O_CREAT,(S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH));

    int initialTasks =  SLAVES * 2 >= argc ? 1 : 2; 

    FILE * results;
    results = fopen("results.txt", "w");
    if(results == NULL){
        error("No se pudo crear el archivo results");
    }

    slaveinfo slave;
    int slaveCount = (argc - 1 > SLAVES)? SLAVES : argc - 1;
    int remainingTaks = argc - 1;

    int created = createSlaves(slaveCount, initialTasks, (char**) argv, &slave);
    if(created == 1){
        remainingTaks -= slaveCount*initialTasks;
    }


    //assignTasks(&slave, slaveCount, remainingTaks, results, (char**) argv + 1);
   
    //cuando reciva la info del slave
    // for(int i = 0; i < 10; i++)
         
    ptr_write = writeShm(0);
   
    // setvbuf(stdout,NULL,_IONBF,0);
    fclose(results);
    if( sem_close(semaphore) < 0)
        error("Semaphore close failed");

    return 0;
}

int createSlaves(int slaveCount, int initialTasks, char* files[], slaveinfo* slave) {


    FILE* fdPrueba;
    fdPrueba = fopen("log.txt", "w+") ;

    int tasks[2];
    int answers[2];

    char* filesToSend[BUF_SIZE] = {0};
    int counter = 1;
    char readBuf[4900] = {0};    

    printf("initial Tasks: %d\n", initialTasks);
    printf("slaves count: %d\n", slaveCount);

    //for por cada slave -> cortarlo si me quedo sin archivos antes de llenar los 7 slaves
    
    for (int i = 0; i < slaveCount; i++) {  
        printf("esclavo: %d\t", i + 1);      
        if(pipe(tasks) < 0) {
            error("Error al crear pipe task");
        }

        if(pipe(answers) < 0) {
            error("Error al crear pipe answer");
        }

        int pid;
    
        if ((pid = fork()) == 0) {

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
        
            for (int j = 0; j < initialTasks; j++) {
                printf("file: %s\n", files[counter]);
                filesToSend[j] = files[counter++];
            }

            if (execv(SLAVE_PATH, filesToSend) < 0) {
                error("Error no funciona execv");
            }
        } else if (pid < 0) {
            error("Error del fork");
        }
        
        slave[i].fdAnswersRead = answers[READ];
        slave[i].fdTasksWrite = tasks[READ];
        slave[i].status = USED; 
        slave[i].tasks = initialTasks;

        printf("fdAnswersRead: %d, fdTasksWrite: %d, tasks: %d\n", slave[i].fdAnswersRead, slave[i].fdTasksWrite, slave[i].tasks);
        if(close(tasks[READ]) < 0 || close(answers[WRITE]) < 0){
            error("Error al cerrar pipes");
        }
    }
    //TODO-funcion para el select
    sleep(2);
    if (read(answers[READ], readBuf, 4900) < -1) {
        error("Error al leer el answers, READ");
    }

	fputs(readBuf,fdPrueba);
    // fputs("buenas\n",fdPrueba);
    fclose(fdPrueba);
    return 1;
}

void assignTasks(slaveinfo* slave, int slaveCount, int remainingTasks, FILE* results, char* tasks[]){

    while (remainingTasks >= 0) {
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
        int ready = select(maxFd, &fdSet, NULL, NULL, NULL);
        if(ready < 0){
            error("Error al crear el select");
        }

        for(int i = 0; ready > 0 && i < slaveCount; i++){
            if(FD_ISSET(slave[i].fdAnswersRead, &fdSet)) {
                writeResult(results, slave[i]);
                if(slave[i].tasks == 0 && remainingTasks != 0){
                    newTask(slave[i], tasks, &remainingTasks);
                }
                ready--;
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

char* writeShm(int offset){
    
    int fd;
    char *ptr;
    char buff[50];

    fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (-1 == fd)
       error("shm_open failed");
    
   
    fgets(buff, sizeof(buff), stdin);
    size_t strLength = strlen(buff);
    memcpy(ptr,buff, strLength);

    ptr+=strLength*sizeof(size_t);

    close(fd);
    if(sem_post(semaphore) < 0 ){
        error("Semaphore Master Error");
    }
    return ptr;
}

void writeResult(FILE* results, slaveinfo slave){
    char readBuff[BUF_SIZE] ={0};
    read(slave.fdAnswersRead, readBuff, BUF_SIZE);
    fprintf(results, "%s\n", readBuff);
}

void newTask(slaveinfo slave, char* tasks[], int* remainingTasks) {
    char* file = tasks[sizeof(tasks[0]) - *remainingTasks];
    if(write(slave.fdTasksWrite, file, BUF_SIZE) < 0){
        error("Error al escribir en el pipe");
    }
    *remainingTasks -= 1;

}