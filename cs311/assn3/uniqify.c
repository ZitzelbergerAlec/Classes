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
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_WORD_LEN 100 //maximum word length

//Function prototypes
void 	closePipe(int pfd);
void 	createPipe(int *fds);
int 	**generatePipesArray(int numpipes);
void 	help();
void 	PukeAndExit(char *errormessage);
void 	RRParser(int numsorts, int **outPipe);
void  	spawnSorts(int numsorts, int **inPipe, int **outPipe);
void 	strtoupper(char *str);
void 	spawnSuppressor(int numsorts, int **inPipe);
void 	suppressorProcess(int numsorts, int **inPipe);

int main(int argc, char **argv){	
	int i;
	if(argc < 2){
		help();
		exit(0);
	}
	
	//Get number of processes to use
	int numsorts = atoi(argv[1]);

	//Generate all necessary pipes for sort
	int **sortpipefds = generatePipesArray(numsorts);

	//Generate pipes for the suppressor
	int **suppipefds = generatePipesArray(numsorts);

	//Spawn sort processes
	spawnSorts(numsorts, sortpipefds, suppipefds);

	//parse STDIN
	RRParser(numsorts, sortpipefds);

	//Spawn suppressor process
	spawnSuppressor(numsorts, suppipefds);	

	for(i=0; i<numsorts; i++){ //wait for child processes to die. 
		wait(NULL);
	}
	return(0);
}

void help(){
	printf("Usage:\n");
	printf("uniqify [number of sorting processes]\n");
	exit(0);
}

void PukeAndExit(char *errormessage){
	perror(errormessage);
	printf("Errno = %d\n", errno);
	exit(-1);
}

void closePipe(int pfd){
	if(close(pfd) == -1){
		char errormessage[8];
		snprintf(errormessage, 8, "Error closing pipe %d\n", pfd);
		PukeAndExit(errormessage);
	}
}

void createPipe(int *fds){
	if(pipe(fds) == -1)
                PukeAndExit("Error creating pipes\n");
}

int **generatePipesArray(int numpipes){ //returns an empty 2-dimensional array
	//To do: why can't I create the pipes in here?
	int **pipesArray = (int **) malloc(sizeof(int *) * (numpipes));
	int i;
	for(i = 0; i < numpipes; i++){
    		pipesArray[i] = (int *) malloc(sizeof(int) * 2);
	}
	return(pipesArray);
}

void spawnSorts(int numsorts, int **inPipe, int **outPipe){
	//returns an array containing all the PIDs of the child processes
	//Spawn all the sort processes
	pid_t pid;
	int i; 
	for(i = 0; i < numsorts; i++){ 
		createPipe(inPipe[i]);
		createPipe(outPipe[i]);
		switch(pid = fork()){
			case -1: //oops case
					PukeAndExit("Forking error\n");
			case 0: //child case
				//Bind stdin to inPipe
				closePipe(inPipe[i][1]); //close write end of input pipe
				if(inPipe[i][0] != STDIN_FILENO){ //Defensive check
					if(dup2(inPipe[i][0], STDIN_FILENO) == -1)
						PukeAndExit("dup2 0");
					closePipe(inPipe[i][0]); //Close duplicate pipe
				}
				//Bind stdout to outPipe
				closePipe(outPipe[i][0]); //close read end of output pipe
				if(outPipe[i][1] != STDOUT_FILENO){ //Defensive check
					if(dup2(outPipe[i][1], STDOUT_FILENO) == -1)
						PukeAndExit("dup2 1");
					closePipe(outPipe[i][1]); //Close duplicate pipe
				}
	            		execlp("sort", "sort", (char *)NULL);
	            		break;
			default: //parent case
				closePipe(inPipe[i][0]); //Close read end of pipe in parent
				closePipe(outPipe[i][1]); //Close write end of output pipe in parent
		}
	}
}

void RRParser(int numsorts, int **outPipe){ //Round Robin parser
	int i;
	char word[MAX_WORD_LEN];
	int readnum;
    	char buf[1];
    	i = 0;    
    	while ((readnum = read(STDIN_FILENO, buf, 1)) != 0) {
	        if (isalpha(buf[0])) {
	            	buf[0] = toupper(buf[0]);
	        } else {
	                buf[0] = '\n';
	                i++;
	        }
	        write(outPipe[i % numsorts][1], buf, 1);
    	}

	//Flush the streams:
	for(i=0; i < numsorts; i++){
		closePipe(outPipe[i][1]);
	}
}

void strtoupper(char *str){ //convert a string to uppercase
	int i = 0;
	while(str[i] != '\0'){
		str[i] = toupper(str[i]);
		i++;
	}
}

void spawnSuppressor(int numsorts, int **inPipe){
	pid_t pid;
	int i; 
	//Fork to suppressor
	switch(pid = fork()){
		case -1:
			//oops
			break;
		case 0:
			suppressorProcess(numsorts, inPipe);
			_exit(EXIT_SUCCESS);
			break;
		default:
			waitpid(pid, NULL, 0);
			break;
	}
}

void suppressorProcess(int numsorts, int **inPipe){
	int i;
	char buf[MAX_WORD_LEN]; 

	//Fopen all input pipes
	FILE *inputs[numsorts];
	for(i=0; i < numsorts; i++){
		inputs[i] = fdopen(inPipe[i][0], "r");
		if(inputs[i] == NULL)
			printf("Error: could not create input stream.\n");
	}

	i=0;
	while(fgets(buf, MAX_WORD_LEN, inputs[i % numsorts]) != NULL){		
		printf("In pipe: %s\n", buf);
		i++;
	}

	//Close inputs
	for(i=0; i < numsorts; i++){
		fclose(inputs[i]);
	}
}
