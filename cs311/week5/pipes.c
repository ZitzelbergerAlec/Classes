/*
 * Pipes are unidirectional byte streams.
 *  --data only flows in a single direction
 *  --data is byte-oriented
 *
 *  Pipes are represented by file descriptors.
 *  --read() reads data from a pipe
 *  --write() "     "     "   "  " 
 *
 *  --int filedis[2]
 *    --filedes[0] is read end 
 *    --filedes[1] is write end
 *
 *  writes less than PIPE BUF are guaranteed to be atomic
 *  pipes are limited in capacity
 *  ends not in use MUST be closed
 *
 */


#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
	int fds[2];
	char buf[100];
	char s[] = "hello";
	int num_read;
	
	if(pipe(fds) == -1){
		perror("Error creating pipes\n");
		exit(EXIT_FAILURE);
	}
	
	switch(fork()){
		case 0:
			//child case
			//close write end of pipe
			close(fds[1]);
			printf("In child case, PID = %d\n", getpid());	
			num_read = read(fds[0], buf, 100);
			buf[num_read] = '\0';
			printf("buf = %s\n", buf);
			break;
		case -1:
			//oops case
			break;
		default:
			//parent case
			//close read end of pipe
			close(fds[0]);
			printf("In parent case, PID = %d\n", getpid());	
			num_read = read(fds[0], buf, 100);
			write(fds[1], s, 5);	
			printf("In parent case (post-write), PID = %d\n", getpid());	
			num_read = read(fds[0], buf, 100);
			break;

	}
	return 0;
}
