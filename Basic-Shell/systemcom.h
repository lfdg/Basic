#include <stdbool.h>
#include <sys/types.h>
#include <string.h>

#ifndef H_SYSTEM
#define H_SYSTEM 0


int checkInputHasPath(char *args[], int background);

int createProcessWithoutPath(char *args[], pid_t *childpid);

int createProcessWithPath(char *args[], pid_t *childpid);

#endif