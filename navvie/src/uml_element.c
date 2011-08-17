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
	init_list(&e->comments);
	init_list(&e->constraints);
	e->stereotypes = 0;
}

void nv_uml_element_clear(struct UMLElement *e)
{
	if (e != NULL) {
		List *work;
		free(e->name);

		work = e->comments;
		while(work != NULL) {
			struct UMLComment *com = (struct UMLComment *) work->data;
			nv_uml_comment_delete(com);
			work = work->next;
		}
		delete_list(e->comments);

		work = e->constraints;
		while(work != NULL) {
			struct UMLConstraint *con = (struct UMLConstraint *) work->data;
			nv_uml_constraint_delete(con);
			work = work->next;
		}
		delete_list(e->constraints);
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

void nv_uml_element_add_comment(struct UMLElement *e, struct UMLComment *com)
{
	push_back_list(&e->comments, com);
}

void nv_uml_element_set_comments(struct UMLElement *e, List *coms)
{
	e->comments = coms;
}

List *nv_uml_element_get_comments(struct UMLElement *e)
{
	return e->comments;
}

void nv_uml_element_add_constraint(struct UMLElement *e, struct UMLConstraint *con)
{
	push_back_list(&e->constraints, con);
}

void nv_uml_element_set_constraints(struct UMLElement *e, List *cons)
{
	e->constraints = cons;
}

List *nv_uml_element_get_constraints(struct UMLElement *e)
{
	return e->constraints;
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
