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
	init_list(&c->attributes);
	init_list(&c->operations);
	init_list(&c->enumerations);
	init_list(&c->primitivetypes);
	init_list(&c->datatypes);
	init_list(&c->nested_classes);
	init_list(&c->associations);
	c->qualifiers = 0;
	return c;
}

void nv_uml_class_delete(struct UMLClass *c)
{
	if (c != NULL) {
		List *work;
		nv_uml_type_clear(&c->super);

		work = c->attributes;
		while(work != NULL) {
			struct UMLAttribute *a = (struct UMLAttribute *) work->data;
			nv_uml_attribute_delete(a);
			work = work->next;
		}
		delete_list(c->attributes);

		work = c->operations;
		while(work != NULL) {
			struct UMLOperation *o = (struct UMLOperation *) work->data;
			nv_uml_operation_delete(o);
			work = work->next;
		}
		delete_list(c->operations);

		work = c->enumerations;
		while(work != NULL) {
			struct UMLEnumeration *e = (struct UMLEnumeration *) work->data;
			nv_uml_enumeration_delete(e);
			work = work->next;
		}
		delete_list(c->enumerations);

		work = c->primitivetypes;
		while(work != NULL) {
			struct UMLPrimitiveType *p = (struct UMLPrimitiveType *) work->data;
			nv_uml_primitivetype_delete(p);
			work = work->next;
		}
		delete_list(c->primitivetypes);

		work = c->datatypes;
		while(work != NULL) {
			struct UMLDataType *d = (struct UMLDataType *) work->data;
			nv_uml_datatype_delete(d);
			work = work->next;
		}
		delete_list(c->datatypes);

		work = c->nested_classes;
		while(work != NULL) {
			struct UMLClass *c= (struct UMLClass *) work->data;
			nv_uml_class_delete(c);
			work = work->next;
		}
		delete_list(c->nested_classes);

		work = c->associations;
		while(work != NULL) {
			struct UMLAssociation *a = (struct UMLAssociation *) work->data;
			nv_uml_association_delete(a);
			work = work->next;
		}
		delete_list(c->associations);
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

void nv_uml_class_set_attributes(struct UMLClass *c, List *l)
{
	c->attributes = l;
}

List *nv_uml_class_get_attributes(struct UMLClass *c)
{
	return c->attributes;
}

void nv_uml_class_set_operations(struct UMLClass *c, List *l)
{
	c->operations = l;
}

List *nv_uml_class_get_operations(struct UMLClass *c)
{
	return c->operations;
}

void nv_uml_class_set_enumerations(struct UMLClass *c, List *l)
{
	c->enumerations = l;
}

List *nv_uml_class_get_enumerations(struct UMLClass *c)
{
	return c->enumerations;
}

void nv_uml_class_set_primitivetypes(struct UMLClass *c, List *l)
{
	c->primitivetypes = l;
}

List *nv_uml_class_get_primitivetypes(struct UMLClass *c)
{
	return c->primitivetypes;
}

void nv_uml_class_set_datatypes(struct UMLClass *c, List *l)
{
	c->datatypes = l;
}

List *nv_uml_class_get_datatypes(struct UMLClass *c)
{
	return c->datatypes;
}

void nv_uml_class_set_classes(struct UMLClass *c, List *l)
{
	c->nested_classes = l;
}

List *nv_uml_class_get_classes(struct UMLClass *c)
{
	return c->nested_classes;
}

void nv_uml_class_set_associations(struct UMLClass *c, List *l)
{
	c->associations = l;
}

List *nv_uml_class_get_associations(struct UMLClass *c)
{
	return c->associations;
}
