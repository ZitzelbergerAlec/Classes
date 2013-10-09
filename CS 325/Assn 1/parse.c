/*
 * Parses text file into appropriate array.
 */

#define BLOCKSIZE 1
#define INPUT_FILE "close_to_zero_problems.txt"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(){
	FILE *fd;
	fd = fopen(INPUT_FILE, "r");
	int comma_count = 0;
	
	if(fd != NULL){
		char *buffer;
		buffer = (char*) malloc (sizeof(char));
		fread (buffer,1,1,fd);
		
		//First pass, to count the commas and determine the size of the array we need
		while(buffer[0] != ']'){
			fread (buffer,1,1,fd);
			if(buffer[0] == ','){
				comma_count++;
			}
		}
		
		int arr[comma_count + 1]; //Array to hold the numbers
		
		//Second pass, to get all elements into the array
		//Rewind, read opening bracket character
		rewind(fd);
		fread (buffer,1,1,fd);
		
		char curr_elt[5]; //Current element temp buffer
		int curr_elt_idx = 0; //Index of current element temp buffer
		int arr_idx = 0; //Index of bigger array
		
		while(buffer[0] != ']'){
			fread (buffer,1,1,fd);
			if(buffer[0] == ','){
				//convert current element to int, store in big array
				curr_elt[curr_elt_idx] = '\0'; //Append escape character
				arr[arr_idx] = atoi(curr_elt);
				arr_idx++;
				curr_elt_idx = 0; //Start over
			}else {
				printf("buffer == %c", buffer[0]);
				if(buffer[0] != ' '){ //ignore spaces
					//store the current element in the temp buffer
					curr_elt[curr_elt_idx] = buffer[0];
					curr_elt_idx++;
				}
			}
		}
		
		//Testing: print the array
		printf("Testing: print the array\n");
		for(int i = 0; i< comma_count + 1; i++){
			printf("%d\n", arr[i]);
		}
		
		fclose(fd);
	} else {
		printf("Error opening file.\n");
	}
	
	
}