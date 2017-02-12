#include "systemcom.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>


/*if the user wants to see on the another path*/
int createProcessWithPath (char *args[], pid_t *childpid){

	execl(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10]);
	perror("Child failed to execl");
	return 1; 

}


/* if the user wants to see on current working directory */
int createProcessWithoutPath (char *args[], pid_t *childpid){

	execvp(*args, &args[0]);
	return 1;
}

int checkInputHasPath (char *args[], int background){
	pid_t childpid;
	char isPath = '/';
	childpid = fork();
	int i;

	if (childpid == -1)  {
		perror("Failed to fork");
		return 1; 
	}

	// Here, We control input (args[0]) for seperating execl-execvp. 

	if (childpid == 0 ) { 
		// child code 
		if(args[0][0]==isPath ) { //If input has path, its first character has '/'.
		    createProcessWithPath(args, &childpid); 
		}else { // if input has no path 
		    createProcessWithoutPath(args, &childpid);
		}

		fprintf(stderr,"bash: ");    
	    for (i = 0; args[i] != NULL; i++){
	   		fprintf(stderr,"%s: " , args[i]);
	    }

	    fprintf(stderr,"command not found...\n");

		return 1;

	}else if (background != 1 ){
		wait(NULL);
	}

	if ( background ) {	//checking background
		addBackground(childpid);
		fprintf(stderr, "\n" );
	}
 
}
