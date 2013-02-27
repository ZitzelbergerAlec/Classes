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
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/* Function prototypes */
unsigned int count_primes();
unsigned int count_happys();
int converges(unsigned int j);
void *elim_composites(void *vp);
void *elim_sads(void *vp);
void find_primes(unsigned int min, unsigned int max, unsigned int offset);
void grim_reaper(int s);
void help();
void init_bitmap();
void init_convergence_array();
int in_sums_array(unsigned int number, unsigned int *prev_sums, int array_size);
int is_happy(unsigned int j);
int is_prime(unsigned int n);
void *mount_shmem(char *path, int object_size);
unsigned int next_seed(unsigned int cur_seed);
unsigned int next_prime(unsigned int cur_prime);
double count_nonzero_digits(unsigned int *digit_array);
void puke_and_exit(char *errormessage);
void seed_primes();
void set_not_prime(unsigned int n);
void set_not_happy(unsigned int n);
void set_prime(unsigned int n);
void spawn_happy_threads();
void spawn_prime_threads();
unsigned int *split_number(unsigned int number);
unsigned int sum_digit_squares(unsigned int number);
void toggle(unsigned int n);
int in_array(unsigned int number, unsigned int *int_array);

/* 
Global variables and typedefs.
Bitmap functions.
Reference: http://stackoverflow.com/questions/1225998/what-is-bitmap-in-c 
*/
typedef char word_t;
enum { BITS_PER_WORD = 8 };
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)
#define SHM_NAME "/merrickd_primes"

unsigned int num_primes;
unsigned char *bitmap;
unsigned int num_threads = 500;
unsigned int max_prime = UINT_MAX;
unsigned int convergence_array[112]; /* Array for numbers whose digits converge to 1 when squared and summed */

int main(int argc, char **argv)
{
	/* Initialize signal handling */
	struct sigaction act;

	act.sa_handler = grim_reaper;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGHUP, &act, NULL);

	unsigned int bitmap_size = max_prime / BITS_PER_WORD + 1;

	/* Create a shared memory object */
	void *addr = mount_shmem(SHM_NAME, bitmap_size);

	/* Initialize bitmap */
	bitmap = (unsigned char *) addr;
	init_bitmap();

	/* Create the threads */
	printf("Seeding primes...\n");
	fflush(stdout);
	seed_primes();

	/* Find all the primes */
	printf("Eliminating composites...\n");
	fflush(stdout);
	spawn_prime_threads();

	/* Count the primes */
	/*
	printf("Counting primes...\n");
	unsigned int num_primes = count_primes();
	printf("Number of primes found is %u\n", num_primes);
	*/

	/* Find all the happy primes */
	init_convergence_array();

	printf("Finding happy primes...\n");
	fflush(stdout);
	spawn_happy_threads();

	/* Count the happy primes */
	printf("Counting happy primes...\n");
	unsigned int num_happys = count_happys();
	printf("Number of happy primes found is %u\n", num_happys);

	/* Delete the shared memory object */
	if (shm_unlink(SHM_NAME) == -1) {
		puke_and_exit("Error deleting shared memory object");
	}

	return 0;
}

void grim_reaper(int s)
{
	/* Delete the shared memory object */
	if (shm_unlink(SHM_NAME) == -1) {
		puke_and_exit("Error deleting shared memory object");
	}
	exit(EXIT_FAILURE);
}

/* 
Uses Sieve of Eratosthenes to seed the bitmap with primes up until sqrt(max_prime). 
Every number from sqrt(max_prime) to max_prime will either be prime or a multiple of them.
*/
void seed_primes()
{
	unsigned int limit = sqrt(max_prime);
	unsigned int i, j;
	for (i = 3; i <= sqrt(limit); i++) {
		if (is_prime(i))
			for (j = 3; (i * j) <= limit; j++)
				set_not_prime(i * j);
	}
}

/*
Spawns the number of threads specified by num_threads.
To do: Optimization: Use a function pointer and make spawn_prime_threads and spawn_happy_threads one function.
*/
void spawn_prime_threads()
{
	/* Spawn 2 threads to find primes */
	pthread_t *thread;
	pthread_attr_t attr;

	/* Allocate the number of pthreads given by num_proc */
	thread = malloc(num_threads * sizeof(pthread_t));

	/* initialize thread attribute with defaults */
	pthread_attr_init(&attr);

	unsigned int i;
	for (i = 0; i < num_threads; i++) {
		if (pthread_create
		    (&thread[i], &attr, elim_composites, (void *) i) != 0)
			puke_and_exit("Error creating thread.\n");
	}

	/* Wait on threads */
	for (i = 0; i < num_threads; i++) {
		pthread_join(thread[i], NULL);
	}
}

void spawn_happy_threads()
{
	/* Spawn 2 threads to find primes */
	pthread_t *thread;
	pthread_attr_t attr;

	/* Allocate the number of pthreads given by num_proc */
	thread = malloc(num_threads * sizeof(pthread_t));

	/* initialize thread attribute with defaults */
	pthread_attr_init(&attr);

	unsigned int i;
	for (i = 0; i < num_threads; i++) {
		if (pthread_create
		    (&thread[i], &attr, elim_sads, (void *) i) != 0)
			puke_and_exit("Error creating thread.\n");
	}

	/* Wait on threads */
	for (i = 0; i < num_threads; i++) {
		pthread_join(thread[i], NULL);
	}
}

void *elim_composites(void *vp)
{
	unsigned int i = (unsigned int) vp;
	unsigned int min = i * (max_prime / num_threads) + 1;
	/* If we're on the last thread, set the max equal to the max_prime */
	unsigned int max =
	    (i ==
	     num_threads - 1) ? max_prime : min +
	    (max_prime / num_threads) - 1;
	unsigned int j;
	unsigned long k;	//This is much faster as long as opposed to unsigned int
	j = 1;
	while ((j = next_seed(j)) != 0) {
		for (k = (min / j < 3) ? 3 : (min / j); (j * k) <= max;
		     k++) {
			set_not_prime(j * k);
		}
	}
	pthread_exit(EXIT_SUCCESS);
}

/*  Eliminates sad numbers. */
void *elim_sads(void *vp){
	unsigned int i = (unsigned int) vp;
	unsigned int min = i * (max_prime / num_threads) + 1;
	/* If we're on the last thread, set the max equal to the max_prime */
	unsigned int max =
	    (i ==
	     num_threads - 1) ? max_prime : min +
	    (max_prime / num_threads) - 1;
	unsigned int j = min - 1;
	
	while (((j = next_prime(j)) <= max) && (j != 0)) {
		if(!is_happy(j)){
			set_not_happy(j);
		}
	}
	pthread_exit(EXIT_SUCCESS);
}

/* Returns 1 if a prime at index n is happy or 0 if not */ 
 int is_happy(unsigned int j)
{
	unsigned int sum = sum_digit_squares(j);
	return in_sums_array(sum, convergence_array, 112);
 }

/* 
Checks an array of prev_sums for a value. Returns if the array contains it.
*/
int in_sums_array(unsigned int number, unsigned int *int_array, int max_index){
	int i;
	for(i=0; i<max_index; i++){
		if(int_array[i] == number)
			return 1;
	}
	return 0;
}

/* 
Checks an array of prev_sums for a value. Returns if the array contains it.
*/
int in_array(unsigned int number, unsigned int *int_array){
	int i;
	for(i=0; i<sizeof(int_array); i++){
		if(int_array[i] == number)
			return 1;
	}
	return 0;
}

/* 
Expects a digit array of size 9.
*/
unsigned int sum_digit_squares(unsigned int number){
	unsigned int k, l;
	unsigned int *digit_array = split_number(number);
	unsigned int sum = 0;
	unsigned int i;
	for(i=0; i < 10; i++){
		sum += (digit_array[i] * digit_array[i]);
	}
	return sum;
}

/* Creates an array of numbers that converge to 1. */
void init_convergence_array(){
	unsigned int i;
	unsigned int count = 0;
	int j = 0; /* index for convergence array */
	for(i = 2; i < 810; i++){
		if(converges(i)){
			convergence_array[j] = i;
			j++;
		}
	}
}

/* Returns 1 if a number converges to 1. */
int converges(unsigned int j)
{
	int i;
	unsigned int repeat_array[810];
	unsigned int sum = j;
	for(i = 0; i<810; i++){
		sum = sum_digit_squares(sum);
		if(sum == 1)
			return 1;
		if(in_sums_array(sum, repeat_array, i))
			return 0;
		repeat_array[i] = sum;
	}
	return 0;
}

/* Splits an unsigned integer into an array of digits and returns the array. */
unsigned int *split_number(unsigned int number){
	unsigned int *digit_array = malloc(10 * sizeof(unsigned int));
	unsigned int k, l;
	for(k = 1000000000, l = 9; k >= 1; k /= 10, l--){
		if(k == 1000000000){
  			digit_array[l] = number/k;
		} else if(k==1) {
  			digit_array[l] = number%10;
		} else {
   			digit_array[l] = (number%(k*10))/k;
		}
	}
	return digit_array;
}

/* 
This function is an iterator for the seed primes.
Given the current seed prime, it will return the next one. 
Will return 0 if none were found.
*/
unsigned int next_seed(unsigned int cur_seed)
{
	unsigned int i;
	for (i = cur_seed + 1; i <= sqrt(max_prime); i++)
		if (is_prime(i))
			return i;
	return 0;
}

/* Returns the next prime in the bitmap given the current prime */
unsigned int next_prime(unsigned int cur_prime)
{
	unsigned int i;
	for (i = cur_prime + 1; i <= max_prime; i++)
		if (is_prime(i))
			return i;
	return 0;
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
	addr =
	    mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED,
		 shmem_fd, 0);

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

/* Sets a bit to 0 (not happy) */
void set_not_happy(unsigned int n)
{
	set_not_prime(n);
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
	for (i = 0; i < (max_prime / BITS_PER_WORD + 1); i++) {
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

/* Counts number of primes in bitmap */
unsigned int count_primes()
{
	unsigned int prime_count = 0;
	unsigned long i;
	for (i = 2; i <= max_prime; i++) {
		if (is_prime(i)) {
			prime_count++;
		}
	}
	return prime_count;
}

/* Counts number of happy primes in bitmap */
unsigned int count_happys()
{
	return count_primes();
}