#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define ERROR 1


int main(int argc, char const *argv[]){
    if(argc < 2) {
        fprintf(stderr, "Incorrect number of arguments: %s/n", argv[0]);
        exit(ERROR);
    }
    printf("algo");
    setvbuf(stdout,NULL,_IONBF,0); 
   
}