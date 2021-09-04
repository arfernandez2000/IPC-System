#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int main(int argc, char const *argv[]){

    int numberOfFiles=0;

    setvbuf(stdout,NULL,_IONBF,0); 
    if(argc > 2 ){
        perror("Wrong number of arguments");
        exit(EXIT_FAILURE);
    }else{
      numberOfFiles = atoi(argv[1]);
    }
}