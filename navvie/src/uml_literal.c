#include <stdlib.h>
#include <string.h>

#include "uml_literal.h"

struct UMLLiteral *nv_uml_literal_new()
{
	struct UMLLiteral *l = (struct UMLLiteral *) malloc(sizeof(struct UMLLiteral));
	if (l == NULL) {
		return NULL;
	}
	nv_uml_element_init(&l->super);

	return l;
}

void nv_uml_literal_delete(struct UMLLiteral *l)
{
	if (l != NULL) {
		nv_uml_element_clear(&l->super);
		free(l);
	}
}
