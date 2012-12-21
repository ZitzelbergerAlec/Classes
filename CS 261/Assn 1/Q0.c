/* CS261- Assignment 1 - Q. 0*/
/* Name: David Merrick
 * Date: 1-14-12
 * Solution description: declares integer, prints the address and value of integer, creates function that prints value of pointer parameter, its address, and address of the pointer itself. 
 * Development environment: GCC with nano editor on flop.engr.oregonstate.edu.
 */
 
#include <stdio.h>
#include <stdlib.h>


/*
	<function description>: prints value of input parameter, address pointed to by input parameter, and address of input parameter itself
	<preconditions>: input parameter is a pointer
	<postconditions>: values are printed
*/

void fooA(int* iptr){
     /*Print the value pointed to by iptr*/ 
     printf("The value pointed to by iptr: %d\n", *iptr);

     /*Print the address pointed to by iptr*/
     printf("The address pointed to by iptr: %p\n", iptr);

     /*Print the address of iptr itself*/
     printf("The address of iptr itself: %p\n", &iptr);
}

int main(){
    
    /*declare an integer x*/
    int x = 0;
    
    /*print the address of x*/
    printf("The address pointed to by x: %p\n", (void *)&x);

    /*Call fooA() with the address of x*/
    fooA(&x);

    /*print the value of x*/
    printf(x);    

    return 0;
}
