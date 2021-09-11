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
#include <semaphore.h>

extern sem_t* semaphore;

int main(int argc, char const *argv[]) {
    int fd;
    char *ptr_read;
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

    ptr_read = mmap(NULL, shm_st.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if(ptr_read == MAP_FAILED)
    {
        perror("Map failed");
    }

    int i=0;
    while (i < fileCount){
       
       if(sem_wait(semaphore) < 0){
        error("Semaphore View Error");
       };

       int size = printf("%s \n", ptr_read);
       ptr_read += size+1;
       i++;
    }

    close(fd);
    
    return 0;
}