#include <stdlib.h>
#include <string.h>

#include "uml_package.h"
#include "uml_class.h"
#include "uml_enumeration.h"
#include "uml_primitivetype.h"
#include "uml_datatype.h"
#include "uml_association.h"

struct UMLPackage *nv_uml_package_new(void)
{
	struct UMLPackage *p = (struct UMLPackage *) malloc(sizeof(struct UMLPackage));
	if (p == NULL) {
		return NULL;
	}
	nv_uml_element_init(&p->super);
	nv_list_init(&p->packages);
	nv_list_init(&p->classes);
	nv_list_init(&p->datatypes);
	nv_list_init(&p->enumerations);
	nv_list_init(&p->primitivetypes);
	nv_list_init(&p->associations);
	return p;
}

void nv_uml_package_delete(struct UMLPackage *p)
{
	if (p != NULL) {
		nv_uml_element_clear(&p->super);
		NV_LIST_FOREACH(&p->packages,       struct UMLPackage,       link, nv_uml_package_delete      );
		NV_LIST_FOREACH(&p->classes,        struct UMLClass,         link, nv_uml_class_delete        );
		NV_LIST_FOREACH(&p->datatypes,      struct UMLDataType,      link, nv_uml_datatype_delete     );
		NV_LIST_FOREACH(&p->enumerations,   struct UMLEnumeration,   link, nv_uml_enumeration_delete  );
		NV_LIST_FOREACH(&p->primitivetypes, struct UMLPrimitiveType, link, nv_uml_primitivetype_delete);
		NV_LIST_FOREACH(&p->associations,   struct UMLAssociation,   link, nv_uml_association_delete  );
		free(p);
	}
}
