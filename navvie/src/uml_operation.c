#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uml_operation.h"
#include "uml_parameter.h"

struct UMLOperation *nv_uml_operation_new()
{
	struct UMLOperation *o = (struct UMLOperation *) malloc(sizeof(struct UMLOperation));
	if (o == NULL) {
		return NULL;
	}

	init_list(&o->parameters);
	o->return_parameter = NULL;
	o->qualifiers = 0;
	nv_uml_element_init(&o->super);
	return o;
}

void nv_uml_operation_delete(struct UMLOperation *o)
{
	if (o != NULL) {
		List *work;
		nv_uml_element_clear(&o->super);
		work =  o->parameters;
		while(work != NULL) {
			struct UMLParameter *p = (struct UMLParameter *) work->data;
			nv_uml_parameter_delete(p);
			work = work->next;
		}
		delete_list(o->parameters);

		free(o);
	}
}

void nv_uml_operation_add_parameter(struct UMLOperation *o, struct UMLParameter *p)
{
	if (nv_uml_parameter_get_direction(p) == NV_RETURN) {
		if (o->return_parameter == NULL) {
			o->return_parameter = p;
		} else {
			fprintf(stderr, "Operation %s has multiple return parameters.\n",
				nv_get_name(o));
			fprintf(stderr, "Setting %s to IN.\n", nv_get_name(p));
			nv_uml_parameter_set_direction(p, NV_IN);
		}
	}
	/* order matters here so best push back */
	push_back_list(&o->parameters, p);
}

List *nv_uml_operation_get_parameters(struct UMLOperation *o)
{
	return o->parameters;
}

struct UMLParameter *nv_uml_operation_get_return_parameter(struct UMLOperation *o)
{
	return o->return_parameter;
}

void nv_uml_operation_set_qualifier(struct UMLOperation *o, enum UMLQualifier q)
{
	o->qualifiers |= q;
}

void nv_uml_operation_clear_qualifier(struct UMLOperation *o, enum UMLQualifier q)
{
	o->qualifiers &= ~q;
}

int nv_uml_operation_get_qualifier(struct UMLOperation *o, enum UMLQualifier q)
{
	return !!(o->qualifiers & q);
}
