#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include "builtin.h"

#ifndef H_PIPE
#define H_PIPE 0

Commands* retrievePipeCommands();

void splitArguments(char ** args);

void addPipe(char **args, Commands** head);

void multiPipe();

#endif