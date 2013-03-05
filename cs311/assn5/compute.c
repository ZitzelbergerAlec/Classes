/* Includes */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/* Function prototypes */
int is_perfect(int test_number);

int main(int argc, const char *argv[])
{
	int i;
	for(i=3; i < 100; i++){
		if(is_perfect(i))
			printf("%d is perfect\n", i);
	}
	return 0;
}

int is_perfect(int test_number)
{	
	int i;
	int sum = 1;
	for(i=2; i<test_number; i++){
		if((test_number % i) == 0){
			sum += i;
		}
	}
	if(sum == test_number)
		return 1;
	return 0;
}