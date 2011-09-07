#ifndef _NAVVIE_UML_ATTRIBUTE_H_
#define _NAVVIE_UML_ATTRIBUTE_H_

#include "uml_qualifier.h"
#include "uml_element.h"
#include "uml_multiplicity.h"
#include "list.h"

struct UMLType;
struct UMLAssociation;

enum UMLAggregation {
	NV_NONE,
	NV_COMPOSITE,
	NV_AGGREGATION
};

struct UMLAttribute {
	struct UMLElement super;
	struct UMLType *type;
	struct ListLink link;
	int qualifiers;
	struct UMLAssociation *association;
	enum UMLAggregation aggregation;
	int multiplicity[2];
};

struct UMLAttribute *nv_uml_attribute_new();
void nv_uml_attribute_delete(struct UMLAttribute *a);

void nv_uml_attribute_set_type(struct UMLAttribute *a, struct UMLType *type);
struct UMLType *nv_uml_attribute_get_type(struct UMLAttribute *a);

void nv_uml_attribute_set_qualifier(struct UMLAttribute *a, enum UMLQualifier q);
int nv_uml_attribute_get_qualifier(struct UMLAttribute *a, enum UMLQualifier q);

void nv_uml_attribute_set_association(struct UMLAttribute *a, struct UMLAssociation *as);
struct UMLAssociation *nv_uml_attribute_get_association(struct UMLAttribute *a);

void nv_uml_attribute_set_aggregation(struct UMLAttribute *a, enum UMLAggregation ag);
enum UMLAggregation nv_uml_attribute_get_aggregation(struct UMLAttribute *a);

void nv_uml_attribute_set_multiplicity(struct UMLAttribute *a, int v, enum UMLMultiplicity m);
int nv_uml_attribute_get_multiplicity(struct UMLAttribute *a, enum UMLMultiplicity m);

#endif
