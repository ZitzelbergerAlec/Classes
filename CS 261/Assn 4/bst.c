/*
 * David Merrick
 * 2/11/12
 * CS 261
 * Development Environment: Notepad++ with gcc.
*/

/*
 File: bst.c
 Implementation of the binary search tree data structure.

 */
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "bst.h"


struct Node {
	TYPE         val;
	struct Node *left;
	struct Node *right;
};

struct BSTree {
	struct Node *root;
	int          cnt;
};

/*----------------------------------------------------------------------------*/
/*
 function to initialize the binary search tree.
 param tree
 pre: tree is not null
 post:	tree size is 0
		root is null
 */

void initBSTree(struct BSTree *tree){
	tree->cnt  = 0;
	tree->root = 0;
}

/*
 function to create a binary search tree.
 param: none
 pre: none
 post: tree->count = 0
		tree->root = 0;
 */

struct BSTree*  newBSTree(){
	struct BSTree *tree = (struct BSTree *)malloc(sizeof(struct BSTree));
	assert(tree != 0);

	initBSTree(tree);
	return tree;
}

/*----------------------------------------------------------------------------*/
/*
function to free the nodes of a binary search tree
param: node  the root node of the tree to be freed
 pre: none
 post: node and all descendants are deallocated
*/

void _freeBST(struct Node *node){
	if (node != 0) {
		_freeBST(node->left);
		_freeBST(node->right);
		free(node);
	}
}

/*
 function to clear the nodes of a binary search tree
 param: tree    a binary search tree
 pre: tree ! = null
 post: the nodes of the tree are deallocated
		tree->root = 0;
		tree->cnt = 0
 */
void clearBSTree(struct BSTree *tree)
{
	_freeBST(tree->root);
	tree->root = 0;
	tree->cnt  = 0;
}

/*
 function to deallocate a dynamically allocated binary search tree
 param: tree   the binary search tree
 pre: tree != null;
 post: all nodes and the tree structure itself are deallocated.
 */
void deleteBSTree(struct BSTree *tree){
	clearBSTree(tree);
	free(tree);
}

/*----------------------------------------------------------------------------*/
/*
 function to determine if  a binary search tree is empty.

 param: tree    the binary search tree
 pre:  tree is not null
 */
int isEmptyBSTree(struct BSTree *tree) { return (tree->cnt == 0); }

/*
 function to determine the size of a binary search tree

param: tree    the binary search tree
pre:  tree is not null
*/
int sizeBSTree(struct BSTree *tree) { return tree->cnt; }

/*----------------------------------------------------------------------------*/
/*
 recursive helper function to add a node to the binary search tree.
 HINT: You have to use the compare() function to compare values.
 param:  cur	the current root node
		 val	the value to be added to the binary search tree
 pre:	val is not null
 */
struct Node *_addNode(struct Node *cur, TYPE val){
	struct Node *newNode;
	assert(val != 0 && val != NULL);
	if(cur == 0){ /*Add a new node*/
		newNode = (struct Node *) malloc(sizeof(struct Node));
		assert(newNode != 0);
		newNode->val = val;
		newNode->left = 0;
		newNode->right = 0;
		return newNode;
	}
	if(compare(cur->val, val) <= 0){
		cur->right = _addNode(cur->right, val);
	} else {
		cur->left = _addNode(cur->left, val);
	}
	return cur;
}

/*
 function to add a value to the binary search tree
 param: tree   the binary search tree
		val		the value to be added to the tree

 pre:	tree is not null
		val is not null
 pose:  tree size increased by 1
		tree now contains the value, val
 */
void addBSTree(struct BSTree *tree, TYPE val){
	tree->root = _addNode(tree->root, val);
	tree->cnt++;
}


/*
 function to determine if the binary search tree contains a particular element
 HINT: You have to use the compare() function to compare values.
 param:	tree	the binary search tree
		val		the value to search for in the tree
 pre:	tree is not null
		val is not null
 post:	none
 */

/*----------------------------------------------------------------------------*/
int containsBSTree(struct BSTree *tree, TYPE val){
	assert(tree != 0);
	assert(val != 0 && val != NULL);
	struct Node* noderator = tree->root; /*noderator is node iterator*/
	while(noderator != 0){
		if(compare(val, noderator->val) == 0){
				return 1;
		} else if (compare(val, noderator->val) < 0){ //val is less than current node's value, go left
			noderator = noderator->left;
		} else { //val is greater than current node's value, go left
			noderator = noderator->right;
		}
	}
	return 0;
}

/*
 helper function to find the left most child of a node
 return the value of the left most child of cur
 param: cur		the current node
 pre:	cur is not null
 post: none
 */

/*----------------------------------------------------------------------------*/
TYPE _leftMost(struct Node *cur){
	assert(cur != 0);
	while(cur->left != 0) 
		cur = cur->left;
	return cur->val;
}


/*
 helper function to remove the left most child of a node
 HINT: this function returns cur if its left child is NOT NULL. Otherwise,
 it returns the right child of cur and free cur.

 param: cur	the current node
 pre:	cur is not null
 post:	the left most node of cur is not in the tree
 */
/*----------------------------------------------------------------------------*/
struct Node *_removeLeftMost(struct Node *cur){
	assert(cur != 0);
	if(cur->left != 0){
		cur->left = _removeLeftMost(cur->left);
		return cur;
	}
	struct Node *rightNode = cur->right;
	free(cur);
	return rightNode;
}
/*
 recursive helper function to remove a node from the tree
 HINT: You have to use the compare() function to compare values.
 param:	cur	the current node
		val	the value to be removed from the tree
 pre:	val is in the tree
		cur is not null
		val is not null
 */
/*----------------------------------------------------------------------------*/
struct Node *_removeNode(struct Node *cur, TYPE val){
	assert(cur != 0);
	assert(val != 0 && val != NULL);
	if(compare(cur->val, val) == 0){ //found it
		if(cur->right == 0){
			struct Node *temp = cur->left;
			free(cur);
			return temp;
		}
		cur->val = _leftMost(cur->right);
		cur->right = _removeLeftMost(cur->right);
	} else if(compare(cur->val, val) > 0){ //val is less than cur->val. Go right.
		cur->left = _removeNode(cur->left, val);
	} else { //val is greater than cur->val. Go right.
		cur->right = _removeNode(cur->right, val);
	}
	return cur;
}
/*
 function to remove a value from the binary search tree
 param: tree   the binary search tree
		val		the value to be removed from the tree
 pre:	tree is not null
		val is not null
		val is in the tree
 pose:	tree size is reduced by 1
 */
void removeBSTree(struct BSTree *tree, TYPE val){
	if (containsBSTree(tree, val)) {
		tree->root = _removeNode(tree->root, val);
		tree->cnt--;
	}
}

/*----------------------------------------------------------------------------*/

/* The following is used only for debugging, set to "#if 0" when used
  in other applications */
#if 1
#include <stdio.h>

/*----------------------------------------------------------------------------*/
void printNode(struct Node *cur) {
	 if (cur == 0) return;
	 printf("(");
	 printNode(cur->left);
	 /*print_type prints the value of the TYPE*/
	 print_type(cur->val);
	 printNode(cur->right);
	 printf(")");
}

void printTree(struct BSTree *tree) {
	 if (tree == 0) return;
	 printNode(tree->root);
}
/*----------------------------------------------------------------------------*/

#endif
