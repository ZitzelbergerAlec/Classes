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
int check_prime(unsigned int n);
void find_primes(unsigned int min, unsigned int max, int offset);
void help();
void init_num_array();
void *mount_shmem(char *path, int object_size);
void *process_primes_thread(void *vp);
void puke_and_exit(char *errormessage);
void set_not_prime(unsigned int n);
void set_prime(unsigned int n);
void spawn_threads();

/* Global variables and typedefs */
/* Reference: http://stackoverflow.com/questions/1225998/what-is-bitmap-in-c */
typedef char word_t;
enum { BITS_PER_WORD = 8 };	
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
unsigned int num_primes;
unsigned char *num_array;
unsigned int num_threads;
unsigned int max_prime;

int main(int argc, char **argv)
{
	/*
	   if(argc < 3)
	   help();
	 */

	/* To do: Let 0 designate primes, anything else designate not prime */

	num_threads = 2;
	max_prime = UINT_MAX;

	unsigned int object_size = 4294967295/BITS_PER_WORD; // ~500 MB

	/* Create a shared memory object */
	void *addr = mount_shmem("/merrickd_primes", object_size); /* 4294967295 = 2^32 */

	num_array = (unsigned char*) addr; /* num_array is our bitmap */

	/* Find the prime numbers */
	init_num_array();

	num_primes = max_prime;

	/* Create the threads */
	//spawn_threads();
	//debug
	
	find_primes(1, max_prime, num_threads);

	printf("Number of primes found is %u\n", num_primes);

	/* Delete the shared memory object */
	if (shm_unlink("/merrickd_primes") == -1) {
		printf("Error deleting shared memory object");
		exit(EXIT_FAILURE);
	}

	return 0;
}

void spawn_threads()
{
	/* Spawn 2 threads to find primes */
	pthread_t *thread;
	pthread_attr_t attr;

	/* Allocate the number of pthreads given by num_proc */
	thread = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

	/* initialize thread attribute with defaults */
	pthread_attr_init(&attr);

	/* i is used as the thread id, is passed as the thread's only argument */
	int i;
	for (i = 0; i < num_threads; i++) {
		if (pthread_create(&thread[i],	/* thread */
				   &attr,	/* thread attribute */
				   process_primes_thread,	/* function to execute */
				   i) != 0) {	/* thread argument */
			perror("Cannot create the thread.");
			exit(-1);
		}
	}

	for (i = 0; i < 2; i++) {
		pthread_join(thread[i], NULL);
	}
}

void *process_primes_thread(void *vp)
{
	find_primes((unsigned int) vp, max_prime, num_threads);
	pthread_exit(EXIT_SUCCESS);	/* exit */
}

void find_primes(unsigned int min, unsigned int max, int offset)
{
	unsigned int k = min;
	unsigned int i;
	int s; 
	unsigned int loc;
	while (k <= floor(sqrt(max))) {
		k += offset;
		/* Lock num_primes with our mutex so we can decrement it */
		s = pthread_mutex_lock(&mtx);
		if(s != 0)
			puke_and_exit("Error locking mutex\n");
		if (!check_prime(k)) { /* To do: bit variable is always -128, 2, or 32. Why? Means this block never executes */
			i = 2;
			while (k * i <= max) {
				if(check_prime(k*i-1)){
					loc = num_primes;
					loc--;
					num_primes = loc;
				}
				set_not_prime(k * i - 1);
				i++;
			}
		} 
		s = pthread_mutex_unlock(&mtx);
	}
}

void help()
{
	printf
	    ("Usage: tprimes [number of threads] [max number for primes]");
	exit(1);
}

/* Mounts a shared memory object. Copied from code from class */
void *mount_shmem(char *path, int object_size)
{
	int shmem_fd;
	void *addr;

	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if (shmem_fd == -1)
		puke_and_exit("Failed to open shared memory object\n");

	if (ftruncate(shmem_fd, object_size) == -1)
		puke_and_exit("Failed to resize shared memory object\n");

	/* map the shared memory object */
	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);

	if (addr == MAP_FAILED)
		puke_and_exit("Failed to map shared memory object\n");

	return addr;
}

void set_not_prime(unsigned int n) 
{ 
    num_array[WORD_OFFSET(n)] |= ~(1 << BIT_OFFSET(n));
}

void set_prime(unsigned int n) 
{ 
    num_array[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

int check_prime(unsigned int n)
{	
    word_t bit = num_array[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit == 0; 
}

/* Initialize to all odds */
void init_num_array()
{
	/* Initialize odds to prime */
	num_array[0] = 0b10101100; //Special case, set bits 0 and 1 off, bit 2 on
	int i;
	for (i = 1; i < (4294967295/255); i++) { //8 bits can represent 256 numbers
		num_array[i] = 170; //0b10101010
	}
}

void puke_and_exit(char *errormessage)
{
	perror(errormessage);
	printf("Errno = %d\n", errno);
	exit(EXIT_FAILURE);
}
