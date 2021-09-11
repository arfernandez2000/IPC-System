#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

void error(char * message){
    perror(message);
    exit(EXIT_FAILURE);
}