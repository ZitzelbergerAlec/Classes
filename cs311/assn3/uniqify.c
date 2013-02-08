/*
 * Notes:
 * cast everything to lowercase
 * have command line argument for number of processes
 * 
 *
 */ 

#include <errno.h>
//#include <pipe.h> //can't find
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

//Function prototypes
void 	closePipe(int pfd);
void 	createPipe(int *fds);
void 	help();
void 	PukeAndExit(char *errormessage);


int main(int argc, char **argv){	
	FILE *fp;
	int i; 
	int num_read;
	int num_written;
	char buf[100]; //Buffer for reading from pipe
	int status;	
	pid_t child;
	char word[100];

	//Get number of processes to use
	 
	char *cvalue;
	int c; 
	int numsorts;
	while ((c = getopt (argc, argv, "p:")) != -1)
         switch (c){
           case 'p':
            	numsorts = atoi(optarg);
             	break;
    }
    printf("numsorts = %d\n", numsorts);
    int numpipes = numsorts * 2;
	int processarray[numsorts]; //Array of processes
	
	//Generate all necessary pipes
	//So all read ends will be even
	int pipefds[4 * numsorts];
	for(i = 0; i < numpipes; i++){
    	if(pipe(pipefds + i*2) < 0 ){
        	PukeAndExit("Error creating pipes\n");
    	}
	}

	//Spawn all the sort processes
	pid_t pid;
	for(i = 0; i < (numsorts*2); i+=2){ //increment by 2 for pipes
		pid = fork();
		if(pid == 0){ //child case
			printf("Spawning sort process \n");
			//Bind stdin to sort process
			closePipe(pipefds[i+1]); //close write end
			if (pipefds[i] != STDIN_FILENO) { //Defensive check
				if (dup2(pipefds[i], STDIN_FILENO) == -1)
					PukeAndExit("dup2 0");
				closePipe(pipefds[i]);
			}
            execlp("sort", "sort", (char *)NULL);
		}
	}

	//Close read end of pipes in parent
	for(i = 0; i < numpipes; i += 2){
    	closePipe(pipefds[i]);
	}

	//Fopen all output pipes
	int j = 0; //counter for outputFiles array
	FILE *outputFiles[numpipes/2];
	for(i=1; i < numpipes; i+= 2){
		outputFiles[j] = fdopen(pipefds[i], "w");
		j++;

	}

	//Distribute words to them
	i = 0;
	while(scanf("%[^,]%*c,", word) != EOF){
		i++;
		fputs(word, outputFiles[i % numsorts]); //round robin
		fputs("\n", outputFiles[i % numsorts]); //sort needs newline
	}

	//Flush the pipes
	for(i=1; i < numpipes/2; i++){
		fclose(outputFiles[i]);
	}

	return(0);
}

void help(){
	printf("Usage:\n");
	printf("uniqify -p [number of sorting processes]\n");
	exit(0);
}

void PukeAndExit(char *errormessage){
	perror(errormessage);
	exit(-1);
}

void closePipe(int pfd){
	char errormessage[8];
	if(close(pfd) == -1){
				snprintf(errormessage, 8, "Close %d\n", pfd);
				PukeAndExit(errormessage);
	}
}

void createPipe(int *fds){
	if(pipe(fds) == -1)
                PukeAndExit("Error creating pipes\n");
}