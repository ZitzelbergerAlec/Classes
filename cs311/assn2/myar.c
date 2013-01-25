#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int append(){
	//printf("Appending: %s\n", filename);
	printf("Appending\n");
	return(0);
}


int extract(){
	//printf("Extracting from archive: %s\n", filename);
	printf("Extracting!\n");
	return(0);
}



int main(int argc, char* argv[]){
	char *c;
	char *qvalue = NULL;
	c = argv[1];
	switch(c){
		case "-q": //quickly append named files to archive
			append();
		case "-x": //extract named files	
			extract();
		case "-t": //print a concise table of contents of the archive
			printf("You entered t!\n");
		case "-v": //print a verbose table of contents of the archive	
			printf("You entered v!\n");
		case "-d": //delete named files from archive
			printf("You entered d!\n");
		case "-A": //quickly append all ``regular'' files in the current directory (except the archive itself)	
			printf("You entered A!\n");
		case "-w": //Extra credit: for a given timeout, add all modified files to the archive (except the archive itself)	
			printf("You entered w!\n");
		default:
			printf("You didn't enter anything!\n");
	};
	return(0);
}
