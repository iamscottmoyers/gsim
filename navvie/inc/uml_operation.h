#ifndef _NAVVIE_UML_OPERATION_H_
#define _NAVVIE_UML_OPERATION_H_

#include "uml_qualifier.h"
#include "uml_element.h"
#include "list.h"

struct UMLParameter;

struct UMLOperation {
	struct UMLElement super;
	struct List parameters;
	struct UMLParameter *return_parameter;
	unsigned int qualifiers;
	struct ListLink link;
};

struct UMLOperation *nv_uml_operation_new();
void nv_uml_operation_delete(struct UMLOperation *o);

void nv_uml_operation_add_parameter(struct UMLOperation *o, struct UMLParameter *p);
struct UMLParameter *nv_uml_operation_get_return_parameter(struct UMLOperation *o);

void nv_uml_operation_set_qualifier(struct UMLOperation *o, enum UMLQualifier q);
void nv_uml_operation_clear_qualifier(struct UMLOperation *o, enum UMLQualifier q);
int nv_uml_operation_get_qualifier(struct UMLOperation *o, enum UMLQualifier q);

#endif
