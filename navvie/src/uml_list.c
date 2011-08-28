#include <string.h>
#include "uml_list.h"

void nv_uml_list_init( struct UMLList *l )
{
	l->front = NULL;
	l->back = NULL;
}

void nv_uml_list_push_back( struct UMLList *l, struct UMLListLink *el )
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

void nv_uml_list_push_front( struct UMLList *l, struct UMLListLink *el )
{
	el->next = l->front;
	l->front = el->next;

	if( l->back == NULL ) {
		l->back = el;
	}
}

struct UMLListLink *nv_uml_list_front( struct UMLList *l )
{
	return l->front;
}

struct UMLListLink *nv_uml_list_next( struct UMLListLink *el )
{
	return el->next;
}
