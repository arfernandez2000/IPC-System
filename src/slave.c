#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "errors.h"

#define BUFFER_SIZE 4096

int main(){
    
    //recibir por un PIPE el path a un archivo desde el master
    char path[BUFFER_SIZE] = "hole6.cnf";

    // ejecutar minisat
    char * parser = "grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"";
    char command[BUFFER_SIZE];
    char resultSolver[BUFFER_SIZE];
    char masterResult[BUFFER_SIZE];
    
    if(sprintf(command,"minisat %s | %s",path,parser) < 0){
        error("Command build failed : sprintf error");
    }
    FILE* fd = popen(command,"r");
    if(fd == NULL){
        error("popen failed");
    }
    fread(resultSolver,sizeof(char),BUFFER_SIZE,fd);
    if(ferror(fd)){
        error("fread failed");
    }
    if(pclose(fd) < 0){
        error("Closing file descriptor failed");
    };
    if(sprintf(masterResult,"PID: %d \nFilename: %s\n%s\n",getpid(),path,resultSolver) < 0){
        error("Master response build failed");
    }
    printf("\n%s", masterResult);
    // comunicarle el resultado al proceso master

    return 0;
}