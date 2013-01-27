#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h> //for errno variable
#include <stdio.h>
#include <stdlib.h> //for exit() function
#include <string.h>
#include <unistd.h>
#include <time.h> //for ctime() function
#include <ar.h>

#define BLOCKSIZE 1

//Function prototypes
void help();
int checkformat(char *filename);
int append(char **argv, int argc);
int appendfile(int ar_fd, char *filename);
int extract(char **argv, int argc);
int extractfile(char *filename);

void help(){ //prints usage of program
        printf("Usage: myar -key archive filename ...\n");
}

//If file does not exist or is in the wrong format, displays error message
int checkformat(char *filename){ 
	printf("Checking format of %s\n", filename);
	//Check if the file exists
	//ENOENT is the error returned if file doesn't exist and O_CREAT
	//was not specified
	int fd;
	fd = open(filename, O_RDONLY);
	if(fd == -1){
		printf("There was an error\n");
		if(errno == ENOENT){ //File definitely doesn't exist
			printf("File does not exist!\n");	
		}
		exit(1);
	} else {
		//File opened but...
		//Check if it's in the right format
		//Spefically, that it starts with !<arch>
		char buf[SARMAG]; //variable to hold "!<arch>"
		read(fd, buf, SARMAG); //SARMAG = 8
		printf("buf = %s\n", buf);
		//Broken
		//Why doesn't this work?! Newline character?
		if(strcmp(buf, ARMAG) != 0){ //ARMAG = "!<arch>\n"
			printf("Error, archive file is in the wrong format.\n");
			exit(1);
		}
	}
	return(0);
}

//Debug for append() function
int dummyfunction(int blah, char *filename){
         printf("Appending: %s\n", filename);
         return 0;
}
//end debug

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
	
	//write !<arch> at beginning of the file if it's new
	if(lseek(ar_fd, 0, SEEK_SET) == lseek(ar_fd, 0, SEEK_END)){ 
		//file offset at beginning is same as at end, file exists but is empty
		write(ar_fd, ARMAG, SARMAG);
	}
	int i;
	//debug 
	printf("argv[1] = %s\n", argv[1]);
	printf("argv[2] = %s\n", argv[2]);
	printf("argv[3] = %s\n", argv[3]);
	printf("argv[4] = %s\n", argv[4]);
	printf("argc = %d\n", argc);
	//debug
	for(i = 3; i < argc; i++){ //To do: This only works for the first case. Why?
		printf("i = %d\n", i);
	 	printf("in loop: argv[4] = %s\n", argv[4]);	
		//dummyfunction(ar_fd, argv[i]);
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
	//Get file stats
	struct stat sb; //Status buffer
	//fstat() returns information about a file referred to by an open file descriptor.
	fstat(ar_fd, &sb);
		
	struct ar_hdr fileheader;
	//To do: initialize all values in fileheader arrays to spaces
	
	//For some reason, the following strcpy line is destroying argv. Fix this! 
	//strcpy(fileheader.ar_name, strcat(filename, "/"));
	//Alternative:
	snprintf(fileheader.ar_name, 16, "%s/", filename);
	snprintf(fileheader.ar_date, 12, "%ld", sb.st_mtime); //Works, but is in right format?
	snprintf(fileheader.ar_uid, 6, "%ld", (long) sb.st_uid);
	snprintf(fileheader.ar_gid, 6, "%ld", (long) sb.st_gid);	
	snprintf(fileheader.ar_size, 10, "%lld", (long long) sb.st_size); //Gives bytes, needs to be in decimal
	strcpy(fileheader.ar_fmag, ARFMAG);
	// End get file stats

	file_size = lseek(in_fd, 0, SEEK_END); //Get the file size using the last byte of the file
	copied = 0;
	lseek(in_fd, 0, SEEK_SET);
	write(ar_fd, fileheader.ar_name, 16); //Write the filename
	write(ar_fd, fileheader.ar_date, 12); //Write the filename
	write(ar_fd, fileheader.ar_uid, 6); //Write the filename
	write(ar_fd, fileheader.ar_gid, 6); //Write the filename
	write(ar_fd, fileheader.ar_size, 10); //Write the filename
	write(ar_fd, fileheader.ar_fmag, 2); //ARFMAG = "`\n"
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
	checkformat(argv[2]);
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

int printconcise(char **argv, int argc){
	return(0);
}

int printverbose(char **argv, int argc){
	return(0);
}

int deletefiles(char **argv, int argc){
	return(0);
}

int appendcurrdirr(char **argv){
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
	} else if(!strcmp(c,"-t")){ //print a concise table of contents of the archive
                printconcise(argv, argc);
        } else if(!strcmp(c,"-v")){ //print a verbose table of contents of the archive
                printverbose(argv, argc);
        } else if(!strcmp(c,"-d")){ //delete named files from archive
                deletefiles(argv, argc);
        } else if(!strcmp(c,"-A")){ //quickly append all ``regular'' files in the current directory. (except the archive itself)
                appendcurrdirr(argv);
	} else if(!strcmp(c,"-w")){  //Extra credit: for a given timeout, add all modified files to the archive. (except the archive itself)
		return(0);
        } else if(!strcmp(c,"-h")){ //Display usage
                help();
        }  else {
		help();
	}
	
	return(0);
}
