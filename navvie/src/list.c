#include <string.h>
#include "list.h"

void nv_list_init( struct List *l )
{
	l->front = NULL;
	l->back = NULL;
}

void nv_list_push_back( struct List *l, struct ListLink *el )
{
	el->next = NULL;

	if( l->back ) {
		l->back->next = el;
	}

	l->back = el;

	if( l->front == NULL ) {
		l->front = el;
	}
}

void nv_list_push_front( struct List *l, struct ListLink *el )
{
	el->next = l->front;
	l->front = el;

	if( l->back == NULL ) {
		l->back = el;
	}
}

struct ListLink *nv_list_front( struct List *l )
{
	return l->front;
}

struct ListLink *nv_list_next( struct ListLink *el )
{
	return el->next;
}
