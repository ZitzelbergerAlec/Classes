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
void find_primes(int *num_array, int min, int max, int offset);
void help();
void init_num_array(int *num_array);
void *mount_shmem(char *path, int object_size);
void *process_primes_thread(void * vp);
void puke_and_exit(char *errormessage);

/* Global variables */
int num_primes;
int *num_array;
int num_threads;

int main(int argc, char **argv)
{
	/*
	if(argc < 3)
		help();
	*/	

	num_threads = 2;

	unsigned char *bitmap;
	int bitmap_size = 10000 / 8 + 1;

	/* Create a shared memory object */
	void *addr = mount_shmem("/merrickd_primes", 10000*sizeof(int)); //Find primes in first 10k numbers

	bitmap = (unsigned char*) addr;
      	num_array = (int *) (bitmap + bitmap_size);

      	/* Find the prime numbers */
      	init_num_array(num_array);
      	num_primes = 10000;
      	
      	/* Spawn 2 threads to find primes */
      	pthread_t *thread;	
        pthread_attr_t attr;	
        
         /* Allocate the number of pthreads given by num_proc */
        thread = (pthread_t*)malloc(2 * sizeof(pthread_t));
        
        /* initialize thread attribute with defaults */
        pthread_attr_init(&attr);

        /* i is used as the thread id, is passed as the thread's only argument */
        int i; 
        for (i = 0; i < num_threads; i++) {
            if (pthread_create(&thread[i], 			 	/* thread */
                               &attr, 				 	/* thread attribute */
                               process_primes_thread, 	/* function to execute */
                               i+1) != 0) {            /* thread argument */
                perror("Cannot create the thread.");
                exit(-1);
            }
        }
        
        for (i = 0; i < 2; i++) {
            pthread_join(thread[i], NULL);
    	}

	printf("Number of primes found is %d\n", num_primes);

	/* Delete the shared memory object */
	if(shm_unlink("/merrickd_primes") == -1){
                  printf("Error deleting shared memory object");
                  exit(EXIT_FAILURE);
        }

	return 0;
}

void *process_primes_thread(void *vp)
{
    	find_primes(num_array, (int) vp, 10000, num_threads);
	pthread_exit(EXIT_SUCCESS); /* exit */
}

void find_primes(int *num_array, int min, int max, int offset)
{
	int k = min;
	int i;
	while(k <= floor(sqrt(max))){
		k+=offset;
		if(num_array[k-1] == 1){ //Check the k-1 spot because arrays are indexed starting at zero
			i = 2;
			while(k*i <= 10000){
				if(!(num_array[k*i-1] == 0)){
					num_primes--;
				}
				num_array[k*i-1] = 0;
				i++;
			}
		}
	}
}

void help()
{
	printf("Usage: tprimes [number of threads] [max number for primes]");
	exit(1);
}

/* Mounts a shared memory object. Copied from code from class */
void *mount_shmem(char *path, int object_size)
{
	int shmem_fd;
	void *addr;

	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if(shmem_fd == -1)
		puke_and_exit("Failed to open shared memory object\n");

	if(ftruncate(shmem_fd, object_size) == -1)
		puke_and_exit("Failed to resize shared memory object\n");

	/* map the shared memory object */
	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);

	if(addr == MAP_FAILED)
		puke_and_exit("Failed to map shared memory object\n");

	return addr;
}

void init_num_array(int *num_array)
{
	/* Initialize prime_array numbers to 1 */
      	int i;
      	for(i=0;i<=10000;i++){
		num_array[i] = 1;
	}

}

void puke_and_exit(char *errormessage)
{
	perror(errormessage);
	printf("Errno = %d\n", errno);
	exit(EXIT_FAILURE);
}
