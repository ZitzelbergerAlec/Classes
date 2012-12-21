/* Name: David Merrick
 * Date: 1/28/12
 * Development Environment: Visual Studio 2010
 */
 
#include "linkedList.h"
#include "assert.h"
#include <stdlib.h>
#include <stdio.h>


/* Double Link*/
struct DLink {
	TYPE value;
	struct DLink * next;
	struct DLink * prev;
};

/* Double Linked List with Head and Tail Sentinels  */

struct linkedList{
	int size;
	struct DLink *firstLink;
	struct DLink *lastLink;
};

struct bag{/*Wrapper for bag*/
	struct linkedList* lst;/*List implementing the bag*/
};

/*
	initList
	param lst the linkedList
	pre: lst is not null
	post: lst size is 0
*/

void _initList (struct linkedList *lst) {
  assert(lst != NULL);
  lst->size = 0;
  lst->firstLink = (struct DLink *) malloc(sizeof(struct DLink));
  lst->lastLink = (struct DLink *) malloc(sizeof(struct DLink));
  assert(lst->firstLink != 0); //make sure they initialized properly before we do anything with them.
  assert(lst->lastLink != 0);
  lst->firstLink->next = lst->lastLink;
  lst->lastLink->prev = lst->firstLink;
}

/*
 createList
 param: none
 pre: none
 post: firstLink and lastLink reference sentinels
 */

struct linkedList *createLinkedList(){
	struct linkedList *newList = malloc(sizeof(struct linkedList));
	_initList(newList);
	return(newList);
}

/*
	_addLinkBeforeBefore
	param: lst the linkedList
	param: l the  link to add before
	param: v the value to add
	pre: lst is not null
	pre: l is not null
	post: lst is not empty
*/

/* Adds Before the provided link, l */

void _addLinkBefore(struct linkedList *lst, struct DLink *l, TYPE v){
	assert(lst != NULL);
	assert(l != NULL);
	struct DLink* newLink = (struct DLink *)(malloc(sizeof(struct DLink)));
	assert(newLink != 0); //make sure it initialized properly before we do anything with it.
	newLink->value = v;
	newLink->prev = l->prev;
	newLink->next = l;
	l->prev->next = newLink;
	l->prev = newLink;
	lst->size++;
}

/*
	addFrontList
	param: lst the linkedList
	param: e the element to be added
	pre: lst is not null
	post: lst is not empty, increased size by 1
*/

void addFrontList(struct linkedList *lst, TYPE e){
	assert(lst != NULL);
	_addLinkBefore(lst, lst->firstLink->next, e);
}

/*
	addBackList
	param: lst the linkedList
	pre: lst is not null
	post: lst is not empty
*/

void addBackList(struct linkedList *lst, TYPE e) {
  assert(lst != NULL);
  _addLinkBefore(lst, lst->lastLink, e);
}

/*
	frontList
	param: lst the linkedList
	pre: lst is not null
	pre: lst is not empty
	post: none
*/

TYPE frontList(struct linkedList *lst) {
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	return(lst->firstLink->next->value);
}

/*
	backList
	param: lst the linkedList
	pre: lst is not null
	pre: lst is not empty
	post: lst is not empty
*/

TYPE backList(struct linkedList *lst){
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	return(lst->lastLink->prev->value);
}

/*
	_removeLink
	param: lst the linkedList
	param: l the link to be removed
	pre: lst is not null
	pre: l is not null
	post: lst size is reduced by 1
*/

void _removeLink(struct linkedList *lst, struct DLink *l){
	assert(lst != NULL);
	assert(l != NULL);
	l->prev->next = l->next;
	l->next->prev = l->prev;
	free(l);
	lst->size--;
}

/*
	removeFrontList
	param: lst the linkedList
	pre:lst is not null
	pre: lst is not empty
	post: size is reduced by 1
*/

void removeFrontList(struct linkedList *lst) {
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	_removeLink(lst, lst->firstLink->next);
}

/*
	removeBackList
	param: lst the linkedList
	pre: lst is not null
	pre:lst is not empty
	post: size reduced by 1
*/

void removeBackList(struct linkedList *lst){	
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	_removeLink(lst, lst->lastLink->prev);
}

/*
	isEmptyList
	param: lst the linkedList
	pre: lst is not null
	post: none
*/

int isEmptyList(struct linkedList *lst) {
 	assert(lst != NULL);
	return(lst->size == 0);
}


/* Function to print list
 Pre: lst is not null
 */
void _printList(struct linkedList* lst){
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	struct DLink *idx = lst->firstLink->next;
	assert(idx != 0); //make sure it initialized properly before we do anything with it.
	while(idx != lst->lastLink){
		printf("%d\n", idx->value);
		idx = idx->next;
	}
}

/* Iterative implementation of contains() 
 Pre: lst is not null
 */

void addList(struct linkedList *lst, TYPE v){
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	addBackList(lst, v);
}

/* Iterative implementation of contains() 
 Pre: lst is not null
 pre: list is not empty
 */
int containsList(struct linkedList *lst, TYPE e) {
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	struct DLink* idx = lst->firstLink->next;
	assert(idx != 0); //make sure it initialized properly before we do anything with it.
	while(idx != lst->lastLink){
		if(idx->value == e)
			return(1);
		idx = idx->next;
	}
	return(0);
}

/* Iterative implementation of remove() 
 Pre: lst is not null
 pre: lst is not empty
 */
void removeList(struct linkedList *lst, TYPE e) {
	assert(lst != NULL);
	assert(!isEmptyList(lst));
	struct DLink* idx = lst->firstLink;
	assert(idx != 0); //make sure it initialized properly before we do anything with it.
	while(idx != lst->lastLink){
		idx = idx->next;
		if(idx->value == e){
			_removeLink(lst, idx);
			break;
		}
	}
}



/*Bag Wrapper Interface*/
struct bag *createBag(){
	struct bag *myBag = malloc(sizeof(struct bag));
	myBag->lst = createLinkedList();
	return myBag;
}

void addToBag(struct bag* b, TYPE val){
	assert(b != NULL);
	addFrontList(b->lst, val);
}

void removeFromBag(struct bag* b, TYPE val){
	assert(b != NULL);
	removeList(b->lst, val);
}

int containsBag(struct bag* b, TYPE val){
	assert(b != NULL);
	return(containsList(b->lst, val));
}

int isEmptyBag(struct bag* b){
	assert(b != NULL);
	return(isEmptyList(b->lst));
}

void printBag(struct bag *b){
	assert(b != NULL);
	_printList(b->lst);
}
