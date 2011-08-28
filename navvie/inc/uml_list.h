#ifndef _NAVVIE_UML_LIST_H_
#define _NAVVIE_UML_LIST_H_

struct UMLList {
	struct UMLListLink *front;
	struct UMLListLink *back;
};

struct UMLListLink {
	struct UMLListLink *next;
};

#define OFFSET_OF( _type, _member ) ( (size_t) &((_type *) 0)->_member )
#define CONTAINER_OF( _ptr, _type, _member ) ((_type *)((char *)_ptr - OFFSET_OF(_type, _member)))
#define NV_UML_LIST_GET_DATA( _link, _type, _link_name ) CONTAINER_OF( _link, _type, _link_name )

void nv_uml_list_init( struct UMLList *l );
void nv_uml_list_push_back( struct UMLList *l, struct UMLListLink *el );
void nv_uml_list_push_front( struct UMLList *l, struct UMLListLink *el );
struct UMLListLink *nv_uml_list_front( struct UMLList *l );
struct UMLListLink *nv_uml_list_next( struct UMLListLink *el );

#endif
