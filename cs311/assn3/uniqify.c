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
int 	*generatePipes(int numpipes);
void 	help();
void 	PukeAndExit(char *errormessage);


int main(int argc, char **argv){	
	int i; 
	char buf[100]; //Buffer for reading from pipe
	pid_t child;
	char word[100];

	//Get number of processes to use
	int c; 
	int numsorts;
	while ((c = getopt(argc, argv, "p:")) != -1)
         switch (c){
           case 'p':
            	numsorts = atoi(optarg);
             	break;
    }

	int processarray[numsorts]; //Array of processes
	
	//Generate all necessary pipes for sort

	//So all read ends will be even
	int *sortpipefds = generatePipes(numsorts);

	//Generate pipes for the suppressor
	int *suppipefds = generatePipes(numsorts);

	//Spawn all the sort processes
	pid_t pid;
	int j = 0; //counter for pipes, increments by 2
	for(i = 0; i < numsorts; i++){ 
		pid = fork();
		if(pid == 0){ //child case
			printf("Sort process spawned\n");
			//Bind stdin to pipe
			closePipe(sortpipefds[j+1]); //close write end of input pipe
			if(sortpipefds[j] != STDIN_FILENO){ //Defensive check
				if(dup2(sortpipefds[j], STDIN_FILENO) == -1)
					PukeAndExit("dup2 0");
				closePipe(sortpipefds[j]);
			}
			//Bind stdout to suppressor pipe
			closePipe(suppipefds[j]); //close read end of output pipe
			if(suppipefds[j+1] != STDOUT_FILENO){ //Defensive check
				if(dup2(suppipefds[j+1], STDOUT_FILENO) == -1)
					PukeAndExit("dup2 1"); //problem here
				closePipe(suppipefds[j+1]);
			}
            execlp("sort", "sort", (char *)NULL);
		} else {
			processarray[i] = pid;
		}
		j += 2;
	}

	//Close read end of pipes in parent (parser)
	for(i = 0; i < numsorts; i+=2){
    	closePipe(sortpipefds[i]);
	}

	//Fopen all output pipes
	j = 0; //counter for outputs array
	FILE *outputs[numsorts];
	for(i=1; i < numsorts*2; i+= 2){
		outputs[j] = fdopen(sortpipefds[i], "w");
		j++;

	}

	//Distribute words to them
	i = 0;
	while(scanf("%[^,]%*c,", word) != EOF){
		i++;
		fputs(word, outputs[i % numsorts]); //round robin
		fputs("\n", outputs[i % numsorts]); //sort needs newline
	}

	//Flush the pipes:
	//Close output pipes pipes
	for(i=0; i < numsorts; i++){
		fclose(outputs[i]);
	}

	//Fopen all input pipes
	j = 0; //counter for outputs array
	FILE *inputs[numsorts];
	for(i=0; i < numsorts*2; i+= 2){
		inputs[j] = fdopen(suppipefds[i], "r");
		j++;
	}

	//Fork to suppressor
	switch(fork()){
		case -1:
			//oops
			break;
		case 0:
			printf("Spawning suppressor process");
			char inbuf[100];
			//Close write end of pipes in suppressor
			for(i = 1; i < numsorts; i+=2){
		    	closePipe(suppipefds[i]);
			}
			for(i=0;i<numsorts;i++){
				fgets(inbuf, 100, inputs[i]);
				printf("In pipe: %s\n", inbuf);

			}		
		default:
			break;

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
	printf("Errno = %d\n", errno);
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

int *generatePipes(int numpipes){ //returns an array of pipes of size 2*numpipes
	int *pipesArray = (int *)malloc(sizeof(int) * (2 * numpipes));
	int i;
	for(i = 0; i < numpipes; i++){
    	if(pipe(pipesArray + i*2) < 0){
        	PukeAndExit("Error creating pipes\n");
    	}
	}
	return(pipesArray);
}