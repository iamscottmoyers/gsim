#ifndef _NAVVIE_UML_LIST_H_
#define _NAVVIE_UML_LIST_H_

#include "util.h"

struct List {
	struct ListLink *front;
	struct ListLink *back;
};

struct ListLink {
	struct ListLink *next;
};

#define NV_LIST_GET_DATA( _link, _type, _link_name ) CONTAINER_OF( _link, _type, _link_name )

void nv_list_init( struct List *l );
void nv_list_push_back( struct List *l, struct ListLink *el );
void nv_list_push_front( struct List *l, struct ListLink *el );
struct ListLink *nv_list_front( struct List *l );
struct ListLink *nv_list_next( struct ListLink *el );

#endif
