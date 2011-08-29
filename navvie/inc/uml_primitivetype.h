#ifndef _NAVVIE_UML_PRIMITIVETYPE_H_
#define _NAVVIE_UML_PRIMITIVETYPE_H_

#include "uml_type.h"
#include "list.h"

struct UMLPrimitiveType {
	struct UMLType super;
	struct ListLink link;
};

struct UMLPrimitiveType *nv_uml_primitivetype_new();
void nv_uml_primitivetype_delete(struct UMLPrimitiveType *p);
void nv_uml_primitivetype_set_name(struct UMLPrimitiveType *p, const char *name);
const char *nv_uml_primitivetype_get_name(struct UMLPrimitiveType *p);

#endif
