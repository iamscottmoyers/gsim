#ifndef _NAVVIE_UML_OWNEDEND_H_
#define _NAVVIE_UML_OWNEDEND_H_

#include <stdio.h>
#include "uml_element.h"
#include "uml_multiplicity.h"

struct UMLClass;
struct UMLAssociation;

struct UMLOwnedEnd {
	struct UMLElement super;
	struct UMLClass *type;
	struct UMLAssociation *association;
	int multiplicity[2];
};

struct UMLOwnedEnd *nv_uml_ownedend_new();
void nv_uml_ownedend_delete(struct UMLOwnedEnd *a);

void nv_uml_ownedend_set_type(struct UMLOwnedEnd *o, struct UMLClass * c);
struct UMLClass * nv_uml_ownedend_get_type(struct UMLOwnedEnd *o);

void nv_uml_ownedend_set_association(struct UMLOwnedEnd *o, struct UMLAssociation *a);
struct UMLAssociation *nv_uml_ownedend_get_association(struct UMLOwnedEnd *o);

void nv_uml_ownedend_set_multiplicity(struct UMLOwnedEnd *o, int v, enum UMLMultiplicity m);
int nv_uml_ownedend_get_multiplicity(struct UMLOwnedEnd *o, enum UMLMultiplicity m);

#endif
