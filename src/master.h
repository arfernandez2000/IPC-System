#ifndef MASTER_H
#define MASTER_H


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
char * writeShm(int offset);
int assignProcesses(int fileCount);


#endif