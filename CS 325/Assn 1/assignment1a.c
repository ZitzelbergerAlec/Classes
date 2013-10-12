/* Assignment 1 - Finding the Largest Subarray
 * Group members: David Merrick, Nick A, Taylor Friesen
 */

#include <stdlib.h>
#include <stdio.h>


//Prototypes
void close_to_zero(int *arr, int size);

//This struct contains the indices min and max to indicate the bounds of the subarray
//Also contains minimum sum
struct sum_struct{
	int min;
	int max;
	int sum;
} min_sum;

int main()
{
	int myarr[] = {5, -7, 2};
	close_to_zero(myarr, 3);
	printf("Minimum sum is %d\n", min_sum.sum);
	printf("Found between indices %d and %d\n", min_sum.min, min_sum.max);
	return 0;
}

//this method computes pretty much every subarray individually
void close_to_zero(int *arr, int size)
{
    int temp_sum;
    int compare_sum; //variable to hold absolute value
    min_sum.sum = arr[0]; //set initial value
    min_sum.min = 0;
    min_sum.max = 0;
	for(int i = 0; i < size; i++){
    	temp_sum = 0;
    	if(abs(arr[i]) < min_sum.sum){
      		min_sum.sum = arr[i];
      		min_sum.min = i;
      		min_sum.max = i;
    	}
    	for(int j = i+1; j < size; j++){
	    	temp_sum = arr[i];
            for(int k = i+1; k <= j; k++){
                temp_sum += arr[k];
            }
            compare_sum = abs(temp_sum);
            if(compare_sum < min_sum.sum){
                min_sum.sum = compare_sum;
                min_sum.min = i;
                min_sum.max = j;
            }
        }
    }
}
