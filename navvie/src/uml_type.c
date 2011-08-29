#include <stdlib.h>

#include "uml_type.h"

void nv_uml_type_init(struct UMLType *t, enum UMLBaseType b)
{
	nv_uml_element_init(&t->super);
	t->base_type = b;
	nv_uml_list_init(&t->super_types);
}

void nv_uml_type_clear(struct UMLType *t)
{
	nv_uml_element_clear(&t->super);
}

enum UMLBaseType nv_uml_type_get_base_type(struct UMLType *t)
{
	return t->base_type;
}
