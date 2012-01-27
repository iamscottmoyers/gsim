#ifndef _NAVVIE_UML_PACKAGE_H_
#define _NAVVIE_UML_PACKAGE_H_

#include "list.h"
#include "uml_element.h"

struct UMLPackage {
	struct UMLElement super;
	struct List packages;
	struct List classes;
	struct List datatypes;
	struct List enumerations;
	struct List primitivetypes;
	struct List associations;
	struct ListLink link;
};

struct UMLPackage *nv_uml_package_new(void);
void nv_uml_package_delete(struct UMLPackage *p);

#endif
