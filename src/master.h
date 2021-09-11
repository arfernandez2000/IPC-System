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
char * writeShm(int offset);
int assignProcesses(int fileCount);


#endif