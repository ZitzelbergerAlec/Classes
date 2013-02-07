#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char **argv){
	pid_t p;
	switch(p=fork()){
		case 0:
			//this is the child case
			printf("I am the child. My PID is %d. Parent ID = %d\n", getpid(), getppid());
			break;
		case -1:
			//this is the error case
			perror("Could not create child.\n");
			exit(-1);
		default:
			//this is the parent case
			printf("I am the parent. My PID is %d\n", getpid());
			break;		
	}	
	//both child and parent continue from here
	
	return(0);
}
