#include <stdlib.h>
#include <assert.h>

#include "uml_association.h"
#include "uml_ownedend.h"

struct UMLAssociation *nv_uml_association_new(void)
{
	struct UMLAssociation *a = (struct UMLAssociation *) malloc(sizeof(struct UMLAssociation));
	if (a == NULL) {
		return NULL;
	}
	nv_uml_element_init(&a->super);
	a->owned_ends[0] = NULL;
	a->owned_ends[1] = NULL;
	return a;
}

void nv_uml_association_delete(struct UMLAssociation *a)
{
	if (a != NULL) {
		nv_uml_element_clear(&a->super);
		nv_uml_ownedend_delete(a->owned_ends[0]);
		nv_uml_ownedend_delete(a->owned_ends[1]);
		free(a);
	}
}

void nv_uml_association_set_ownedend(struct UMLAssociation *a, struct UMLOwnedEnd *o, unsigned int pos)
{
	assert(pos < 2);
	a->owned_ends[pos] = o;
}

struct UMLOwnedEnd *nv_uml_association_get_ownedend(struct UMLAssociation *a, unsigned int pos)
{
	assert(pos < 2);
	return a->owned_ends[pos];
}
