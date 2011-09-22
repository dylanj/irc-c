#include "list.h"

list *ListCreate( void *data ) {
	list *l = malloc( sizeof ( list ) );
	listNode *node = ListCreateNode( data );

	// single item right
	node->next = node;
	node->prev = node;

	// update list ptrs.
	l->head = node;
	l->tail = node;
	l->itemCount = 1;
	
	return l;
}

// maybe use a C struct feature here to simplify.
listNode *ListCreateNode( void *data ) {
	listNode *node = malloc( sizeof( listNode ) );
	node->next = NULL;
	node->prev = NULL;
	node->data = data;
	return node;
}

void *ListContains( list *l, void *data ) {
	listNode *node = l->head;
	
	for( int i = 0; i < l->itemCount; i++ ) {
		if ( node->data == data ) {
			return data;
		}

		node = node->next;
	}
	
	return NULL;
}

// deletes a node and fixes ptrs around it.
int ListDeleteNode( listNode *node ) {

	if ( node == NULL )
		return 1;
	
	listNode *prev = node->prev;
	listNode *next = node->next;

	// link pointers on each side together
	prev->next = next;
	next->prev = prev;

	// set all members to null ( why? )
	node->data = NULL;
	node->next = NULL;
	node->prev = NULL;
	
	// free mem
	free( node );

	return 1;
}

// deletes an item at the specified index. ( 0 based )
int ListDeleteAt( list *l, int pos ) {
	if ( l == NULL || pos < 0 || pos > l->itemCount )
		return 0;
	
	listNode *node = l->head;

	for( int i = 0; i < l->itemCount; i++ ) {
		if ( i == pos ) {
			ListDeleteNode( node );
			l->itemCount--;
			return 1;
		}

		node = node->next;
	}

	return 0;
}

// deletes the specified item ( linear search )
int ListDeleteItem( list *l, void *data ) {
	
	if ( l == NULL || data == NULL )
		return 0;

	listNode *node = l->head;

	for( int i = 0; i < l->itemCount; i++ ) {
		// compare pointers. obvs wont work for strings.
		if ( data == node->data ) {
			ListDeleteNode( node );
			l->itemCount--;
			return 1;
		}

		node = node->next;
	}

	return 0;
}

// frees the list and its nodes
void ListFree( list *l ) {
	
	// don't bother with null ptrs.
	if ( l == NULL )
		return;

	listNode *node = l->head;
	listNode *tmp;
	
	// traverse and free nodes!
	for( int i = 0; i < l->itemCount; i++ ) {
		tmp = node->next;
		free( node );
		node = tmp;
	}
	
	// free list itself
	free( l );
}

// inserts at the specified index. 
// example: insert(2) would be third item in the list.
int ListInsertAt( list *l, void *data, int pos ) {

	if ( l == NULL || data == NULL )
		return 0;

	listNode *node = l->head;

	if ( pos <= 0 ) {
		ListInsertAtFront( l, data );
		return 1;
	} 
	else if ( pos > l->itemCount ) {
		ListInsertAtEnd( l, data );
		return 1;
	}

	for( int i = 0; i < l->itemCount; i++ ) {
		if ( i == pos ) {
			listNode *prev = node->prev;
			listNode *newNode = ListCreateNode( data );
			
			node->prev = newNode; // insert before this.
			prev->next = newNode; // tell prev node this new node follows it.
			
			newNode->prev = prev;
			newNode->next = node;

			l->itemCount++;
		}
		node = node->next;
	}

	return 1;
}

// inserts as tail
int ListInsertAtEnd( list *l, void *data ) {
	
	if ( l == NULL || data == NULL )
		return 0;

	listNode *node = ListCreateNode( data );
	
	node->prev = l->tail; // adding to end so tail is before this
	node->next = l->head; // circally doubly linked list

	l->tail->next = node; // current tail points to this new node
	l->tail = node; // set tail ptr to this new node.
	l->itemCount++;
	
	return 1;
}

// inserts as head
int ListInsertAtFront( list *l, void *data ) {
	if ( l == NULL || data == NULL )
		return 0;

	listNode *node = ListCreateNode( data );

	node->prev = l->tail;
	node->next = l->head;

	l->head->prev = node;
	l->head = node;
	l->itemCount++;

	return 1;
}

// returns items in list
int ListLength( list* l ) {
	if ( l == NULL )
		return 0;

	return l->itemCount;
}

// prints a list of strings
void ListPrint( list* l ) {
	if ( l == NULL )
		return;

	listNode *node = l->head;
	for( int i = 0; i < l->itemCount; i++ ) {
		printf( "DATA[%i]: %s\n", i, (char *)node->data );
		node = node->next;
	}
}

list *ListSort( list *unsortedList ) {
	
	return unsortedList;
}

int str_cmp( const void *a, const void *b ) { 
	const char **sa = (const char **) a;
	const char **sb = (const char **) b;
    
	return strcmp(*sa, *sb);
} 
		
list *ListSortStrings( list *unsortedList ) {
	list *sortedList = NULL;
	
	char** unsortedArray = (char**) ListToArray( unsortedList );
	for( int i = 0; i < unsortedList->itemCount; i++ ) {
		printf( "$[%s]\n", unsortedArray[i] );
	}

	qsort( unsortedArray, unsortedList->itemCount, sizeof( char * ), str_cmp );
	
	sortedList = ListCreate( unsortedArray[0] );
	
	for( int i = 1; i < unsortedList->itemCount; i++ ) {
		ListInsertAtEnd( sortedList, unsortedArray[i] );
	}

	free( unsortedArray );

	return sortedList;	
}

void ListSwap( listNode *a, listNode *b ) {
	listNode *aprev = a->prev;
	listNode *anext = a->next;

	listNode *bprev = b->prev;
	listNode *bnext = b->next;

	b->next = anext;
	b->prev = aprev;

	a->next = bnext;
	a->prev = bprev;
}

void **ListToArray( list *l ) {
	if ( l == NULL || l->head == NULL || l->head->data == NULL ) 
		return NULL;

	void **array = malloc( l->itemCount * sizeof( l->head->data ) );
	
	listNode *node = l->head;
	for( int i = 0; i < l->itemCount; i++ ) {
		array[i] = node->data;
		node = node->next;
	}

	return array;
}
