#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#define BACKSIZE 50

// the array which is going to hold background processes id's.
pid_t backPr[BACKSIZE] = {0};

// pointers to keep the commands in a link list
Commands *head = NULL;
Commands *tail = NULL;

char path[120];
int check;
char oldpath[120];
int comNumber = 10; // the default command number for hist 


void builtin_cd (char *direc) {

	if (direc == NULL ) {
		builtin_dir(0);
		check = chdir (getenv("HOME"));
	} else if (direc[0]=='~' && direc[1] == '\0')  {
		builtin_dir(0);
		check = chdir (getenv("HOME"));
	} else if (direc[0]=='~' && direc[1] == '/') {

		direc = direc + 2*sizeof(char);
		builtin_dir(0);
		check = chdir (getenv("HOME"));
		check = chdir (direc);
	} else if (direc [0] == '-') {
		check = chdir (oldpath);
	} else {
		builtin_dir(0);
		check = chdir (direc);
	}
	

	if (check == -1) {
		fprintf(stderr, "cd: %s: No such file or directory \n", direc);
	}

}
	


void builtin_dir(int mode) {
	
	if (mode == 1) {
		getcwd (path, 120);
		fprintf(stderr, "%s\n", path);
	} else {
		getcwd (oldpath, 120);
	}
}

void builtin_clr() {
 	system("clear");
 }

void addNode (char **arg, Commands **head) {

    // önceki vagonla bi öncekini birleştir, aynı zamanda yeni koyduğun vagonun içini doldur
    // create new vagon, yeni args
    Commands *newCommand = (Commands *)malloc(sizeof(Commands));

    if (newCommand == NULL) {
 		fprintf(stderr, "malloc failed\n" );
 		
 	}
    // yeni oluşan args'ın size'ını bulalım
    int size = 0;
    while (arg[size] != NULL) {
    	size++;
    }

    newCommand->args = (char **)malloc(sizeof(char*)*(size +1));
    newCommand->args[size] = NULL;
    int i;
    for(i = 0; i<size; i++) {
    	newCommand->args[i] = (char *)malloc((strlen(arg[i])+1)*sizeof(char));
    	strcpy(newCommand->args[i], arg[i]);
    } 

    if(*head == NULL){
        *head = newCommand;
        //printf("added at beginning\n");
    }

    else
    {
        //else loop through the list and find the last
        //node, insert next to it
        Commands *current = *head;
        while (1) { 
            if(current->nextptr == NULL)
            {
                current->nextptr = newCommand;
                newCommand->preptr = current;
               // printf("added later\n");
                break; 
            }
            current = current->nextptr;
        }
    }

    tail = newCommand;
}



void keepCom(char **args) {
	
	addNode(args,&head);

}

void retrieveCom () {
	int i;
	//int j = 0;
	int k = 0;
	Commands *tcopy = tail;

	for (i = 0; i<comNumber; i++) {

		if (tcopy != NULL) {	
			int j = 0;
			while (tcopy->args[j] != NULL) {
				fprintf(stderr, "%s ", tcopy->args[j]);
				j++;
			}

			tcopy = tcopy->preptr;
			fprintf(stderr, "\n" );

		} else {
			break;
		}
	}

}


// retrieve the comments if argument after ! is integer
Commands* retComInt(int n) { 

	if (n > comNumber) {
		fprintf(stderr, "I keep your last %d commands. Please enter a number smaller than that\n", comNumber );
		return NULL;
	} else {
		Commands *tcopy = tail;
		int i;
		for (i = 0; i<n && tcopy->preptr != NULL ; i++) {
			tcopy = tcopy->preptr;
		}

		return tcopy;
	}
}

// retrieve the comments if argument after ! is string
Commands* retComSt(char *string) { 

	Commands *tcopy = tail;

	int i,j;
	for (i = 0; i < comNumber && tcopy->preptr != NULL; i++) {
		int check = strncmp(string, tcopy->args[0] , strlen(string));
		if (check == 0) {
			return tcopy;
		}
	}

	return NULL ;
}




void builtin_hist (char **args) {

	if (args[1] != NULL && (strcmp(args[1], "-set") == 0)) {
		if (args[2] != NULL) {
			int num = atoi(args[2]);
			if (num == 0) {
				fprintf(stderr, "Please enter a value grater than 0\n");
			} else {
				comNumber = num;
			}
			
		}
	} else if (args[1] == NULL){
		retrieveCom(); // get the commands
	}
}



void builtin_exit() {

	int i, answer, exist;
	int rval;
	int control=1;

	for (i = 0; i<BACKSIZE; i++) {
		if (backPr [i] != 0) {
			fprintf(stderr, "Waiting for process: %d to terminate.\n",backPr[i]);
			waitpid(backPr[i], &rval, 0);
			backPr[i] = 0;
		}
	}
	exit(0);

}

void builtin_wait() {

	int i,k;

	for (i = 0; i<BACKSIZE; i++) { // scan the array from the beginnig
		if (backPr[i] != 0) {
			fprintf(stderr, "Waiting for the process %d ..", backPr[i]);
			waitpid(backPr[i], &k, 0);
			backPr[i] = 0;
			fprintf(stderr, "Closed . \n" );
		}
	}

}

void addBackground(pid_t pid){		//adding new background process node
	

	int i;
	for (i = 0; i<BACKSIZE; i++) {
		if (backPr [i] == 0) {
			backPr[i] = pid;
			break; 
		}
	}

}


