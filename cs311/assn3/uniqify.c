//Compiler directives
#define _XOPEN_SOURCE		//needed to make sigaction, etc work
#define _POSIX_SOURCE		//for fdopen()

//Includes
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_WORD_LEN 100	//maximum word length

struct wordCounter {
	char word[MAX_WORD_LEN];
	int count;
};

//Function prototypes
int alphaIndex(int numWords, char **words);
void closePipe(int pfd);
void createPipe(int *fds);
int **generatePipesArray(int numpipes);
void grimReaper(int s);
void help();
int isEmpty(char *str);
void printWords(int numWords, char **words, struct wordCounter *curWord);
void PukeAndExit(char *errormessage);
void RRParser(int numSorts, int **outPipe);
void spawnSorts(int numSorts, int **inPipe, int **outPipe);
char *stripNewline(char *word);
void spawnSuppressor(int numSorts, int **inPipe);
void suppressorProcess(int numSorts, int **inPipe);
void waitOnChildren(int numChildren);


int main(int argc, char **argv)
{
	if (argc < 2)
		help();

	//Setup signal handlers
	struct sigaction act;

	act.sa_handler = grimReaper;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGHUP, &act, NULL);

	//Get number of processes to use
	int numSorts = atoi(argv[1]);

	//Generate all necessary pipes for sort
	int **sortpipefds = generatePipesArray(numSorts);

	//Generate pipes for the suppressor
	int **suppipefds = generatePipesArray(numSorts);

	//Spawn sort processes
	spawnSorts(numSorts, sortpipefds, suppipefds);

	//parse STDIN
	RRParser(numSorts, sortpipefds);

	//Spawn suppressor process
	spawnSuppressor(numSorts, suppipefds);

	//Wait for child processes to die
	waitOnChildren(numSorts);

	return (0);
}

//Signal Handler
//Reference: http://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event-c
void grimReaper(int s)
{
	//Clean up
	//Issue a QUIT signal to all processes in group
	kill(getpgrp(), SIGQUIT);
	exit(1);
}

void waitOnChildren(int numChildren)
{
	int i;
	for (i = 0; i < numChildren; i++) {
		wait(NULL);
	}
}

void help()
{
	printf("Usage:\n");
	printf("uniqify [number of sorting processes]\n");
	exit(0);
}

void PukeAndExit(char *errormessage)
{
	perror(errormessage);
	printf("Errno = %d\n", errno);
	exit(-1);
}

void closePipe(int pfd)
{
	if (close(pfd) == -1) {
		char errormessage[8];
		snprintf(errormessage, 8, "Error closing pipe %d\n", pfd);
		PukeAndExit(errormessage);
	}
}

void createPipe(int *fds)
{
	if (pipe(fds) == -1)
		PukeAndExit("Error creating pipes\n");
}

int **generatePipesArray(int numpipes)
{				//returns an empty 2-dimensional array
	int **pipesArray = malloc(sizeof(int *) * (numpipes));
	int i;
	for (i = 0; i < numpipes; i++) {
		pipesArray[i] = malloc(sizeof(int) * 2);
	}
	return (pipesArray);
}

void spawnSorts(int numSorts, int **inPipe, int **outPipe)
{
	//returns an array containing all the PIDs of the child processes
	//Spawn all the sort processes
	pid_t pid;
	int i;
	for (i = 0; i < numSorts; i++) {
		createPipe(inPipe[i]);
		createPipe(outPipe[i]);
		switch (pid = fork()) {
		case -1:	//oops case
			PukeAndExit("Forking error\n");
		case 0:	//child case
			//Bind stdin to inPipe
			closePipe(inPipe[i][1]);	//close write end of input pipe
			if (inPipe[i][0] != STDIN_FILENO) {	//Defensive check
				if (dup2(inPipe[i][0], STDIN_FILENO) == -1)
					PukeAndExit("dup2 0");
				closePipe(inPipe[i][0]);	//Close duplicate pipe
			}
			//Bind stdout to outPipe
			closePipe(outPipe[i][0]);	//close read end of output pipe
			if (outPipe[i][1] != STDOUT_FILENO) {	//Defensive check
				if (dup2(outPipe[i][1], STDOUT_FILENO) ==
				    -1)
					PukeAndExit("dup2 1");
				closePipe(outPipe[i][1]);	//Close duplicate pipe
			}
			execlp("sort", "sort", (char *) NULL);
			break;
		default:	//parent case
			closePipe(inPipe[i][0]);	//Close read end of pipe in parent
			closePipe(outPipe[i][1]);	//Close write end of output pipe in parent
		}
	}
}

void RRParser(int numSorts, int **outPipe)
{				//Round Robin parser
	//Sends words that contain only alphabetical characters
	int i = 0;
	char buf[1];
	while (read(STDIN_FILENO, buf, 1) != 0) {
		if (isalpha(buf[0])) {
			buf[0] = tolower(buf[0]);
		} else {
			buf[0] = '\n';
			i++;
		}
		write(outPipe[i % numSorts][1], buf, 1);
	}

	//Flush the streams:
	for (i = 0; i < numSorts; i++) {
		closePipe(outPipe[i][1]);
	}
}

void spawnSuppressor(int numSorts, int **inPipe)
{
	pid_t pid;
	int i;
	//Fork to suppressor
	switch (pid = fork()) {
	case -1:
		//oops
		break;
	case 0:
		suppressorProcess(numSorts, inPipe);
		_exit(EXIT_SUCCESS);
		break;
	default:
		waitpid(pid, NULL, 0);
		break;
	}
}

void suppressorProcess(int numSorts, int **inPipe)
{
	int i;
	char buf[MAX_WORD_LEN];
	char **words;
	FILE *inputs[numSorts];
	struct wordCounter *curWord = malloc(sizeof(struct wordCounter));
	int alpha;		//index of alpha word in pipe

	//initialize word array
	words = malloc(numSorts * sizeof(char *));
	for (i = 0; i < numSorts; i++) {
		words[i] = malloc(MAX_WORD_LEN * sizeof(char));
	}

	//fdopen inPipes
	//And get first batch of words to initialize curWord with
	for (i = 0; i < numSorts; i++) {
		inputs[i] = fdopen(inPipe[i][0], "r");
		if (fgets(words[i], MAX_WORD_LEN, inputs[i % numSorts]) ==
		    NULL)
			words[i] = NULL;
	}
	alpha = alphaIndex(numSorts, words);
	strncpy(curWord->word, stripNewline(words[alpha]), MAX_WORD_LEN);
	curWord->count = 1;

	int nullCount = 0;
	while (nullCount < numSorts) {
		if (fgets(words[alpha], MAX_WORD_LEN, inputs[alpha]) ==
		    NULL) {
			words[alpha] = NULL;
			nullCount++;
		}
		alpha = alphaIndex(numSorts, words);
		if (alpha == -1)
			break;
		if (!strcmp(curWord->word, stripNewline(words[alpha]))) {
			curWord->count++;
		} else {
			if (!isEmpty(curWord->word))
				printf("%d %s\n", curWord->count,
				       curWord->word);
			strncpy(curWord->word, words[alpha], MAX_WORD_LEN);
			curWord->count = 1;
		}
	}

	//Free words array
	for (i = 0; i < numSorts; i++) {
		free(words[i]);
	}
	free(words);
	free(curWord);

	//Close inputs
	for (i = 0; i < numSorts; i++) {
		fclose(inputs[i]);
	}
}

int isEmpty(char *str)
{				//returns 1 if a string is empty
	if (str[0] == '\0' || str[0] == '\n')
		return 1;
	return 0;
}

char *stripNewline(char *word)
{				//removes trailing newline characters from strings
	if (word[strlen(word) - 1] == '\n')
		word[strlen(word) - 1] = '\0';
	return word;
}

int alphaIndex(int numWords, char **words)
{
	//returns the index of the lowest alphabetical word in a set
	int alpha = -1;
	int i;
	//First, find the first alpha word. If all the words are NULL, return -1 for error
	for (i = 0; i < numWords; i++) {
		if (words[i] == NULL) {
			continue;
		} else {
			alpha = i;
			break;
		}
	}
	if (alpha == -1)
		return -1;
	//Now find the lowest alphabetical word
	for (i = 0; i < numWords; i++) {
		if (words[i] == NULL)
			continue;
		if (strcmp(words[i], words[alpha]) < 0)
			alpha = i;
	}
	return alpha;
}
