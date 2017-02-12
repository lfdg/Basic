#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "builtin.h"
#include "systemcom.h"
#include "pipe.h"
#include <stdbool.h>

 
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */


void methods (char **args,int background, int var) { 

    

    if (strcmp (args[0], "cd") == 0) {
        builtin_cd(args[1]);

    } else if (strcmp (args[0], "dir") == 0) {
       builtin_dir(1);
    } else if (strcmp (args[0], "clr") == 0) {
        builtin_clr();
    } else if (strcmp (args[0], "hist") == 0) {
        builtin_hist(args);
    }  else if (strcmp (args[0], "exit") == 0) {
        builtin_exit();
    } else if (strcmp (args[0], "!") == 0) {
        int check = atoi(args[1]);
        Commands *com = NULL;
        if (check == 0) {
            com = retComSt(args[1]);
        } else {
            com = retComInt(check);
        }
        
        methods(com->args, background, var);
    } else if (strcmp (args[0], "wait") == 0) {
        builtin_wait();
    } else if (var == 1) {
        splitArguments(args);

    } else   {             
        checkInputHasPath(args, background); 
    }

    keepCom(args);
    

}

void searcAmp(char **args) { // if we see the "&" value, we will handle other args what later
    bool key = false;
    int i;
    for(i = 0; args[i] != NULL; i++){
        if(strcmp(args[i],"&") == 0 || key == true){
            args[i] = NULL; 
            key = true; 
        }   
    }
}

/* The setup function below will not return any value, but it will just: read
in the next command line; separate it into distinct arguments (using blanks as
delimiters), and set the args array entries to point to the beginning of what
will become null-terminated, C-style strings. */

void setup(char inputBuffer[], char *args[],int *background,int *var) {
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */

    
    ct = 0;
        
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);  

    /* 0 is the system predefined file descriptor for stdin (standard input),
       which is the user's screen in this case. inputBuffer by itself is the
       same as &inputBuffer[0], i.e. the starting address of where to store
       the command that is read, and length holds the number of characters
       read in. inputBuffer is not a null terminated C-string. */

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */

    /* the signal interrupted the read system call */
    /* if the process is in the read() system call, read returns -1
    However, if this occurs, errno is set to EINTR. We can check this  value
    and disregard the -1 value */
    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

	//printf(">>%s<<",inputBuffer);
    for (i=0;i<length;i++){ /* examine every character in the inputBuffer */

        switch (inputBuffer[i]){
    	    case ' ':
    	    case '\t' :               /* argument separators */
    		  if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
    		  }

              inputBuffer[i] = '\0'; /* add a null char; make a C string */
    		  start = -1;
    		  break;

            case '\n':                 /* should be the final char examined */
        		if (start != -1){
                    args[ct] = &inputBuffer[start];     
        		    ct++;
        		}

                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;
            case '|':
                *var =1; //*********************************** to check if we have a "|" char in the argument array

    	    default :             /* some other character */
        		if (start == -1)
        		    start = i;

                if (inputBuffer[i] == '&'){
        		  *background  = 1;
                  inputBuffer[i-1] = '\0';
        		}

	    } /* end of switch */
    }/* end of for */
    
    args[ct] = NULL; /* just in case the input line was > 80 */

} /* end of setup routine */
 
int main(void) {
    chdir (getenv("HOME"));
    char inputBuffer[MAX_LINE]; /*buffer to hold command entered */
    int background; /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2 + 1]; /*command line arguments */
    char path[120];
    int var = 0; // to get the info if our argument has a "|" operator

    while (1){
        background = 0;
        if (strcmp(getcwd(path,120), getenv("HOME")) == 0) {
            fprintf(stderr, "CSE333sh:~$" );

        } else {
            
            fprintf(stderr, "CSE333sh:%s$ ", getcwd(path, 120));
        }
        /*setup() calls exit() when Control-D is entered */
        setup(inputBuffer, args, &background, &var);
        searcAmp(args);

        if (args[0] != NULL ){
                     
            /** the steps are:
            (1) fork a child process using fork()
            (2) the child process will invoke execvp() or execl()
    		(3)) if background == 0, the parent will wait,
            otherwise it will invoke the setup() function again. */

            methods(args, background, var);
        }
    }
}

