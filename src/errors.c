#include "errors.h"

void error(char * message){
    perror(message);
    exit(EXIT_FAILURE);
}