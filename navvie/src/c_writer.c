#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "c_writer.h"
#include "uml_element.h"

#include "hash-table.h"
#include "hash-pointer.h"
#include "compare-pointer.h"
#include "util.h"

#define COMMENT_WIDTH 80

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
#include "uml_comment.h"

static void nv_c_write_type(FILE *fp, struct UMLType *t, enum UMLAggregation agg)
{
	if (t != NULL) {
		const char *tn = nv_get_name(t);
		switch(nv_uml_type_get_base_type(t)) {
		case NV_PRIMITIVETYPE:
			fprintf(fp, "%s ", tn);
			break;
		case NV_CLASS:
			fprintf(fp, "struct %s ", tn);
			break;
		case NV_ENUMERATION:
			fprintf(fp, "enum %s ", tn);
			break;
		default:
			fprintf(stderr, "Unrecognised base type %d\n",
				nv_uml_type_get_base_type(t));
			break;
		}
	} else {
		fprintf(fp, "void ");
	}

	/* Make non-composite types pointers. */
	if( agg != NV_COMPOSITE ) {
		fprintf(fp, "*");
	}
}

static void nv_c_write_attribute(FILE *fp, struct UMLModel *m, struct UMLClass *c, struct UMLAttribute *a)
{
	struct UMLType *ty = nv_uml_attribute_get_type(a);

	if (nv_uml_attribute_get_qualifier(a, NV_READONLY)) {
		fprintf(fp, "const ");
	}

	nv_c_write_type(fp, ty, nv_uml_attribute_get_aggregation(a));

	if (nv_uml_attribute_get_qualifier(a, NV_STATIC)) {
		fprintf(fp, "%s_%s_", nv_get_name(m), nv_get_name(c));
	}

	fprintf(fp, "%s;\n", nv_get_name(a));
}

static void nv_c_write_function_argument(FILE *fp, struct UMLParameter *p)
{
	/* Classes are passed call by reference, everything else is call by value. */
	struct UMLType *ty = nv_uml_parameter_get_type(p);
	enum UMLAggregation agg = NV_COMPOSITE;
	if( nv_uml_type_get_base_type( ty ) == NV_CLASS ) {
		agg = NV_AGGREGATION;
	}

	nv_c_write_type(fp, nv_uml_parameter_get_type(p), agg);
	fprintf(fp, "%s", nv_get_name(p));
}

static void nv_c_write_function_name(FILE *fp, struct UMLModel *m, struct UMLClass *c, struct UMLOperation *o)
{
	if(nv_uml_element_get_stereotypes((struct UMLElement *)o) & NV_STEREOTYPE_CREATE) {
		fprintf(fp, "%s_%s_create", nv_get_name(m), nv_get_name(c));
	} else if(nv_uml_element_get_stereotypes((struct UMLElement *)o) & NV_STEREOTYPE_DESTROY) {
		fprintf(fp, "%s_%s_destroy", nv_get_name(m), nv_get_name(c));
	} else {
		fprintf(fp, "%s_%s_%s", nv_get_name(m), nv_get_name(c), nv_get_name(o));
	}
}

static void nv_c_write_function_head(FILE *fp, struct UMLModel *m, struct UMLClass *c, struct UMLOperation *o)
{
	struct ListLink *iter;
	struct UMLParameter *p;

	p = nv_uml_operation_get_return_parameter(o);
	if (p != NULL) {
		/* Classes are returned by reference, everything else is returned by value. */
		struct UMLType *ty = nv_uml_parameter_get_type(p);
		enum UMLAggregation agg = NV_COMPOSITE;
		if( nv_uml_type_get_base_type( ty ) == NV_CLASS ) {
			agg = NV_AGGREGATION;
		}

		nv_c_write_type(fp, nv_uml_parameter_get_type(p), agg);
	} else {
		nv_c_write_type(fp, NULL, NV_COMPOSITE);
	}

	nv_c_write_function_name(fp, m, c, o);
	fprintf(fp, "(");

	/* All C functions take the object as the first argument */
	nv_c_write_type(fp, (struct UMLType *)c, NV_AGGREGATION);
	fprintf(fp, "object");

	for(iter = nv_list_front(&o->parameters); iter; iter = nv_list_next(iter)) {
		struct UMLParameter *p = NV_UML_LIST_GET_DATA( iter, struct UMLParameter, link );
		if (nv_uml_parameter_get_direction(p) != NV_RETURN) {
			fprintf(fp, ", ");
			nv_c_write_function_argument(fp, p);
		}
	}

	fprintf(fp, ")");
}

static void nv_c_write_function_header(FILE *fp, struct UMLModel *m, struct UMLClass *c, struct UMLOperation *o)
{
	nv_c_write_function_head(fp, m, c, o);
	fprintf(fp, ";\n");
}

static void nv_c_write_function(FILE *fp, struct UMLModel *m, struct UMLClass *c, struct UMLOperation *o)
{
	nv_c_write_function_head(fp, m, c, o);
	fprintf(fp, "\n");
	fprintf(fp, "{\n");
	/* method */
	fprintf(fp, "}\n\n");
}

static void nv_c_write_literal(FILE *fp, struct UMLLiteral *l)
{
	fprintf(fp, "\t%s", nv_get_name(l));
}

static void nv_c_write_enumeration(FILE *fp, struct UMLEnumeration *e)
{
	struct ListLink *iter;
	int comma = 0;
	fprintf(fp, "enum %s {", nv_get_name(e));
	for(iter = nv_list_front(&e->literals); iter; iter = nv_list_next(iter)) {
		struct UMLLiteral *l = NV_UML_LIST_GET_DATA( iter, struct UMLLiteral, link );
		if (comma == 1) {
			fprintf(fp, ",");
		}
		fprintf(fp, "\n");
		nv_c_write_literal(fp, l);
		comma = 1;
	}
	fprintf(fp, "\n};\n\n");
}

static void nv_c_write_guard_end(FILE *fp, const char *mn, const char *n)
{
	char *g = (char *) malloc(sizeof(char) *(strlen(mn) + strlen(n) + 6));
	sprintf(g, "_%s_%s_H_", mn, n);
	nv_str_toupper(g);
	fprintf(fp, "\n#endif /*!%s*/\n", g);
	free(g);
}

static void nv_c_write_guard_start(FILE *fp, const char *mn, const char *n)
{
	char *g = (char *) malloc(sizeof(char) * (strlen(mn) + strlen(n) + 6));
	sprintf(g, "_%s_%s_H_", mn, n);
	nv_str_toupper(g);
	fprintf(fp, "#ifndef %s\n", g);
	fprintf(fp, "#define %s\n\n", g);
	free(g);
}

static void nv_c_write_class_declaration(FILE *fp, struct UMLModel *m, struct UMLClass *c)
{
	struct ListLink *iter;
	fprintf(fp, "struct %s\n{\n", nv_get_name(c));
	for(iter = nv_list_front(&((struct UMLType*)c)->super_types); iter; iter = nv_list_next(iter)) {
		struct UMLType *ty = NV_UML_LIST_GET_DATA( iter, struct UMLType, link );
		fprintf(fp, "\t");
		nv_c_write_type(fp, ty, NV_COMPOSITE);
		fprintf(fp, "super; /**< 'super' must be the first member in the structure */\n");
	}

	for(iter = nv_list_front(&c->attributes); iter; iter = nv_list_next(iter)) {
		struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
		if(!nv_uml_attribute_get_qualifier(a, NV_STATIC)) {
			fprintf(fp, "\t");
			nv_c_write_attribute(fp, m, c, a);
		}
	}
	fprintf(fp, "};\n\n");
}

static void nv_c_write_function_prototypes(FILE *fp, struct UMLModel *m, struct UMLClass *c)
{
	struct ListLink *iter;
	for(iter = nv_list_front(&c->operations); iter; iter = nv_list_next(iter)) {
		struct UMLOperation *o = NV_UML_LIST_GET_DATA( iter, struct UMLOperation, link );
		if (nv_get_visibility(o) != NV_PRIVATE) {
			nv_c_write_function_header(fp, m, c, o);
		}
	}
}

#define FORWARD_DECLARATION (1)
#define INCLUDE (2)

static void nv_c_write_header_includes(FILE *fp, struct UMLClass *c)
{
	struct ListLink *iter;
	HashTable *includes;

	fprintf(fp, "#include \"primitive_types.h\"\n");
	includes = hash_table_new(pointer_hash, pointer_equal);

	/* Class generalization includes */
	for(iter = nv_list_front(&((struct UMLType*)c)->super_types); iter; iter = nv_list_next(iter)) {
		struct UMLType *t = NV_UML_LIST_GET_DATA( iter, struct UMLType, link );
		fprintf(fp, "#include \"%s.h\"\n", nv_get_name(t));
		hash_table_insert(includes, t, (void *)INCLUDE);
	}

	/* Class attribute includes */
	for(iter = nv_list_front(&c->attributes); iter; iter = nv_list_next(iter)) {
		struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
		struct UMLType *t = nv_uml_attribute_get_type(a);
		if( NV_CLASS == nv_uml_type_get_base_type(t) ||
		    NV_ENUMERATION == nv_uml_type_get_base_type(t) ) {
			if( NV_COMPOSITE == nv_uml_attribute_get_aggregation(a) &&
			    !hash_table_lookup(includes, t) ) {
				fprintf(fp, "#include \"%s.h\"\n", nv_get_name(t));
				hash_table_insert(includes, t, (void *)INCLUDE);
			}
		}
	}

	/* Class operation parameter includes */
	for(iter = nv_list_front(&c->operations); iter; iter = nv_list_next(iter)) {
		struct UMLOperation *o = NV_UML_LIST_GET_DATA( iter, struct UMLOperation, link );
		struct ListLink *iter2;
		for( iter2 = nv_list_front(&o->parameters); iter2; iter2 = nv_list_next(iter2) ) {
			struct UMLParameter *p = NV_UML_LIST_GET_DATA( iter2, struct UMLParameter, link );
			struct UMLType *t = nv_uml_parameter_get_type(p);
			/* TODO: Assume enumerations are pass by value for now */
			if( NV_ENUMERATION == nv_uml_type_get_base_type(t) &&
			    !hash_table_lookup(includes, t) ) {
				fprintf(fp, "#include \"%s.h\"\n", nv_get_name(t));
				hash_table_insert(includes, t, (void *)INCLUDE);
			}
		}
	}
	fprintf(fp, "\n");

	/* Class attribute forward declarations */
	for(iter = nv_list_front(&c->attributes); iter; iter = nv_list_next(iter)) {
		struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
		struct UMLType *t = nv_uml_attribute_get_type(a);
		if( NV_CLASS == nv_uml_type_get_base_type(t) ||
		    NV_ENUMERATION == nv_uml_type_get_base_type(t) ) {
			if( NV_COMPOSITE != nv_uml_attribute_get_aggregation(a) &&
			    !hash_table_lookup(includes, t) ) {
				fprintf(fp, "struct %s;\n", nv_get_name(t));
				hash_table_insert(includes, t, (void *)FORWARD_DECLARATION);
			}
		}
	}

	/* Class operation parameter forward declarations */
	for(iter = nv_list_front(&c->operations); iter; iter = nv_list_next(iter)) {
		struct UMLOperation *o = NV_UML_LIST_GET_DATA( iter, struct UMLOperation, link );
		struct ListLink *iter2;
		for( iter2 = nv_list_front(&o->parameters); iter2; iter2 = nv_list_next(iter2) ) {
			struct UMLParameter *p = NV_UML_LIST_GET_DATA( iter2, struct UMLParameter, link );
			struct UMLType *t = nv_uml_parameter_get_type(p);
			/* TODO: Assume classes are call by reference for now */
			if( NV_CLASS == nv_uml_type_get_base_type(t) &&
			    !hash_table_lookup(includes, t) ) {
				fprintf(fp, "struct %s;\n", nv_get_name(t));
				hash_table_insert(includes, t, (void *)FORWARD_DECLARATION);
			}
		}
	}
	fprintf(fp, "\n");
	hash_table_free(includes);
}

static void nv_c_write_class_header(const char *dir, struct UMLModel *m, struct UMLClass *c)
{
	FILE *fp;
	char *header = (char *) malloc(sizeof(char) * (strlen(dir) + strlen(nv_get_name(c)) + 4));

	sprintf(header, "%s/%s.h", dir, nv_get_name(c));
	fp = fopen(header, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", header);
		exit(1);
	}

	nv_c_write_guard_start(fp, nv_get_name(m), nv_get_name(c));
	nv_c_write_header_includes(fp, c);

	/* Write externs for global variables */
	{
		struct ListLink *iter;
		for(iter = nv_list_front(&c->attributes); iter; iter = nv_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			if(nv_uml_attribute_get_qualifier(a, NV_STATIC)) {
				fprintf(fp, "extern ");
				nv_c_write_attribute(fp, m, c, a);
			}
		}
	}
	fprintf(fp, "\n");

	nv_c_write_class_declaration(fp, m, c);
	nv_c_write_function_prototypes(fp, m, c);
	nv_c_write_guard_end(fp, nv_get_name(m), nv_get_name(c));
	fclose(fp);
	free(header);
}

static void nv_c_write_source_includes(FILE *fp, struct UMLClass *c)
{
	fprintf(fp, "#include \"%s.h\"\n\n", nv_get_name(c));
}

static void nv_c_write_functions(FILE *fp, struct UMLModel *m, struct UMLClass *c)
{
	struct ListLink *iter;
	for(iter = nv_list_front(&c->operations); iter; iter = nv_list_next(iter)) {
		struct UMLOperation *o = NV_UML_LIST_GET_DATA( iter, struct UMLOperation, link );
		nv_c_write_function(fp, m, c, o);
	}
}

static void nv_c_write_class_source(const char *dir, struct UMLModel *m, struct UMLClass *c)
{
	FILE *fp;
	char *source = (char *) malloc(sizeof(char) *
					    (strlen(dir)
					     + strlen(nv_get_name(c))
					     + 4));

	sprintf(source, "%s/%s.c", dir, nv_get_name(c));
	fp = fopen(source, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", source);
		exit(1);
	}

	nv_c_write_source_includes(fp, c);

	/* Write global variables */
	{
		struct ListLink *iter;
		for(iter = nv_list_front(&c->attributes); iter; iter = nv_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			if(nv_uml_attribute_get_qualifier(a, NV_STATIC)) {
				nv_c_write_attribute(fp, m, c, a);
			}
		}
	}
	fprintf(fp, "\n");

	nv_c_write_functions(fp, m, c);
	fclose(fp);
	free(source);
}

static void nv_c_write_class(const char *dir, struct UMLModel *m, struct UMLClass *c)
{
	nv_c_write_class_header(dir, m, c);
	nv_c_write_class_source(dir, m, c);
}

static void nv_c_write_model_enumeration(const char *dir, struct UMLModel *m, struct UMLEnumeration *e)
{
	FILE *fp;
	char *header = (char *) malloc(sizeof(char) *
					    (strlen(dir)
					     + strlen(nv_get_name(e))
					     + 4));

	sprintf(header, "%s/%s.h", dir, nv_get_name(e));
	fp = fopen(header, "w");
	if(fp == NULL) {
		fprintf(stderr, "Unable to open file %s for writing\n", header);
		exit(1);
	}

	nv_c_write_guard_start(fp, nv_get_name(m), nv_get_name(e));
	nv_c_write_enumeration(fp, e);
	nv_c_write_guard_end(fp, nv_get_name(m), nv_get_name(e));
	fclose(fp);
	free(header);
}

static void nv_c_write_primitive_types(const char *dir, struct UMLModel *m)
{
	FILE *fp;
	struct ListLink *iter;
	const char *types_name = "primitive_types";
	char *source = (char *) malloc(sizeof(char) * (strlen(dir) + strlen(types_name) + 4));

	sprintf(source, "%s/%s.h", dir, types_name);
	fp = fopen(source, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", source);
		exit(1);
	}
	free(source);

	nv_c_write_guard_start(fp, nv_get_name(m), types_name);
	for(iter = nv_list_front(&m->super.primitivetypes); iter; iter = nv_list_next(iter)) {
		struct UMLPrimitiveType *p = NV_UML_LIST_GET_DATA( iter, struct UMLPrimitiveType, link );
		fprintf(fp, "typedef unsigned int %s; ", nv_get_name(p));
		fprintf(fp, "/* TODO: select correct C type. */\n");
	}
	nv_c_write_guard_end(fp, nv_get_name(m), types_name);

	fclose(fp);
}

static void nv_c_write_makefile(const char *dir, struct UMLModel *m)
{
	FILE *fp;
	struct ListLink *iter;
	const char *makefile_name = "Makefile";
	char *source = (char *) malloc(sizeof(char) * (strlen(dir) + strlen(makefile_name) + 2));
	sprintf(source, "%s/%s", dir, makefile_name);
	fp = fopen(source, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", source);
		exit(1);
	}
	free(source);

	fprintf(fp, "LIBNAME := %s\n", nv_get_name(m));
	fprintf(fp, "CXX := gcc\n");
	fprintf(fp, "WARNING_FLAGS := -Wall -Wc++-compat -Wextra --pedantic -ansi\n");
	fprintf(fp, "CXX_FLAGS := $(WARNING_FLAGS) -g\n");
	fprintf(fp, "LIBS :=\n");
	fprintf(fp, "OBJDIR := .\n");
	fprintf(fp, "SRCDIR := .\n");
	fprintf(fp, "INCDIR := .\n");
	fprintf(fp, "INCLUDE := -I $(INCDIR)\n");
	fprintf(fp, "OBJECTS :=");
	for(iter = nv_list_front(&m->super.classes); iter; iter = nv_list_next(iter)) {
		struct UMLClass *c = NV_UML_LIST_GET_DATA( iter, struct UMLClass, link );
		fprintf(fp, "\\\n$(OBJDIR)/%s.o", nv_get_name(c));
	}
	fprintf(fp, "\n");
	fprintf(fp, "DEPS = $(OBJECTS:.o=.d)\n");
	fprintf(fp, "\n");

	fprintf(fp, "$(LIBNAME) : $(OBJECTS)\n");
	fprintf(fp, "\tar rcs lib$(LIBNAME).a $(OBJECTS)\n\n");
	fprintf(fp, "-include $(DEPS)\n\n");
	fprintf(fp, "$(OBJDIR)/%%.o : $(SRCDIR)/%%.c\n");
	fprintf(fp, "\t$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@\n");
	fprintf(fp, "\t$(CXX) $(CXXFLAGS) $(INCLUDE) -MM -MF $(patsubst %%.o,%%.d,$@) $<\n");

	fprintf(fp, "\n");
	fprintf(fp, "clean:\n");
	fprintf(fp, "\trm -f *.o *.a *.d *~\n");
	fprintf(fp, "\trm -f lib$(LIBNAME).a\n");

	fclose(fp);
}

void nv_c_write_model(struct UMLModel *m, const char *dir)
{
	struct ListLink *iter;
	for(iter = nv_list_front(&m->super.classes); iter; iter = nv_list_next(iter)) {
		struct UMLClass *c = NV_UML_LIST_GET_DATA( iter, struct UMLClass, link );
		nv_c_write_class(dir, m, c);
	}

	/*for(l = nv_uml_model_get_datatypes(m);l ;l = l->next) {
		nv_c_write_datatype(dir, m, (struct UMLDataType *) l->data);
		}*/

	for(iter = nv_list_front(&m->super.enumerations); iter; iter = nv_list_next(iter)) {
		struct UMLEnumeration *e = NV_UML_LIST_GET_DATA( iter, struct UMLEnumeration, link );
		nv_c_write_model_enumeration(dir, m, e);
	}

	nv_c_write_primitive_types(dir, m);
	nv_c_write_makefile(dir, m);
}
