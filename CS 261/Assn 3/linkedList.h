#ifndef __LISTDEQUE_H
#define __LISTDEQUE_H

# ifndef TYPE
# define TYPE      int
# define TYPE_SIZE sizeof(int)
# endif
# ifndef LT
# define LT(A, B) ((A) < (B))
# endif

# ifndef EQ
# define EQ(A, B) ((A) == (B))
# endif

/* todo
	- should build a wrapper for deque
*/

struct linkedList;

struct bag;

struct linkedList *createLinkedList();

/* Deque Interface */
int 	isEmptyList(struct linkedList *lst);
void  addBackList(struct linkedList *lst, TYPE val);
void 	addFrontList(struct linkedList *lst, TYPE val);

TYPE  frontList(struct linkedList *lst);
TYPE 	backList(struct linkedList *lst);

void  removeFrontList(struct linkedList *lst);
void 	removeBackList(struct linkedList *lst);

/*Bag Interface */
void addList(struct linkedList *lst, TYPE val);
int containsList(struct linkedList *lst, TYPE val);
void removeList(struct linkedList *lst, TYPE val);

/*Bag Wrapper Interface*/
struct bag *createBag();
void addToBag(struct bag* b, TYPE val);
void removeFromBag(struct bag* b, TYPE val);
int containsBag(struct bag* b, TYPE val);
int isEmptyBag(struct bag* b);
void printBag(struct bag*b);

#endif

