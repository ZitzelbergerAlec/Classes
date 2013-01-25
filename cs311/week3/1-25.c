#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>


int main(){
	char *input = argv[1];
	char *output = argv[2];

	int in_fd;
	int out_fd;
	
	char buf[BLOCKSIZE];

	int num_read;
	int num_written;
	
	int file_size;
	int num_written;

	int file_size;
	int copied;

	in_fd = open(input, O_RDONLY);
	if(in_fd == -1){ //puke and error
		perror("Can't open input file");
		exit(-1);
	}

	out_fd = open(output, O_WRONLY | O_CREAT, 0777); // leading 0 means octal
	
	if(in_fd == -1){ //puke and error
                perror("Can't open input file");
                exit(-1);
        }

	file_size = lseek(in_fd, SEEK_END, 0);
	lseek(in_fd, SEEK_END, -1);
	copied = 0;
	while(copied < file_size){
		num_read = read(in_fd, buf, BLOCKSIZE);
		num_written = write(out_fd, buf, BLOCKSIZE);
		
		if(num_read != num_written){
			perror("Error writing file");
			unlink(output); //deletes file descriptor. Unlink is system call that "rm" uses.
			exit(-1);
		}

		copied += num_written;
		lseek(in_fd, -2, SEEK_CUR);
	}
	
	return 0;
}
