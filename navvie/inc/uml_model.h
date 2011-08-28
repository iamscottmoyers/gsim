#ifndef _NAVVIE_UML_MODEL_H_
#define _NAVVIE_UML_MODEL_H_

#include <stdio.h>

#include "list.h"
#include "uml_package.h"

struct UMLModel {
	struct UMLPackage super;
};

#define nv_uml_model_new (struct UMLModel *) nv_uml_package_new
#define nv_uml_model_delete(m) nv_uml_package_delete((struct UMLPackage *) m)

#define nv_uml_model_set_packages(m, l) nv_uml_package_set_packages((struct UMLPackage *) m, l)
#define nv_uml_model_get_packages(m) nv_uml_package_get_packages((struct UMLPackage *) m)

#define nv_uml_model_set_classes(m, l) nv_uml_package_set_classes((struct UMLPackage *) m, l)
#define nv_uml_model_get_classes(m) nv_uml_package_get_classes((struct UMLPackage *) m)

#define nv_uml_model_set_datatypes(m, l) nv_uml_package_set_datatypes((struct UMLPackage *) m, l)
#define nv_uml_model_get_datatypes(a) nv_uml_package_get_datatypes((struct UMLPackage *) a)

#define nv_uml_model_set_primitivetypes(m, l) nv_uml_package_set_primitivetypes((struct UMLPackage *) m, l)
#define nv_uml_model_get_primitivetypes(a) nv_uml_package_get_primitivetypes((struct UMLPackage *) a)

#define nv_uml_model_set_associations(m, l) nv_uml_package_set_associations((struct UMLPackage *) m, l)
#define nv_uml_model_get_associations(a) nv_uml_package_get_associations((struct UMLPackage *) a)

#endif
