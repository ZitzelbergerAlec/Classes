//Working with wait() and child proceses

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){
	pid_t p;
	pid_t child;
	int status;

	switch(p=fork()){
		case 0:
			//this is the child case
			printf("I am the child. My PID is %d. Parent ID = %d\n", getpid(), getppid());
			sleep(10);
			break;
		case -1:
			//this is the error case
			perror("Could not create child.\n");
			exit(-1);
		default:
			//this is the parent case
			printf("I am the parent. My PID is %d\n", getpid());
			child = wait(&status);
			printf("Waited on %d\n", child);
			break;		
	}	
	//both child and parent continue from here
	printf("My PID is %d\n", getpid());
	return(0);
}
