#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int findnumprimes(int *numarray, int n){
	int i, k;

    int numprimes = n; //Set it to n and count down whenever we find a composite
    
    //initialize everything to one for prime. Assume prime until proven composite
	for(i=0;i<=n;i++){
		*(numarray + i) = 1;
	}
	k = 1; //initialize k to 1
	while(k <= floor(sqrt(n))){
		k++;
		if(*(numarray +k-1) == 1){ //Check the k-1 spot because arrays are indexed starting at zero
			i = 2;
			while(k*i <= n){
                if(!(*(numarray + k*i-1) == 0)){ //then it hasn't already been marked. Decrement numprimes
                    numprimes--;
                }
                *(numarray + k*i-1) = 0;
				i++;
			}
		}
	}
	return numprimes;
}

void findprimes(int *numarray, int *primes, int n, int numprimes){
    int i, k;
    k = 0; //This is our counter for the prime loop
    for(i=0;i<n;i++){
        if(*(numarray + i)==1){
            *(primes + k) = i+1;
            k++;
        }
    }
}


int main(int argc, const char * argv[]){
	int n, k, i;
	
	if(argc > 1){
		n = atoi(argv[1]); //Set n to the command line arguments
	} else {
		n = 5; //Set n to 5 by default
	}
    printf("Calculating primes from 1 to %d using Sieve of Eratosthenes method...\n", n);
    
	int *numarray = (int *)malloc(n * sizeof(int));

	int numprimes = findnumprimes(numarray, n);
    int *primes = (int *)malloc(numprimes * sizeof(int));
    findprimes(numarray, primes, n, numprimes);
	printf("Primes:\n");
        for(i=0;i<numprimes;i++){
                printf("%d\n", *(primes + i));
        }
	printf("Number of primes is %d.\n", numprimes);
	free(numarray);
	free(primes);
	return 0;
}

