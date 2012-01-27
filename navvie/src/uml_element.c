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
	nv_list_init(&e->comments);
	nv_list_init(&e->constraints);
	e->stereotypes = 0;
}

void nv_uml_element_clear(struct UMLElement *e)
{
	if (e != NULL) {
		free(e->name);
		NV_LIST_FOREACH(&e->comments,    struct UMLComment,    link, nv_uml_comment_delete   );
		NV_LIST_FOREACH(&e->constraints, struct UMLConstraint, link, nv_uml_constraint_delete);
	}
}

void nv_uml_element_set_name(struct UMLElement *e, const char *name)
{
	free(e->name);
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
