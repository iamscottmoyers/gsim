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
	nv_uml_list_init(&p->packages);
	nv_uml_list_init(&p->classes);
	nv_uml_list_init(&p->datatypes);
	nv_uml_list_init(&p->enumerations);
	nv_uml_list_init(&p->primitivetypes);
	nv_uml_list_init(&p->associations);
	return p;
}

void nv_uml_package_delete(struct UMLPackage *p)
{
	if (p != NULL) {
		struct UMLListLink *iter;
		nv_uml_element_clear(&p->super);

		for(iter = nv_uml_list_front(&p->packages); iter;) {
			struct UMLPackage *p2 = NV_UML_LIST_GET_DATA(iter, struct UMLPackage, link);
			iter = nv_uml_list_next(iter);
			nv_uml_package_delete(p2);
		}

		for(iter = nv_uml_list_front(&p->classes); iter;) {
			struct UMLClass *c = NV_UML_LIST_GET_DATA(iter, struct UMLClass, link);
			iter = nv_uml_list_next(iter);
			nv_uml_class_delete(c);
		}

		for(iter = nv_uml_list_front(&p->datatypes); iter;) {
			struct UMLDataType *d = NV_UML_LIST_GET_DATA(iter, struct UMLDataType, link);
			iter = nv_uml_list_next(iter);
			nv_uml_datatype_delete(d);
		}

		for(iter = nv_uml_list_front(&p->enumerations); iter;) {
			struct UMLEnumeration *e = NV_UML_LIST_GET_DATA(iter, struct UMLEnumeration, link);
			iter = nv_uml_list_next(iter);
			nv_uml_enumeration_delete(e);
		}

		for(iter = nv_uml_list_front(&p->primitivetypes); iter;) {
			struct UMLPrimitiveType *p = NV_UML_LIST_GET_DATA(iter, struct UMLPrimitiveType, link);
			iter = nv_uml_list_next(iter);
			nv_uml_primitivetype_delete(p);
		}

		for(iter = nv_uml_list_front(&p->associations); iter;) {
			struct UMLAssociation *a = NV_UML_LIST_GET_DATA(iter, struct UMLAssociation, link);
			iter = nv_uml_list_next(iter);
			nv_uml_association_delete(a);
		}

		free(p);
	}
}
