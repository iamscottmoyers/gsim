#ifndef _NAVVIE_UML_DATATYPE_H_
#define _NAVVIE_UML_DATATYPE_H_

#include "uml_list.h"
#include "uml_type.h"

struct UMLDataType {
	struct UMLType super;
	struct UMLList attributes;
	struct UMLList operations;
	struct UMLListLink link;
};

struct UMLDataType *nv_uml_datatype_new();
void nv_uml_datatype_delete(struct UMLDataType *c);

#endif
