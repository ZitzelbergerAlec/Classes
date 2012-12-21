/* CS261- Assignment 1 - Q.2*/
/* Name: David Merrick
 * Date: 1-14-12
 * Solution description: creates and calls function that doubles first input parameter, halves second input parameter, and assigns third input parameter to be the sum of first and second. 
 * Development environment: GCC with nano editor on flop.engr.oregonstate.edu.
 */
 
#include <stdio.h>
#include <stdlib.h>

/*
	<function description>: doubles int value pointed to by first param, halves int value pointed to by second param, assigns third param to first + second param and returns it. 
	<preconditions>: params are inputted correctly-- a and b must be int pointers and c must be an int.
	<postconditions>: correct integer value is returned
*/

int foo(int* a, int* b, int c){
    /*Set a to double its original value*/
    *a *= 2;    

    /*Set b to half its original value*/
    *b *= .5;    

    /*Assign a+b to c*/
    c = *a + *b;    

    /*Return c*/
    return c;
}

int main(){
    /*Declare three integers x,y and z and initialize them to 5, 6, 7 respectively*/
    int x = 5, y = 6, z = 7;

    /*Print the values of x, y and z*/
    printf("%d\n%d\n%d\n\n", x, y, z);    

    /*Call foo() appropriately, passing x,y,z as parameters*/
    /*Print the value returned by foo*/
    printf("%d\n\n", foo(&x,&y,z));
        
    /*Print the values of x, y and z again*/
    printf("%d\n%d\n%d\n", x, y, z);    
 
    /*Is the return value different than the value of z?  Why?*/
    return 0;
}
    
    
