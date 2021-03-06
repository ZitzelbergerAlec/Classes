/* Name: David Merrick
 * Date: 1/28/12
 * Development Environment: Visual Studio 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include "cirListDeque.h"

/* Double Link Struture */
struct DLink {
	TYPE value;/* value of the link */
	struct DLink * next;/* pointer to the next link */
	struct DLink * prev;/* pointer to the previous link */
};

# define TYPE_SENTINEL_VALUE DBL_MAX 


/* ************************************************************************
 Deque ADT based on Circularly-Doubly-Linked List WITH Sentinel
 ************************************************************************ */

struct cirListDeque {
	int size;/* number of links in the deque */
	struct DLink *Sentinel;	/* pointer to the sentinel */
};
/* internal functions prototypes */
struct DLink* _createLink (TYPE val);
void _addLinkAfter(struct cirListDeque *q, struct DLink *lnk, struct DLink *newLnk);
void _removeLink(struct cirListDeque *q, struct DLink *lnk);



/* ************************************************************************
	Deque Functions
************************************************************************ */

/* Initialize deque.

	param: 	q		pointer to the deque
	pre:	q is not null
	post:	q->backSentinel is allocated and q->size equals zero
*/
void _initCirListDeque (struct cirListDeque *q) {
  	assert(q != NULL && q != 0);
	q->size = 0;
	q->Sentinel = (struct DLink *)malloc(sizeof(struct DLink));
	assert(q->Sentinel != 0);
	q->Sentinel->next = q->Sentinel;
	q->Sentinel->prev = q->Sentinel;
}

/*
 create a new circular list deque
 
 */

struct cirListDeque *createCirListDeque(){
	struct cirListDeque *newCL = malloc(sizeof(struct cirListDeque));
	_initCirListDeque(newCL);
	return(newCL);
}


/* Create a link for a value.

	param: 	val		the value to create a link for
	pre:	none
	post:	a link to store the value
*/
struct DLink * _createLink (TYPE val){
	struct DLink* newLink = (struct DLink *)(malloc(sizeof(struct DLink)));
	assert(newLink != 0);
	newLink->value = val;
	return(newLink);
}

/* Adds a link after another link

	param: 	q		pointer to the deque
	param: 	lnk		pointer to the existing link in the deque
	param: 	newLnk	pointer to the new link to add after the existing link
	pre:	q is not null
	pre: 	lnk and newLnk are not null
	pre:	lnk is in the deque 
	post:	the new link is added into the deque after the existing link
*/
void _addLinkAfter(struct cirListDeque *q, struct DLink *lnk, struct DLink *newLnk){
	assert(q != NULL && q != 0);
	assert(lnk != NULL);
	assert(newLnk != NULL);
	newLnk->next = lnk->next;
	newLnk->prev = lnk;
	lnk->next->prev = newLnk;
	lnk->next = newLnk;
	q->size++;
}

/* Adds a link to the back of the deque

	param: 	q		pointer to the deque
	param: 	val		value for the link to be added
	pre:	q is not null
	post:	a link storing val is added to the back of the deque
*/
void addBackCirListDeque(struct cirListDeque *q, TYPE val) {
	printf("inside addBackCirListDeque(). val is: %f\n", val);
	assert(q != NULL && q != 0);
	_addLinkAfter(q, q->Sentinel->prev, _createLink(val));
}

/* Adds a link to the front of the deque

	param: 	q		pointer to the deque
	param: 	val		value for the link to be added
	pre:	q is not null
	post:	a link storing val is added to the front of the deque
*/
void addFrontCirListDeque(struct cirListDeque *q, TYPE val){
	printf("inside addFrontCirListDeque(). val is: %d\n", val);
	assert(q != NULL && q != 0);
	_addLinkAfter(q, q->Sentinel, _createLink(val));
}

/* Get the value of the front of the deque

	param: 	q		pointer to the deque
	pre:	q is not null and q is not empty
	post:	none
	ret: 	value of the front of the deque
*/
TYPE frontCirListDeque(struct cirListDeque *q) {
	assert(q != NULL && q != 0);
	assert(q->size != 0);
	return(q->Sentinel->next->value);
}

/* Get the value of the back of the deque

	param: 	q		pointer to the deque
	pre:	q is not null and q is not empty
	post:	none
	ret: 	value of the back of the deque
*/
TYPE backCirListDeque(struct cirListDeque *q){
	assert(q != NULL && q != 0);
	assert(q->size != 0);
	return(q->Sentinel->prev->value);
}

/* Remove a link from the deque

	param: 	q		pointer to the deque
	param: 	lnk		pointer to the link to be removed
	pre:	q is not null and q is not empty
	pre:	lnk is in the deque 
	post:	the link is removed from the deque
*/
void _removeLink(struct cirListDeque *q, struct DLink *lnk){
	assert(q != NULL && q != 0);
	assert(lnk != 0);
	assert(q->size != 0);
	lnk->prev->next = lnk->next;
	lnk->next->prev = lnk->prev;
	printf("\n\n");
	free(lnk);
	q->size--;
}

/* Remove the front of the deque

	param: 	q		pointer to the deque
	pre:	q is not null and q is not empty
	post:	the front is removed from the deque
*/
void removeFrontCirListDeque (struct cirListDeque *q) {
	assert(q != NULL && q != 0);
	assert(q->size != 0);
	_removeLink(q, q->Sentinel->next);
}

/* Remove the back of the deque

	param: 	q		pointer to the deque
	pre:	q is not null and q is not empty
	post:	the back is removed from the deque
*/
void removeBackCirListDeque(struct cirListDeque *q){
  	assert(q != NULL && q != 0);
	assert(q->size != 0);
	_removeLink(q, q->Sentinel->prev);
}

/* De-allocate all links of the deque

	param: 	q		pointer to the deque
	pre:	none
	post:	All links (including backSentinel) are de-allocated
*/
void freeCirListDeque(struct cirListDeque *q){
	assert(q != NULL && !isEmptyCirListDeque(q));
	assert(q->size != 0);
	while(q->size > 0)
		removeFrontCirListDeque(q);
	free(q->Sentinel);
	q->Sentinel = 0;
}

/* Check whether the deque is empty

	param: 	q		pointer to the deque
	pre:	q is not null
	ret: 	1 if the deque is empty. Otherwise, 0.
*/
int isEmptyCirListDeque(struct cirListDeque *q) {
  	assert(q != NULL && q != 0);
	return(q->size == 0);
}

/* Print the links in the deque from front to back

	param: 	q		pointer to the deque
	pre:	q is not null and q is not empty
	post: 	the links in the deque are printed from front to back
*/
void printCirListDeque(struct cirListDeque *q){
	printf("size of deque is: %d\n", q->size);
	assert(q != NULL && q != 0);
	assert(!isEmptyCirListDeque(q));
	struct DLink* idx = q->Sentinel->next;
	assert(idx != 0);
	while(idx != q->Sentinel){
		printf("%f\n", idx->value);
		idx = idx->next;
	}
}

/* Reverse the deque

	param: 	q		pointer to the deque
	pre:	q is not null and q is not empty
	post: 	the deque is reversed
*/
void reverseCirListDeque(struct cirListDeque *q){
	assert(q != NULL && q != 0);
	assert(!isEmptyCirListDeque(q));	 
	struct DLink* idx = q->Sentinel->next;
	struct DLink* temp;
	q->Sentinel->prev = idx;
	printf("reversing deque\n");
	do{
		temp = idx->next;
		if(temp->next == q->Sentinel){
			q->Sentinel->next = temp;
		}
		idx->next = idx->prev;
		idx->prev = temp;
		idx = temp;
	} while(idx != q->Sentinel);	
}
