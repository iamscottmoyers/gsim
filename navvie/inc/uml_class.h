#ifndef _NAVVIE_UML_CLASS_H_
#define _NAVVIE_UML_CLASS_H_

#include "uml_list.h"
#include "uml_type.h"
#include "uml_qualifier.h"

struct UMLClass {
	struct UMLType super;
	struct UMLList attributes;
	struct UMLList operations;
	struct UMLList enumerations;
	struct UMLList primitivetypes;
	struct UMLList datatypes;
	struct UMLList nested_classes;
	struct UMLList associations;
	unsigned int qualifiers;
	struct UMLListLink link;
};

struct UMLClass *nv_uml_class_new();
void nv_uml_class_delete(struct UMLClass *c);

void nv_uml_class_set_qualifier(struct UMLClass *c, enum UMLQualifier q);
void nv_uml_class_clear_qualifier(struct UMLClass *c, enum UMLQualifier q);
int nv_uml_class_get_qualifier(struct UMLClass *c, enum UMLQualifier q);

#endif
