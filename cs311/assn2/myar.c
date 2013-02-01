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
#include <ar.h>

#define BLOCKSIZE 1
#define FP_SPECIAL 1 //for filePermStr() to work
#ifndef S_ISVTX
#define S_ISVTX  0001000
#endif

//Function prototypes
void help();
int checkformat(char *filename);
void writeheader(int dest_fd, struct ar_hdr *fileheader);
int append(char **argv, int argc);
int appendfile(int ar_fd, char *filename);
int extract(char **argv, int argc);
int printconcise(char **argv, int argc);
int printverbose(char **argv, int argc);
int appendcurrdirr(char **argv);
int validatename(char *filename);
int findfile(int ar_fd, char *filename, int (*doSomething)(int ar_fd, struct ar_hdr *));
int extractfile(int ar_fd, struct ar_hdr *header);
char *filePermStr(mode_t perm, int flags);
void init_archive(int ar_fd);
void close_archive(int ar_fd);

//borrowed from the book, page 296
//Converts octal permissions to a string
//To do: test this function!
char *filePermStr(mode_t perm, int flags){ //If flag is set, prints special permissions.
	int str_size = sizeof("rwxrwxrwx");
	char *str = (char *)malloc(sizeof(char) * str_size);
	snprintf(str, str_size, "%c%c%c%c%c%c%c%c%c",
	(perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
	(perm & S_IXUSR) ?
		(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
		(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	(perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
	(perm & S_IXGRP) ?
		(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
		(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	(perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
	(perm & S_IXOTH) ?
		(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
		(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));
	return str;
}

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
	//Puke and exit
	if(ar_fd == -1){
		perror("Can't open archive file");
		exit(-1);
	}

	//Initialize archive
	init_archive(ar_fd);
	
	int i;	
	for(i = 3; i < argc; i++){
		appendfile(ar_fd, argv[i]);
	}
	close_archive(ar_fd);
	return(0);
}

void close_archive(int ar_fd){
	if(close(ar_fd) == -1){ //File is now closed
		//errExit("close"); //this is undefined...?
		printf("error closing file");
	}
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
	//To do: This isn't formatting shit correctly. Puts a goddamn ^@ in front of everything.
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
	// End get file stats

	file_size = lseek(in_fd, 0, SEEK_END); //Get the file size using the last byte of the file
	copied = 0;
	lseek(in_fd, 0, SEEK_SET);
	writeheader(ar_fd, &fileheader);
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


struct ar_hdr *reconstructheader(struct ar_hdr *badheader){ //takes an unformatted ar_hdr and formats it
	struct ar_hdr *goodheader = malloc(sizeof(struct ar_hdr)); 
	char str[16];
	strcpy(str, badheader->ar_name);
	strcat(str, "/");
	snprintf(goodheader->ar_name, 60, "%-16s%-12s%-7s%-7s%-7s%-11s", str,
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
	char buf[BLOCKSIZE];
	int openFlags, outFile;
	openFlags = O_CREAT | O_WRONLY;
	//convert char header->ar_mode to long
	unsigned long ulmode;
	ulmode = strtoul(header->ar_mode, NULL, 0);
	outFile = open(header->ar_name, openFlags, ulmode);
	
	int num_read;
	int num_written;
	off_t copied;
	copied = 0;	
	while(copied < atoi(header->ar_size)){
		num_read = read(ar_fd, buf, BLOCKSIZE);
		num_written = write(outFile, buf, BLOCKSIZE);

		if(num_read != num_written){
			perror("Error writing file");
			//unlink(output);
			exit(-1);
		}
		copied += num_written;
		lseek(ar_fd, 0, SEEK_CUR);
	}
	//To do: change modification time
	long uid, gid;
	uid = strtol(header->ar_uid, NULL, 0);
	gid = strtol(header->ar_gid, NULL, 0);	
	chown(header->ar_name, uid, gid);
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
	while(1){
		if(is_nextheader(ar_fd, offset)){	
			lseek(ar_fd, offset, SEEK_CUR);
			header = get_nextheader(ar_fd);
			printfunction(header);
			offset = atoi(header->ar_size);
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
	printheaders(ar_fd, &printconciseheader);
	close(ar_fd);
	return(0);
}

void printverboseheader(struct ar_hdr *header){
	char *permsString = filePermStr(strtol(header->ar_mode, NULL, 0), 0);
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
			lseek(ar_fd, offset, SEEK_CUR);
			header = get_nextheader(ar_fd);
			if(!strcmp(header->ar_name,filename)){ //found a match
				doSomething(ar_fd, header);
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

int delete(char **argv, int argc){
	//To do: finish this function. Same as findfile now.
	//The way this will work:
	//Open a temporary archive file for writing.
	//Get a header, loop through argv. If it finds a match, get the next
	//header. If not, write the file to the new archive. Then get next
	//header. 
	int ar_fd, temp_fd, openFlags, restorepos;
	openFlags = O_CREAT | O_WRONLY;
	temp_fd = open("temp_archive", openFlags, 0666);
	
	//prepare to copy
	char buf[BLOCKSIZE];	
	int num_read;
	int num_written;
	off_t copied;	
	
	//Write ARMAG to new file
	write(temp_fd, ARMAG, SARMAG);
	ar_fd = open(argv[2], O_RDONLY);	
	
	//get first header
	lseek(ar_fd, SARMAG, SEEK_SET);
	//restorepos = lseek(ar_fd, 0L, SEEK_CUR); //Get restore position
	struct ar_hdr *header;
	
	int offset = 0;
	int match = 0;
	int i;
	while(1){
		if(is_nextheader(ar_fd, offset)){
			lseek(ar_fd, offset, SEEK_CUR);
			header = get_nextheader(ar_fd);
			for(i=3;i<argc;i++){ //loop through argv arguments. If find a match, skip writing header to temp file
				if(!strcmp(header->ar_name,argv[i])){ //if found a match, skip next step
					match = 1; //found a match
					break;
				}
			}
			offset = atoi(header->ar_size);	
			if(!match){	
				struct ar_hdr *goodheader = reconstructheader(header);
				writeheader(temp_fd, goodheader);
			} else {
				match = 0; //reset match
				continue;
			}
			while(num_written < offset){ //copy the file
				num_read = read(ar_fd, buf, BLOCKSIZE);
				num_written += write(temp_fd, buf, BLOCKSIZE);	
			}
			offset = 0; //because we just moved the file descriptor through the previous offset
		} else {
			break;
		}							
	}
	close_archive(ar_fd);
	unlink(argv[2]); //delete current archive
	rename("temp_archive", argv[2]);
	close(temp_fd);
	return(0);
}

void init_archive(int ar_fd){
	//write !<arch> at beginning of the file if it's new
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
