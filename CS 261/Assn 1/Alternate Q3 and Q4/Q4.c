/* CS261- Assignment 1 - Q.4*/
/* Name: David Merrick
 * Date: 1-14-12
 * Solution description: create an array of struct students, assign them random scores and IDs, print the unsorted and sorted arrays (sorted by score in ascending order)
 * Development environment: GCC with nano editor on flop.engr.oregonstate.edu.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h> //to seed the rand() function
#include <math.h> //to implement the rand() function

struct student{
	int id;
	int score;
};


/*
	<function description>: sorts the first parameter array of length specified in parameter 2
	<preconditions>: array has content that is students, first parameter is a pointer to that array.
	<postconditions>: array is sorted in ascending order by score
*/

void sort(struct student* students, int n){
     /*Sort the n students based on their score*/     
	 int j, tempScore, tempID;
        for(int i = 1; i < n; i++){
                tempScore = (students + i)->score;
                tempID = (students + i)->id;
                j = i;
                while((j>0) && ((students + (j-1))->score > tempScore)){
                        j--;
                }
                for (int k=i; k>j; k--){
                        (students + k)->score = (students + (k-1))->score;
                        (students + k)->id = (students + (k-1))->id;
                }
                (students + j)->score = tempScore;
                (students + j)->id = tempID;
        }
}

int main(){
    /*Declare an integer n and assign it a value.*/
	int n = 5;    

    /*Allocate memory for n students using malloc.*/
	struct student *pStudent = (struct student *)(malloc(n*sizeof(struct student)));

    /*Generate random IDs and scores for the n students, using rand().*/
	srand(time(NULL)); //Seed the rand() function with the current time
	for(int i=0; i<n; i++){
		(pStudent + i)->id = i+1; //Keep the IDs unique and between 1 and 10
		(pStudent + i)->score = rand() % 100 + 1; //Keep the scores between 1 and 100
	}

    /*Print the contents of the array of n students.*/
	for(int i=0; i<n; i++){
		printf("%d %d\n", (pStudent + i)->id, (pStudent + i)->score);
	}

    /*Pass this array along with n to the sort() function*/
	sort(pStudent, n);	
    
    /*Print the contents of the array of n students.*/
        printf("\n");
	for(int i=0; i<n; i++){
                printf("%d %d\n", (pStudent + i)->id, (pStudent + i)->score);
        }
    
    return 0;
}
