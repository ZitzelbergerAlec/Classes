/*	stack.c: Stack application.
 * Name: David Merrick
 * Date: 1/21/12
 * Development Environment: Notepad++ and GCC (on flop.engr.oregonstate.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //for strlen function
#include "dynArray.h"


/* ***************************************************************
Using stack to check for unbalanced parentheses.
***************************************************************** */

/* Returns the next character of the string, once reaches end return '0' (zero)
	param: 	s pointer to a string 	
	pre: s is not null		
*/
char nextChar(char* s)
{
	static int i = -1;	
	char c;
	++i;	
	c = *(s+i);			
	if ( c == '\0' )
		return '\0';	
	else 
		return c;
}

/* Checks whether the (), {}, and [] are balanced or not
	param: 	s pointer to a string 	
	pre: s is not null	
	post:	
*/
int isBalanced(char* s)
{
	if(s == NULL){ //make sure s is not NULL.
		return 0;
	}
	int cap = strlen(s);
	DynArr* pArr = newDynArr(cap); //array for parentheses: "()"
	DynArr* cArr = newDynArr(cap); //array for curly brackets: "{}"
	DynArr* sArr = newDynArr(cap); //array for square brackets: "[]"
	char currChar; //current character we're working with
	while((currChar = nextChar(s)) != '\0'){ //nextChar will return a '\0' when it gets to the end of the string. Continue until that point.
		switch(currChar) {
			case '(':
				pushDynArr(pArr, currChar); 
				break;
			case ')':
				if(sizeDynArr(pArr) == 0){ //there's nothing in the array, so we already know it's imbalanced. return 0.
					deleteDynArr(pArr); //free the memory used by the arrays
					deleteDynArr(cArr);
					deleteDynArr(sArr);
					return 0;
				}
				popDynArr(pArr);
				break;
			case '{':
				pushDynArr(cArr, currChar);
				break;
			case '}':
				if(sizeDynArr(cArr) == 0){ //there's nothing in the array, so we already know it's imbalanced. return 0.
					deleteDynArr(pArr); //free the memory used by the arrays
					deleteDynArr(cArr);
					deleteDynArr(sArr);
					return 0;
				}
				popDynArr(cArr);
				break;
			case '[':
				pushDynArr(sArr, currChar);
				break;
			case ']':
				if(sizeDynArr(sArr) == 0){ //there's nothing in the array, so we already know it's imbalanced. return 0.
					deleteDynArr(pArr); //free the memory used by the arrays
					deleteDynArr(cArr);
					deleteDynArr(sArr);					
					return 0;
				}
				popDynArr(sArr);
				break;
			default:
				continue; //skip the rest of the loop if no relevant parenthetical was found
		}
	}
	if((sizeDynArr(pArr) + sizeDynArr(cArr) + sizeDynArr(sArr)) == 0){ 
		deleteDynArr(pArr); //free the memory used by the arrays
		deleteDynArr(cArr);
		deleteDynArr(sArr);
		return 1;
	} 
	deleteDynArr(pArr); //free the memory used by the arrays
	deleteDynArr(cArr);
	deleteDynArr(sArr);
	return 0;
}

int main(int argc, char* argv[]){
	int res;	
	char* s=argv[1];	

	printf("Assignment 2\n");

	res = isBalanced(s);

	if (res)
		printf("The string %s is balanced\n",s);
	else 
		printf("The string %s is not balanced\n",s);
	
	return 0;	
}

