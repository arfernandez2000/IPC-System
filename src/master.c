// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _XOPEN_SOURCE 500 //ftruncate warning
#include "master.h"
#define READ 0
#define WRITE 1
#define STDOUT 1
#define STDIN 0
#define USED 1
#define UNUSED 0

#define SLAVES 3
#define SLAVE_PATH "./slave"

// sem_t semaphore;
int currentTask = 1;
int totalTasks;
char *ptr_write;
sem_t * semaphore;

int main(int argc, char const *argv[]){
    if(argc < 2){
        error("Cantidad incorrecta de argumentos");
    }
    if( setvbuf(stdout, NULL, _IONBF, 0) ){
        error("Setvbuf failed");
    }
    
    ptr_write = createShm(argc-1);   

    semaphore = sem_open(SEM,O_CREAT,SEM_FLAGS,0);

    
    int initialTasks =  SLAVES * 2 >= argc ? 1 : 2; 

    FILE * results;
    results = fopen("results.txt", "w");
    if(results == NULL){
        error("No se pudo crear el archivo results");
    }

    int slaveCount = (argc - 1 > SLAVES)? SLAVES : argc - 1;
    slaveinfo slave[slaveCount];
    int remainingTasks = argc - 1;
    totalTasks = argc - 1;
    printf("%d",argc-1);

    sleep(2);
    
    createSlaves(slaveCount, initialTasks, (char**) argv, slave);
    assignTasks(slave, slaveCount, remainingTasks, results, (char**) argv);

    if(fclose(results) < 0){
        error("Error al cerrar results.txt");
    }

     if( sem_close(semaphore) < 0)
         error("Semaphore close failed");

    closeSlaves(slave, slaveCount);  
    closeShm();
    return 0;
}

void createSlaves(int slaveCount, int initialTasks, char* files[], slaveinfo* slave) {


    // FILE* fdPrueba;
    // fdPrueba = fopen("log.txt", "w+") ;

    int tasks[2];
    int answers[2];

    char* filesToSend[BUF_SIZE] = {0};
    int counter = 1;
    // char readBuf[10000] = {0};    
    //for por cada slave -> cortarlo si me quedo sin archivos antes de llenar los 7 slaves
    
    for (int i = 0; i < slaveCount; i++) {       
        if(pipe(tasks) < 0) {
            error("Error al crear pipe task");
        }

        if(pipe(answers) < 0) {
            error("Error al crear pipe answer");
        }
        int pid;
    
        if ((pid = fork()) == 0) {
            
            for (int j = 0; j < initialTasks; j++) {
                filesToSend[j] = files[counter++];
            }

            if ( dup2(answers[WRITE], STDOUT) < 0) {
                error("Error al hacer el dup2 de STDOUT");
            }

            if ( dup2(tasks[READ], STDIN) < 0) {
                error("Error al hacer el dup2 de STDIN");
            }
            
            if (close(tasks[READ]) < 0) {
                error("Error al cerras el fd de tasks, READ");
            }
            if (close(tasks[WRITE]) < 0) {
                error("Error al cerras el fd de tasks, WRITE");
            }
            if (close(answers[READ]) < 0) {
                error("Error al cerras el fd de answers, READ");
            }
            if (close(answers[WRITE]) < 0) {
                error("Error al cerras el fd de answers, WRITE");
            }
        
            if (execv(SLAVE_PATH, filesToSend) < 0) {
                error("Error no funciona execv");
            }
        } else if (pid < 0) {
            error("Error del fork");
        }
        
        slave[i].fdAnswersRead = answers[READ];
        slave[i].fdTasksWrite = tasks[WRITE];
        slave[i].status = USED; 
        slave[i].tasks = initialTasks;

        if(close(tasks[READ]) < 0 || close(answers[WRITE]) < 0){
            error("Error al cerrar pipes");
        }
        counter += initialTasks;
    }
    
    //TODO-funcion para el select

    // sleep(2);
    
    // for (int i = 0; i < slaveCount; i++) {
    //     if (read(slave[i].fdAnswersRead, readBuf, 10000) < 0) {
    //         error("Error al leer el answers, READ");
    //     }
    //     fprintf(fdPrueba, "%s\n", readBuf);
    //     close(slave[i].fdAnswersRead);
    // }
	
    // fclose(fdPrueba);
    currentTask += slaveCount*initialTasks;
}

void closeSlaves(slaveinfo* slave, int slaveCount){

    for (int i = 0; i < slaveCount; i++){
        close(slave[i].fdAnswersRead);
        close(slave[i].fdTasksWrite);
    }
    
}

void closeWriteSlaves(slaveinfo* slave, int slaveCount){

    for (int i = 0; i < slaveCount; i++){
        close(slave[i].fdTasksWrite);
    }
    
}

void assignTasks(slaveinfo* slave, int slaveCount, int remainingTasks, FILE* results, char* tasks[]){
    while (remainingTasks > 0) {
        fd_set fdSet;
        FD_ZERO(&fdSet);
        FD_SET(0, &fdSet);

        int fd;
        int maxFd = -1;
        for(int i = 0; i < slaveCount; i++) {
            if( slave[i].status ){
                fd = slave[i].fdAnswersRead;
                FD_SET(fd, &fdSet);
                maxFd = (maxFd > fd)? maxFd : fd;              
            }
        }
        
        int ready = select(maxFd + 1, &fdSet, NULL, NULL, NULL);
        if(ready < 0){
            error("Error al crear el select");
        }

        for(int i = 0; i < slaveCount; i++){
            if(FD_ISSET(slave[i].fdAnswersRead, &fdSet)) {
                if (writeResult(results, slave[i]) > 0) {
                    if(slave[i].tasks > 1){
                        remainingTasks--;
                    }
                    slave[i].tasks--;
                    if(slave[i].tasks == 0 && remainingTasks != 0){
                        remainingTasks--;
                        if(currentTask <= totalTasks) {
                            newTask(&slave[i], tasks);
                        }   
                    }
                }
            }
        }
    }   
}

char *  createShm(int tasks){

    int fd;
    char * ptr;
 

    fd  = shm_open(SHM_RESULTS, O_CREAT | O_RDWR, 00600);
  
    if (-1 == fd)
        error("shm_open failed");

    if(-1 == ftruncate(fd, SHM_SIZE * tasks)){
        error("ftruncate failed");
    }
    ptr = mmap(NULL,SHM_SIZE * tasks, PROT_WRITE, MAP_SHARED, fd, 0);

    if(ptr == MAP_FAILED){
        error("Map failed");
    }

    return ptr;   
}  
void closeShm(){
    shm_unlink(SHM_RESULTS);
}

void writeShm(char * results){
    
    int fd;
    

    fd = shm_open(SHM_RESULTS, O_RDWR, 0);
    if (-1 == fd)
       error("shm_open failed");
    

    size_t strLength = strlen(results)+1;

    memcpy(ptr_write, results, strLength);

    ptr_write+=SHM_SIZE;

    close(fd);
    return;
}

int writeResult(FILE* results, slaveinfo slave){
    char readBuff[BUF_SIZE] ={0};
    int charRead;
    charRead = read(slave.fdAnswersRead, readBuff, BUF_SIZE);
    fputs(readBuff, results);
    fflush(results);

    // sem_wait(semaphore);

    writeShm(readBuff);
    // sem_wait(semaphore);

    sem_post(semaphore);
    return charRead;
}

void newTask(slaveinfo* slave, char* tasks[]) {
    char* file = tasks[currentTask++];

    if(write(slave->fdTasksWrite, file, sizeof(file) + 1) < 0){
        error("Error al escribir en el pipe");
    }

    char readBuff[4096] ={0};
    read(slave->fdTasksWrite, readBuff, 4096);
    printf("%s\n", readBuff);
    slave->tasks = 1;
}