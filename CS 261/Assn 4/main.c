#include<stdio.h>
#include<stdlib.h>
#include "bst.h"
#include "structs.h"

/* Example main file to begin exercising your tree */

int main(int argc, char *argv[])
{
	struct BSTree *tree	= newBSTree();

	/*Create value of the type of data that you want to store*/
	struct data myData1;
	struct data myData2;
	struct data myData3;
	struct data myData4;
	struct data myData5;
	struct data myData6;

	myData1.number = 5;
	myData1.name = "rooty";
	myData2.number = 1;
	myData2.name = "lefty";
	myData3.number = 10;
	myData3.name = "righty";
	myData4.number = 3;
	myData4.name = "righty";
	myData5.number = 2;
	myData5.name = "righty";
	myData6.number = 4;
	myData6.name = "righty";

	/*add the values to BST*/
	addBSTree(tree, &myData1);
	addBSTree(tree, &myData2);
	addBSTree(tree, &myData3);
	addBSTree(tree, &myData4);
	addBSTree(tree, &myData5);
	addBSTree(tree, &myData6);

	/*Print the entire tree*/
	printTree(tree);
	/*(( 1 ( 3 ) ) 5 ( 10 ))*/
	
	//TYPE test = 3;
	//TYPE test2 = 11;
	//printf("Contains: 3 (should be 1) = %d\n", containsBSTree(tree, &myData4.number));
	//printf("Contains: 11 (should be 0) = %d\n", containsBSTree(tree, &test2));
	
	printf("Pointer to myData4.number: %d\n", myData4.number);
	removeBSTree(tree, &myData4.number);
	printf("Tree after removing 3:\n");
	printTree(tree);
	
	return 1;
}

