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
		struct ListLink *iter;
		nv_uml_type_clear(&d->super);

		for(iter = nv_list_front(&d->attributes); iter;) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA(iter, struct UMLAttribute, link);
			iter = nv_list_next(iter);
			nv_uml_attribute_delete(a);
		}

		for(iter = nv_list_front(&d->operations); iter;) {
			struct UMLOperation *o = NV_UML_LIST_GET_DATA(iter, struct UMLOperation, link);
			iter = nv_list_next(iter);
			nv_uml_operation_delete(o);
		}

		free(d);
	}
}
