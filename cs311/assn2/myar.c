#include <assert.h>
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
int extractfile(int ar_fd, char *filename);
int printconcise(char **argv, int argc);
int printverbose(char **argv, int argc);
int deletefiles(char **argv, int argc);
int appendcurrdirr(char **argv);
int validatename(char *filename);

//borrowed from http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
void trimwhitespace(char *str){
	char *end;
	// Trim leading space
   	while(*str == ' ') str++; 
	
	// Trim trailing space
        end = str + strlen(str) - 1;
        while(end > str && (*end == ' ')) end--;
  
	// Write new null terminator
	*(end+1) = 0;
}

void help(){ //prints usage of program
        printf("Usage: myar -key archive filename ...\n");
}

int validatename(char *filename){
	//Make sure length of filename is < 16 chars. Program only supports that 
	//in struct in header
	if(strlen(filename) > 15){
		printf("Error: filename must be less than 15 characters long");
	        return(-1);
	}
	return(0);
}

//If file does not exist or is in the wrong format, displays error message
int checkformat(char *filename){ 
	int fd;
	fd = open(filename, O_RDONLY);
	if(fd == -1){
		printf("There was an error\n");
		if(errno == ENOENT){ //File doesn't exist
			printf("File does not exist!\n");	
		}
		exit(1);
	} else { //File opened but check if it's in the right format
		char buf[SARMAG+1]; //variable to hold "!<arch>"
		read(fd, buf, SARMAG); //SARMAG = 8
		buf[SARMAG] = '\0';
		if(strcmp(buf, ARMAG) != 0){ //ARMAG = "!<arch>\n"
			printf("Error, archive file is in the wrong format.\n");
			exit(1);
		}
	}
	close(fd);
	return(0);
}

struct ar_hdr *get_nextheader(int fd){
	//Assuming that we're starting at the beginning of a line containing a 
	//header
	struct ar_hdr *phdr = (struct ar_hdr *) malloc(sizeof(struct ar_hdr));
	read(fd, phdr, sizeof(struct ar_hdr));
	
	//Set NULL terminator character on all the strings
	int i;
	for(i=0; i<16; i++){
		if(phdr->ar_name[i] == '/')
			break;
	}
	phdr->ar_name[i]  = '\0';
	phdr->ar_date[11] = '\0';
	phdr->ar_uid[5]   = '\0';
	phdr->ar_gid[5]   = '\0';
	phdr->ar_mode[7]  = '\0';
	phdr->ar_size[9]  = '\0';
	
	//Trim whitespace
	trimwhitespace(phdr->ar_date);	
	trimwhitespace(phdr->ar_uid);
	trimwhitespace(phdr->ar_gid);
	trimwhitespace(phdr->ar_mode);	
	return(phdr);
}

//Returns 1 if there is a next header, 0 if not
//For iterating through headers in conjunction with get_nextheader
//Offset is the offset of the next header, as determined by the current header's
//filesize
//To do: untested function. Test it!.
int is_nextheader(int fd, int offset){
	//preserve current file offset
	int cur_pos = lseek(fd, 0L, SEEK_CUR);
	int file_size = lseek(fd, 0, SEEK_END);
	int returnval = 1; //default return value
	if(cur_pos + offset >= (file_size - 1)){ //to do: Something's wrong here
		returnval = 0;
	}
	//return to previous file offset
	lseek(fd, cur_pos, SEEK_SET);
	return returnval;
}

int append(char **argv, int argc){
	//Create the archive file if it doesn't exist
	int openFlags, ar_fd;
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
		//file offset at beg is same as at end, file exists but is empty
		write(ar_fd, ARMAG, SARMAG);
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
	if(validatename(filename) == -1) //invalid name
		return(-1);
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
	fstat(ar_fd, &sb);	
	struct ar_hdr fileheader;
  	
	//To do: the following line isn't appending "/" to filename
	char str[16];
	strcpy(str, filename);
	strcat(str, "/");	
	snprintf(fileheader.ar_name, 16, "%-16s", str); //%-16s pads the string to the right with 16 spaces
	snprintf(fileheader.ar_date, 12, "% -12ld", sb.st_mtime); //Works, but is in right format?
	snprintf(fileheader.ar_uid, 7, "% -5ld", (long) sb.st_uid); //Pads a long with spaces
	snprintf(fileheader.ar_gid, 7, "% -5ld", (long) sb.st_gid);	
	snprintf(fileheader.ar_size, 11, "% -10lld", (long long) sb.st_size); //Gives bytes, needs to be in decimal
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
	int ar_fd; //file descriptor for archive
	int i;
	ar_fd = open(argv[2], O_RDONLY);
	for(i=3; i < argc; i++){
		extractfile(ar_fd, argv[i]);
	}
	close(ar_fd);
	return(0);
}

int extractfile(int ar_fd, char *filename){
	if(validatename(filename) == -1) //invalid name
                 return(-1);
	printf("Extracting: %s\n", filename);
	
	struct ar_hdr *header;
	int offset;
	//get the first header and offset
	lseek(ar_fd, SARMAG, SEEK_SET); //move file offset to first header
	header = get_nextheader(ar_fd);
	printf("Header = %s\n", header->ar_name);
	if(!strcmp(header->ar_name,filename)){
		printf("Found %s\n", filename);
		return(0);
	}	
	offset = atoi(header->ar_size);
	while(1){
		if(is_nextheader(ar_fd, offset)){	
			lseek(ar_fd, atoi(header->ar_size), SEEK_CUR);
			header = get_nextheader(ar_fd);
			if(!strcmp(header->ar_name,filename)){
				printf("Found %s\n", filename);
				return(0);
			}
			offset = atoi(header->ar_size);
		} else {
			break;
		}
	}
	printf("Sorry, did not find %s\n", filename);
	return(0);
}

int printconcise(char **argv, int argc){
	checkformat(argv[2]);
	int ar_fd;
	ar_fd = open(argv[2], O_RDONLY);
	lseek(ar_fd, SARMAG, SEEK_SET); //move file offset to first header
	struct ar_hdr *header;
	int offset;
	//get the first header and offset
	header = get_nextheader(ar_fd);
	printf("Files in archive:\n");
	printf("%s\n", header->ar_name);
	offset = atoi(header->ar_size);
	while(1){
		if(is_nextheader(ar_fd, offset)){
			lseek(ar_fd, atoi(header->ar_size), SEEK_CUR);
			header = get_nextheader(ar_fd);
			printf("%s\n", header->ar_name);
			offset = atoi(header->ar_size);
		} else {
			break;
		}	
	}
	close(ar_fd);
	return(0);
}

int printverbose(char **argv, int argc){
	checkformat(argv[2]);
	int ar_fd;
	ar_fd = open(argv[2], O_RDONLY);
	lseek(ar_fd, SARMAG, SEEK_SET); //move file offset to first header
	struct ar_hdr *header;
	int offset;
         //get the first header and offset
        header = get_nextheader(ar_fd);
        printf("Files in archive:\n");
        printf("%s, %s, %s, %s, %s, %s\n", header->ar_name,
        header->ar_date, header->ar_uid, header->ar_gid,
        header->ar_mode, header->ar_size);
	offset = atoi(header->ar_size);
        while(1){
                if(is_nextheader(ar_fd, offset)){ 
                       	lseek(ar_fd, atoi(header->ar_size), SEEK_CUR);
                       	header = get_nextheader(ar_fd);
                       	printf("%s, %s, %s, %s, %s, %s\n", header->ar_name, 
			header->ar_date, header->ar_uid, header->ar_gid, 
			header->ar_mode, header->ar_size);
                        offset = atoi(header->ar_size);
                } else {
                       	break;
                }       
       	}       
        close(ar_fd);	
	return(0);
}

//This function takes a function as a parameter. Used for deleting, extracting
//files, etc.
int findfile(int ar_fd, char *filename, int (*doSomething)(struct ar_hdr *)){
	if(validatename(filename) == -1) //invalid name
                 return(-1);
	int offset;	
	struct ar_hdr *header;
	//get the first header and offset
	lseek(ar_fd, SARMAG, SEEK_SET); //move file offset to first header
	header = get_nextheader(ar_fd);
	printf("Header = %s\n", header->ar_name);
	if(!strcmp(header->ar_name,filename)){
		printf("Found %s\n", filename);
		return(0);
	}	
	offset = atoi(header->ar_size);
	while(1){
		if(is_nextheader(ar_fd, offset)){	
			lseek(ar_fd, atoi(header->ar_size), SEEK_CUR);
			header = get_nextheader(ar_fd);
			if(!strcmp(header->ar_name,filename)){
				doSomething(header);
				return(0);
			}
			offset = atoi(header->ar_size);
		} else {
			break;
		}
	}
	printf("Sorry, did not find %s\n", filename);
	return(0);
}

int deletefile(struct ar_hdr *header){
	printf("In deletefile. Header name = %s\n", header->ar_name);	
	return(0);
}

int delete(char **argv, int argc){
	//To do: Right now is same as extract().
	checkformat(argv[2]);
	if(argc < 4){
		printf("Please name files to extract\n");
		help();
		return(-1); //indicating error
	}
	int ar_fd; //file descriptor for archive
	int i;
	ar_fd = open(argv[2], O_RDONLY);
	for(i=3; i < argc; i++){
		findfile(ar_fd, argv[i], &deletefile);
	}
	close(ar_fd);
	return(0);
}

int appendcurrdirr(char **argv){
	return(0);
}

//To do: Does this need to check for multiple flags?
//To do: to make functions more robust, have them take in an archive filename 
//instead of argv.
int main(int argc, char* argv[]){
	//Order of arguments:
	//myar key afile name ...
	// where key is "-q" etc
	if(argc < 3){	
		help();
		return 0;
	}
	char *c;
	c = argv[1];
	if(!strcmp(c,"-q")){ //quickly append named files to archive
		append(argv, argc);
	} else if(!strcmp(c,"-x")){ //extract named files
		extract(argv, argc);	
	} else if(!strcmp(c,"-t")){ 
		//print a concise table of contents of the archive
                printconcise(argv, argc);
        } else if(!strcmp(c,"-v")){ 
		//print a verbose table of contents of the archive
                printverbose(argv, argc);
        } else if(!strcmp(c,"-d")){ //delete named files from archive
                delete(argv, argc);
        } else if(!strcmp(c,"-A")){ 
		//quickly append all ``regular'' files in the current directory.
		//(except the archive itself)
                appendcurrdirr(argv);
	} else if(!strcmp(c,"-w")){  
		//Extra credit: for a given timeout, add all modified 
		//files to the archive. (except the archive itself)
		return(0);
        } else if(!strcmp(c,"-h")){ //Display usage
                help();
        }  else {
		help();
	}	
	return(0);
}
