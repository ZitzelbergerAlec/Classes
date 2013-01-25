#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCKSIZE 1

void help(){ //prints usage of program
        printf("Usage: myar -key archive filename ...\n");
}

int append(char **argv, int argc){
	//Create the archive file if it doesn't exist
	//argv[2] is the archive file to use
	int openFlags, ar_fd;
	// O_CREAT: Create file if it doesn’t already exist.
	// O_WRONLY: Write only.
	// O_APPEND: Writes only to the end of the file.
	openFlags = O_CREAT | O_WRONLY | O_APPEND;
	
	//Use permissions 666
	ar_fd = open(argv[2], openFlags, 0666);	
	//Puke and exit
	if(ar_fd == -1){
		perror("Can't open archive file");
		exit(-1);
	}
	int i;
	for(i = 3; i < argc; i++){
		appendfile(ar_fd, argv[i]);
	}
	
	if (close(ar_fd) == -1){ //File is now closed
		//errExit("close"); //this is undefined...?
		printf("error closing file");
	}
	return(0);
}

int appendfile(int ar_fd, char *filename){
	//The following code was borrowed from an in-class example
	//but the loop was written backwards where indicated to work properly
	int in_fd; //File descriptor for file to append
	char buf[BLOCKSIZE];
	printf("Appending: %s\n", filename);

	int num_read;
	int num_written;

	off_t file_size;
	off_t copied;

	in_fd = open(filename, O_RDONLY);
	//puke and exit
	if(in_fd == -1){
		perror("Can't open input file");
		exit(-1);
	}
	
	file_size = lseek(in_fd, 0, SEEK_END); //Get the file size using the last byte of the file
	copied = 0;
	lseek(in_fd, 0, SEEK_SET);
	
	while(copied < file_size){
		num_read = read(in_fd, buf, BLOCKSIZE);
		num_written = write(ar_fd, buf, BLOCKSIZE);

		if (num_read != num_written){
			perror("Error writing file");
			//unlink(output);
			exit(-1);
		}

		copied += num_written;

		lseek(in_fd, 0, SEEK_CUR); //Changed this to move forward
	}
	close(in_fd);	
	return 0;
	
}


int extract(char **argv, int argc){
	//printf("Extracting from archive: %s\n", filename);
	if(argc < 4){
		printf("Please name files to extract\n");
		help();
		return(-1); //indicating error
	}
	printf("Extracting...\n");
	//Archive file to use is argv[2]	
	int i;
	for(i=3; i < argc; i++){
		extractfile(argv[i]);
	}
	return(0);
}

int extractfile(char *filename){
	printf("Extracting: %s\n", filename);
	return(0);
}

int main(int argc, char* argv[]){
	//Order of arguments:
	//myar key afile name ...
	// where key is "-q" etc
	if(argc < 2){
		help();
		return 0;
	}
	char *c;
	c = argv[1];
	if(!strcmp(c,"-q")){ //quickly append named files to archive
		append(argv, argc);
	} else if(!strcmp(c,"-x")){ //extract named files
		extract(argv, argc);	
	} else if(!strcmp(c,"-v")){ //print a concise table of contents of the archive
                printf("-v\n");
        } else if(!strcmp(c,"-d")){ //print a verbose table of contents of the archive
                printf("-d\n");
        } else if(!strcmp(c,"-A")){ //delete named files from archive
                printf("-A\n");
        } else if(!strcmp(c,"-w")){ //quickly append all ``regular'' files in the current directory. (except the archive itself)
                printf("-w\n");
        } else if(!strcmp(c,"-h")){ //Extra credit: for a given timeout, add all modified files to the archive. (except the archive itself)
                help();
        }  else {
		help();
	}
	
	return(0);
}
