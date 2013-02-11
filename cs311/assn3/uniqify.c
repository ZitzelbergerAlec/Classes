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

struct wordCounter{
	char word[MAX_WORD_LEN];
	int count;
};

//debug
void DebugPrintWords(int numWords, char **words);

//Function prototypes
void  	alphWords(int numWords, char **words);
void 	clearWords(int numWords, char **words);
void 	closePipe(int pfd);
void 	createPipe(int *fds);
int 	**generatePipesArray(int numpipes);
void 	help();
int 	isEmpty(char *str);
void 	printWords(int numWords, char **words, struct wordCounter *curWord);
void 	PukeAndExit(char *errormessage);
void 	RRParser(int numsorts, int **outPipe);
void  	spawnSorts(int numsorts, int **inPipe, int **outPipe);
void 	stripNewline(char *word);
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
	char **words;
	FILE *inputs[numsorts];
	struct wordCounter *curWord = (struct wordCounter *) malloc(sizeof(struct wordCounter));
	curWord->count = 0;

        //initialize word array
        words = (char**)malloc(numsorts * sizeof(char*));
        for (i = 0; i < numsorts; i++) {
                words[i] = (char*)malloc(MAX_WORD_LEN * sizeof(char));
        }

        //fdopen inPipes
        for(i = 0; i < numsorts; i++) {
                inputs[i] = fdopen(inPipe[i][0], "r");
        }

        i = 0;
        int j = 0;
        int nullCount = 0;
        while(1){
        	if(fgets(words[j], MAX_WORD_LEN, inputs[i % numsorts]) == NULL){
                        strncpy(words[j], "0", MAX_WORD_LEN); //mark words
                        nullCount++;
                        if(nullCount == numsorts)
                        	break;
                } else {
                	stripNewline(words[j]);
                	if(isEmpty(words[j]))
                		continue;
                }
                j++;
                if(!(j < numsorts)){
                	alphWords(j, words);
                	printWords(j, words, curWord);
                	clearWords(j, words);
                	j = 0;
                }
        	i++;
        }


        //Free words array
        for (i = 0; i < numsorts; i++) {
                free(words[i]);
        }
        free(words);
        free(curWord);

	//Close inputs
	for(i=0; i < numsorts; i++){
		fclose(inputs[i]);
	}
}

int isEmpty(char *str){ //returns 1 if a string is empty
	if(str[0] == '\0')
		return 1;
	return 0;
}

void DebugPrintWords(int numWords, char **words){
	//debugs words array by printing them out
	int i;
	for(i=0; i<numWords; i++){
		if(!strcmp(words[i], "0"))
			continue;
		printf("words[%d] = %s", i, words[i]);
	}
}

void printWords(int numWords, char **words, struct wordCounter *curWord){
	//Print the unique words and their counts
	int i = 0;
	if(curWord->count == 0){ //curWord is uninitializied; this is first time through loop
		strncpy(curWord->word, words[0], MAX_WORD_LEN);
		curWord->count = 1;
		i = 1;
	}
	for(i;i<numWords;i++){
		if(!strcmp(words[i], "0"))
			continue;
		if(!strcmp(words[i],curWord->word)){
			curWord->count++;
		} else {
			printf("%s %d\n", curWord->word, curWord->count);
			strncpy(curWord->word, words[i], MAX_WORD_LEN);
			curWord->count = 1;
		}
	}
}

void stripNewline(char *word){ //removes trailing newline characters from strings
	if (word[strlen(word) - 1] == '\n')
  		word[strlen(word) - 1] = '\0';
}

void clearWords(int numWords, char **words){
	//Clears the array of words
	int i; 
	for(i = 0; i < numWords; i++){
		strncpy(words[i], "0", MAX_WORD_LEN);
	}
}

void alphWords(int numWords, char **words){ //Alphabetizes words
	//Takes an unorganized array of words
	//Returns one organized in alphabetical order
	int i, j;
	int alpha; //index of next lowest alphabetical word
	//Perform an insertion sort
	for(j=0; j < numWords; j++){
		alpha = j;
		for(i = j; i < numWords; i++){
			if(!strcmp(words[i], "0"))
				continue;
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
