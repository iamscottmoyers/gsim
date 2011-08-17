#include <stdlib.h>

#include "uml_type.h"

void nv_uml_type_init(struct UMLType *t, enum UMLBaseType b)
{
	nv_uml_element_init(&t->super);
	t->base_type = b;
	init_list(&t->super_types);
}

void nv_uml_type_clear(struct UMLType *t)
{
	nv_uml_element_clear(&t->super);
	delete_list(t->super_types);
}

enum UMLBaseType nv_uml_type_get_base_type(struct UMLType *t)
{
	return t->base_type;
}

void nv_uml_type_add_super(struct UMLType *t, struct UMLType *s)
{
	push_back_list(&t->super_types, s);
}

List *nv_uml_type_get_supers(struct UMLType *t)
{
	return t->super_types;
}
