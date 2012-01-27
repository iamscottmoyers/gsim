#include <stdlib.h>
#include <assert.h>

#include "uml_ownedend.h"

struct UMLOwnedEnd *nv_uml_ownedend_new(void)
{
	struct UMLOwnedEnd *o = (struct UMLOwnedEnd *) malloc(sizeof(struct UMLOwnedEnd));
	if (o == NULL) {
		return NULL;
	}
	nv_uml_element_init(&o->super);
	return o;
}

void nv_uml_ownedend_delete(struct UMLOwnedEnd *o)
{
	if (o != NULL) {
		nv_uml_element_clear(&o->super);
		free(o);
	}
}

void nv_uml_ownedend_set_type(struct UMLOwnedEnd *o, struct UMLClass *c)
{
	o->type = c;
}

struct UMLClass *nv_uml_ownedend_get_type(struct UMLOwnedEnd *o)
{
	return o->type;
}

void nv_uml_ownedend_set_association(struct UMLOwnedEnd *o, struct UMLAssociation *a)
{
	o->association = a;
}

struct UMLAssociation *nv_uml_ownedend_get_association(struct UMLOwnedEnd *o)
{
	return o->association;
}

void nv_uml_ownedend_set_multiplicity(struct UMLOwnedEnd *o, int v, enum UMLMultiplicity m)
{
	o->multiplicity[m] = v;
}

int nv_uml_ownedend_get_multiplicity(struct UMLOwnedEnd *o, enum UMLMultiplicity m)
{
	return o->multiplicity[m];
}
