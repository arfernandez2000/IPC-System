// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _XOPEN_SOURCE 500
#include "view.h"

int main(int argc, char const *argv[]) {
    int fd;
    char *ptr_read;
    struct stat shm_st;
    int fileCount;

    sem_t * semaphore = sem_open(SEM,O_CREAT,SEM_FLAGS);

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