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
	 
	char *cvalue;
	int c; 
	int numsorts;
	while ((c = getopt(argc, argv, "p:")) != -1)
         switch (c){
           case 'p':
            	numsorts = atoi(optarg);
             	break;
    }

    int num_sortpipes = numsorts * 2; //number of pipes for sort
    int num_suppipes = numsorts * 2; //number of pipes for suppressor
	int processarray[numsorts]; //Array of processes
	
	//Generate all necessary pipes for sort
	//So all read ends will be even
	int sortpipefds[2 * num_sortpipes];
	for(i = 0; i < num_sortpipes; i++){
    	if(pipe(sortpipefds + i*2) < 0){
        	PukeAndExit("Error creating pipes\n");
    	}
	}

	//Generate all necessary pipes for suppressor
	int suppipefds[2 * num_suppipes];
	for(i = 0; i < num_suppipes; i++){
    	if(pipe(suppipefds + i*2) < 0){
        	PukeAndExit("Error creating pipes\n");
    	}
	}

	//Spawn all the sort processes
	pid_t pid;
	int j = 0; //counter for pipes, increments by 2
	for(i = 0; i < numsorts; i++){ 
		pid = fork();
		processarray[i] = pid;
		if(pid == 0){ //child case
			printf("Sort process spawned\n");
			//Bind stdin to sort process
			closePipe(sortpipefds[j+1]); //close write end
			if(sortpipefds[j] != STDIN_FILENO){ //Defensive check
				if(dup2(sortpipefds[j], STDIN_FILENO) == -1)
					PukeAndExit("dup2 0");
				closePipe(sortpipefds[j]);
			}
			closePipe(suppipefds[j]);
			//Bind stdout to suppressor pipe
			if(suppipefds[j+1] != STDOUT_FILENO){ //Defensive check
				if(dup2(suppipefds[j+1], STDOUT_FILENO) == -1)
					PukeAndExit("dup2 1");
				closePipe(suppipefds[j+1]);
			}
            execlp("sort", "sort", (char *)NULL);
		}
		j += 2;
	}

	//Close read end of pipes in parent
	for(i = 0; i < num_sortpipes; i+=2){
    	closePipe(sortpipefds[i]);
	}

	//Fopen all output pipes
	j = 0; //counter for outputs array
	FILE *outputs[num_sortpipes/2];
	for(i=1; i < num_sortpipes; i+= 2){
		outputs[j] = fdopen(sortpipefds[i], "w");
		j++;

	}

	//Fopen all input pipes to suppressor
	j = 0; //counter for inputs array
	FILE *inputs[num_suppipes/2];
	for(i=0; i < num_suppipes; i+= 2){
		inputs[j] = fdopen(suppipefds[i], "r");
		j++;
	}

	//Distribute words to them
	i = 0;
	while(scanf("%[^,]%*c,", word) != EOF){
		i++;
		fputs(word, outputs[i % numsorts]); //round robin
		fputs("\n", outputs[i % numsorts]); //sort needs newline
	}

	//Read stuff to test if sort processes are outputting to correct pipes
	//Close write end of suppressor pipes in parent
	//Close read end of pipes in parent
	for(i = 1; i < num_suppipes; i+=2){
    	closePipe(suppipefds[i]);
	}

	char buf2[10];
	for(i=0; i<num_suppipes;i++){
		fgets(buf2, 10, inputs[i]); //hangs here
		printf("buf = %s\n", buf2);
	}

	//Flush the pipes:
	//Close output pipes and input pipes
	for(i=1; i < num_sortpipes/2; i++){
		fclose(outputs[i]);
		fclose(inputs[i]);
	}

	//Close pipes
	/*
	for(i=1; i<numpipes; i+=2){
		closePipe(pipefds[i]);
	}
	*/

	//Reap all zombies
	int status;
	/*
	for(i=0; i < numsorts; i++){
		waitpid(processarray[i], &status, 0);
	}
	*/

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