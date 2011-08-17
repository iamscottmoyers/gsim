#include <stdlib.h>
#include <string.h>

#include "uml_primitivetype.h"

struct UMLPrimitiveType *nv_uml_primitivetype_new()
{
	struct UMLPrimitiveType *p = (struct UMLPrimitiveType *) malloc(sizeof(struct UMLPrimitiveType));
	if (p == NULL) {
		return NULL;
	}

	nv_uml_type_init(&p->super, NV_PRIMITIVETYPE);
	return p;
}


void nv_uml_primitivetype_delete(struct UMLPrimitiveType *p)
{
	if (p != NULL) {
		nv_uml_type_clear(&p->super);
		free(p);
	}
}
