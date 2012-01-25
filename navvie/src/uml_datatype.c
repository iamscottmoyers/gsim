#include <stdlib.h>
#include <string.h>

#include "uml_datatype.h"
#include "uml_attribute.h"
#include "uml_operation.h"

struct UMLDataType *nv_uml_datatype_new()
{
	struct UMLDataType *d = (struct UMLDataType *) malloc(sizeof(struct UMLDataType));
	if (d == NULL) {
		return NULL;
	}
	nv_uml_type_init(&d->super, NV_DATATYPE);
	nv_list_init(&d->attributes);
	nv_list_init(&d->operations);
	return d;
}

void nv_uml_datatype_delete(struct UMLDataType *d)
{
	if (d != NULL) {
		nv_uml_type_clear(&d->super);
		NV_LIST_DESTROY(&d->attributes, struct UMLAttribute, link, nv_uml_attribute_delete);
		NV_LIST_DESTROY(&d->operations, struct UMLOperation, link, nv_uml_operation_delete);
		free(d);
	}
}
