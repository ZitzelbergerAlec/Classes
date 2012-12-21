/* CS261- Assignment 1 - Q.5*/
/* Name: David Merrick
 * Date: 1-14-12
 * Solution description: Set the even characters of the word the user inputted to capital letters and the odds to lower, then print the word.
 * Development environment: GCC with nano editor on flop.engr.oregonstate.edu. 
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // necessary for strlen() function
#include <ctype.h>  //necessary for isupper() and islower() functions

/*converts ch to upper case, assuming it is in lower case currently*/
char toUpperCase(char ch){
     return ch-'a'+'A';
}

/*converts ch to lower case, assuming it is in upper case currently*/
char toLowerCase(char ch){
     return ch-'A'+'a';
}


/*
	<function description>: converts the parameter string into "sticky caps," where every other letter is capitalized and the others are lowercase
	<preconditions>: parameter string has no spaces, and has characters in the array
	<postconditions>: correctly converts string to sticky caps
*/

void sticky(char* word){
     /*Convert to sticky caps*/
	int len = strlen(word);

	for(int i=0; i<len; i++){
		if(i % 2 == 0){ //it's even, so uppercase it
			if(!isupper(*(word + i))){ //if it's not uppercase already, make it so
				*(word + i) = toUpperCase(*(word + i));
			}
		} else { //make character lowercase
			if(!islower(*(word + i))){ //if it's not lowercase already, make it so
				*(word + i) = toLowerCase(*(word + i));
			}
		}
	}
}

int main(){
	/*Input a word*/
	printf("Please type a word:\n");
	char word[50];
	scanf("%s", word);
 
	/*Call sticky*/
	sticky(word);

	/*Print the new word*/
	printf("%s\n", word);   

	return 0;
}
