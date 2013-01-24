#include <stdio.h>


int main(){
	int i; 
	int myarray[5] = {1, 1, 2, 3, 5};
	int *mypointer = myarray;
	//Does the same thing as:
	//int *mypointer;
	//mypointer = &myarray[0];

	for(i = 0; i< 5; i++){
		printf("Array[%d] = %d\n", i, *(mypointer + i));
	}
	return(0);
}
