#include <stdlib.h>
#include <string.h>

#include "uml_enumeration.h"
#include "uml_literal.h"

struct UMLEnumeration *nv_uml_enumeration_new()
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
		struct ListLink *iter;
		nv_uml_type_clear(&e->super);

		for(iter = nv_list_front(&e->literals); iter;) {
			struct UMLLiteral *l = NV_UML_LIST_GET_DATA(iter, struct UMLLiteral, link);
			iter = nv_list_next(iter);
			nv_uml_literal_delete(l);
		}

		free(e);
	}
}
