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
 /*Sort the given array number , of length n*/
        int nextLowestScore, nextLowestId, tempId, tempScore, jPosition;
        for(int i=0; i<n; i++){
                tempId = (students + i)->id;
                tempScore = (students + i)->score;
                nextLowestScore = tempScore; //set nextLowestScore to tempScore for comparison purposes
                for(int j=i; j<n; j++){ //The purpose of this loop is to find the next lowest score.
                        if(((students + j)->score) < nextLowestScore){ //If found, set the number to our current array spot.
                                jPosition = j;
                                nextLowestId = (students + j)->id; //store the array position of the next lowest number so we can swap after this loop.
                                nextLowestScore = (students + j)->score;
                        }
                }
                if(nextLowestScore < tempScore){ //If it found a lower score, then swap the values
                        (students + jPosition)->id = tempId;
                        (students + jPosition)->score = tempScore;
                        (students + i)->id = nextLowestId;
                        (students + i)->score = nextLowestScore;
                }
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
