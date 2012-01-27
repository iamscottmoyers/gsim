#ifndef _NAVVIE_UML_CLASS_H_
#define _NAVVIE_UML_CLASS_H_

#include "list.h"
#include "uml_type.h"
#include "uml_qualifier.h"

struct UMLClass {
	struct UMLType super;
	struct List attributes;
	struct List operations;
	struct List enumerations;
	struct List primitivetypes;
	struct List datatypes;
	struct List nested_classes;
	struct List associations;
	unsigned int qualifiers;
	struct ListLink link;
};

struct UMLClass *nv_uml_class_new(void);
void nv_uml_class_delete(struct UMLClass *c);

void nv_uml_class_set_qualifier(struct UMLClass *c, enum UMLQualifier q);
int nv_uml_class_get_qualifier(struct UMLClass *c, enum UMLQualifier q);

#endif
