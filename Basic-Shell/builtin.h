#include <stdbool.h>
#include <sys/types.h>
#include <string.h>

#ifndef H_BUILT
#define H_BUILT 0

typedef struct  commands {
	char **args;
	int flag;
	struct commands *nextptr;
	struct commands *preptr;
} Commands ;

void builtin_cd (char *direc);

void builtin_dir(int mode);

void builtin_clr();

void addNode (char **arg, Commands **head);

void keepCom(char **args);

void retrieveCom ();

void builtin_hist (char **args);

Commands* retComInt(int n);

Commands* retComSt(char *string) ;

char** execComSt (Commands *com, char **arg);

void addBackground(pid_t pid);

void builtin_exit();

void builtin_wait();

#endif