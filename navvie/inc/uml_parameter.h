#ifndef _NAVVIE_UML_PARAMETER_H_
#define _NAVVIE_UML_PARAMETER_H_

#include "uml_element.h"
#include "list.h"
#include "uml_multiplicity.h"

struct UMLType;

enum UMLParameterDirection {
	NV_RETURN,
	NV_IN,
	NV_OUT,
	NV_INOUT
};

struct UMLParameter {
	struct UMLElement super;
	struct UMLType *type;
	enum UMLParameterDirection direction;
	int multiplicity[2];
	struct ListLink link;
};

struct UMLParameter *nv_uml_parameter_new();
void nv_uml_parameter_delete(struct UMLParameter *p);

void nv_uml_parameter_set_type(struct UMLParameter *p, struct UMLType *type);
struct UMLType *nv_uml_parameter_get_type(struct UMLParameter *p);

void nv_uml_parameter_set_direction(struct UMLParameter *p, enum UMLParameterDirection d);
enum UMLParameterDirection nv_uml_parameter_get_direction(struct UMLParameter *p);
const char *nv_uml_parameter_direction_string(struct UMLParameter *p);

void nv_uml_parameter_set_multiplicity(struct UMLParameter *p, int v, enum UMLMultiplicity m);
int nv_uml_parameter_get_multiplicity(struct UMLParameter *p, enum UMLMultiplicity m);

#endif
