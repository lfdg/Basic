#include "pipe.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
Commands *pipeHead = NULL;
int count = 0;


void splitArguments(char ** arg) {
    // split each commands arguments since we will use pipe
	pipeHead = NULL;
	int i;
    int size = 0;
    count = 0;
    while (arg[size] != NULL) {
    	size++;
    }
    int k = 0;
    for (i = 0; i < size; i++) { // iterate over all arguments
    	if (strcmp(arg[i],"|")== 0 || arg[i] == NULL || i == size - 1) { // if there is a pipe, split it
    		if (k != i || i == size - 1) {
	    		addPipe(arg+k, &pipeHead); // add the command's arguments to a new argument list
	    		count++;
    		} else {
    			fprintf(stderr, "Syntax Error!!!!! \n" );
    			break;
    		}
    		k = i+1;
    	}
    }

    multiPipe();
 
}

void multiPipe() {
	
	int i;
	int rtVal;
	int f0, f1;
    int *fd = (int*) malloc(sizeof(int) * (count - 1) * 2);

	for(i = 0; i < (count - 1) * 2;  i = i + 2) {
		pipe(fd + i); // create pipes for each process
	}

	Commands *tmp = pipeHead;

	for(i = 0; i < count ; i++, tmp = tmp->nextptr) {

		pid_t childpid;
		childpid = fork(); // fork a new child

		if (childpid == -1)  {
			perror("Failed to fork");
		}


		if (childpid == 0 ) {
        // calculate the necessary pipe orders 
			f0 = 2 * (i -1);
			f1 = 1 + (2*i);
			if(i != 0) {
				dup2(fd[f0], 0); // set the current process input as last process output
			}
            if( i != count - 1) {
				dup2(fd[f1], 1); // set the current process output as next process input
				
			}

            int pNum;
                for(pNum =0 ; pNum < (count - 1) * 2; pNum++) {
                    if(pNum != f0 && pNum != f1 ) {
                        close(fd[pNum]); // close an pipe that we dont use
                    }
                }
		
			createProcessWithoutPath(tmp->args, &childpid);
		} else {
			f0 = (i -1) * 2;
			f1 = (2*i) + 1;
            // close an pipe that we dont use
			if(i != 0) {
				close(fd[f0]);
			}
            if( i != count-1) {
				close(fd[f1]);
			}
			
			// wait for current process to terminate
			waitpid(childpid, &rtVal, 0);
			
		}
		
	}

}

Commands* retrievePipeCommands() {

	Commands *pipeHeadCopy = pipeHead;
	return pipeHeadCopy;

}

// add pipe will hold the arguments for pipe
void addPipe(char **args, Commands** head) {

    // combine the trucks, and also fill the new trucks
    // create new vagon, yeni args
    Commands *newCommand = (Commands *)malloc(sizeof(Commands));

    if (newCommand == NULL) {
 		fprintf(stderr, "malloc failed\n" );
 		
 	}
    // yeni oluşan args'ın size'ını bulalım
    int size = 0;
   // fprintf(stderr, "before while \n" );
    while (args[size] != NULL && strcmp(args[size], "|") != 0) {
    	size++;
    }
    //fprintf(stderr, "after while \n" );
    newCommand->args = (char **)malloc(sizeof(char*)*(size +1));
    newCommand->args[size] = NULL;
    int i;
    for(i = 0; i<size; i++) {
    	newCommand->args[i] = (char *)malloc((strlen(args[i])+1)*sizeof(char));
    	strcpy(newCommand->args[i], args[i]);
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

    //tail = newCommand;
}