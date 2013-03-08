/* Finds the range that can be calculated in 15 seconds */
#include	<stdio.h>
#include	<stdlib.h>
#include 	<math.h> /* for sqrt */
#include	<time.h> /* timespec{} for pselect() */



int mods_per_sec();
int is_perfect(int test_number);

int main(int argc, const char *argv[])
{
	/* 
 * 	Test out mods per sec to see if actually takes 15 sec */
	int i;
	int min = 3;
	int num_mods = 0;
	int j = 3;
	int mod_ceiling = 15 * mods_per_sec();
	while(num_mods < mod_ceiling){
		num_mods += floor(sqrt(i));
		j++;
	}
	
	int test_max = j;
	printf("Can calculate in the range of %d to %d\n", min, test_max);
	
	clock_t start, stop;
	printf("running calculations...\n");
	start = clock();
	for(i=1; i<test_max; i++){
		if(is_perfect(i))
			printf("%d is perfect\n", i);
	}

	stop = clock();
	printf("Calculations took %f sec", ((double) stop - (double) start)/CLOCKS_PER_SEC);
	return 0;
}


int mods_per_sec()
{
	int test_max = 100000000;
	int sqrt_test_max = sqrt(test_max);
	clock_t start, stop;
	start = clock();
	stop = clock();
	int i = 0;
	while((stop-start)/CLOCKS_PER_SEC < 0.05){
		test_max % sqrt_test_max; /* Time the mod operation */
		stop = clock();
		i+=2; //Because each loop also divides by CLOCKS_PER_SEC
	}
	return i*20; /* mods per sec */
}

/* 
 * Returns 1 if a test_number is a perfect number.
 * */
int is_perfect(int test_number)
{	
	int i;
	int sum = 1;
	for(i=2; i<sqrt(test_number); i++){
		if((test_number % i) == 0){
			sum += i;
			sum += test_number/i;
		}
	}
	if(sum == test_number && test_number != 1) //1 works, but is not a perfect number
		return 1;
	return 0;
}
