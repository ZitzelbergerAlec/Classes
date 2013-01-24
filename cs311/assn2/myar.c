#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int append(char *filename){
	printf("Appending: %s\n", filename);
	return(0);
}


int extract(char *filename){
	printf("Extracting from archive: %s\n", filename);
	return(0);
}



int main(int argc, char* argv[]){
	int c;
	char *qvalue = NULL;
	while((c = getopt(argc, argv, "q:x:tvd:Aw:")) != -1){
		switch(c){
			case 'q': //quickly append named files to archive
				append(optarg);
				break;
			case 'x': //extract named files	
				extract(optarg);
				break;
			case 't': //print a concise table of contents of the archive
				printf("You entered t!\n");
				break;
			case 'v': //print a verbose table of contents of the archive	
				printf("You entered v!\n");
				break;
			case 'd': //delete named files from archive
				printf("You entered d!\n");
				break;
			case 'A': //quickly append all ``regular'' files in the current directory (except the archive itself)	
				printf("You entered A!\n");
				break;
			case 'w': //Extra credit: for a given timeout, add all modified files to the archive (except the archive itself)	
				printf("You entered w!\n");
				break;
			default:
				printf("You didn't enter anything!\n");
				break;
		}
	}
	return(0);
}
