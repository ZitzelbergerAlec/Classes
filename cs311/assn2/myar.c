#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int append(){
	//printf("Appending: %s\n", filename);
	printf("Appending\n");
	return(0);
}


int extract(char *argv[]){
	//printf("Extracting from archive: %s\n", filename);
	printf("Extracting!\n");
	printf("argv1: %s\n", argv[1]);
	printf("Archive file to use is %s\n", argv[2]);
	return(0);
}

void help(){ //prints usage of program
	printf("Usage: myar -key archive filename ...\n");
}


int main(int argc, char* argv[]){
	//Order of arguments:
	//myar key afile name ...
	// where key is "-q" etc
	char *c;
	c = argv[2]; // Set to 2 for debugging purposes. Set this back to 1 later!!;
	if(!strcmp(c,"-q")){ //quickly append named files to archive
		append();
	} else if(!strcmp(c,"-x")){
		extract(argv);	
	} else if(!strcmp(c,"-v")){
                printf("-v\n");
        } else if(!strcmp(c,"-d")){
                printf("-d\n");
        } else if(!strcmp(c,"-A")){
                printf("-A\n");
        } else if(!strcmp(c,"-w")){
                printf("-w\n");
        } else if(!strcmp(c,"-h")){
                help();
        }  else {
		help();
	}
	
	return(0);
}
