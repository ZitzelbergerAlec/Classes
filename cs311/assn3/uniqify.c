/*
 * Notes:
 * cast everything to lowercase
 * have command line argument for number of processes
 * 
 *
 */ 
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


void 	help();
void 	PukeAndExit(char *errormessage);


int main(int argc, char **argv){	
	FILE *fp;
	int num_read;
	int num_written;
	char buf[100]; //Buffer for reading from pipe
	int status;	
	pid_t p;
	pid_t child;
	int fds[2];

	//Create a pipe
	if(pipe(fds) == -1){
                perror("Error creating pipes\n");
                exit(EXIT_FAILURE);
        }
	/*
	if((fp=fopen(argv[1], "r")) == NULL) {
    		PukeAndExit("Cannot open file.\n");
  	}
	*/
	char word[100];
	int output;
	output = 1;
	//while(fscanf(fp, "%[^,]%*c", word) != EOF){
		switch(p=fork()){
			case 0:
				//Child case
				//Bind stdin to sort process
				close(fds[1]);
				if (fds[0] != STDIN_FILENO) { /* Defensive check */
					if (dup2(fds[0], STDIN_FILENO) == -1)
						PukeAndExit("dup2 0");
					if (close(fds[0]) == -1) //Close duplicate
						PukeAndExit("close 0");
				}
                execlp("sort", "sort", (char *)NULL);
				break;
			case -1:
				//Oops case
				exit(-1);
			default:
				//Parent case
				close(fds[0]);
				write(fds[1], "a\nb\nd\nc", 7);
				//To do: Close the pipe
				if(close(fds[1]) == -1)
					PukeAndExit("Close 1");
				child = wait(&status);
				break;
		}
	//}	
	return(0);
}

void help(){
	printf("Usage:\n");
	printf("uniqify [filename] [number of sorting processes]\n");
	exit(0);
}

void PukeAndExit(char *errormessage){
	perror(errormessage);
	exit(-1);
}

