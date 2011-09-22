#ifndef LISH_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct list_node_s {
	struct list_node_s *prev, *next;
	void 			   *data;
} listNode;

typedef struct list_s {
	listNode *head, *tail;
	int itemCount;
} list;

list     *ListCreate( void *data );
listNode *ListCreateNode( void *data );
void	 *ListContains( list *l, void *data );
int       ListDeleteAt( list *l, int pos );
int       ListDeleteItem( list *l, void *data );
int       ListDeleteNode( listNode *node );
void      ListFree( list *l );
int       ListInsertAt( list *l, void *data, int pos );
int       ListInsertAtEnd( list *l, void *data );
int       ListInsertAtFront( list *l, void *data );
int       ListLength( list* l );
void      ListPrint( list* l );
list     *ListSort( list *l );
list     *ListSortStrings( list *l );
void    **ListToArray( list *l );

#endif
