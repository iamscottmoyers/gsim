#include <stdlib.h>
#include <string.h>

#include "uml_enumeration.h"
#include "uml_literal.h"

struct UMLEnumeration *nv_uml_enumeration_new(void)
{
	struct UMLEnumeration *e = (struct UMLEnumeration *) malloc(sizeof(struct UMLEnumeration));
	if (e == NULL) {
		return NULL;
	}
	nv_uml_type_init(&e->super, NV_ENUMERATION);
	nv_list_init(&e->literals);
	return e;
}

void nv_uml_enumeration_delete(struct UMLEnumeration *e)
{
	if (e != NULL) {
		nv_uml_type_clear(&e->super);
		NV_LIST_FOREACH(&e->literals, struct UMLLiteral, link, nv_uml_literal_delete);
		free(e);
	}
}
