#ifndef _NAVVIE_UML_MODEL_H_
#define _NAVVIE_UML_MODEL_H_

#include <stdio.h>

#include "uml_package.h"

struct UMLModel {
	struct UMLPackage super;
};

#define nv_uml_model_new (struct UMLModel *) nv_uml_package_new
#define nv_uml_model_delete(m) nv_uml_package_delete((struct UMLPackage *) m)

#endif
