main.out: mainSetup.c builtin.c systemcom.c pipe.c	
	gcc -o main.out mainSetup.c builtin.c systemcom.c pipe.c -I.