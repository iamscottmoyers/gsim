#include <stdlib.h>
#include <string.h>

#include "uml_class.h"
#include "uml_attribute.h"
#include "uml_operation.h"
#include "uml_enumeration.h"
#include "uml_primitivetype.h"
#include "uml_datatype.h"
#include "uml_association.h"

struct UMLClass *nv_uml_class_new()
{
	struct UMLClass *c = (struct UMLClass *) malloc(sizeof(struct UMLClass));
	if (c == NULL) {
		return NULL;
	}
	nv_uml_type_init(&c->super, NV_CLASS);
	nv_list_init(&c->attributes);
	nv_list_init(&c->operations);
	nv_list_init(&c->enumerations);
	nv_list_init(&c->primitivetypes);
	nv_list_init(&c->datatypes);
	nv_list_init(&c->nested_classes);
	nv_list_init(&c->associations);
	c->qualifiers = 0;
	return c;
}

void nv_uml_class_delete(struct UMLClass *c)
{
	if (c != NULL) {
		struct ListLink *iter;
		nv_uml_type_clear(&c->super);

		for(iter = nv_list_front(&c->attributes); iter;) {
			struct UMLAttribute *a = NV_LIST_GET_DATA(iter, struct UMLAttribute, link);
			iter = nv_list_next(iter);
			nv_uml_attribute_delete(a);
		}

		for(iter = nv_list_front(&c->operations); iter;) {
			struct UMLOperation *o = NV_LIST_GET_DATA(iter, struct UMLOperation, link);
			iter = nv_list_next(iter);
			nv_uml_operation_delete(o);
		}

		for(iter = nv_list_front(&c->enumerations); iter;) {
			struct UMLEnumeration *e = NV_LIST_GET_DATA(iter, struct UMLEnumeration, link);
			iter = nv_list_next(iter);
			nv_uml_enumeration_delete(e);
		}

		for(iter = nv_list_front(&c->primitivetypes); iter;) {
			struct UMLPrimitiveType *p = NV_LIST_GET_DATA(iter, struct UMLPrimitiveType, link);
			iter = nv_list_next(iter);
			nv_uml_primitivetype_delete(p);
		}

		for(iter = nv_list_front(&c->datatypes); iter;) {
			struct UMLDataType *d = NV_LIST_GET_DATA(iter, struct UMLDataType, link);
			iter = nv_list_next(iter);
			nv_uml_datatype_delete(d);
		}

		for(iter = nv_list_front(&c->nested_classes); iter;) {
			struct UMLClass *c = NV_LIST_GET_DATA(iter, struct UMLClass, link);
			iter = nv_list_next(iter);
			nv_uml_class_delete(c);
		}

		for(iter = nv_list_front(&c->associations); iter;) {
			struct UMLAssociation *a = NV_LIST_GET_DATA(iter, struct UMLAssociation, link);
			iter = nv_list_next(iter);
			nv_uml_association_delete(a);
		}

		free(c);
	}
}

void nv_uml_class_set_qualifier(struct UMLClass *c, enum UMLQualifier q)
{
	c->qualifiers |= q;
}

void nv_uml_class_clear_qualifier(struct UMLClass *c, enum UMLQualifier q)
{
	c->qualifiers &= ~q;
}

int nv_uml_class_get_qualifier(struct UMLClass *c, enum UMLQualifier q)
{
	return !!(c->qualifiers & q);
}
