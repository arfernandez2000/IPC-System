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

void createSlaves(int slaveCount, int initialTasks, char* files[], slaveinfo* slave);
void closeSlaves(slaveinfo* slave, int slaveCount);
void closeWriteSlaves(slaveinfo* slave, int slaveCount);
void assignTasks(slaveinfo* slave, int slaveCount, int remainingTasks, FILE* results, char* tasks[], sem_t* semaphore);
int writeResult(FILE* results, slaveinfo slave, sem_t * semaphore);
void newTask(slaveinfo* slave, char* tasks[]);
char* createShm(int tasks);
void writeShm( char * results);
int assignProcesses(int fileCount);
void closeShm();


#endif