#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphviz_writer.h"
#include "uml_element.h"

#include "util.h"
#include "list.h"

#include "uml_class.h"
#include "uml_package.h"
#include "uml_datatype.h"
#include "uml_model.h"
#include "uml_enumeration.h"
#include "uml_operation.h"
#include "uml_primitivetype.h"
#include "uml_parameter.h"
#include "uml_literal.h"
#include "uml_attribute.h"
#include "uml_association.h"
#include "uml_ownedend.h"
#include "uml_comment.h"

static char visibility_char(enum UMLVisibility v)
{
	switch(v) {
		case NV_PUBLIC:
			return '+';
		case NV_PROTECTED:
			return '#';
		case NV_PRIVATE:
			return '-';
		case NV_PACKAGE:
			return ' ';
	}

	/* Dead code */
	return ' ';
}

static void nv_graphviz_write_attribute(FILE *fp, struct UMLAttribute *a)
{
	const char *atname;
	enum UMLVisibility v;
	struct UMLType *t;

	/* Attributes with associations will be represented with
	   association arrows rather than appearing in the class */
	if(nv_uml_attribute_get_association(a) != NULL) {
		return;
	}

	atname = nv_uml_element_get_name((struct UMLElement *)a);
	v = nv_uml_element_get_visibility((struct UMLElement *)a);
	t = nv_uml_attribute_get_type(a);
	fprintf(fp, "%c ", visibility_char(v));
	fprintf(fp, "%s : ", atname);
	fprintf(fp, "%s", nv_uml_element_get_name((struct UMLElement *)t));
	fprintf(fp, "\\l");
}

static void nv_graphviz_write_operation(FILE *fp, struct UMLOperation *o)
{
	List *l;
	char comma[3] = {'\0','\0','\0'};
	struct UMLParameter *p;
	struct UMLType *t;
	enum UMLVisibility v;

	v = nv_uml_element_get_visibility((struct UMLElement *)o);
	fprintf(fp, "%c ", visibility_char(v));
	if( nv_uml_element_get_stereotypes((struct UMLElement *)o) & NV_STEREOTYPE_DESTROY) {
		fprintf(fp, "~");
	}
	fprintf(fp, "%s(",nv_get_name(o));
	for(l = nv_uml_operation_get_parameters(o);l ;l = l->next) {
		struct UMLParameter *p = (struct UMLParameter *) l->data;
		if (nv_uml_parameter_get_direction(p) != NV_RETURN) {
			t = nv_uml_parameter_get_type(p);
			fprintf(fp, "%s", comma);
			fprintf(fp, "%s ", nv_uml_element_get_name((struct UMLElement *)t));
			fprintf(fp, "%s", nv_uml_element_get_name((struct UMLElement *)p));
			comma[0] = ',';
			comma[1] = ' ';
		}
	}

	fprintf(fp, ")");
	p = nv_uml_operation_get_return_parameter(o);
	if (p != NULL) {
		t = nv_uml_parameter_get_type(p);
		fprintf(fp, ":%s ", nv_uml_element_get_name((struct UMLElement *)t));
		fprintf(fp, "%s", nv_uml_element_get_name((struct UMLElement *)p));
	}
	fprintf(fp, "\\l");
}

static void nv_graphviz_write_class(FILE *fp, struct UMLClass *c)
{
	struct UMLListLink *iter;
	const char *name = nv_uml_element_get_name((struct UMLElement *)c);
	fprintf(fp, "\tnode [shape = \"record\"]\n");
	fprintf(fp, "\t%s [\n", name);
	fprintf(fp, "\t\tlabel = \"{%s|", name);

	/* print attributes */
	{
		struct UMLListLink *iter;
		for(iter = nv_uml_list_front(&c->attributes); iter; iter = nv_uml_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			nv_graphviz_write_attribute(fp, a);
		}
	}
	fprintf(fp, "|");

	/* print operations */
	for(iter = nv_uml_list_front(&c->operations); iter; iter = nv_uml_list_next(iter)) {
		struct UMLOperation *o = NV_UML_LIST_GET_DATA( iter, struct UMLOperation, link );
		nv_graphviz_write_operation(fp, o);
	}
	fprintf(fp, "}\"\n\t]\n");
}

/* TODO: We could do with a proper multiplicity object rather than an array. */
static void nv_graphviz_write_multiplicity(FILE *fp, int m[])
{
	if(m[NV_LOWER] == NV_MANY)
	{
		fprintf(fp, "*");
	}
	else
	{
		fprintf(fp, "%d", m[NV_LOWER]);
	}

	if(m[NV_LOWER] != m[NV_UPPER])
	{
		fprintf(fp, "..");
		if(m[NV_UPPER] == NV_MANY)
		{
			fprintf(fp, "*");
		}
		else
		{
			fprintf(fp, "%d", m[NV_UPPER]);
		}
	}

}

static void nv_graphviz_write_ownedend(FILE *fp, struct UMLOwnedEnd *o)
{
	struct UMLClass *c = nv_uml_ownedend_get_type(o);
	const char *name = nv_uml_element_get_name((struct UMLElement *)c);
	fprintf(fp, "\tedge [\n");
	fprintf(fp, "\t\tarrowhead = \"open\"\n");
	fprintf(fp, "\t\ttaillabel = \"");
	nv_graphviz_write_multiplicity(fp, o->multiplicity);
	fprintf(fp, "\"\n");
	fprintf(fp, "\t]\n");

	fprintf(fp, "\t%s -> ", name);
	{
		struct UMLListLink *iter;
		for(iter = nv_uml_list_front(&c->attributes); iter; iter = nv_uml_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			if(nv_uml_attribute_get_association(a) == nv_uml_ownedend_get_association(o)) {
				struct UMLType *t = nv_uml_attribute_get_type(a);
				fprintf(fp, "%s ", nv_uml_element_get_name((struct UMLElement *)t));
				fprintf(fp, "[headlabel = \"");
				nv_graphviz_write_multiplicity(fp, a->multiplicity);
				fprintf(fp, "\"]");
				fprintf(fp, "\n");
				break;
			}
		}
	}
}

static void nv_graphviz_write_association(FILE *fp, struct UMLAssociation *a)
{
	unsigned int i;
	for(i = 0; i < 2; i++) {
		struct UMLOwnedEnd *o = nv_uml_association_get_ownedend(a, i);
		if(o != NULL)
		{
			nv_graphviz_write_ownedend(fp, o);
		}
	}
}

void nv_graphviz_write_model(struct UMLModel *m, FILE *fp)
{
	List *l;
	fprintf(fp, "digraph %s {\n",
	        nv_uml_element_get_name((struct UMLElement *) m));
	fprintf(fp, "\trankdir = \"BT\"\n");
	/* print classes */
	for(l = nv_uml_model_get_classes(m); l; l = l->next) {
		nv_graphviz_write_class(fp, (struct UMLClass *) l->data);
	}
	/* print associations */
	for(l = nv_uml_model_get_associations(m); l; l = l->next) {
		struct UMLAssociation *a = (struct UMLAssociation *)l->data;
		nv_graphviz_write_association(fp, a);
	}
	/* print class generalizations */
	for(l = nv_uml_model_get_classes(m); l; l = l->next) {
		List *l2;
		const char *src_name = nv_uml_element_get_name((struct UMLElement *)l->data);
		for(l2 = nv_uml_type_get_supers((struct UMLType *) l->data); l2; l2 = l2->next) {
			const char *dst_name = nv_uml_element_get_name((struct UMLElement *)l2->data);
			fprintf(fp, "\tedge [\n" );
			fprintf(fp, "\t\tarrowhead = \"empty\"\n");
			fprintf(fp, "\t]\n" );
			fprintf(fp, "\t%s -> %s\n", src_name, dst_name );
		}
	}

	fprintf(fp, "}\n");
}
