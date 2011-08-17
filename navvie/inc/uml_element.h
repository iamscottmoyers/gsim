#ifndef _NAVVIE_UML_ELEMENT_H_
#define _NAVVIE_UML_ELEMENT_H_

#include "uml_visibility.h"
#include "uml_stereotype.h"
#include "list.h"

struct UMLComment;
struct UMLConstraint;

struct UMLElement {
	enum UMLVisibility visibility;
	List *comments;
	List *constraints;
	char *name;
	UMLStereotype stereotypes;
};

void nv_uml_element_init(struct UMLElement *e);
void nv_uml_element_clear(struct UMLElement *e);

void nv_uml_element_set_name(struct UMLElement *e, const char *name);
const char *nv_uml_element_get_name(struct UMLElement *e);
#define nv_get_name(e) nv_uml_element_get_name((struct UMLElement *) e)
#define nv_set_name(e, n) nv_uml_element_set_name((struct UMLElement *) e, n)

void nv_uml_element_add_comment(struct UMLElement *e, struct UMLComment *com);
void nv_uml_element_set_comments(struct UMLElement *e, List *coms);
List *nv_uml_element_get_comments(struct UMLElement *e);
#define nv_add_comment(e, c) nv_uml_element_add_comment((struct UMLElement *) e, c)
#define nv_set_comments(e, cs) nv_uml_element_set_comments((struct UMLElement *) e, cs)
#define nv_get_comments(e) nv_uml_element_get_comments((struct UMLElement *) e)

void nv_uml_element_add_constraint(struct UMLElement *e, struct UMLConstraint *con);
void nv_uml_element_set_constraints(struct UMLElement *e, List *cons);
List *nv_uml_element_get_constraints(struct UMLElement *e);
#define nv_add_constraint(e, c) nv_uml_element_add_constraint((struct UMLElement *) e, c)
#define nv_set_constraints(e, cs) nv_uml_element_set_constraints((struct UMLElement *) e, cs)
#define nv_get_constraints(e) nv_uml_element_get_constraints((struct UMLElement *) e)

void nv_uml_element_set_visibility(struct UMLElement *e, enum UMLVisibility v);
enum UMLVisibility nv_uml_element_get_visibility(struct UMLElement *e);
#define nv_get_visibility(e) nv_uml_element_get_visibility((struct UMLElement *) e)
#define nv_set_visibility(e, n) nv_uml_element_set_visibility((struct UMLElement *) e, n)

void nv_uml_element_add_stereotypes(struct UMLElement *e, UMLStereotype s);
UMLStereotype nv_uml_element_get_stereotypes(struct UMLElement *e);

#endif
