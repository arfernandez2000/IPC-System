#ifndef MASTER_H
#define MASTER_H


typedef struct slaveinfo {

    int fdAnswersRead;
    int fdTasksWrite;
    char isBeingUsed;

} slaveinfo;

void createSlaves(int fileCount, int initTasks, const char* files[]);
void assignTasks();
void createShm();
void writeShm();
int assignProcesses(int fileCount);


#endif