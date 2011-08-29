#ifndef _NAVVIE_UML_CONSTRAINT_H_
#define _NAVVIE_UML_CONSTRAINT_H_

#include "list.h"

struct UMLConstraint {
	char *body;
	struct ListLink link;
};

struct UMLConstraint *nv_uml_constraint_new();
void nv_uml_constraint_delete(struct UMLConstraint *c);

void nv_uml_constraint_set_body(struct UMLConstraint *c, const char *body);
const char *nv_uml_constraint_get_body(struct UMLConstraint *c);

#endif
