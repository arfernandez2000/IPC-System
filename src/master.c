#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include "shm.h"
#include <string.h>
#include "errors.h"


#define READ 0
#define WRITE 1
#define STDOUT 1
#define STDIN 0

#define SLAVES 7
#define DIRECTION_SENSE 2
#define RW_END 2

#define SLAVE_PATH "./slave"
#define P_NAME "/tmp/mi_fifo"

static int slaves[SLAVES]={0};

void createSlaves(int fileCount, int initTasks, const char* files[]);
void assignTasks();
void createShm();
void writeShm();
int assignProcesses(int fileCount);


int main(int argc, char const *argv[]){
    
    if(argc < 2){
        error("Cantidad incorrecta de argumentos");
    }
 
    // createShm();

    // Imprime la cantidad de archivos para que el view sepa cuando parar
    // printf("%d", argc - 1);

    sleep(2);

    int initTasks = assignProcesses(argc);
    
    FILE * results;
    results = fopen("results.txt", "w");
    if(results == NULL){
        error("No se pudo crear el archivo results");
    }
    
    createSlaves(argc - 1, initTasks, argv);

    //assignTasks();
   
    //cuando reciva la info del slave
    // for(int i = 0; i < 10; i++)
    //     writeShm();

    // setvbuf(stdout,NULL,_IONBF,0);
    return 0;
}

int assignProcesses(int fileCount) {
    if(SLAVES * 2 >= fileCount) {
        return 1;  
    }
    else {
        return 2;
    }    
}

void createShm(){

    int fd;

    fd  = shm_open(SHM_NAME, O_CREAT | O_RDWR, 00700);
    if (-1 == fd)
        error("shm_open failed");
    

    if(-1 == ftruncate(fd, SHM_SIZE)){
        error("ftruncate failed");
    };

        
}

void createSlaves(int fileCount, int initTasks, const char* files[]) {

    printf("entre");

    FILE* fdPrueba;
    fdPrueba = fopen("log.txt", "w+") ;

    int tasks[2];
    int answers[2];

    int counter = 0;
    int slaves = (fileCount > SLAVES)? SLAVES : fileCount;
    printf(slaves);

    const char** filesToSend = {0};

    //for por cada slave -> cortarlo si me quedo sin archivos antes de llenar los 7 slaves
    
    for (int i = 0; i < slaves; i++) {
        printf("for num: %d", i);
        
        if(pipe(tasks) < 0){
            error("Error al crear pipe task");
        }

        if(pipe(answers) < 0){
            error("Error al crear pipe answer");
        }
    
        if (fork() == 0) {

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
        
            
            for (int j = 0; j < initTasks; j++) {
                filesToSend[j] = files[counter++];
            }
        
            if (execv(SLAVE_PATH, filesToSend) < 0) {
                error("Error no funciona execv");
            }
        }
    }

    char readBuf[4900] = {0};
    sleep(2);
    if (read(answers[READ], readBuf, 4096) < -1) {
        error("Error al leer el answers, READ");
    }

	fputs(readBuf,fdPrueba);
    fclose(fdPrueba);

}

// void assignTasks(){
//     fd_set fd;
//     int fdSlave;
//     FD_ZERO(&fd);
//     FD_SET(0, &fd);
//     int ready = select(fdSlave, &fd, NULL. NULL, NULL);
    
// }

/* Transfer blocks of data from stdin to shared memory */

void writeShm(){
    
    int fd;
    char *ptr;
    char buff[50];

    fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (-1 == fd)
       error("shm_open failed");
    
    ptr = mmap(NULL, 50, PROT_WRITE, MAP_SHARED, fd, 0);

    if(ptr == MAP_FAILED){
        error("Map failed");
    }

    fgets(buff, sizeof(buff), stdin);

    memcpy(ptr,buff, sizeof(buff));

    close(fd);
}