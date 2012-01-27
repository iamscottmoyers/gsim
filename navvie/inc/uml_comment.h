#ifndef _NAVVIE_UML_COMMENT_H_
#define _NAVVIE_UML_COMMENT_H_

#include "list.h"

struct UMLComment {
	char *body;
	struct ListLink link;
};

struct UMLComment *nv_uml_comment_new(void);
void nv_uml_comment_delete(struct UMLComment *c);

void nv_uml_comment_set_body(struct UMLComment *c, const char *body);
const char *nv_uml_comment_get_body(struct UMLComment *c);

#endif
