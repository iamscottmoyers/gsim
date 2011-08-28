#ifndef _NAVVIE_UML_CLASS_H_
#define _NAVVIE_UML_CLASS_H_

#include "list.h"
#include "uml_list.h"
#include "uml_type.h"
#include "uml_qualifier.h"

struct UMLClass {
	struct UMLType super;
	struct UMLList attributes;
	struct UMLList operations;
	struct UMLList enumerations;
	List *primitivetypes;
	List *datatypes;
	List *nested_classes;
	List *associations;
	unsigned int qualifiers;
};

struct UMLClass *nv_uml_class_new();
void nv_uml_class_delete(struct UMLClass *c);

void nv_uml_class_set_qualifier(struct UMLClass *c, enum UMLQualifier q);
void nv_uml_class_clear_qualifier(struct UMLClass *c, enum UMLQualifier q);
int nv_uml_class_get_qualifier(struct UMLClass *c, enum UMLQualifier q);

void nv_uml_class_set_primitivetypes(struct UMLClass *c, List *l);
List *nv_uml_class_get_primitivetypes(struct UMLClass *c);

void nv_uml_class_set_datatypes(struct UMLClass *c, List *l);
List *nv_uml_class_get_datatypes(struct UMLClass *c);

void nv_uml_class_set_classes(struct UMLClass *c, List *l);
List *nv_uml_class_get_classes(struct UMLClass *c);

void nv_uml_class_set_associations(struct UMLClass *c, List *l);
List *nv_uml_class_get_associations(struct UMLClass *c);

#endif
