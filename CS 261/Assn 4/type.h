/*	type.h
	
	Defines the type to be stored in the data structure.  These macros
	are for convenience to avoid having to search and replace/dup code
	when you want to build a structure of doubles as opposed to ints
	for example.
*/

#ifndef __TYPE_H
#define __TYPE_H

# ifndef TYPE
# define TYPE      void*
# endif


/* function used to compare two TYPE values to each other, define this in your compare.c file */
int compare(TYPE left, TYPE right);
/* function used to print TYPE values, define this in your compare.c file */
void print_type(TYPE curval);

#endif
