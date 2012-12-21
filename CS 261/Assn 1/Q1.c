/* CS261- Assignment 1 - Q.1*/
/* Name: David Merrick
 * Date: 1-14-12
 * Solution description: creates array of 10 students, assigns random IDs and scores to them, calculates and prints min, max and average scores for student array, and then deallocates the memory from the array. 
 * Development environment: GCC with nano editor on flop.engr.oregonstate.edu.
 */
 
#include <stdio.h>
#include<stdlib.h>
#include <time.h> //Necessary to seed the rand() function with the current time
#include<math.h> //Necessary to implement the rand() function

struct student{
	int id;
	int score;
};


/*
	<function description>: allocates memory for 10 students, returns the pointer 
	<preconditions>: there must be memory available to allocate
	<postconditions>: returns the pointer to the memory allocated
*/

struct student* allocate(){
     /*Allocate memory for ten students*/     
     struct student *pStudents = (struct student *) malloc(10*sizeof(struct student)); //Allocate memory for 10 students and cast this as a pointer to a struct student
     
     /*return the pointer*/
     return pStudents;
}

/*
	<function description>: assigns random IDs and scores to 10 students
	<preconditions>: array of 10 students exists 
	<postconditions>: students are assigned with IDs between 1-10 and scores between 0-100
*/

void generate(struct student* students){
     /*Generate random ID and scores for ten students, ID being between 1 and 10, scores between 0 and 100*/
     srand(time(NULL)); //Return the current time and use it to seed the rand() function.
     
     for(int i=0; i<10; i++){ //loop through students and generate random ID and score
		(students + i)->id = rand() % 10 + 1; //generate a random integer between 1 and 10 for the id
		(students + i)->score = rand() % 100 + 1; //generate a random integer between 1 and 100 for the score
     }
}


/*
        <function description>: prints the IDs and scores of the student array in the specified format
        <preconditions>: array of 10 students exists and students have assigned IDs and scores
        <postconditions>: student IDs and scores are printed in the correct format
*/

void output(struct student* students){
     /*Output information about the ten students in the format: */
     for(int i=0; i<10; i++){         
           printf("%d %d\n", (students + i)->id, (students + i)->score);
     }
}


/*
        <function description>: calculates and outputs the average, minimum, and maximum scores of the students
        <preconditions>: array of 10 students exists and students have assigned IDs and scores
        <postconditions>: correctly prints the score computations.
*/

void summary(struct student* students){
     /*Compute and print the minimum, maximum and average scores of the ten students*/
     int min, max, total = 0, currScore;
     float avg;
     for(int i=0; i<10; i++){ //find the max, min, and total in the same loop
	currScore = (students + i)->score;
	total += currScore;
	if(i == 0){ //set the initial conditions
		max = currScore;
		min = max;
		continue; //skip the rest of the loop the first time
	}
	if(currScore < min){ //we have a new minimum
		min = currScore;
	} else if(currScore > max){ //we have a new maximum
		max = currScore;
	}
     }  
     avg = total/10;
     printf("Average score: %f\n", avg);
     printf("Maximum Score: %d\n", max);
     printf("Minimum Score: %d\n", min);
}

/*
        <function description>: Deallocates the previously-allocated memory from the student array.
        <preconditions>: input parameter must be a pointer to a struct.
        <postconditions>: memory is freed, stud variable is set to 0
*/


void deallocate(struct student* stud){
     /*Deallocate memory from stud*/
     if(stud != 0){ //make sure this function has not already been called
     	free(stud);
     	stud = 0;
     }
}

int main(){
    struct student* stud = NULL;
    
    /*call allocate*/
    stud = allocate();
    
    /*call generate*/
    generate(stud);    

    /*call output*/
    output(stud);    

    /*call summary*/
    summary(stud);    

    /*call deallocate*/
    deallocate(stud);

    return 0;
}
