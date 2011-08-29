#ifndef _NAVVIE_UML_PACKAGE_H_
#define _NAVVIE_UML_PACKAGE_H_

#include <stdio.h>

#include "uml_list.h"
#include "uml_element.h"

struct UMLPackage {
	struct UMLElement super;
	struct UMLList packages;
	struct UMLList classes;
	struct UMLList datatypes;
	struct UMLList enumerations;
	struct UMLList primitivetypes;
	struct UMLList associations;
	struct UMLListLink link;
};

struct UMLPackage *nv_uml_package_new();
void nv_uml_package_delete(struct UMLPackage *p);

#endif
