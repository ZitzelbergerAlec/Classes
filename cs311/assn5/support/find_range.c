/* Finds the range that can be calculated in 15 seconds */
#include	<stdio.h>
#include	<stdlib.h>
#include 	<math.h> /* for sqrt */
#include	<time.h> /* timespec{} for pselect() */
#include <limits.h>


unsigned int mods_per_sec();
int is_perfect(int test_number);

int main()
{
	/* 
 * 	Test out mods per sec to see if actually takes 15 sec */
	
	unsigned int i;
	unsigned int num_mods = 0;
	unsigned int min = 3;
	unsigned int j = 3;
	
	unsigned int mod_ceiling = mods_per_sec();
	mod_ceiling *= 15;
	printf("mod ceiling = %u\n", mod_ceiling);
	while(num_mods < mod_ceiling){
		num_mods += (sqrt(j)/2);
		j++;
	}
	
	unsigned int test_max = j;
	printf("Mods per sec: %u\n", mod_ceiling);
	printf("Can calculate in the range of %u to %u. This is off by %u.\n", min, test_max, 3357779-test_max);
	
	/*
	clock_t start, stop;
	printf("running calculations from 3 to %u\n", UINT_MAX);
	start = clock();
	i=3;
	double time_spent = 0;
	while(time_spent < 15){
		if(is_perfect((int) i))
			printf("%d is perfect\n", (int) i);
		i++;
		stop = clock();
		time_spent = (double)(stop-start) / CLOCKS_PER_SEC;
	}

	
	printf("Calculations took %f sec\n", ((double) stop - (double) start)/CLOCKS_PER_SEC);
	printf("counted up to %u\n", i);
	*/
	return 0;
}


unsigned int mods_per_sec()
{
    double time_spent=0;
    clock_t begin, end;
    int result, count=0, mod_max, low, high;

	/* VB's Timing Loop */
    while(time_spent < 1){
        result = count % 7;
        count++;
        end = clock();
        time_spent = (double)(end-begin) / CLOCKS_PER_SEC;
    }
    mod_max = count;
    return (unsigned int) mod_max;
}

/* 
 * Returns 1 if a test_number is a perfect number.
 * */
int is_perfect(int n)
{	
	if(n == 1)
		return 0;
	int i;
	int sum = 1;
	for(i=2; i<sqrt(n); i++){
		if((n % i) == 0){
			sum += i;
			sum += n/i;
		}
	}
	if(sum == n) //1 works, but is not a perfect number
		return 1;
	return 0;
}
