#ifndef _NAVVIE_UML_PACKAGE_H_
#define _NAVVIE_UML_PACKAGE_H_

#include <stdio.h>

#include "list.h"
#include "uml_element.h"

struct UMLPackage {
	struct UMLElement super;
	List *packages;
	List *classes;
	List *datatypes;
	List *enumerations;
	List *primitivetypes;
	List *associations;
};

struct UMLPackage *nv_uml_package_new();
void nv_uml_package_delete(struct UMLPackage *p);

void nv_uml_package_set_packages(struct UMLPackage *p, List *l);
List *nv_uml_package_get_packages(struct UMLPackage *p);

void nv_uml_package_set_classes(struct UMLPackage *p, List *l);
List *nv_uml_package_get_classes(struct UMLPackage *p);

void nv_uml_package_set_datatypes(struct UMLPackage *p, List *l);
List *nv_uml_package_get_datatypes(struct UMLPackage *p);

void nv_uml_package_set_enumerations(struct UMLPackage *p, List *l);
List *nv_uml_package_get_enumerations(struct UMLPackage *p);

void nv_uml_package_set_primitivetypes(struct UMLPackage *p, List *l);
List *nv_uml_package_get_primitivetypes(struct UMLPackage *p);

void nv_uml_package_set_associations(struct UMLPackage *p, List *l);
List *nv_uml_package_get_associations(struct UMLPackage *p);

#endif
