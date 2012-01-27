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
#define NV_LIST_FOREACH( _list, _type, _link_name, _function )\
do {\
	struct ListLink *_iter;\
	for(_iter = nv_list_front((_list)); _iter;) {\
		_type *_el = NV_LIST_GET_DATA(_iter, _type, _link_name);\
		_iter = nv_list_next(_iter);\
		(_function)(_el);\
	}\
} while(0)

void nv_list_init( struct List *l );
void nv_list_push_back( struct List *l, struct ListLink *el );
void nv_list_push_front( struct List *l, struct ListLink *el );
struct ListLink *nv_list_front( struct List *l );
struct ListLink *nv_list_next( struct ListLink *el );

#endif
