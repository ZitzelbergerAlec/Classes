#include <stdio.h>
#include <stdlib.h>

char *scanword(char *filename);


int main(int argc, char **argv){	
	char *scanned;
	scanned = scanword(argv[1]);
	printf("Scanned = %s\n", scanned);
	return(0);
}

char *scanword(char *filename){
	FILE *fp;
	if((fp=fopen(filename, "r")) == NULL) {
    	printf("Cannot open file.\n");
    	exit(1);
  	}
	char *word = (char *) malloc(sizeof(char) * 100);
	fscanf(fp, "%[^0-9^ ]", word); //segfaults here
	return word;
}
