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

//Debug function prototypes
int *debugSorts(int numsorts, int **inPipe, int **outPipe);
void debugProcess(int *inPipe);


//Function prototypes
void 	closePipe(int pfd);
void 	createPipe(int *fds);
int 	**generatePipes(int numpipes);
void 	help();
void 	PukeAndExit(char *errormessage);
void 	RRParser(int numsorts, int **outPipe);
int  	*spawnSorts(int numsorts, int **inPipe, int **outPipe);
void 	strtoupper(char *str);
void 	spawnSuppressor(int numsorts, int **suppipefds);
void 	suppressorProcess(int numsorts, int **suppipefds);

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
	int **sortpipefds = generatePipes(numsorts);

	//Generate pipes for the suppressor
	int **suppipefds = generatePipes(numsorts);

	//Spawn sort processes
	//int *processArray = spawnSorts(numsorts, sortpipefds, suppipefds);

	int *processArray = debugSorts(numsorts, sortpipefds, suppipefds);

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

int **generatePipes(int numpipes){ //returns a 2-dimensional array of pipes
	int **pipesArray = (int **) malloc(sizeof(int *) * (numpipes));
	int i;
	for(i = 0; i < numpipes; i++){
    		pipesArray[i] = (int *) malloc(sizeof(int) * 2);
    		if(pipe(pipesArray[i]) < 0)
        		PukeAndExit("Error creating pipes\n");
	}
	return(pipesArray);
}

int *debugSorts(int numsorts, int **inPipe, int **outPipe){
	//Debugs spawned processes by printing everything 
	//Spawn all the sort processes
	pid_t pid;
	int i; 
	int *processArray = (int *)malloc(sizeof(int) * numsorts);
	for(i = 0; i < numsorts; i++){ 
		switch(pid = fork()){
			case -1: //oops case
					PukeAndExit("Forking error\n");
			case 0: //child case
				//Bind stdin to pipe
				closePipe(inPipe[i][1]); //close write end of input pipe
	            debugProcess(inPipe[i]);
	            break;
			default: //parent case
				processArray[i] = pid;
		}
	}
	return processArray;
}

void debugProcess(int *inPipe){
	int pid = getpid();
	char word[MAX_WORD_LEN];
	FILE *input;
	input = fdopen(inPipe[0], "r");
	while(fgets(word, MAX_WORD_LEN, input) != NULL){
		printf("word = %s\n", word);
	}
	exit(0);
}

int *spawnSorts(int numsorts, int **inPipe, int **outPipe){
	//returns an array containing all the PIDs of the child processes
	//Spawn all the sort processes
	pid_t pid;
	int i; 
	int *processArray = (int *)malloc(sizeof(int) * numsorts);
	for(i = 0; i < numsorts; i++){ 
		switch(pid = fork()){
			case -1: //oops case
					PukeAndExit("Forking error\n");
			case 0: //child case
				//Bind stdin to pipe
				closePipe(inPipe[i][1]); //close write end of input pipe
				if(inPipe[i][0] != STDIN_FILENO){ //Defensive check
					if(dup2(inPipe[i][0], STDIN_FILENO) == -1)
						PukeAndExit("dup2 0");
					closePipe(inPipe[i][0]); //Close duplicate pipe
				}
	            execlp("sort", "sort", (char *)NULL);
	            break;
			default: //parent case
				processArray[i] = pid;
		}
	}
	return processArray;
}

void RRParser(int numsorts, int **outPipe){ //Round Robin parser
	int i;
	char word[MAX_WORD_LEN];

	//Close read end of pipes
	for(i = 0; i < numsorts; i++){
    	closePipe(outPipe[i][0]);
	}

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

void spawnSuppressor(int numsorts, int **suppipefds){
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

void suppressorProcess(int numsorts, int **suppipefds){
	char buf[100];
	int i, j; 
	
	//Fopen all input pipes
	FILE *inputs[numsorts];
	for(i=0; i < numsorts; i++){
		inputs[i] = fdopen(suppipefds[i][0], "r");
		if(inputs[i] == NULL)
			printf("Error: could not create input stream.\n");
	}

	//Close write end of pipes in suppressor
	for(i = 0; i < numsorts; i++){
    	closePipe(suppipefds[i][1]);
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
