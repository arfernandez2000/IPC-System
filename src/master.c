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

#define SLAVES 7
#define DIRECTION_SENSE 2
#define RW_END 2

#define SLAVE_PATH "./slave"
#define P_NAME "/tmp/PRUEBITA"


int createSlaves();
void createShm();
void writeShm();

int main(int argc, char const *argv[]){
    
    sleep(2);
    
    // int pipes[SLAVES][DIRECTION_SENSE][RW_END] = createPipes(argc-1);

    // write(pipes[6][0][1], "hola mundo", 10);

    if(argc < 2){
        error("Cantidad incorrecta de argumentos");
    }

    if(mkfifo(P_NAME,0666) < 0){
        error("Error al crear pipe");
    }

    

    char * argSlave[] = {P_NAME, argv[1]};

    execv(SLAVE_PATH, argSlave);

    char buff[4096];
    int fdPipe = open(P_NAME, O_RDONLY);

    fd_set fds;
    
    FD_ZERO(&fds);
    FD_SET(fdPipe, &fds);

    select(fdPipe, &fds, NULL, NULL, NULL);

    int res;
    if(FD_ISSET(fdPipe, &fds)){
        res = read(fdPipe, buff, sizeof(buff));  
        if (res > 0) {
            error("Error al leer pipe");
        }
        
    } 

    close(fdPipe);

    printf("%s", buff);

    // createShm();

    // //cuando reciva la info del slave
    // for(int i = 0; i < 10; i++)
    //     writeShm();

    // setvbuf(stdout,NULL,_IONBF,0); 
   
}
// int *** createPipes(int fileCount){
//     int pipes[SLAVES][DIRECTION_SENSE][RW_END];

//     int slavesToUse = fileCount>SLAVES ? SLAVES: fileCount;

//     for(int i=0; i< slavesToUse; i++){
//             for(int j=0; j<DIRECTION_SENSE; j++){
//                 if(pipe(pipes[i][j]) < 0){
//                     error("Pipe Creation Error");
//                 }
//             }
//     }
//     return pipes;
// }
// void closePipes(int fileCount, int *** pipes){
//     for(int i=0 ; i< fileCount; i++){
//         for(int j=0; j<DIRECTION_SENSE; j++){
//             close(pipes[i][j][0]);
//             close(pipes[i][j][1]);
//         }
       
//     }

// }

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
        error("shm_open failed");
    

    if(-1 == ftruncate(fd, SHM_SIZE)){
        error("ftruncate failed");
    };
        
}

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