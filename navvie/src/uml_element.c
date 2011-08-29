#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "uml_element.h"
#include "uml_comment.h"
#include "uml_constraint.h"

void nv_uml_element_init(struct UMLElement *e)
{
	e->name = (char *)malloc(sizeof(char));
	e->name[0] = '\0';
	e->visibility = NV_PUBLIC;
	nv_uml_list_init(&e->comments);
	nv_uml_list_init(&e->constraints);
	e->stereotypes = 0;
}

void nv_uml_element_clear(struct UMLElement *e)
{
	if (e != NULL) {
		struct UMLListLink *iter;
		free(e->name);

		for(iter = nv_uml_list_front(&e->comments); iter;) {
			struct UMLComment *com = NV_UML_LIST_GET_DATA(iter, struct UMLComment, link);
			iter = nv_uml_list_next(iter);
			nv_uml_comment_delete(com);
		}

		for(iter = nv_uml_list_front(&e->constraints); iter;) {
			struct UMLConstraint *con = NV_UML_LIST_GET_DATA(iter, struct UMLConstraint, link);
			iter = nv_uml_list_next(iter);
			nv_uml_constraint_delete(con);
		}
	}
}

void nv_uml_element_set_name(struct UMLElement *e, const char *name)
{
	if (e->name != NULL) {
		free(e->name);
	}

	e->name = (char *) malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(e->name, name);
}

const char *nv_uml_element_get_name(struct UMLElement *e)
{
	return e->name;
}

void nv_uml_element_set_visibility(struct UMLElement *e, enum UMLVisibility v)
{
	e->visibility = v;
}

enum UMLVisibility nv_uml_element_get_visibility(struct UMLElement *e)
{
	return e->visibility;
}

void nv_uml_element_add_stereotypes(struct UMLElement *e, UMLStereotype s)
{
	e->stereotypes |= s;
}

UMLStereotype nv_uml_element_get_stereotypes(struct UMLElement *e)
{
	return e->stereotypes;
}
