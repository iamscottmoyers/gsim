#ifndef _NAVVIE_UML_LITERAL_H_
#define _NAVVIE_UML_LITERAL_H_

#include "uml_element.h"

struct UMLLiteral {
	struct UMLElement super;
	struct ListLink link;
};

struct UMLLiteral *nv_uml_literal_new(void);
void nv_uml_literal_delete(struct UMLLiteral *l);

#endif
