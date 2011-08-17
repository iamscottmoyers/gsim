#ifndef _NAVVIE_UML_DATATYPE_H_
#define _NAVVIE_UML_DATATYPE_H_

#include "list.h"
#include "uml_type.h"

struct UMLDataType {
	struct UMLType super;
	List *attributes;
	List *operations;
};

struct UMLDataType *nv_uml_datatype_new();
void nv_uml_datatype_delete(struct UMLDataType *c);

void nv_uml_datatype_set_attributes(struct UMLDataType *c, List *l);
List *nv_uml_datatype_get_attributes(struct UMLDataType *c);

void nv_uml_datatype_set_operations(struct UMLDataType *c, List *l);
List *nv_uml_datatype_get_operations(struct UMLDataType *c);

#endif
