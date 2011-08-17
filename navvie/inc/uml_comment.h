#ifndef _NAVVIE_UML_COMMENT_H_
#define _NAVVIE_UML_COMMENT_H_

struct UMLComment {
	char *body;
};

struct UMLComment *nv_uml_comment_new();
void nv_uml_comment_delete(struct UMLComment *c);

void nv_uml_comment_set_body(struct UMLComment *c, const char *body);
const char *nv_uml_comment_get_body(struct UMLComment *c);

#endif
