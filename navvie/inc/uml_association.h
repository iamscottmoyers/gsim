#ifndef _NAVVIE_UML_ASSOCIATION_H_
#define _NAVVIE_UML_ASSOCIATION_H_

#include "uml_element.h"
#include "uml_list.h"

struct UMLClass;
struct UMLOwnedEnd;

struct UMLAssociation {
	struct UMLElement super;
	struct UMLOwnedEnd *owned_ends[2];
	struct UMLListLink link;
};

struct UMLAssociation *nv_uml_association_new();
void nv_uml_association_delete(struct UMLAssociation *a);

void nv_uml_association_set_ownedend(struct UMLAssociation *a, struct UMLOwnedEnd *o, unsigned int pos);
struct UMLOwnedEnd *nv_uml_association_get_ownedend(struct UMLAssociation *a, unsigned int pos);

#endif
