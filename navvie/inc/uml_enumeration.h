#ifndef _NAVVIE_UML_ENUMERATION_H_
#define _NAVVIE_UML_ENUMERATION_H_

#include "list.h"
#include "uml_type.h"

struct UMLLiteral;

struct UMLEnumeration {
	struct UMLType super;
	List *literals;
};

struct UMLEnumeration *nv_uml_enumeration_new();
void nv_uml_enumeration_delete(struct UMLEnumeration *e);

void nv_uml_enumeration_add_literal(struct UMLEnumeration *e, struct UMLLiteral *l);
List *nv_uml_enumeration_get_literals(struct UMLEnumeration *e);

#endif
