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
	init_list(&d->attributes);
	init_list(&d->operations);
	return d;
}

void nv_uml_datatype_delete(struct UMLDataType *d)
{
	if (d != NULL) {
		List *work;
		nv_uml_type_clear(&d->super);

		work = d->attributes;
		while(work != NULL) {
			struct UMLAttribute *a = (struct UMLAttribute *) work->data;
			nv_uml_attribute_delete(a);
			work = work->next;
		}
		delete_list(d->attributes);

		work = d->operations;
		while(work != NULL) {
			struct UMLOperation *o = (struct UMLOperation *) work->data;
			nv_uml_operation_delete(o);
			work = work->next;
		}
		delete_list(d->operations);

		free(d);
	}
}

void nv_uml_datatype_set_attributes(struct UMLDataType *d, List *l)
{
	d->attributes = l;
}

List *nv_uml_datatype_get_attributes(struct UMLDataType *d)
{
	return d->attributes;
}

void nv_uml_datatype_set_operations(struct UMLDataType *d, List *l)
{
	d->operations = l;
}

List *nv_uml_datatype_get_operations(struct UMLDataType *d)
{
	return d->operations;
}
