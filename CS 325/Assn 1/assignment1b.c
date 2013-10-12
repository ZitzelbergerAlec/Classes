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
    int myarr[] = {-360,948,-29,-447,515,-53,-491,-761,-719,-854,-77,-677,473,262,-95,844,-84,-875,241,-320, 143,888,-524,-947,-652,260,503,882,-692,223,745,-245,37,-387,602,862,125,653,938,635,-960,-375,555,-730,-198,-691,-74,336,112,62,-319,-877,-416,-646,-207,616,608,737,-143,-747,-387,788,105,258,408,517,451,-718,358,118,-738,72,-457,-551,-726,580,-297,-662,-135,220,-267,773,117,669,905,152,-789,-243,-901,-241,-223,380,668,-514,-916,723,957,-222,605,-371};
	close_to_zero(myarr, 100);
    printf("Minimum sum is %d\n", min_sum.sum);
	printf("Found between indices %d and %d\n", min_sum.min, min_sum.max);
	printf("Corresponding to %d and %d\n", myarr[min_sum.min], myarr[min_sum.max]);
	return 0;
}

//this method computes pretty much every subarray individually
void close_to_zero(int *arr, int size)
{
    int temp_sum;
    int compare_sum; //variable to hold absolute value
    min_sum.sum = abs(arr[0]); //set initial value
    min_sum.min = 0;
    min_sum.max = 0;
	for(int i = 0; i < size; i++){
    	temp_sum = 0;
    	compare_sum = abs(arr[i]);
        if(compare_sum < min_sum.sum){
      		min_sum.sum = compare_sum;
      		min_sum.min = i;
      		min_sum.max = i;
    	}
    	temp_sum = arr[i];
    	for(int j = i+1; j < size; j++){
      		temp_sum += arr[j];
            compare_sum = abs(temp_sum);
      		if(compare_sum < min_sum.sum){
				min_sum.sum = compare_sum;
				min_sum.min = i;
				min_sum.max = j;
      		}
    	}
    }
}