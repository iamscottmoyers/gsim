#ifndef _NAVVIE_UML_ENUMERATION_H_
#define _NAVVIE_UML_ENUMERATION_H_

#include "uml_type.h"
#include "list.h"

struct UMLLiteral;

struct UMLEnumeration {
	struct UMLType super;
	struct List literals;
	struct ListLink link;
};

struct UMLEnumeration *nv_uml_enumeration_new(void);
void nv_uml_enumeration_delete(struct UMLEnumeration *e);

#endif
