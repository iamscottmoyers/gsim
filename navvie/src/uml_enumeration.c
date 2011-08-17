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
	init_list(&e->literals);
	return e;
}

void nv_uml_enumeration_delete(struct UMLEnumeration *e)
{
	if (e != NULL) {
		List *work;
		nv_uml_type_clear(&e->super);

		work = e->literals;
		while(work != NULL) {
			struct UMLLiteral *l = (struct UMLLiteral *)work->data;
			nv_uml_literal_delete(l);
			work = work->next;
		}
		delete_list(e->literals);

		free(e);
	}
}

void nv_uml_enumeration_add_literal(struct UMLEnumeration *e, struct UMLLiteral *l)
{
	push_back_list(&e->literals, l);
}

List *nv_uml_enumeration_get_literals(struct UMLEnumeration *e)
{
	return e->literals;
}
