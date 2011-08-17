#include <stdlib.h>
#include <string.h>

#include "uml_constraint.h"

struct UMLConstraint *nv_uml_constraint_new()
{
	struct UMLConstraint *c = (struct UMLConstraint *) malloc(sizeof(struct UMLConstraint));
	if (c == NULL) {
		return NULL;
	}

	c->body = (char *)malloc(sizeof(char));
	c->body[0] = '\0';
	return c;
}

void nv_uml_constraint_delete(struct UMLConstraint *c)
{
	if (c != NULL) {
		free(c->body);
		free(c);
	}
}

void nv_uml_constraint_set_body(struct UMLConstraint *c, const char *body)
{
	if (c->body != NULL) {
		free(c->body);
	}

	c->body = (char *) malloc(sizeof(char) * (strlen(body) + 1));
	strcpy(c->body, body);
}

const char *nv_uml_constraint_get_body(struct UMLConstraint *c)
{
	return c->body;
}
