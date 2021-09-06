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

#define ERROR 1

int createSlaves();
void createShm();
void writeShm();

int main(int argc, char const *argv[]){
    // if(argc < 2) {
    //     fprintf(stderr, "Incorrect number of arguments: %s/n", argv[0]);
    //     exit(ERROR);
    // }

    createShm();

    //cuando reciva la info del slave
    for(int i = 0; i < 10; i++)
        writeShm();

    setvbuf(stdout,NULL,_IONBF,0); 
   
}

void createShm(){

    // *semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    // if (semid == -1)
    //     errExit("semget");

    // if (initSemAvailable(semid, WRITE_SEM) == -1)
    //     errExit("initSemAvailable");
    // if (initSemInUse(semid, READ_SEM) == -1)
    //     errExit("initSemInUse");
    int fd;

    fd  = shm_open(SHM_NAME, O_CREAT | O_RDWR, 00700);
    if (-1 == fd)
        perror("shm_open failed");
    

    if(-1 == ftruncate(fd, SHM_SIZE)){
        perror("ftruncate failed");
    };
        
}

/* Transfer blocks of data from stdin to shared memory */

void writeShm(){
    
    int fd;
    char *ptr;
    char buff[50];

    fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (-1 == fd)
       perror("shm_open failed");
    
    ptr = mmap(NULL, 50, PROT_WRITE, MAP_SHARED, fd, 0);

    if(ptr == MAP_FAILED){
        perror("Map failed");
    }

    fgets(buff, sizeof(buff), stdin);

    memcpy(ptr,buff, sizeof(buff));

    close(fd);

    // for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
    //     if (reserveSem(semid, WRITE_SEM) == -1)         /* Wait for our turn */
    //         errExit("reserveSem");

    //     shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
    //     if (shmp->cnt == -1)
    //         errExit("read");

    //     if (releaseSem(semid, READ_SEM) == -1)          /* Give reader a turn */
    //         errExit("releaseSem");

    //     /* Have we reached EOF? We test this after giving the reader
    //        a turn so that it can see the 0 value in shmp->cnt. */

    //     if (shmp->cnt == 0)
    //         break;
    // }
}