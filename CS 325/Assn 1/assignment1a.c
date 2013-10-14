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
    int myarr[] = {715,-782,469,548,438,-290,-560,-916,573,-362,-654,494,62,128,-589,-22,159,674,-875,490,-393,460,317,-350,232,872,470,-443,-814,-702,0,-126,-352,-945,-905,-113,-393,248,477,-720,-546,-102,785,826,-19,-943,872,857,50,935,894,416,794,448,-769,-88,728,-935,-96,-126,847,324,924,-389,-643,-253,523,-292,-786,-974,-155,-644,-895,246,771,457,-584,-537,477,34,879,782,0,158,5,-452,211,-780,-786,-638,-819,-751,226,-392,685,764,17,916,-823,562};
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
        if(compare_sum < abs(min_sum.sum)){
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
            if(compare_sum < abs(min_sum.sum)){
                min_sum.sum = temp_sum;
                min_sum.min = i;
                min_sum.max = j;
            }
        }
    }
}
