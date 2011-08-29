#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include "uml_parameter.h"

struct UMLParameter *nv_uml_parameter_new()
{
	struct UMLParameter *p = (struct UMLParameter *) malloc(sizeof(struct UMLParameter));
	if (p == NULL) {
		return NULL;
	}
	nv_uml_element_init(&p->super);
	p->type = NULL;
	p->direction = NV_IN;
	return p;
}

void nv_uml_parameter_delete(struct UMLParameter *p)
{
	if (p != NULL) {
		nv_uml_element_clear(&p->super);
		free(p);
	}
}

void nv_uml_parameter_set_type(struct UMLParameter *p, struct UMLType *type)
{
	assert(p->type == NULL);
	p->type = type;
}

struct UMLType *nv_uml_parameter_get_type(struct UMLParameter *p)
{
	return p->type;
}

void nv_uml_parameter_set_direction(struct UMLParameter *p, enum UMLParameterDirection d)
{
	p->direction = d;
}

enum UMLParameterDirection nv_uml_parameter_get_direction(struct UMLParameter *p)
{
	return p->direction;
}

const char *nv_uml_parameter_direction_string(struct UMLParameter *p)
{
	switch(p->direction) {
	case NV_RETURN:
		return "RETURN";
	case NV_IN:
		return "IN";
	case NV_OUT:
		return "OUT";
	case NV_INOUT:
		return "INOUT";
	default:
		return "<UNKNOWN_PARAMETER_DIRECTION>";
	}
}

void nv_uml_parameter_set_multiplicity(struct UMLParameter *p, int v, enum UMLMultiplicity m)
{
	p->multiplicity[m] = v;
}

int nv_uml_parameter_get_multiplicity(struct UMLParameter *p, enum UMLMultiplicity m)
{
	return p->multiplicity[m];
}
