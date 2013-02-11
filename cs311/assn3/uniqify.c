/*
 * Notes:
 * cast everything to uppercase
 * have command line argument for number of processes
 * To do: signal handling
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

typedef struct wordCounter{
	char word[MAX_WORD_LEN];
	int count;
} curWord;

//debug
void DebugPrintWords(int numWords, char **words);

//Function prototypes
void 	clearWords(int numWords, char **words);
void 	closePipe(int pfd);
void 	createPipe(int *fds);
int 	**generatePipesArray(int numpipes);
void 	help();
void  	alphWords(int numWords, char **words);
void 	printWords(int numWords, char **words, struct wordCounter curWord);
void 	PukeAndExit(char *errormessage);
void 	RRParser(int numsorts, int **outPipe);
void  	spawnSorts(int numsorts, int **inPipe, int **outPipe);
void 	strtoupper(char *str);
void 	spawnSuppressor(int numsorts, int **inPipe);
void 	suppressorProcess(int numsorts, int **inPipe);
void 	swapWords(int i, int j, char **words);
void 	waitOnChildren(int numChildren);

int main(int argc, char **argv){	
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
	//spawnSuppressor(numsorts, suppipefds);	
	//Debug
	suppressorProcess(numsorts, suppipefds);	

	//Wait for child processes to die
	waitOnChildren(numsorts);
	
	return(0);
}

void waitOnChildren(int numChildren){
	int i;
	for(i=0; i<numChildren; i++){
		wait(NULL);
	}
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
    	char buf[1];
    	i = 0;    
    	while(read(STDIN_FILENO, buf, 1) != 0) {
	        if(isalpha(buf[0])){
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
	char *words[MAX_WORD_LEN];

	//Fopen all input pipes
	FILE *inputs[numsorts];
	for(i=0; i < numsorts; i++){
		inputs[i] = fdopen(inPipe[i][0], "r");
		if(inputs[i] == NULL)
			printf("Error: could not create input stream.\n");
	}


	int j; //index for words and inner for loop, respectively
	i=j=0; //i is the index for pipes
	
	//To do: make this struct
	//structure for counting words
	struct wordCounter curWord;
	
	//To do: This loop should have a NULL counter instead that counts the number of NULLS at every go around
	//That way it won't break out too early if one of the pipes is empty but others still have data.
	//If the number of NULLS is equal to numsorts, break.

	int nullCount = 0; //counts number of nulls from fgets
	clearWords(numsorts, words); //initialize word array
	

	if(fgets(words[0], MAX_WORD_LEN, inputs[0]) != NULL)
		printf("words[0] = %s\n", words[0]);

	/*
	while(1){
		if(fgets(words[j], MAX_WORD_LEN, inputs[i % numsorts]) == NULL){
 			nullCount++;
			if(nullCount == numsorts){
				//alphWords(j+1, words);
				//DebugPrintWords(j+1, words);
				//printWords(j+1, words, curWord); //print leftovers
				break;
			} else {
				i++;
				continue;
			}
		}
		if(!(j < numsorts)){
			//DebugPrintWords(j, words);
			//alphWords(j+1, words); //this causes a segfault
			//DebugPrintWords(j+1, words);
			//printWords(j, words, curWord);
			//clearWords(j, words);
			j = 0;
		}
		//printf("words[0] = %s\n", words[0]);
		//printf("words[1] = %s\n", words[1]);
		j++;
		i++;
	}
	*/

	//Close inputs
	for(i=0; i < numsorts; i++){
		fclose(inputs[i]);
	}
}

void DebugPrintWords(int numWords, char **words){
	//debugs words array by printing them out
	int i;
	for(i=0; i<numWords; i++){
		printf("words[%d] = %s", i, words[i]);
	}
}

void printWords(int numWords, char **words, struct wordCounter curWord){
	//Print the unique words and their counts
	int i = 0;
	if(curWord.count == 0){ //curWord is uninitializied; this is first time through loop
		//curWord.word = words[0];
		curWord.count = 1;
		i = 1;
	}
	for(i;i<numWords;i++){
		if(!strcmp(words[i],curWord.word)){
			curWord.count++;
		} else {
			printf("%s %d\n", curWord.word, curWord.count);
			//curWord.word = words[i];
			curWord.count = 1;
		}
	}
}

void clearWords(int numWords, char **words){
	//Clears the array of words
	int i; 
	for(i = 0; i < numWords; i++){
		words[i] = "0";
	}
}

void alphWords(int numWords, char **words){ //Alphabetizes words
	//Takes an unorganized array of words
	//Returns one organized in alphabetical order
	int i, j;
	int alpha; //index of next lowest alphabetical word
	//Perform an insertion sort
	for(j=0; j < numWords; j++){
		if(!strcmp(words[j], "0"))
				break;
		alpha = j;
		for(i = j; i < numWords; i++){
			if(!strcmp(words[i], "0"))
				break;
			if(strcmp(words[i],words[alpha]) < 0)
				alpha = i; //found a new alpha word
		}
		swapWords(j, alpha, words);
	}
}

void swapWords(int i, int j, char **words){
	//Swaps 2 words in an array of words
	char *temp;
	temp = words[i];
	words[i] = words[j];
	words[j] = temp;
}
