#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, const char * argv[]){
	int n, m, k, i, j, notprime, count;
	
	if(argc > 1){
		n = atoi(argv[1]); //Set n to the command line arguments
	} else {
		n = 5; //Set n to 5 by default
	}	
	int primes[n];
	int composites[n];
	
	i = 0; //This is our counter for primes 
	j = 0; //This is our counter for composites
	k = 1; 

	//Debug: set all composites to 0
	for(count=0; count<=n; count++){
		composites[count]=0;
	}
	//\Debug
	
	while(k<=floor(sqrt(n))){
		notprime = 0;
		k++;
		for(int count = j; count <= n; count++){
			if(composites[count] == k){ //number is not prime
				notprime = 1;
				break;
			}	
		}
		if(notprime){
			continue;
		} else {
			primes[i] = k;
			count = 2;
			while(count*k <= n){
				composites[j] = count * k;
				count++;
				j++;
			}
			i++;
		}
			 
		
	}
	printf("Composites are:\n");
	for(count=0;count<n;count++){
		printf("%d\n", composites[count]);
	}
	
	
	printf("Primes are:\n");
	for(count=0;count<n;count++){
		printf("%d\n", primes[count]);
	}
	

	return(0);	
}

