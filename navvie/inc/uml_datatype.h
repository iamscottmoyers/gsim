#ifndef _NAVVIE_UML_DATATYPE_H_
#define _NAVVIE_UML_DATATYPE_H_

#include "list.h"
#include "uml_type.h"

struct UMLDataType {
	struct UMLType super;
	struct List attributes;
	struct List operations;
	struct ListLink link;
};

struct UMLDataType *nv_uml_datatype_new(void);
void nv_uml_datatype_delete(struct UMLDataType *c);

#endif
