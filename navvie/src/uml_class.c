#include <stdlib.h>
#include <string.h>

#include "uml_class.h"
#include "uml_attribute.h"
#include "uml_operation.h"
#include "uml_enumeration.h"
#include "uml_primitivetype.h"
#include "uml_datatype.h"
#include "uml_association.h"

struct UMLClass *nv_uml_class_new(void)
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
		nv_uml_type_clear(&c->super);
		NV_LIST_FOREACH(&c->attributes,     struct UMLAttribute,     link, nv_uml_attribute_delete    );
		NV_LIST_FOREACH(&c->operations,     struct UMLOperation,     link, nv_uml_operation_delete    );
		NV_LIST_FOREACH(&c->enumerations,   struct UMLEnumeration,   link, nv_uml_enumeration_delete  );
		NV_LIST_FOREACH(&c->primitivetypes, struct UMLPrimitiveType, link, nv_uml_primitivetype_delete);
		NV_LIST_FOREACH(&c->datatypes,      struct UMLDataType,      link, nv_uml_datatype_delete     );
		NV_LIST_FOREACH(&c->nested_classes, struct UMLClass,         link, nv_uml_class_delete        );
		NV_LIST_FOREACH(&c->associations,   struct UMLAssociation,   link, nv_uml_association_delete  );
		free(c);
	}
}

void nv_uml_class_set_qualifier(struct UMLClass *c, enum UMLQualifier q)
{
	c->qualifiers |= q;
}

int nv_uml_class_get_qualifier(struct UMLClass *c, enum UMLQualifier q)
{
	return c->qualifiers & q;
}
