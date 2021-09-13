// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

void error(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}