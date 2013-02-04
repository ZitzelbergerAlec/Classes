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
#include <dirent.h> //for DIR constant, used in reading directories
#include <utime.h> //For utime() function to change file m_time
#include <ar.h>

#define BLOCKSIZE 1

//Function prototypes
int 	append(char **argv, int argc);
int 	appendcurrdirr(char **argv);
int 	appendfile(int ar_fd, char *filename);
int 	checkformat(char *filename);
int 	checkopen(int fd);
void 	close_archive(int ar_fd);
void 	copyFile(int in_fd, int out_fd, int offset);
int 	delete(char **argv, int argc);
int 	extract(char **argv, int argc);
int 	extractfile(int ar_fd, struct ar_hdr *header);
char 	*filePermStr(mode_t perm);
int 	findfile(int ar_fd, char *filename, int (*doSomething)(int ar_fd, struct ar_hdr *));
off_t	get_filesize(int fd);
struct 	ar_hdr *get_nextheader(int fd, int offset);
void 	help();
void 	init_archive(int ar_fd);
int 	is_nextheader(int fd, int offset);
int 	isRegularFile(char *filename);
int 	printconcise(char **argv, int argc);
void 	printconciseheader(struct ar_hdr *header);
void 	printheaders(int ar_fd, void (*printfunction)(struct ar_hdr *));
int 	printverbose(char **argv, int argc);
void 	printverboseheader(struct ar_hdr *header);
void 	PukeAndExit(char *errormessage);
struct 	ar_hdr *reconstructheader(struct ar_hdr *badheader);
void 	trimwhitespace(char *str);
int 	validatename(char *filename);
void 	writeheader(int dest_fd, struct ar_hdr *fileheader);

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
		printf("Sorry, this option is currently not supported.\n");
		return(0);
    } else if(!strcmp(c,"-h")){ //Display usage
                help();
    }  else {
		help();
	}	
	return(0);
}

//Borrowed from the book, page 296
//Converts octal permissions to a string
char *filePermStr(mode_t perm){ 
	int str_size = sizeof("rwxrwxrwx");
	char *str = (char *) malloc(sizeof(char) * str_size);
	snprintf(str, str_size, "%c%c%c%c%c%c%c%c%c",
		//User permissions
		(perm & S_IRUSR) ? 'r' : '-', 
		(perm & S_IWUSR) ? 'w' : '-',
		(perm & S_IXUSR) ? 'x' : '-',
		//Group permissions
		(perm & S_IRGRP) ? 'r' : '-', 
		(perm & S_IWGRP) ? 'w' : '-',
		(perm & S_IXGRP) ? 'x' : '-',
		//Other permissions
		(perm & S_IROTH) ? 'r' : '-', 
		(perm & S_IWOTH) ? 'w' : '-',
		(perm & S_IXOTH) ? 'x' : '-');
	return str;
}

//borrowed from http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
void trimwhitespace(char *str){
	char *end;
	// Trim leading space
   	while(*str == ' ') str++; //trim newlines also 
	
	// Trim trailing space
        end = str + strlen(str) - 1;
        while(end > str && (*end == ' ')) end--;
  
	// Write new null terminator
	*(end+1) = 0;
}

void help(){ //prints usage of program
    printf("Usage: myar -key archive filename ...\n");
	printf("commands:\n");
	printf("-q	 quickly append named files to archive\n");
	printf("-x	 extract named files\n");
	printf("-t	 print a concise table of contents of the archive\n");
	printf("-v	print a verbose table of contents of the archive\n");
	printf("-d	delete named files from archive\n");
	printf("-A	 quickly append all \"regular\" files in the current directory (except the archive itself)\n");
}

int validatename(char *filename){
	//Make sure length of filename is < 16 chars. Program only supports that 
	//in struct in header
	if(strlen(filename) > 15){
		printf("Error: filename must be less than 15 characters long.\n");
	        return(-1);
	}
	return(0);
}

int isRegularFile(char *filename){
	struct stat sb; //Status buffer
	int returnval = 1; //Default return value
	stat(filename, &sb);
	if(!S_ISREG(sb.st_mode)){
		returnval = 0;
	}
	return returnval;
}

//If file does not exist or is in the wrong format, displays error message
int checkformat(char *filename){ 
	int fd;
	fd = open(filename, O_RDONLY);
	if(fd == -1){
		printf("Error opening file\n");
		if(errno == ENOENT){ //File doesn't exist
			printf("File does not exist!\n");	
		}
		exit(1);
	} else { //File opened but check if it's in the right format
		char buf[SARMAG+1]; //variable to hold "!<arch>"
		read(fd, buf, SARMAG); //SARMAG = 8
		buf[SARMAG] = '\0';
		if(strcmp(buf, ARMAG) != 0){ //ARMAG = "!<arch>\n"
			PukeAndExit("Error, archive file is in the wrong format.\n");
		}
	}
	close(fd);
	return(0);
}

struct ar_hdr *get_nextheader(int fd, int offset){
	//Assuming that we're starting at the beginning of a line containing a 
	//header
	lseek(fd, offset, SEEK_CUR);
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
int is_nextheader(int fd, int offset){
	//preserve current file offset
	int cur_pos = lseek(fd, 0L, SEEK_CUR);
	int file_size = lseek(fd, 0, SEEK_END);
	int returnval = 1; //default return value
	if(cur_pos + offset >= (file_size - 1)){
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
	checkopen(ar_fd);

	//Initialize archive
	init_archive(ar_fd);
	
	int i;	
	for(i = 3; i < argc; i++){
		appendfile(ar_fd, argv[i]);
	}
	close_archive(ar_fd);
	return(0);
}


/*
int special_append(char **argv, int argc){
	//Anytime a file exists in the archive,
	//remove the old copy if they are not the same
	//Otherwise, don't add the new file
	
	//First, if archive exists, open it for reading
	//If it doesn't exist, open it and just append all the files in
	//the regular way and return.
	int ar_fd, temp_fd, openFlags, restorepos;
	openFlags = O_CREAT | O_WRONLY;
	temp_fd = open("temp_archive", openFlags, 0666);
	checkopen(temp_fd);
	
	//Write ARMAG to new file
	write(temp_fd, ARMAG, SARMAG);
	ar_fd = open(argv[2], O_RDONLY);
	checkopen(ar_fd);
	//get first header
	lseek(ar_fd, SARMAG, SEEK_SET);
	struct ar_hdr *header;
	
	int offset = 0;
	int match = 0;
	int i;
	int cmp_fd; //file descriptor if we find a file in archive that matches name
	while(1){
		if(is_nextheader(ar_fd, offset)){
			header = get_nextheader(ar_fd, offset);
			for(i=3;i<argc;i++){ //loop through argv arguments. If find a match, skip writing header to temp file
				if(!strcmp(header->ar_name,argv[i])){ //if found a match, skip next step
					match = 1; //found a match
					break;
				}
			}
			offset = atoi(header->ar_size);	
			offset += offset % 2; //If file offset is odd, we need to add one
			if(match){	
				match = 0; //Reset match
				cmp_fd = open(argv[i], O_RDONLY);
				//Compare the files based on modification time, etc
				
			}
			copyFile(ar_fd, temp_fd, offset);
			offset = 0; //because we just moved the file descriptor through the previous offset
		} else {
			break;
		}							
	}
	close_archive(ar_fd);
	unlink(argv[2]); //delete current archive
	rename("temp_archive", argv[2]);
	close_archive(temp_fd);
	return(0);
}
*/

void close_archive(int ar_fd){
	if(close(ar_fd) == -1){
		PukeAndExit("Error closing file\n");
	}
}

void PukeAndExit(char *errormessage){
	perror(errormessage);
	exit(-1);
}

off_t get_filesize(int fd){
	off_t file_size;
	int curpos;
	curpos = lseek(fd, 0L, SEEK_CUR); //Save current position
	file_size = lseek(fd, 0, SEEK_END); //Get the file size using the last byte of the file
	lseek(fd, curpos, SEEK_SET); //Restore current position
	return file_size;
}


int appendfile(int ar_fd, char *filename){
	if(validatename(filename) == -1) //invalid name
		return(-1);
	if(!isRegularFile(filename)){ //Not a regular file
		printf("Error: %s is not a regular file\n", filename);
		return(-1);
	}
	int in_fd; //File descriptor for file to append
	printf("Appending: %s\n", filename);
	in_fd = open(filename, O_RDONLY);
	checkopen(in_fd);
	//Get file stats
	struct stat sb; //Status buffer
	fstat(in_fd, &sb);	
	struct ar_hdr fileheader;	
	char str[16];
	strcpy(str, filename);
	strcat(str, "/");	
	snprintf(fileheader.ar_name, 60, "%-16s%-12ld%-6ld%-6ld%-8lo%-10lld", str,
	sb.st_mtime, (long) sb.st_uid, (long) sb.st_gid, (unsigned long) sb.st_mode,
	(long long) sb.st_size); 
	strcpy(fileheader.ar_fmag, ARFMAG);
	lseek(in_fd, 0, SEEK_SET);
	writeheader(ar_fd, &fileheader);
	copyFile(in_fd, ar_fd, get_filesize(in_fd));
	close(in_fd);	
	return 0;
}

void copyFile(int in_fd, int out_fd, int offset){ 
	//Copies the contents of in_fd to out_fd for amount of offset 
	char buf[BLOCKSIZE];
	off_t copied = 0;
	int num_read = 0;
	int num_written = 0;
	while(copied < offset){
		num_read = read(in_fd, buf, BLOCKSIZE);
		num_written = write(out_fd, buf, BLOCKSIZE);

		if (num_read != num_written){
			PukeAndExit("Error writing file");
		}
		copied += num_written;
		lseek(in_fd, 0, SEEK_CUR); //Changed this to move forward
	}
}

struct ar_hdr *reconstructheader(struct ar_hdr *badheader){ //takes an unformatted ar_hdr and formats it
	struct ar_hdr *goodheader = malloc(sizeof(struct ar_hdr)); 
	char str[16];
	strcpy(str, badheader->ar_name);
	strcat(str, "/");
	snprintf(goodheader->ar_name, 60, "%-16s%-12s%-6s%-6s%-8s%-10s", str,
	badheader->ar_date, badheader->ar_uid, badheader->ar_gid, badheader->ar_mode,
	badheader->ar_size);
	strcpy(goodheader->ar_fmag, ARFMAG);
	return goodheader;
}

void writeheader(int dest_fd, struct ar_hdr *fileheader){
	write(dest_fd, fileheader->ar_name, 16); 
	write(dest_fd, fileheader->ar_date, 12);
	write(dest_fd, fileheader->ar_uid,   6);
	write(dest_fd, fileheader->ar_gid,   6);
	write(dest_fd, fileheader->ar_mode,  8);
	write(dest_fd, fileheader->ar_size, 10);
	write(dest_fd, fileheader->ar_fmag,  2); //ARFMAG = "`\n"
}

int checkopen(int fd){ //Makes sure a file opened correctly
	if (fd == -1){
		PukeAndExit("Error opening file.\n");
	}
	return(0);
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
		findfile(ar_fd, argv[i], &extractfile);
	}
	close(ar_fd);
	return(0);
}

int extractfile(int ar_fd, struct ar_hdr *header){
	//Preserve current file offset in archive
	//get_nextheader() depends on this
	int restore_pos = lseek(ar_fd, 0L, SEEK_CUR);
	printf("Extracting: %s\n", header->ar_name);
	int openFlags, outFile;
	openFlags = O_CREAT | O_WRONLY;
	//convert char header->ar_mode to long
	unsigned long ulmode;
	ulmode = strtoul(header->ar_mode, NULL, 0);
	outFile = open(header->ar_name, openFlags, ulmode);
	checkopen(outFile);
	copyFile(ar_fd, outFile, atoi(header->ar_size));
	//Change modification and access time (borrowed from the book, page 288)
	time_t seconds; 
	seconds = (time_t) strtol(header->ar_date, NULL, 0);
	struct stat sb;
	struct utimbuf utb;

	if (stat(header->ar_name, &sb) == -1)
		exit(-1);
	utb.actime = seconds;
	utb.modtime = seconds;
	if (utime(header->ar_name, &utb) == -1)
		exit(-1);	
	long uid, gid;
	uid = strtol(header->ar_uid, NULL, 0);
	gid = strtol(header->ar_gid, NULL, 0);	
	chown(header->ar_name, uid, gid);
	//Change permissions
	chmod(header->ar_name, strtoul(header->ar_mode, NULL, 8));
	close(outFile);
	
	//restore file offset of ar_fd
	lseek(ar_fd, restore_pos, SEEK_SET);
	return(0);
}

void printheaders(int ar_fd, void (*printfunction)(struct ar_hdr *)){
	lseek(ar_fd, SARMAG, SEEK_SET); //move file offset to first header
	struct ar_hdr *header;
	int offset;
	offset = 0; //because we're at the first header
	char buf[BLOCKSIZE];
	while(1){
		if(is_nextheader(ar_fd, offset)){	
			header = get_nextheader(ar_fd, offset);
			printfunction(header);
			offset = atoi(header->ar_size);
			offset += offset % 2; //If file offset is odd, we need to add one
		} else {
			break;
		}	
	}
}

void printconciseheader(struct ar_hdr *header){
			printf("%s\n", header->ar_name);
}

int printconcise(char **argv, int argc){
	checkformat(argv[2]);
	int ar_fd;
	ar_fd = open(argv[2], O_RDONLY);	
	checkopen(ar_fd);
	printheaders(ar_fd, &printconciseheader);
	close(ar_fd);
	return(0);
}

void printverboseheader(struct ar_hdr *header){
	char *permsString = filePermStr(strtoul(header->ar_mode, NULL, 8));
	char buf[1000];
	time_t seconds; 
	seconds = (time_t) strtol(header->ar_date, NULL, 0);
	struct tm *mytime = localtime(&seconds);
	strftime(buf, 1000, "%b %d %H:%M %Y", mytime);
	printf("%s %s/%s     %s%s %s\n", permsString, header->ar_uid, 
	header->ar_gid, header->ar_size, buf, header->ar_name);
}

int printverbose(char **argv, int argc){
	checkformat(argv[2]);
	int ar_fd;
	ar_fd = open(argv[2], O_RDONLY);
        printheaders(ar_fd, &printverboseheader);
	close(ar_fd);	
	return(0);
}

//This function takes a function as a parameter. Used for deleting, extracting
//files, etc.
int findfile(int ar_fd, char *filename, int (*doSomething)(int ar_fd, struct ar_hdr *)){
	if(validatename(filename) == -1) //invalid name
                 return(-1);
	int offset;	
	struct ar_hdr *header;
	lseek(ar_fd, SARMAG, SEEK_SET); //move file offset to first header
	offset = 0;
	while(1){
		if(is_nextheader(ar_fd, offset)){	
			header = get_nextheader(ar_fd, offset);
			if(!strcmp(header->ar_name,filename)){ //found a match
				doSomething(ar_fd, header);
				return(0);
			}
			offset = atoi(header->ar_size);
			offset += offset % 2; //If file offset is odd, we need to add one
		} else {
			break;
		}
	}
	printf("Sorry, did not find %s\n", filename);
	return(0);
}

int delete(char **argv, int argc){
	//Opens a temporary archive file for writing.
	//Gets a header, loops through argv. If it finds a match, gets the next
	//header. If not, writes the file to the new archive. Then gets next
	//header. 
	checkformat(argv[2]);
	int ar_fd, temp_fd, openFlags, restorepos;
	openFlags = O_CREAT | O_WRONLY;
	temp_fd = open("temp_archive", openFlags, 0666);
	checkopen(temp_fd);
	
	//Write ARMAG to new file
	write(temp_fd, ARMAG, SARMAG);
	ar_fd = open(argv[2], O_RDONLY);	
	checkopen(ar_fd);
	//get first header
	lseek(ar_fd, SARMAG, SEEK_SET);
	struct ar_hdr *header;
	
	int offset = 0;
	int match = 0;
	int i;
	while(1){
		if(is_nextheader(ar_fd, offset)){
			header = get_nextheader(ar_fd, offset);
			for(i=3;i<argc;i++){ //loop through argv arguments. If find a match, skip writing header to temp file
				if(!strcmp(header->ar_name,argv[i])){ //if found a match, skip next step
					match = 1; //found a match
					break;
				}
			}
			offset = atoi(header->ar_size);	
			offset += offset % 2; //If file offset is odd, we need to add one
			if(!match){	
				struct ar_hdr *goodheader = reconstructheader(header);
				writeheader(temp_fd, goodheader);
			} else {
				match = 0; //reset match
				continue;
			}
			copyFile(ar_fd, temp_fd, offset);
			offset = 0; //because we just moved the file descriptor through the previous offset
		} else {
			break;
		}							
	}
	close_archive(ar_fd);
	unlink(argv[2]); //delete current archive
	rename("temp_archive", argv[2]);
	close_archive(temp_fd);
	return(0);
}

void init_archive(int ar_fd){
	//write ARMAG at beginning of the file if it's new
	if(lseek(ar_fd, 0, SEEK_SET) == lseek(ar_fd, 0, SEEK_END)){ 
		//file offset at beg is same as at end, file exists but is empty
		write(ar_fd, ARMAG, SARMAG);
	}
}

int appendcurrdirr(char **argv){
	//Get list of files in current directory
	char cwd[1024];
	DIR *dirp;
	struct dirent *dp;
	struct stat sb; //Status buffer
	int openFlags, ar_fd;
	openFlags = O_CREAT | O_WRONLY | O_APPEND;
	ar_fd = open(argv[2], openFlags, 0666);	
	checkopen(ar_fd);
	init_archive(ar_fd);
	dirp = opendir("."); //open current working directory
	while(1){
		dp = readdir(dirp);
		if(dp == NULL)
			break;
		stat(dp->d_name, &sb);
		//Skip if file is regular, is the name of the archive, name of 
		//source code, or is name of program
		if (!S_ISREG(sb.st_mode) || !strcmp(argv[2], dp->d_name) || 
		!strcmp(argv[0], dp->d_name) || !strcmp("myar.c", dp->d_name))
			continue; //file is not regular
		appendfile(ar_fd, dp->d_name);
	}
	close_archive(ar_fd);
	return(0);
}
