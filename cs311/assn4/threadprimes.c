/* Compiler directives */
#define _POSIX_SOURCE
#define _BSD_SOURCE

/* Includes */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/* Function prototypes */
void *mount_shmem(char *path, int object_size);


int main(int argc, char **argv)
{

	unsigned char *bitmap;
	int bitmap_size = 10000 / 8 + 1;

	int *num_array;

	/* Create a shared memory object */
	void *addr = mount_shmem("/merrickd_primes", 10000*sizeof(int)); //Find primes in first 10k numbers

	bitmap = (unsigned char*) addr;
      	num_array = (int *) (bitmap + bitmap_size);

      	/* Find the prime numbers */
      	/* Initialize prime_array numbers to 1 */
      	int i;
      	for(i=0;i<=10000;i++){
		num_array[i] = 1;
	}
	int k = 1; //initialize k to 1
	int numprimes = 10000;
	while(k <= floor(sqrt(10000))){
		k++;
		if(num_array[k-1] == 1){ //Check the k-1 spot because arrays are indexed starting at zero
			i = 2;
			while(k*i <= 10000){
				if(!(num_array[k*i-1] == 0)){
					numprimes--;
				}
				num_array[k*i-1] = 0;
				i++;
			}
		}
	}

	/* Print the primes */
	for(i=0;i<10000;i++){
		if(num_array[i] == 1)
			printf("%d\n", i);
	}

	printf("Number of primes found is %d\n", numprimes);

	/* Delete the shared memory object */
	if(shm_unlink("/merrickd_primes") == -1){
                  printf("Error deleting shared memory object");
                  exit(EXIT_FAILURE);
        }

	return 0;
}

/* Mounts a shared memory object. Copied from code from class */
void *mount_shmem(char *path, int object_size)
{
	int shmem_fd;
	void *addr;

	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if(shmem_fd == -1){
		fprintf(stdout, "Failed to open shared memory object\n");
		exit(EXIT_FAILURE);
	}

	if(ftruncate(shmem_fd, object_size) == -1){
		fprintf(stdout, "Failed to resize shared memory object\n");
		exit(EXIT_FAILURE);
	}

	/* map the shared memory object */
	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);

	if(addr == MAP_FAILED){
		fprintf(stdout, "Failed to map shared memory object\n");
		exit(EXIT_FAILURE);
	}

	return addr;
}
