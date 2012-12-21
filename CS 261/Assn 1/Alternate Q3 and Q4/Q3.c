/* CS261- Assignment 1 - Q.3*/
/* Name: David Merrick
 * Date: 1-14-12
 * Solution description: creates an array of ints, fills it with random values, sorts the array in order from least to greatest, and prints its values. 
 * Development environment: GCC with nano editor on flop.engr.oregonstate.edu.
 */
 
#include <stdio.h>
#include<stdlib.h>
#include <time.h> //to seed the rand() function
#include <math.h> //to implement the rand() function

/*
	<function description>: sorts the input parameter array of length input parameter 2
	<preconditions>: array is an array of integers and has content
	<postconditions>: array is sorted from least to greatest value
*/

void sort(int* number, int n){
     /*Sort the given array number , of length n*/
	int j;
	for(int i = 1; i < n; i++){
		int temp = *(number + i);
		j = i;
		while((j>0) && *(number + (j-1)) > temp){
			j--;
		}
		for (int k=i; k>j; k--){
			*(number + k) = *(number + (k-1));
		}
		*(number + j) = temp;
	}
}

int main(){
    /*Declare an integer n and assign it a value of 20.*/
	int n = 20;    

    /*Allocate memory for an array of n integers using malloc.*/
	int *pArray = (int *)(malloc(n*sizeof(int)));    

    /*Fill this array with random numbers, using rand().*/
    srand(time(NULL)); //seed the rand() function
    for(int i=0; i<n; i++){
	*(pArray +i) = rand() % 100; //Under 100 so it's easier to read
    }

    /*Print the contents of the array.*/
    for(int i=0; i<n; i++){
        printf("%d\n", *(pArray + i));
    }
    printf("\n");

    /*Pass this array along with n to the sort() function of part a.*/
    sort(pArray, n);    

    /*Print the contents of the array.*/    
    for(int i=0; i<n; i++){
        printf("%d\n", *(pArray + i));
    }

    return 0;
}
