#include <stdio.h>


void changepointer(int *myarr){
	int i; 
	for(i=0;i<5;i++){
		*(myarr + i) = 2;
	}
} 


int main(){
	//make a null pointer
	int *myarr = 0;
	int i;
	myarr = (int *)malloc(5 * sizeof(int));
	changepointer(myarr);
	printf("Printing array: \n");
	for(i=0;i<5;i++){
                printf("%d\n", *(myarr + i));
        }


}
