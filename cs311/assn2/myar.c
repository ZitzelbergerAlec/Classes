#include <stdio.h>

//added this comment
int append(int numfiles){
	return(0);
}


int main(int argc, char* argv[]){
	int c;
	char *cvalue = NULL;
	while((c = getopt(argc, argv, "q:x:tvd:Aw:")) != -1){
		switch(c){
			case 'q':
				cvalue = optarg;
				printf("cvalue = %s\n", cvalue);
				break;
			case 'x':	
				printf("You entered x!\n");
				break;
			case 't':	
				printf("You entered t!\n");
				break;
			case 'v':	
				printf("You entered v!\n");
				break;
			case 'd':	
				printf("You entered d!\n");
				break;
			case 'A':	
				printf("You entered A!\n");
				break;
			case 'w':	
				printf("You entered w!\n");
				break;
			default:
				printf("You didn't enter anything!\n");
				break;
		}
	}
	return(0);
}
