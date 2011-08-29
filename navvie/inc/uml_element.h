#ifndef _NAVVIE_UML_ELEMENT_H_
#define _NAVVIE_UML_ELEMENT_H_

#include "uml_visibility.h"
#include "uml_stereotype.h"
#include "list.h"

struct UMLComment;
struct UMLConstraint;

struct UMLElement {
	enum UMLVisibility visibility;
	struct List comments;
	struct List constraints;
	char *name;
	UMLStereotype stereotypes;
};

void nv_uml_element_init(struct UMLElement *e);
void nv_uml_element_clear(struct UMLElement *e);

void nv_uml_element_set_name(struct UMLElement *e, const char *name);
const char *nv_uml_element_get_name(struct UMLElement *e);
#define nv_get_name(e) nv_uml_element_get_name((struct UMLElement *) e)
#define nv_set_name(e, n) nv_uml_element_set_name((struct UMLElement *) e, n)

void nv_uml_element_set_visibility(struct UMLElement *e, enum UMLVisibility v);
enum UMLVisibility nv_uml_element_get_visibility(struct UMLElement *e);
#define nv_get_visibility(e) nv_uml_element_get_visibility((struct UMLElement *) e)
#define nv_set_visibility(e, n) nv_uml_element_set_visibility((struct UMLElement *) e, n)

void nv_uml_element_add_stereotypes(struct UMLElement *e, UMLStereotype s);
UMLStereotype nv_uml_element_get_stereotypes(struct UMLElement *e);

#endif
