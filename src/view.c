#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "errors.h"
#include "shm.h"

int main(int argc, char const *argv[]) {
    int fd;
    char *ptr;
    struct stat shm_st;
    int fileCount;
    
    if(argc ==2){
        fileCount = atoi(argv[1]);
    }
    else if( argc == 1){
        char buffer[BUF_SIZE];
        read(STDIN_FILENO, buffer, BUF_SIZE);
        fileCount = atoi(buffer); 
    }
    else{
        error("Wrong number  of arguments");
    }

    fd = shm_open (SHM_NAME,  O_RDONLY  , 00400); /* open s.m object*/
    if(fd == -1)
    {
        perror("shm_open failed");
    }
    
    if(fstat(fd, &shm_st) == -1)
    {
        perror("fstat failed");
    }

    ptr = mmap(NULL, shm_st.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if(ptr == MAP_FAILED)
    {
        perror("Map failed");
    }

    int i=0;
    while (i < fileCount){
        //semaforo para ver que  no este escribiendo
        sem_wait(shm);
        int size = printf("%s \n", ptr);
        ptr += size+1;
        i++;
        //habilita el semaforo para escribir
    }

    close(fd);
    
    return 0;
}