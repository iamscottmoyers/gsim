#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <assert.h>

#include "xml_reader.h"

#include "hash-table.h"
#include "hash-string.h"
#include "compare-string.h"
#include "hash-pointer.h"
#include "compare-pointer.h"
#include "util.h"
#include "list.h"

#include "uml_class.h"
#include "uml_package.h"
#include "uml_datatype.h"
#include "uml_enumeration.h"
#include "uml_model.h"
#include "uml_operation.h"
#include "uml_primitivetype.h"
#include "uml_parameter.h"
#include "uml_literal.h"
#include "uml_association.h"
#include "uml_ownedend.h"
#include "uml_attribute.h"
#include "uml_parameter.h"
#include "uml_comment.h"
#include "uml_constraint.h"

struct xmlCharListNode {
	xmlChar *id;
	struct ListLink link;
};

static HashTable *types_hash; /* hashes an xmi id to the actual uml_type */
static HashTable *attr_to_id; /* hashes an attribute/parameter to it's xmi id */
static HashTable *super_ids; /* hashes a struct UMLType *to a List of xmlChar IDs */
static HashTable *ownedend_to_type_id;
static HashTable *attr_to_assoc_id;
static HashTable *id_to_assoc;
static HashTable *stereotypes; /* hashes a stereotyped element to the stereotype enum */

static void nv_xmlr_get_constraints(struct List *l, xmlNode *a_node);
static void nv_xmlr_get_comments(struct List *l, xmlNode *a_node);
static enum UMLVisibility nv_xmlr_get_visibility(xmlNode *a_node);

static void nv_xmlr_generalisations(xmlNode *a_node, struct UMLType *t);

static struct UMLPrimitiveType *nv_xmlr_primitivetype(xmlNode *a_node);
static void nv_xmlr_primitivetypes(struct List *l, xmlNode *a_node, const char *nested);

static void nv_xmlr_stereotypes(xmlNode *a_node);
static struct UMLModel *nv_xmlr_model(xmlNode *a_node);

static void nv_xmlr_multiplicity(xmlNode *a_node, int *multiplicity);

static void nv_xmlr_attribute_qualifiers(xmlNode *a_node, struct UMLAttribute *a);
static void nv_xmlr_attributes(struct List *l, xmlNode *a_node);

static void nv_xmlr_operation_parameters(xmlNode *a_node, struct UMLOperation *o);
static void nv_xmlr_operation_qualifiers(xmlNode *a_node, struct UMLOperation *o);
static struct UMLOperation *nv_xmlr_operation(xmlNode *a_node);
static void nv_xmlr_operations(struct List *l, xmlNode *a_node);

static void nv_xmlr_ownedends(xmlNode *a_node, struct UMLAssociation *a);
static struct UMLAssociation *nv_xmlr_association(xmlNode *a_node);
static void nv_xmlr_associations(struct List *l, xmlNode *a_node, const char *nested);

static void nv_xmlr_enumeration_literals(xmlNode *a_node, struct UMLEnumeration *e);
static struct UMLEnumeration *nv_xmlr_enumeration(xmlNode *a_node);
static void nv_xmlr_enumerations(struct List *l, xmlNode *a_node, const char *nested);

static struct UMLDataType *nv_xmlr_datatype(xmlNode *a_node);
static void nv_xmlr_datatypes(struct List *l, xmlNode *a_node, const char *nested);

static struct UMLClass *nv_xmlr_class(xmlNode *a_node);
static void nv_xmlr_classes(struct List *l, xmlNode *a_node, const char *nested);

static struct UMLPackage *nv_xmlr_package(xmlNode *a_node);
static void nv_xmlr_packages(struct List *l, xmlNode *a_node, const char *nested);

static void nv_xmlr_resolve_types();

/* iterates through the given node list (iter)  and executes the following code
   if it finds a node with the given name (a) */
#define nv_xml_on_node(iter, a)						\
	for(;iter; iter = iter->next)					\
		if (iter->type == XML_ELEMENT_NODE &&			\
		    !strcmp(iter->name, a))


static void xml_list_free(void *l)
{
	struct ListLink *iter;
	for(iter = nv_list_front((struct List *) l); iter;) {
		struct xmlCharListNode *n = NV_UML_LIST_GET_DATA(iter, struct xmlCharListNode, link);
		iter = nv_list_next(iter);
		free(n);
	}

	free(l);
}

static void xml_hash_list_insert(HashTable *s, struct UMLType *t, xmlChar *gen)
{
	struct List *l = (struct List *) hash_table_lookup(s, t);
	struct xmlCharListNode *node = (struct xmlCharListNode *) malloc(sizeof(struct xmlCharListNode));
	node->id = gen;
	if (l != NULL) {
		nv_list_push_back(l, &node->link);
	} else {
		l = (struct List *) malloc(sizeof(struct List));
		nv_list_init(l);
		nv_list_push_back(l, &node->link);
		hash_table_insert(s, t, l);
	}
}

static void nv_xmlr_get_constraints(struct List *l, xmlNode *a_node)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "ownedRule") {
		xmlNode *child_node = cur_node->children;
		nv_xml_on_node(child_node, "specification") {
			int body = 0;
			xmlNode *child2 = child_node->children;
			nv_xml_on_node(child2, "body") {
				if (body == 0) {
					struct UMLConstraint *con = nv_uml_constraint_new();
					xmlChar *body = xmlNodeGetContent(child2->children);
					nv_fix_new_lines(body);
					nv_uml_constraint_set_body(con, body);
					nv_list_push_back(l, &con->link);
					printf("----------- %s ---------\n", body);
					xmlFree(body);
				} else  {
					fprintf(stderr, "Multiple bodies in a constraint aren't"
						"supported, first one will be kept\n");
					break;
				}
				body = 1;
			}
		}
	}
}

static void nv_xmlr_get_comments(struct List *l, xmlNode *a_node)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "ownedComment") {
		xmlNode *child_node = cur_node->children;
		nv_xml_on_node(child_node, "body") {
			struct UMLComment *com = nv_uml_comment_new();
			xmlChar *body = xmlNodeGetContent(child_node->children);
			nv_fix_new_lines(body);
			nv_uml_comment_set_body(com, body);
			nv_list_push_back(l, &com->link);
			xmlFree(body);
		}
	}
}

static enum UMLVisibility nv_xmlr_get_visibility(xmlNode *a_node)
{
	xmlChar *v = xmlGetProp(a_node, "visibility");
	if (v != NULL) {
		if (!strcmp(v, "public")) {
			xmlFree(v);
			return NV_PUBLIC;
		} else if (!strcmp(v, "protected")) {
			xmlFree(v);
			return NV_PROTECTED;
		} else if (!strcmp(v, "private")) {
			xmlFree(v);
			return NV_PRIVATE;
		} else if (!strcmp(v, "package")) {
			xmlFree(v);
			return NV_PACKAGE;
		} else {
			fprintf(stderr, "Unrecognised Visibility %s\n", v);
			xmlFree(v);
			return NV_PUBLIC;
		}
	}
	return NV_PUBLIC;
}

static void nv_xmlr_generalisations(xmlNode *a_node, struct UMLType *t)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "generalization") {
		xmlChar *gen = xmlGetProp(cur_node, "general");
		if (gen != NULL) {
			xml_hash_list_insert(super_ids, t, gen);
		} else {
			fprintf(stderr, "Malformed xmi file: generalisation does not specify a classifier\n");
		}
	}
}

static struct UMLPrimitiveType *nv_xmlr_primitivetype(xmlNode *a_node)
{
	struct UMLPrimitiveType *p;
	xmlChar *name = xmlGetProp(a_node, "name");
	xmlChar *id = xmlGetProp(a_node, "id");

	p = nv_uml_primitivetype_new();
	nv_set_name(p, name);

	hash_table_insert(types_hash, id, p);
	xmlFree(name);
	/* xmlFree(id) This will be free'd with the hash */

	nv_xmlr_get_comments(&((struct UMLElement *)p)->comments, a_node);
	nv_xmlr_get_constraints(&((struct UMLElement *)p)->constraints, a_node);
	nv_set_visibility(p, nv_xmlr_get_visibility(a_node));
	nv_xmlr_generalisations(a_node, (struct UMLType *) p);

	return p;
}

static void nv_xmlr_primitivetypes(struct List *l, xmlNode *a_node, const char *nested)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, nested) {
		xmlChar *t = xmlGetProp(cur_node, "type");
		if (!strcmp((char *)t, "uml:PrimitiveType")) {
			struct UMLPrimitiveType *p = nv_xmlr_primitivetype(cur_node);
			nv_list_push_back(l, &p->link);
		}
		xmlFree(t);
	}
}

static void nv_xmlr_stereotypes(xmlNode *a_node)
{
	xmlNode *cur_node = a_node;

	/* If the first level is a model then we have no stereotypes */
	if( cur_node->type == XML_ELEMENT_NODE && !strcmp(cur_node->name, "Model") )
		return;

	for(cur_node = cur_node->children; cur_node; cur_node = cur_node->next) {
		/* To support elements with multiple stereotypes we must 'OR' the new stereotype with the old one */
		if (cur_node->type == XML_ELEMENT_NODE && !strcmp(cur_node->name, "Create")) {
			xmlChar *base = xmlGetProp(cur_node, "base_BehavioralFeature");
			UMLStereotype s = (intptr_t) hash_table_lookup(stereotypes, base);
			hash_table_insert(stereotypes, base, (void *) (intptr_t) (s | NV_STEREOTYPE_CREATE));
		} else if (cur_node->type == XML_ELEMENT_NODE && !strcmp(cur_node->name, "Destroy")) {
			xmlChar *base = xmlGetProp(cur_node, "base_BehavioralFeature");
			UMLStereotype s = (intptr_t) hash_table_lookup(stereotypes, base);
			hash_table_insert(stereotypes, base, (void *) (intptr_t) (s | NV_STEREOTYPE_DESTROY));
		}
	}

}

static struct UMLModel *nv_xmlr_model(xmlNode *a_node)
{
	xmlNode *cur_node = a_node;
	struct UMLModel *m = NULL;

	/* First level could be uml:Model or xmi:XMI */
	if( cur_node->type == XML_ELEMENT_NODE && strcmp(cur_node->name, "Model") )
		cur_node = cur_node->children;

	nv_xml_on_node(cur_node, "Model") {
		xmlChar *t = xmlGetProp(cur_node, "name");
		m = nv_uml_model_new();
		nv_set_name(m, t);
		xmlFree(t);
		nv_xmlr_get_comments(&((struct UMLElement *)m)->comments, cur_node);
		nv_xmlr_get_constraints(&((struct UMLElement *)m)->constraints, cur_node);
		nv_set_visibility(m, nv_xmlr_get_visibility(cur_node));

		nv_xmlr_primitivetypes(&m->super.primitivetypes, cur_node, "packagedElement");
		nv_xmlr_classes(&m->super.classes, cur_node, "packagedElement");
		nv_xmlr_datatypes(&m->super.datatypes, cur_node, "packagedElement");
		nv_xmlr_enumerations(&m->super.enumerations, cur_node, "packagedElement");
		nv_xmlr_packages(&m->super.packages, cur_node, "packagedElement");
		nv_xmlr_associations(&m->super.associations, cur_node, "packagedElement");

		nv_xmlr_resolve_types();
		break;
	}
	return m;
}

static void nv_xmlr_attribute_qualifiers(xmlNode *a_node, struct UMLAttribute *a)
{
	xmlChar *n = xmlGetProp(a_node, "isStatic");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_attribute_set_qualifier(a, NV_STATIC);
	}
	xmlFree(n);

	n = xmlGetProp(a_node, "isReadOnly");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_attribute_set_qualifier(a, NV_READONLY);
	}
	xmlFree(n);

	n = xmlGetProp(a_node, "isDerived");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_attribute_set_qualifier(a, NV_DERIVED);
	}
	xmlFree(n);

	n = xmlGetProp(a_node, "isDerivedUnion");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_attribute_set_qualifier(a, NV_DERIVEDUNION);
	}
	xmlFree(n);

	n = xmlGetProp(a_node, "isOrdered");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_attribute_set_qualifier(a, NV_ORDERED);
	}
	xmlFree(n);

	/* isUnique only appears as "false"
	   assume "true" unless we see false */
	nv_uml_attribute_set_qualifier(a, NV_UNIQUE);
	n = xmlGetProp(a_node, "isUnique");
	if (n != NULL && !strcmp(n, "false")) {
		nv_uml_attribute_clear_qualifier(a, NV_UNIQUE);
	}
	xmlFree(n);
}

static void nv_xmlr_multiplicity(xmlNode *a_node, int *multiplicity)
{
	xmlNode *cur_node = a_node->children;
	multiplicity[0] = 1;
	multiplicity[1] = 1;
	for(;cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (!strcmp(cur_node->name, "upperValue")) {
				xmlChar *v = xmlGetProp(cur_node, "value");
				if (v != NULL) {
					if (!strcmp(v, "*")) {
						multiplicity[NV_UPPER] = NV_MANY;
					} else {
						int intn = atoi(v);
						if (intn >= 0) {
							multiplicity[NV_UPPER] = intn;
						} else {
							fprintf(stderr,
								"Negative Multiplicity: %d\n",
								intn);
						}
					}
				} /* else multiplicity is left at 1 */
				xmlFree(v);
			} else if (!strcmp(cur_node->name, "lowerValue")) {
				xmlChar *v = xmlGetProp(cur_node, "value");
				if (v != NULL) {
					if (!strcmp(v, "*")) {
						multiplicity[NV_LOWER] = NV_MANY;
					} else {
						int intn = atoi(v);
						if (intn >= 0) {
							multiplicity[NV_LOWER] = intn;
						} else {
							fprintf(stderr,
								"Negative Multiplicity: %d\n",
								intn);
						}
					}
				} /* else multiplicity is left at 1 */
				xmlFree(v);
			}
		}
	}
}

static void nv_xmlr_attributes(struct List *l, xmlNode *a_node)
{
	int multiplicity[2];
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "ownedAttribute") {
		xmlChar *n;
		struct UMLAttribute *a = nv_uml_attribute_new();
		nv_list_push_back(l, &a->link);

		n = xmlGetProp(cur_node, "name");
		nv_set_name(a, n);
		xmlFree(n);

		nv_xmlr_get_comments(&((struct UMLElement *)a)->comments, cur_node);
		nv_xmlr_get_constraints(&((struct UMLElement *)a)->constraints, cur_node);
		nv_set_visibility(a, nv_xmlr_get_visibility(cur_node));

		nv_xmlr_attribute_qualifiers(cur_node, a);

		n = xmlGetProp(cur_node, "association");
		if (n != NULL) {
			hash_table_insert(attr_to_assoc_id, a, n);
			/* xmlFree(n) This will be free'd with the hash */
		}

		n = xmlGetProp(cur_node, "aggregation");
		if (n != NULL) {
			if (!strcmp(n, "composite")) {
				nv_uml_attribute_set_aggregation(a, NV_COMPOSITE);
			} else if (!strcmp(n, "shared")) {
				nv_uml_attribute_set_aggregation(a, NV_AGGREGATION);
			} else {
				fprintf(stderr, "Unknown aggregation type %s\n", n);
				nv_uml_attribute_set_aggregation(a, NV_NONE);
			}
		} else {
			nv_uml_attribute_set_aggregation(a, NV_NONE);
		}
		xmlFree(n);

		n = xmlGetProp(cur_node, "type");
		if (n != NULL) {
			hash_table_insert(attr_to_id, a, n);
			/* xmlFree(n) This will be free'd with the hash */
		} else {
			fprintf(stderr, "Attribute doesn't specify a type: %s\n",
				nv_get_name(a));
		}

		/* multiplicity */
		nv_xmlr_multiplicity(cur_node, multiplicity);
		nv_uml_attribute_set_multiplicity(a, multiplicity[NV_LOWER], NV_LOWER);
		nv_uml_attribute_set_multiplicity(a, multiplicity[NV_UPPER], NV_UPPER);
	}
}

static void nv_xmlr_operation_parameters(xmlNode *a_node, struct UMLOperation *o)
{
	int multiplicity[2];
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "ownedParameter") {
		/* get parameter name */
		struct UMLParameter *p = nv_uml_parameter_new();
		xmlChar *n = xmlGetProp(cur_node, "name");
		nv_set_name(p, n);
		xmlFree(n);
		nv_xmlr_get_comments(&((struct UMLElement *)p)->comments, cur_node);
		nv_xmlr_get_constraints(&((struct UMLElement *)p)->constraints, cur_node);
		nv_set_visibility(p, nv_xmlr_get_visibility(cur_node));

		/* get parameter direction */
		n = xmlGetProp(cur_node, "direction");
		if (n != NULL) {
			if (!strcmp(n, "return")) {
				nv_uml_parameter_set_direction(p, NV_RETURN);
			} else if (!strcmp(n, "in")) {
				nv_uml_parameter_set_direction(p, NV_IN);
			} else if (!strcmp(n, "out")) {
				nv_uml_parameter_set_direction(p, NV_OUT);
			} else if (!strcmp(n, "inout")) {
				nv_uml_parameter_set_direction(p, NV_INOUT);
			} else {
				fprintf(stderr, "Unrecognised direction: %s (Defaulting to IN)\n", n);
				nv_uml_parameter_set_direction(p, NV_IN);
			}
		} else {
			nv_uml_parameter_set_direction(p, NV_IN);
		}
		xmlFree(n);

		/* get parameter type */
		n = xmlGetProp(cur_node, "type");
		if (n == NULL) {
			fprintf(stderr, "Can't find parameter type: %s\n",
				nv_get_name(p));
		} else {
			hash_table_insert(attr_to_id, p, n);
			/* xmlFree(n) This will be free'd with the hash */
		}

		/* multiplicity */
		nv_xmlr_multiplicity(cur_node, multiplicity);
		nv_uml_parameter_set_multiplicity(p, multiplicity[NV_LOWER], NV_LOWER);
		nv_uml_parameter_set_multiplicity(p, multiplicity[NV_UPPER], NV_UPPER);

		nv_uml_operation_add_parameter(o, p);
	}
}

static void nv_xmlr_operation_qualifiers(xmlNode *a_node, struct UMLOperation *o)
{
	xmlChar *n = xmlGetProp(a_node, "isLeaf");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_operation_set_qualifier(o, NV_LEAF);
	}
	xmlFree(n);

	n = xmlGetProp(a_node, "isStatic");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_operation_set_qualifier(o, NV_STATIC);
	}
	xmlFree(n);

	n = xmlGetProp(a_node, "isAbstract");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_operation_set_qualifier(o, NV_ABSTRACT);
	}
	xmlFree(n);

	n = xmlGetProp(a_node, "isQuery");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_operation_set_qualifier(o, NV_QUERY);
	}
	xmlFree(n);
}

static struct UMLOperation *nv_xmlr_operation(xmlNode *a_node)
{
	xmlChar *n = xmlGetProp(a_node, "name");
	xmlChar *id = xmlGetProp(a_node, "id");
	struct UMLOperation *o = nv_uml_operation_new();
	UMLStereotype s = (intptr_t) hash_table_lookup(stereotypes, id);
	xmlFree(id);
	nv_set_name(o, n);
	xmlFree(n);
	nv_uml_element_add_stereotypes((struct UMLElement *)o, s);
	nv_xmlr_operation_parameters(a_node, o);
	nv_xmlr_operation_qualifiers(a_node, o);
	nv_xmlr_get_comments(&((struct UMLElement *)o)->comments, a_node);
	nv_xmlr_get_constraints(&((struct UMLElement *)o)->constraints, a_node);
	nv_set_visibility(o, nv_xmlr_get_visibility(a_node));
	return o;
}

static void nv_xmlr_operations(struct List *l, xmlNode *a_node)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "ownedOperation") {
		struct UMLOperation *o = nv_xmlr_operation(cur_node);
		nv_list_push_back(l, &o->link);
	}
}

static void nv_xmlr_ownedends(xmlNode *a_node, struct UMLAssociation *a)
{
	int i = 0;
	int multiplicity[2];
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "ownedEnd") {
		struct UMLOwnedEnd *o;
		xmlChar *name, *id, *type;
		if (i >= 2) {
			fprintf(stderr,
				"Associations with more than 2 owned ends aren't supported\n");
			break;
		}
		name = xmlGetProp(cur_node, "name");
		id = xmlGetProp(cur_node, "id");
		o = nv_uml_ownedend_new();
		nv_set_name(o, name);
		xmlFree(name);
		xmlFree(id);
		nv_set_visibility(o, nv_xmlr_get_visibility(cur_node));
		nv_uml_ownedend_set_association(o, a);
		type = xmlGetProp(cur_node, "type");
		if (type != NULL) {
			nv_uml_association_set_ownedend(a, o, i);
			hash_table_insert(ownedend_to_type_id, o, type);
			/* xmlFree(type) will be executed by the hash free function */
		} else {
			fprintf(stderr,
				"Owned end without a type %s\n", nv_get_name(o));
		}

		/* multiplicity */
		nv_xmlr_multiplicity(cur_node, multiplicity);
		nv_uml_ownedend_set_multiplicity(o, multiplicity[NV_LOWER], NV_LOWER);
		nv_uml_ownedend_set_multiplicity(o, multiplicity[NV_UPPER], NV_UPPER);

		i++;
	}
}

static struct UMLAssociation *nv_xmlr_association(xmlNode *a_node)
{
	struct UMLAssociation *a;
	xmlChar *name = xmlGetProp(a_node, "name");
	xmlChar *id = xmlGetProp(a_node, "id");

	a = nv_uml_association_new();
	nv_set_name(a, name);
	xmlFree(name);
	if (id != NULL) {
		hash_table_insert(id_to_assoc, id, a);
		/* xmlFree(id) This will be free'd with the hash */
	} else {
		fprintf(stderr, "Association %s has no id\n", nv_get_name(a));
	}

	nv_xmlr_get_comments(&((struct UMLElement *)a)->comments, a_node);
	nv_xmlr_get_constraints(&((struct UMLElement *)a)->constraints, a_node);
	nv_set_visibility(a, nv_xmlr_get_visibility(a_node));
	nv_xmlr_ownedends(a_node, a);
	return a;
}

static void nv_xmlr_associations(struct List *l, xmlNode *a_node, const char *nested)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, nested) {
		xmlChar *t = xmlGetProp(cur_node, "type");
		if (!strcmp((char *)t, "uml:Association")) {
			struct UMLAssociation *a = nv_xmlr_association(cur_node);
			nv_list_push_back(l, &a->link);
		}
		xmlFree(t);
	}
}

static void nv_xmlr_enumeration_literals(xmlNode *a_node, struct UMLEnumeration *e)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, "ownedLiteral") {
		xmlChar *n = xmlGetProp(cur_node, "name");
		xmlChar *id = xmlGetProp(cur_node, "id");
		struct UMLLiteral *l = nv_uml_literal_new();
		nv_set_name(l, n);
		nv_xmlr_get_comments(&((struct UMLElement *)l)->comments, cur_node);
		nv_xmlr_get_constraints(&((struct UMLElement *)l)->constraints, cur_node);
		nv_set_visibility(l, nv_xmlr_get_visibility(cur_node));
		nv_list_push_back(&e->literals, &l->link);
		xmlFree(n);
		xmlFree(id);
	}
}

static struct UMLEnumeration *nv_xmlr_enumeration(xmlNode *a_node)
{
	struct UMLEnumeration *e;
	xmlChar *name = xmlGetProp(a_node, "name");
	xmlChar *id = xmlGetProp(a_node, "id");

	e = nv_uml_enumeration_new();
	nv_set_name(e, name);

	/* an enumeration is also a type */
	hash_table_insert(types_hash, id, e);
	xmlFree(name);
	/* xmlFree(id) This will be free'd with the hash */
	nv_xmlr_get_comments(&((struct UMLElement *)e)->comments, a_node);
	nv_xmlr_get_constraints(&((struct UMLElement *)e)->constraints, a_node);
	nv_set_visibility(e, nv_xmlr_get_visibility(a_node));

	nv_xmlr_enumeration_literals(a_node, e);
	nv_xmlr_generalisations(a_node, (struct UMLType *) e);

	return e;
}

static void nv_xmlr_enumerations(struct List *l, xmlNode *a_node, const char *nested)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, nested) {
		xmlChar *t = xmlGetProp(cur_node, "type");
		if (!strcmp((char *)t, "uml:Enumeration")) {
			struct UMLEnumeration *e = nv_xmlr_enumeration(cur_node);
			nv_list_push_back(l, &e->link);
		}
		xmlFree(t);
	}
}

static struct UMLDataType *nv_xmlr_datatype(xmlNode *a_node)
{
	struct UMLDataType *d;
	xmlChar *name = xmlGetProp(a_node, "name");
	xmlChar *id = xmlGetProp(a_node, "id");

	d = nv_uml_datatype_new();
	nv_set_name(d, name);

	/* a datatype is also a type */
	hash_table_insert(types_hash, id, d);
	xmlFree(name);
	/* xmlFree(id) This will be free'd with the hash */
	nv_xmlr_get_comments(&((struct UMLElement *)d)->comments, a_node);
	nv_xmlr_get_constraints(&((struct UMLElement *)d)->constraints, a_node);
	nv_set_visibility(d, nv_xmlr_get_visibility(a_node));

	nv_xmlr_attributes(&d->attributes, a_node);
	nv_xmlr_operations(&d->operations, a_node);
	nv_xmlr_generalisations(a_node, (struct UMLType *) d);

	return d;
}

static void nv_xmlr_datatypes(struct List *l, xmlNode *a_node, const char *nested)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, nested) {
		xmlChar *t = xmlGetProp(cur_node, "type");
		if (!strcmp((char *)t, "uml:DataType")) {
			struct UMLDataType *d = nv_xmlr_datatype(cur_node);
			nv_list_push_back(l, &d->link);
		}
		xmlFree(t);
	}
}

static void nv_xmlr_class_qualifiers(xmlNode *a_node, struct UMLClass *c)
{
	xmlChar *n = xmlGetProp(a_node, "isAbstract");
	if (n != NULL && !strcmp(n, "true")) {
		nv_uml_class_set_qualifier(c, NV_ABSTRACT);
	}
	xmlFree(n);
}

static struct UMLClass *nv_xmlr_class(xmlNode *a_node)
{
	struct UMLClass *c;
	xmlChar *name = xmlGetProp(a_node, "name");
	xmlChar *id = xmlGetProp(a_node, "id");

	c = nv_uml_class_new();
	nv_set_name(c, name);

	/* a class is also a type */
	hash_table_insert(types_hash, id, c);
	xmlFree(name);
	/* xmlFree(id) This will be free'd with the hash */

	nv_xmlr_class_qualifiers(a_node, c);
	nv_xmlr_get_comments(&((struct UMLElement *)c)->comments, a_node);
	nv_xmlr_get_constraints(&((struct UMLElement *)c)->constraints, a_node);
	nv_set_visibility(c, nv_xmlr_get_visibility(a_node));

	nv_xmlr_attributes(&c->attributes, a_node);
	nv_xmlr_operations(&c->operations, a_node);
	nv_xmlr_enumerations(&c->enumerations, a_node, "nestedClassifier");
	nv_xmlr_primitivetypes(&c->primitivetypes, a_node, "nestedClassifier");
	nv_xmlr_datatypes(&c->datatypes, a_node, "nestedClassifier");
	nv_xmlr_classes(&c->nested_classes, a_node, "nestedClassifier");
	nv_xmlr_associations(&c->associations, a_node, "nestedClassifier");
	nv_xmlr_generalisations(a_node, (struct UMLType *) c);

	return c;
}

static void nv_xmlr_classes(struct List *l, xmlNode *a_node, const char *nested)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, nested) {
		xmlChar *t = xmlGetProp(cur_node, "type");
		if (!strcmp((char *)t, "uml:Class")) {
			struct UMLClass *c = nv_xmlr_class(cur_node);
			nv_list_push_back(l, &c->link);
		}
		xmlFree(t);
	}
}

static struct UMLPackage *nv_xmlr_package(xmlNode *a_node)
{
	struct UMLPackage *p;
	xmlChar *name = xmlGetProp(a_node, "name");
	xmlChar *id = xmlGetProp(a_node, "id");

	p = nv_uml_package_new();
	nv_set_name(p, name);

	/* a package is also a type */
	hash_table_insert(types_hash, id, p);
	xmlFree(name);
	/* xmlFree(id) This will be free'd with the hash */
	nv_xmlr_get_comments(&((struct UMLElement *)p)->comments, a_node);
	nv_xmlr_get_constraints(&((struct UMLElement *)p)->constraints, a_node);
	nv_set_visibility(p, nv_xmlr_get_visibility(a_node));

	nv_xmlr_enumerations(&p->enumerations, a_node, "packagedElement");
	nv_xmlr_primitivetypes(&p->primitivetypes, a_node, "packagedElement");
	nv_xmlr_datatypes(&p->datatypes, a_node, "packagedElement");
	nv_xmlr_associations(&p->associations, a_node, "packagedElement");
	nv_xmlr_packages(&p->packages, a_node, "packagedElement");

	nv_xmlr_generalisations(a_node, (struct UMLType *) p);

	return p;
}

static void nv_xmlr_packages(struct List *l, xmlNode *a_node, const char *nested)
{
	xmlNode *cur_node = a_node->children;
	nv_xml_on_node(cur_node, nested) {
		xmlChar *t = xmlGetProp(cur_node, "type");
		if (!strcmp((char *)t, "uml:Package")) {
			struct UMLPackage *p = nv_xmlr_package(cur_node);
			nv_list_push_back(l, &p->link);
		}
		xmlFree(t);
	}
}

static void nv_xmlr_resolve_types()
{
	/* attribute types */
	HashTableIterator iterator;
	hash_table_iterate(attr_to_id, &iterator);
	while (hash_table_iter_has_more(&iterator)) {
		struct UMLAttribute *a;
		char *attr_id = (char *) hash_table_iter_next(&iterator, (HashTableKey *) &a);
		struct UMLType *t = (struct UMLType *) hash_table_lookup(types_hash, attr_id);
		if (t != NULL) {
			nv_uml_attribute_set_type(a, t);
		} else {
			fprintf(stderr, "Unable to resolve attribute type: %s (%s)\n",
				nv_get_name(a), attr_id);
		}
	}

	/* attribute associations */
	hash_table_iterate(attr_to_assoc_id, &iterator);
	while (hash_table_iter_has_more(&iterator)) {
		struct UMLAttribute *a;
		char *assoc_id = (char *) hash_table_iter_next(&iterator, (HashTableKey *) &a);
		struct UMLAssociation *assoc = (struct UMLAssociation *) hash_table_lookup(id_to_assoc, assoc_id);
		if (assoc != NULL) {
			nv_uml_attribute_set_association(a, assoc);
		} else {
			fprintf(stderr, "Unable to resolve attributes association: %s (%s)\n",
				nv_get_name(a), assoc_id);
		}
	}

	/* ownedend types */
	hash_table_iterate(ownedend_to_type_id, &iterator);
	while (hash_table_iter_has_more(&iterator)) {
		struct UMLOwnedEnd *o;
		char *owned_id = (char *) hash_table_iter_next(&iterator, (HashTableKey *) &o);
		struct UMLType *t = (struct UMLType *) hash_table_lookup(types_hash, owned_id);
		if (t != NULL && nv_uml_type_get_base_type(t) == NV_CLASS) {
			nv_uml_ownedend_set_type(o, (struct UMLClass *) t);
		} else {
			fprintf(stderr, "Unable to resolve attribute type: %s (%s)\n",
				nv_get_name(o), owned_id);
		}
	}

	/* generalisations */
	hash_table_iterate(super_ids, &iterator);
	while (hash_table_iter_has_more(&iterator)) {
		struct UMLType *t;
		struct List *l = (struct List *) hash_table_iter_next(&iterator, (HashTableKey *) &t);
		struct ListLink *iter;
		for(iter = nv_list_front(l); iter; iter = nv_list_next(iter)) {
			struct xmlCharListNode *node = NV_UML_LIST_GET_DATA(iter, struct xmlCharListNode, link);
			xmlChar *id = node->id;
			struct UMLType *ty = (struct UMLType *) hash_table_lookup(types_hash, id);
			if (ty != NULL) {
				if (nv_uml_type_get_base_type(ty) == nv_uml_type_get_base_type(t)) {
					nv_list_push_back(&t->super_types, &ty->link);
				} else {
					fprintf(stderr, "%s can't inherit from a %s as they are not"
						" of the same type\n", nv_get_name(t), nv_get_name(ty));
				}
			} else {
				fprintf(stderr, "Malformed xmi file: missing super class %s\n", id);
			}
		}
	}
}

struct UMLModel *nv_xmlr_read_file(const char *filename)
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	struct UMLModel *model;

	LIBXML_TEST_VERSION
	doc = xmlReadFile(filename, NULL, 0);
	if (doc == NULL) {
		fprintf(stderr, "Failed to parse %s\n", filename);
		xmlCleanupParser();
		xmlMemoryDump();
		return NULL;
	}

	/* allocate space for the id hashes */
	types_hash = hash_table_new(string_hash, string_equal);
	hash_table_register_free_functions(types_hash,
					   xmlFree, NULL);

	attr_to_id = hash_table_new(pointer_hash, pointer_equal);
	hash_table_register_free_functions(attr_to_id,
					   NULL, xmlFree);

	super_ids = hash_table_new(pointer_hash, pointer_equal);
	hash_table_register_free_functions(super_ids,
					   NULL, xml_list_free);

	ownedend_to_type_id = hash_table_new(pointer_hash, pointer_equal);
	hash_table_register_free_functions(ownedend_to_type_id,
					   NULL, xmlFree);

	attr_to_assoc_id = hash_table_new(pointer_hash, pointer_equal);
	hash_table_register_free_functions(attr_to_assoc_id,
					   NULL, xmlFree);

	id_to_assoc = hash_table_new(string_hash, string_equal);
	hash_table_register_free_functions(id_to_assoc,
					   xmlFree, NULL);

	stereotypes = hash_table_new(string_hash, string_equal);
	hash_table_register_free_functions(stereotypes,
					   xmlFree, NULL);

	root_element = xmlDocGetRootElement(doc);

	nv_xmlr_stereotypes(root_element);

	model = nv_xmlr_model(root_element);
	if (model == NULL) {
		fprintf(stderr, "Unable to find a model\n");
		return NULL;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
	hash_table_free(types_hash);
	hash_table_free(attr_to_id);
	hash_table_free(super_ids);
	hash_table_free(ownedend_to_type_id);
	hash_table_free(attr_to_assoc_id);
	hash_table_free(id_to_assoc);
	hash_table_free(stereotypes);
	return model;
}
