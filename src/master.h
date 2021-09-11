#ifndef MASTER_H
#define MASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include "shm.h"
#include "errors.h"
#include "semIPC.h"


typedef struct slaveinfo {

    int fdAnswersRead;
    int fdTasksWrite;
    char status;
    int tasks;

} slaveinfo;

int createSlaves(int fileCount, int initTasks, char* files[], slaveinfo* slave);
void assignTasks(slaveinfo* slave, int slaveCount, int remainingTasks, FILE* results, char* tasks[]);
void writeResult(FILE* results, slaveinfo slave);
void newTask(slaveinfo slave, char* tasks[], int* remainingTasks);
char* createShm();
char * writeShm(char * ptr_write);
int assignProcesses(int fileCount);


#endif