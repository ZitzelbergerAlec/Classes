/*
 * Notes:
 * cast everything to lowercase
 * have command line argument for number of processes
 * 
 *
 */ 

#define _POSIX_SOURCE //for fdopen()
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN //maximum word length

//Function prototypes
void 	closePipe(int pfd);
void 	createPipe(int *fds);
int 	*generatePipes(int numpipes);
void 	help();
void 	PukeAndExit(char *errormessage);
void 	RRParser(int numsorts, int *sortpipefds);
int  	*spawnSorts(int numsorts, int *sortpipefds, int *suppipefds);
void 	strtoupper(char *str);
void 	spawnSuppressor(int numsorts, int *suppipefds);
void 	suppressorProcess(int numsorts, int *suppipefds);

int main(int argc, char **argv){	
	int i, j; //counters 

	//Get number of processes to use
	int c; 
	int numsorts;
	while ((c = getopt(argc, argv, "p:")) != -1)
         switch (c){
           case 'p':
            	numsorts = atoi(optarg);
             	break;
    }
	
	//Generate all necessary pipes for sort
	int *sortpipefds = generatePipes(numsorts);

	//Generate pipes for the suppressor
	int *suppipefds = generatePipes(numsorts);

	//Spawn sort processes
	int *processArray = spawnSorts(numsorts, sortpipefds, suppipefds);

	//parse STDIN
	RRParser(numsorts, sortpipefds);

	//Spawn suppressor process
	//spawnSuppressor(numsorts, suppipefds);
	
	for(i=0; i<numsorts; i++){ //wait for child processes to die. 
		wait(NULL);
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

int *spawnSorts(int numsorts, int *sortpipefds, int *suppipefds){
	//returns an array containing all the PIDs of the child processes
	//Spawn all the sort processes
	pid_t pid;
	int j = 0; //counter for pipes, increments by 2
	int i; 

	int *processArray = (int *)malloc(sizeof(int) * numsorts);
	for(i = 0; i < numsorts; i++){ 
		switch(pid = fork()){
			case -1: //oops case
					PukeAndExit("Forking error\n");
			case 0: //child case
				//Bind stdin to pipe
				closePipe(sortpipefds[j+1]); //close write end of input pipe
				if(sortpipefds[j] != STDIN_FILENO){ //Defensive check
					if(dup2(sortpipefds[j], STDIN_FILENO) == -1)
						PukeAndExit("dup2 0");
					closePipe(sortpipefds[j]); //Close duplicate pipe
				}
				/* Debug
				//Bind stdout to suppressor pipe
				closePipe(suppipefds[j]); //close read end of output pipe
				if(suppipefds[j+1] != STDOUT_FILENO){ //Defensive check
					if(dup2(suppipefds[j+1], STDOUT_FILENO) == -1)
						PukeAndExit("dup2 1"); //problem here
					closePipe(suppipefds[j+1]);
				}
				*/
	            execlp("sort", "sort", (char *)NULL);
	            break;
			default: //parent case
				printf("child process %d: %d\n", i, pid);
				processArray[i] = pid;
		}
		j += 2;
	}
	return processArray;
}

void RRParser(int numsorts, int *sortpipefds){ //Round Robin parser
	int i, j;
	char word[100];

	//Close read end of pipes
	for(i = 0; i < (numsorts * 2); i+=2){
    	closePipe(sortpipefds[i]);
	}

	//Fopen all output pipes
	j = 0; //counter for outputs array
	FILE *outputs[numsorts];
	for(i=1; i < (numsorts * 2); i+= 2){
		outputs[j] = fdopen(sortpipefds[i], "w");
		if(outputs[j] == NULL)
			printf("Error: could not create output stream.\n");
		j++;

	}

	//Distribute words to them
	i = 0;
	while(scanf("%[^,]%*c,", word) != EOF){
		strtoupper(word);
		fputs(word, outputs[i % numsorts]); //round robin
		fputs("\n", outputs[i % numsorts]); //sort needs newline
		i++;
	}

	//Flush the streams:
	for(i=0; i < numsorts; i++){
		if(fclose(outputs[i]) == EOF)
			printf("Error closing stream.\n");
	}
}

void strtoupper(char *str){ //convert a string to uppercase
	int i = 0;
	while(str[i] != '\0'){
		str[i] = toupper(str[i]);
		i++;
	}
}

void spawnSuppressor(int numsorts, int *suppipefds){
	pid_t pid;
	//Fork to suppressor
	switch(pid = fork()){
		case -1:
			//oops
			break;
		case 0:
			printf("Spawned suppressor process");
			suppressorProcess(numsorts, suppipefds);
		default:
			waitpid(pid, NULL, 0);
			break;
	}
}

void suppressorProcess(int numsorts, int *suppipefds){
	char buf[100];
	int i, j; 
	
	//Fopen all input pipes
	j = 0; //counter for inputs array
	FILE *inputs[numsorts];
	for(i=0; i < numsorts*2; i+= 2){
		inputs[j] = fdopen(suppipefds[i], "r");
		if(inputs[j] == NULL)
			printf("Error: could not create input stream.\n");
		j++;
	}

	//Close write end of pipes in suppressor
	for(i = 1; i < numsorts; i+=2){
    	closePipe(suppipefds[i]);
	}
	for(i=0;i<numsorts;i++){
		fgets(buf, 100, inputs[i]);
		printf("In pipe: %s\n", buf);

	}

	//Close inputs
	for(i=0; i < numsorts; i++){
		fclose(inputs[i]);
	}
}