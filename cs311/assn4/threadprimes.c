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
int is_prime(unsigned int n);
unsigned int count_primes();
void find_primes(unsigned int min, unsigned int max, unsigned int offset);
void help();
void init_num_array();
void *mount_shmem(char *path, int object_size);
void *process_primes_thread(void *vp);
void puke_and_exit(char *errormessage);
void set_not_prime(unsigned int n);
void set_prime(unsigned int n);
void spawn_threads();
void toggle(unsigned int n);

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
unsigned int max_prime = 255;

int main(int argc, char **argv)
{
	/*
	   if(argc < 3)
	   help();
	 */

	num_threads = 2;

	unsigned int object_size = max_prime/BITS_PER_WORD; // ~500 MB

	/* Create a shared memory object */
	void *addr = mount_shmem("/merrickd_primes", object_size); /* 4294967295 = 2^32 */

	num_array = (unsigned char*) addr; /* num_array is our bitmap */

	init_num_array();

	/* Create the threads */
	//spawn_threads();
	//debug
	
	/* Find the primes */	

	find_primes(1, max_prime, 1);

	unsigned int num_primes = count_primes();

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
	unsigned int i;
	for (i = 0; i < num_threads; i++) {
		if (pthread_create(&thread[i], &attr, process_primes_thread, (void *) i) != 0)
			puke_and_exit("Error creating thread.\n");
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


/* Finds primes using Sieve of Atkin method */
void find_primes(unsigned int min, unsigned int max, unsigned int offset)
{
	unsigned int k;
	unsigned int i;
	unsigned int square_multiple;
	unsigned int limit = ceil(sqrt(max));

	/* Initialize candidate primes: */
	unsigned int x, y, n;
	for(x=1;x<limit;x++){
		for(y=1;y<limit;y++){
			n = 4*x*x + y*y;
			if(n <= max && ((n % 12 == 1) || n % 12 == 5))
				toggle(n);
			n = 3*x*x + y*y;
			if(n <= max && n % 12 == 7)
				toggle(n);
			n = 3*x*x - y*y;
			if(x > y && n <= max && n % 12 == 11)
				toggle(n);
		}
	}

	/* Eliminate composites */
	for(k=5; k<limit; k++){
		printf("k = %u\n", k);
		if (is_prime(k)) {
			i = 1;
			square_multiple = k * k * i;
			while (square_multiple <= max) {
				set_not_prime(square_multiple);
				i++;
				square_multiple = k * k * i;
			}
		} 
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
    num_array[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

/* 0 designates a prime number */
void set_prime(unsigned int n) 
{ 
    num_array[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n));
}

/* If n is 1, set to 0 and vice-versa */
void toggle(unsigned int n) 
{
	word_t bit = num_array[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
	if(bit==0){
		num_array[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));			
	} else {
		num_array[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n));		
	}
}

/* Returns 1 if the index of num_primes at n is prime, 0 otherwise */
int is_prime(unsigned int n)
{	
    word_t bit = num_array[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit == 0; 
}

/* Initialize to all odds to prime. */
void init_num_array()
{
	unsigned int i;
	for (i = 1; i < (max_prime); i++) { //8 bits can represent 256 numbers
		set_not_prime(i); /* Set everything to notprime */
	}	
	set_prime(2);
	set_prime(3);
}

void puke_and_exit(char *errormessage)
{
	perror(errormessage);
	printf("Errno = %d\n", errno);
	exit(EXIT_FAILURE);
}

unsigned int count_primes()
{
	unsigned int prime_count = 0;
	unsigned int i;
	for(i=0; i < max_prime; i++){
		if(is_prime(i)){	
			printf("%u is prime\n", i);
			prime_count++;
		}
	}
	return prime_count - 1; //because 1 is not prime, according to wolfram alpha
}
