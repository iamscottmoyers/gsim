#include <stdlib.h>
#include <string.h>

#include "uml_comment.h"

struct UMLComment *nv_uml_comment_new()
{
	struct UMLComment *c = (struct UMLComment *) malloc(sizeof(struct UMLComment));
	if (c == NULL) {
		return NULL;
	}

	c->body = (char *)malloc(sizeof(char));
	c->body[0] = '\0';
	return c;
}

void nv_uml_comment_delete(struct UMLComment *c)
{
	if (c != NULL) {
		free(c->body);
		free(c);
	}
}

void nv_uml_comment_set_body(struct UMLComment *c, const char *body)
{
	if (c->body != NULL) {
		free(c->body);
	}

	c->body = (char *) malloc(sizeof(char) * (strlen(body) + 1));
	strcpy(c->body, body);
}

const char *nv_uml_comment_get_body(struct UMLComment *c)
{
	return c->body;
}
