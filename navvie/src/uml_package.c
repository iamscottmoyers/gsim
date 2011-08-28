#include <stdlib.h>
#include <string.h>

#include "uml_package.h"
#include "uml_class.h"
#include "uml_enumeration.h"
#include "uml_primitivetype.h"
#include "uml_datatype.h"
#include "uml_association.h"

struct UMLPackage *nv_uml_package_new()
{
	struct UMLPackage *p = (struct UMLPackage *) malloc(sizeof(struct UMLPackage));
	if (p == NULL) {
		return NULL;
	}
	nv_uml_element_init(&p->super);
	init_list(&p->packages);
	init_list(&p->classes);
	init_list(&p->datatypes);
	nv_uml_list_init(&p->enumerations);
	init_list(&p->primitivetypes);
	init_list(&p->associations);
	return p;
}

void nv_uml_package_delete(struct UMLPackage *p)
{
	if (p != NULL) {
		List *work;
		nv_uml_element_clear(&p->super);
		work = p->packages;
		while(work != NULL) {
			struct UMLPackage *p = (struct UMLPackage *) work->data;
			nv_uml_package_delete(p);
			work = work->next;
		}
		delete_list(p->packages);

		work = p->classes;
		while(work != NULL) {
			struct UMLClass *c = (struct UMLClass *) work->data;
			nv_uml_class_delete(c);
			work = work->next;
		}
		delete_list(p->classes);

		work = p->datatypes;
		while(work != NULL) {
			struct UMLDataType *d = (struct UMLDataType *) work->data;
			nv_uml_datatype_delete(d);
			work = work->next;
		}
		delete_list(p->datatypes);

		{
			struct UMLListLink *iter;
			for(iter = nv_uml_list_front(&p->enumerations); iter; iter = nv_uml_list_next(iter)) {
				struct UMLEnumeration *e = NV_UML_LIST_GET_DATA(iter, struct UMLEnumeration, link);
				nv_uml_enumeration_delete(e);
			}
		}

		work = p->primitivetypes;
		while(work != NULL) {
			struct UMLPrimitiveType *p = (struct UMLPrimitiveType *) work->data;
			nv_uml_primitivetype_delete(p);
			work = work->next;
		}
		delete_list(p->primitivetypes);

		work = p->associations;
		while(work != NULL) {
			struct UMLAssociation *a = (struct UMLAssociation *) work->data;
			nv_uml_association_delete(a);
			work = work->next;
		}
		delete_list(p->associations);
		free(p);
	}
}

void nv_uml_package_set_packages(struct UMLPackage *p, List *l)
{
	p->packages = l;
}

List *nv_uml_package_get_packages(struct UMLPackage *p)
{
	return p->packages;
}

void nv_uml_package_set_classes(struct UMLPackage *p, List *l)
{
	p->classes = l;
}

List *nv_uml_package_get_classes(struct UMLPackage *p)
{
	return p->classes;
}

void nv_uml_package_set_datatypes(struct UMLPackage *p, List *l)
{
	p->datatypes = l;
}

List *nv_uml_package_get_datatypes(struct UMLPackage *p)
{
	return p->datatypes;
}

void nv_uml_package_set_primitivetypes(struct UMLPackage *p, List *l)
{
	p->primitivetypes = l;
}

List *nv_uml_package_get_primitivetypes(struct UMLPackage *p)
{
	return p->primitivetypes;
}

void nv_uml_package_set_associations(struct UMLPackage *p, List *l)
{
	p->associations = l;
}

List *nv_uml_package_get_associations(struct UMLPackage *p)
{
	return p->associations;
}
