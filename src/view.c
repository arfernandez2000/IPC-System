#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "shm.h"

int main(int argc, char const *argv[]) {
    sleep(10);
    int fd;
    char *ptr;
    struct stat shm_st;
    
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
    
    printf("%s \n", ptr);

    close(fd);
    
    return 0;
}