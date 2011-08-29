#ifndef _NAVVIE_UML_TYPE_H_
#define _NAVVIE_UML_TYPE_H_

#include "uml_element.h"
#include "uml_list.h"

/* is this type a class/enum/datatype? */
enum UMLBaseType {
	NV_NO_TYPE,
	NV_CLASS,
	NV_PRIMITIVETYPE,
	NV_DATATYPE,
	NV_ENUMERATION
};

struct UMLType {
	struct UMLElement super;
	enum UMLBaseType base_type;
	struct UMLList super_types;
	struct UMLListLink link;
};

void nv_uml_type_init(struct UMLType *t, enum UMLBaseType b);
void nv_uml_type_clear(struct UMLType *t);

enum UMLBaseType nv_uml_type_get_base_type(struct UMLType *t);

#endif
