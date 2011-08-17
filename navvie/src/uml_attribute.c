#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "uml_attribute.h"

struct UMLAttribute *nv_uml_attribute_new()
{
	struct UMLAttribute *a = (struct UMLAttribute *) malloc(sizeof(struct UMLAttribute));
	if (a == NULL) {
		return NULL;
	}
	nv_uml_element_init(&a->super);
	a->type = NULL;
	a->qualifiers = 0;
	a->association = NULL;
	a->aggregation = NV_NONE;
	return a;
}

void nv_uml_attribute_delete(struct UMLAttribute *a)
{
	if (a != NULL) {
		nv_uml_element_clear(&a->super);
		free(a);
	}
}

void nv_uml_attribute_set_type(struct UMLAttribute *a, struct UMLType *type)
{
	assert(a->type == NULL);
	a->type = type;
}

struct UMLType *nv_uml_attribute_get_type(struct UMLAttribute *a)
{
	return a->type;
}

void nv_uml_attribute_set_qualifier(struct UMLAttribute *a, enum UMLQualifier q)
{
	a->qualifiers |= q;
}

void nv_uml_attribute_clear_qualifier(struct UMLAttribute *a, enum UMLQualifier q)
{
	a->qualifiers &= ~q;
}

int nv_uml_attribute_get_qualifier(struct UMLAttribute *a, enum UMLQualifier q)
{
	return !!(a->qualifiers & q);
}

void nv_uml_attribute_set_association(struct UMLAttribute *a, struct UMLAssociation *as)
{
	a->association = as;
}

struct UMLAssociation *nv_uml_attribute_get_association(struct UMLAttribute *a)
{
	return a->association;
}

void nv_uml_attribute_set_aggregation(struct UMLAttribute *a, enum UMLAggregation ag)
{
	a->aggregation = ag;
}

enum UMLAggregation nv_uml_attribute_get_aggregation(struct UMLAttribute *a)
{
	return a->aggregation;
}

void nv_uml_attribute_set_multiplicity(struct UMLAttribute *a, int v, enum UMLMultiplicity m)
{
	a->multiplicity[m] = v;
}

int nv_uml_attribute_get_multiplicity(struct UMLAttribute *a, enum UMLMultiplicity m)
{
	return a->multiplicity[m];
}
