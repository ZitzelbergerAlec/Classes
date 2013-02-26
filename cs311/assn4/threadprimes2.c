#define _POSIX_SOURCE
#define _BSD_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>

void initial_errorchk(int argc, char *argv[]);
void spawn_threads();
void process_primes(unsigned int min, unsigned int max);
void *process_primes_thread(void * vp);
unsigned int next_seed(unsigned int k);
void init_bitmap();

void seed_primes();
void puke_and_exit(char *errormessage);

int shmem_fd, num_threads;
char proc_type;
unsigned char *bitmap;
unsigned int bitmap_size;
unsigned int max_prime;
char print;

/* David's stuff */
void *mount_shmem(char *path, unsigned int object_size);
int is_prime(unsigned int n);
void set_not_prime(unsigned int n);
void set_prime(unsigned int n);
unsigned int count_primes();

/* Global variables and typedefs */
/* Reference: http://stackoverflow.com/questions/1225998/what-is-bitmap-in-c */
typedef char word_t;
enum { BITS_PER_WORD = 8 }; 
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)


int main(int argc, char *argv[])
{
    void *addr;
    
    initial_errorchk(argc, argv);
    
    /* update sizes */
    bitmap_size = (max_prime / BITS_PER_WORD + 1);

    /* Mount shared memory */
    addr = mount_shmem("/merrickd_primes", bitmap_size);
    
    /* Initialize bitmap */
    bitmap = addr;
    init_bitmap(bitmap, bitmap_size);
    
    /* Seed primes */
    printf("Seeding primes...\n");
    seed_primes();

    /* Compute primes */
    printf("Computing primes...\n");
    spawn_threads();
    
    printf("Counting primes...\n");
    unsigned int prime_count = count_primes();   
    printf("%u primes found.\n", prime_count);

    /* Delete the shared memory object */
    if (shm_unlink("/merrickd_primes") == -1) {
        printf("Error deleting shared memory object");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

unsigned int count_primes(){
        unsigned int prime_count = 0;
        //count the number of primes
        unsigned long i;
        for (i=0; i<=max_prime; i++){
            if (is_prime(i))
                prime_count++;
        }
        return prime_count;
}

void initial_errorchk(int argc, char *argv[])
{
    unsigned int temp;
    
    /* error checking for number of argc */
    if (argc != 5){
        puke_and_exit("Invalid number of arguments\n" \
               "Usage: primes [p|t] [#processes/threads] [Max_prime_number] [print? y|n]\n");
    }
    
    /* make sure input argv is a int */
    if ((num_threads = atoi(argv[2])) > 0) {
        //printf("Entered number: %d\n", num_threads);
    } else {
        printf("Invalid number of processes/threads\n");
        exit(EXIT_FAILURE);
    }
    
    /* get procType */
    proc_type = argv[1][0];
    if (proc_type != 'p' && proc_type != 't') {
        printf("Invalid command.\nUsage: primes [p|t] [#processes/threads]\n" \
               "Usage: primes [p|t] [#processes/threads] [Max_prime_number] [print? y|n]\n");
        exit(EXIT_FAILURE);
    }

    /* manual max_primes? */
    if ((temp = atoi(argv[3])) > 0)
        max_prime = temp;
    else {
        printf("Invalid number for max primes.\n" \
               "Usage: primes [p|t] [#processes/threads] [Max_prime_number] [print? y|n]\n");
        exit(EXIT_FAILURE);
    }
    
    /* print or not */
    if (argv[4][0] == 'y' || argv[4][0] == 'n') {
        print = argv[4][0];
    } else {
        printf("Invalid entry for print primes.\n" \
               "Usage: primes [p|t] [#processes/threads] [Max_prime_number] [print? y|n]\n");
        exit(EXIT_FAILURE);
    }
}

void spawn_threads()
{
    unsigned int i;
    
    pthread_t *thread;		/* thread object */
    pthread_attr_t attr;			/* thread attributes */
    
    thread = malloc(num_threads * sizeof(pthread_t));
    
    /* init thread attribute with defaults */
    pthread_attr_init(&attr);

    /* i is thread ID is passed as argument */
    for (i = 0; i < num_threads; i++) {
        if (pthread_create(&thread[i], &attr, process_primes_thread, (void*) (i)) != 0)
            puke_and_exit("Cannot create the thread.");
    }
    
    /* Wait on threads to finish */
    for (i = 0; i < num_threads; i++) {
        pthread_join(thread[i], NULL);
    }
}

/* 
Loops through a chunk of the seed primes and sets all their multiples to not prime 
Stops at max/3 because everything after that has been marked previously. 
*/
void process_primes(unsigned int min, unsigned int max)
{    
    unsigned long i;
    unsigned int k=0;
 
    while ((k = next_seed(k)) != 0) {
        if (k > max/3) break; 
        /* 
        Assign 2 to i if min/k < 2, otherwise i = min/k, because 3 is the lowest prime afte 2 and
        bitmap has been initialized without evens. 
        */
        for (i=(min/k < 3 ? 3 : min/k); (i*k) <= max; i++) 
            set_not_prime(i*k); 
    }
}

void *process_primes_thread(void * vp)
{
    unsigned int min, max;
    unsigned int i = (unsigned int) vp;

    min = i * (max_prime/num_threads) + 1;
    max = (i==num_threads-1) ? max_prime : min + (max_prime/num_threads) - 1;

    process_primes(min, max);
    //printf("\nThread %u gets min = %u, max = %u", i, min, max); 
    pthread_exit(EXIT_SUCCESS);
}

/* returns next seed prime or 0 to indicate end */
unsigned int next_seed(unsigned int cur_seed)
{
    unsigned int i;
    for (i=cur_seed+1; i<=sqrt(max_prime); i++)
        if (is_prime(i))
            return i;
    return 0;
}

/* Initialize bitmap with odds */
void init_bitmap()
{
    bitmap[0] = 0b10101100; /* Special case; set 2 to prime, 1 to not prime */
    unsigned int i;
    for(i = 1; i < bitmap_size; i++)
        bitmap[i] = 0b10101010;
}

/* Mounts a shared memory object. Copied from code from class */
void *mount_shmem(char *path, unsigned int object_size)
{
    int shmem_fd;
    void *addr;

    shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (shmem_fd == -1)
        puke_and_exit("Failed to open shared memory object.\n");

    if (ftruncate(shmem_fd, object_size) == -1)
        puke_and_exit("Failed to resize shared memory object.\n");

    /* map the shared memory object */
    addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);

    if (addr == MAP_FAILED)
        puke_and_exit("Failed to map shared memory object.\n");

    return addr;
}

void puke_and_exit(char *errormessage)
{
    perror(errormessage);
    printf("Errno = %d\n", errno);
    exit(EXIT_FAILURE);
}

/* 
Uses Sieve of Eratosthenes to seed the bitmap with primes up until sqrt(max_prime). 
Every number from sqrt(max_prime) to max_prime will either be prime or a multiple of them.
*/
void seed_primes()
{   
    int k=3, i; //k is used for identifing primes and marking multiples of itself
    unsigned int n = sqrt(max_prime);
    while (k <= sqrt(n)) {
        for (i = 2; i * k <= n; i++)
            set_not_prime(i*k);
        do
            k++;
        while(is_prime(i*k));
    }
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