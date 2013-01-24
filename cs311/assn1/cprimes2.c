#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int findprimes(int *primes, int n){
	int i, k;
	int numprimes = n; //Set it to n and count down whenever we find a composite
	int numarray[n]; //Contains all numbers, composite and prime
    
    //initialize everything to one for prime. Assume prime until proven composite
	for(i=0;i<=n;i++){
		numarray[i] = 1;
	}
	k = 1; //initialize k to 1
	while(k <= floor(sqrt(n))){
		k++;
		if(numarray[k-1] == 1){ //Check the k-1 spot because arrays are indexed starting at zero
			i = 2;
			while(k*i <= n){
				numarray[k*i-1] = 0;
				i++;
				numprimes--;
			}
		}
	}
    
    //Create an array with just primes
    primes = (int *)malloc(sizeof(int)*numprimes);
    k = 0; //Reuse k as a counter for the prime array
    for(i=0;i<n;i++){
        if(numarray[i] == 1){
            primes[k] = i+1;
            k++;
        }
    }
    
	return numprimes;
}


int main(int argc, const char * argv[]){
	int n, k, i;
	
	if(argc > 1){
		n = atoi(argv[1]); //Set n to the command line arguments
	} else {
		n = 5; //Set n to 5 by default
	}
    printf("Calculating primes from 1 to %d using Sieve of Eratosthenes method...\n", n);
    
	int *primes;

	int numprimes = findprimes(primes, n);

	printf("Primes:\n");
        for(i=1;i<=n;i++){
                printf("%d: %d\n", i, *(primes + i-1));
        }
	printf("Number of primes is %d.\n", numprimes);

	return 0;
}

