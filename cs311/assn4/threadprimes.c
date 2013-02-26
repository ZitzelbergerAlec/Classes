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
void *elim_composites(void *vp);
void find_primes(unsigned int min, unsigned int max, unsigned int offset);
void help();
void init_bitmap();
void *mount_shmem(char *path, int object_size);
void *process_primes_thread(void *vp);
void puke_and_exit(char *errormessage);
void seed_primes();
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
#define SHM_NAME "/merrickd_primes"
struct thread_args{
	unsigned int thread_id;
	unsigned int min;
	unsigned int max;
};

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
unsigned int num_primes;
unsigned char *bitmap;
unsigned int num_threads;
unsigned int max_prime = 25;

int main(int argc, char **argv)
{
	num_threads = 2;

	unsigned int bitmap_size = max_prime/BITS_PER_WORD + 1;

	/* Create a shared memory object */
	void *addr = mount_shmem(SHM_NAME, bitmap_size); 

	/* Initialize bitmap */
	bitmap = (unsigned char*) addr;
	init_bitmap();
	
	/* Create the threads */
	printf("Seeding primes...\n");
	seed_primes();

	/* Create the threads */
	printf("Calculating primes...\n");
	spawn_threads();
	
	/* Find the primes */	
	unsigned int num_primes = count_primes();
	printf("Number of primes found is %u\n", num_primes);

	/* Delete the shared memory object */
	if (shm_unlink(SHM_NAME) == -1) {
		printf("Error deleting shared memory object");
		exit(EXIT_FAILURE);
	}

	return 0;
}

/* 
Uses Sieve of Eratosthenes to seed the bitmap with primes up until sqrt(max_prime). 
Every number from sqrt(max_prime) to max_prime will either be prime or a multiple of them.
*/
void seed_primes()
{
	unsigned int limit = sqrt(max_prime);
	unsigned int i, j;
	for(i=3; i<=sqrt(limit); i++){
		if(is_prime(i))
			for(j=3; (i*j)<=limit; j++)
				set_not_prime(i*j);
	}
}


void spawn_threads()
{
	/* Spawn 2 threads to find primes */
	pthread_t *thread;
	pthread_attr_t attr;

	/* Allocate the number of pthreads given by num_proc */
	thread = malloc(num_threads * sizeof(pthread_t));

	/* initialize thread attribute with defaults */
	pthread_attr_init(&attr);

	struct thread_args *args;
	args = malloc(sizeof(struct thread_args));

	/* Create a struct of arguments for threads */
	unsigned int i;
	for (i = 0; i < num_threads; i++) {
		args->thread_id = i;
		args->min = i * (max_prime/num_threads) + 1;
		/* If we're on the last thread, set the max equal to the max_prime */
		args->max = (i == num_threads-1) ? max_prime : args->min + (max_prime/num_threads) - 1;
		if (pthread_create(&thread[i], &attr, elim_composites, (void *) args) != 0)
			puke_and_exit("Error creating thread.\n");
	}

	for (i = 0; i < num_threads; i++) {
		pthread_join(thread[i], NULL);
	}
}

void *elim_composites(void *vp){
	struct thread_args *args = (struct thread_args*)(vp);
	printf("thread %u gets min = %u, max = %u\n", args->thread_id, args->min, args->max);
	pthread_exit(EXIT_SUCCESS);
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

/* Returns 1 if the index of num_primes at n is prime, 0 otherwise */
int is_prime(unsigned int n)
{   
    word_t bit = bitmap[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0; 
}

/* Sets a bit to 0 (not prime) */
void set_not_prime(unsigned int n) 
{ 
    bitmap[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n));
}

/* Sets a bit to 1 (prime) */
void set_prime(unsigned int n) 
{ 
    bitmap[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

/* Initialize to all odds to prime. */
void init_bitmap()
{
	unsigned int i;
	/* Set words one at a time */
	for (i = 0; i < (max_prime/BITS_PER_WORD + 1); i++) { 
		bitmap[i] = 0b10101010; 
	}
	/* set 1 to not prime and 2 to prime */
	set_not_prime(1);
	set_prime(2);
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
	for(i=2; i <= max_prime; i++){
		if(is_prime(i)){
			prime_count++;
		}
	}
	return prime_count; 
}
