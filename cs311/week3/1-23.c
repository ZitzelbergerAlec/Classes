#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv){
	char buf[1000];
	int num_read;
	int fd = open("ls.py", O_RDONLY);
	
	if(fd == -1){
		//something bad happened
	}
	
	while(1){
		num_read = read(fd, buf, 7);
	}

	return 0;
}
