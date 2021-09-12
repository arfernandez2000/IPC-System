// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "errors.h"

#define BUFFER_SIZE 4096

void solver(char* pathName);
void waitTasks();

// ./slave pipename filepath
int main(int argc, char * argv[]) {

    if( setvbuf(stdout, NULL, _IONBF, 0) ){
        error("Setvbuf failed");
    } 
    for (int i = 0; i < argc; i++) {
        solver(argv[i]);
    }
    
    waitTasks();
    return 0;

}

void solver(char* pathName) {
    char *filePath = pathName;
    
    char * parser = "grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"";
    char command[BUFFER_SIZE];
    char resultSolver[BUFFER_SIZE];
    char masterResult[BUFFER_SIZE*2];
    
    if(sprintf(command,"minisat %s | %s",filePath,parser) < 0){
        error("Command build failed : sprintf error");
    }
    FILE* fdFile = popen(command,"r");
    if(fdFile == NULL){
        error("popen failed");
    }
    fread(resultSolver,sizeof(char),BUFFER_SIZE,fdFile);
    if(ferror(fdFile)){
        error("fread failed");
    }
    if(pclose(fdFile) < 0){
        error("Closing file descriptor failed");
    };
    if(sprintf(masterResult,"PID: %d \nFilename: %s\n%s\n",getpid(),filePath,resultSolver) < 0){
        error("Master response build failed");
    }
    
    printf("%s", masterResult);
}

void waitTasks(){
    
    char task[BUFFER_SIZE + 1] = {0}; 
    int count = 0;
    
    while ( ( count = read(fileno(stdin), task, BUFFER_SIZE) ) != 0){

        if(count == ERROR){
            error("Read failed");
        }
        task[count] = 0;
        solver(task);
    }
}