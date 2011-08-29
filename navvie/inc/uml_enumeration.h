#ifndef _NAVVIE_UML_ENUMERATION_H_
#define _NAVVIE_UML_ENUMERATION_H_

#include "uml_type.h"
#include "uml_list.h"

struct UMLLiteral;

struct UMLEnumeration {
	struct UMLType super;
	struct UMLList literals;
	struct UMLListLink link;
};

struct UMLEnumeration *nv_uml_enumeration_new();
void nv_uml_enumeration_delete(struct UMLEnumeration *e);

#endif
