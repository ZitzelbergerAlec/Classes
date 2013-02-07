/*
 * Notes:
 * cast everything to lowercase
 * have command line argument for number of processes
 * 
 *
 */ 




#include <stdio.h>
#include <stdlib.h>

void help();

int main(int argc, char **argv){	
	FILE *fp;
	if((fp=fopen(argv[1], "r")) == NULL) {
    	printf("Cannot open file.\n");
    	exit(1);
  	}
	char word[100];
	int output;
	output = 1;
	while(fscanf(fp, "%[^0-9,\t\n\v\f]", word) > 0){
		printf("%s\n", word);
	}	
	return(0);
}

void help(){
	printf("Usage:\n");
	printf("uniqify [filename] [number of sorting processes]\n");
	exit(0);
}
