#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cpp_writer.h"
#include "uml_element.h"

#include "hash-table.h"
#include "hash-pointer.h"
#include "compare-pointer.h"
#include "util.h"
#include "list.h"

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

static void nv_cpp_write_type(FILE *fp, struct UMLType *t, enum UMLAggregation agg, int upper)
{
	if (t != NULL) {
		const char *tn = nv_get_name(t);
		switch(nv_uml_type_get_base_type(t)) {
		case NV_PRIMITIVETYPE:
			fprintf(fp, "%s ", tn);
			break;
		case NV_CLASS:
			fprintf(fp, "%s ", tn);
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

	/* Aggregations of type None that are enumerations or primitive
	   types are not stored as pointers. */
	switch(agg) {
		case NV_NONE:
			if( t == NULL || nv_uml_type_get_base_type(t) == NV_CLASS ) {
				fprintf(fp, "*");
			}
			break;
		case NV_AGGREGATION:
			fprintf(fp, "*");
		case NV_COMPOSITE:
			break;
	}

	/* Multiplicities with an infinite upper bound will have to be passed as a pointer
	   to whatever the current type is */
	if(upper == NV_MANY) {
		fprintf(fp, "*");
	}
}

static void nv_cpp_write_attribute(FILE *fp, struct UMLAttribute *a)
{
	struct UMLType *ty = nv_uml_attribute_get_type(a);
	const char *n = nv_get_name(a);
	fprintf(fp, "\t");
	if (nv_uml_attribute_get_qualifier(a, NV_STATIC)) {
		fprintf(fp, "static ");
	}

	if (nv_uml_attribute_get_qualifier(a, NV_READONLY)) {
		fprintf(fp, "const ");
	}

	nv_cpp_write_type(fp, ty, nv_uml_attribute_get_aggregation(a),
	                  nv_uml_attribute_get_multiplicity(a, NV_UPPER));
	fprintf(fp, "%s;\n", n);
}

static void nv_cpp_write_function_argument(FILE *fp, struct UMLParameter *p)
{
	/* Classes are passed call by reference, everything else is call by value. */
	struct UMLType *ty = nv_uml_parameter_get_type(p);
	enum UMLAggregation agg = NV_COMPOSITE;
	if( nv_uml_type_get_base_type( ty ) == NV_CLASS ) {
		agg = NV_AGGREGATION;
	}

	nv_cpp_write_type(fp, nv_uml_parameter_get_type(p), agg,
	                  nv_uml_parameter_get_multiplicity(p, NV_UPPER));

	fprintf(fp, "%s", nv_get_name(p));
}

static void nv_cpp_write_function_name(FILE *fp, struct UMLModel *m, struct UMLClass *c,
                                       struct UMLOperation *o, unsigned int qualified)
{
	(void)m;
	if(qualified) {
		fprintf(fp, "%s::", nv_get_name(c));
	}
	if(nv_uml_element_get_stereotypes((struct UMLElement *)o) & NV_STEREOTYPE_CREATE) {
		fprintf(fp, "%s", nv_get_name(c));
	} else if(nv_uml_element_get_stereotypes((struct UMLElement *)o) & NV_STEREOTYPE_DESTROY) {
		fprintf(fp, "~%s", nv_get_name(c));
	} else {
		fprintf(fp, "%s", nv_get_name(o));
	}
}

static void nv_cpp_write_function_head(FILE *fp, struct UMLModel *m, struct UMLClass *c,
                                       struct UMLOperation *o, unsigned int qualified)
{
	List *l;
	char comma[3] = {'\0','\0','\0'};
	struct UMLParameter *p;

	p = nv_uml_operation_get_return_parameter(o);
	if (p != NULL) {
		/* Classes are returned by reference, everything else is returned by value. */
		struct UMLType *ty = nv_uml_parameter_get_type(p);
		enum UMLAggregation agg = NV_COMPOSITE;
		if( nv_uml_type_get_base_type( ty ) == NV_CLASS ) {
			agg = NV_AGGREGATION;
		}

		nv_cpp_write_type(fp, nv_uml_parameter_get_type(p), agg,
		                  nv_uml_parameter_get_multiplicity(p, NV_UPPER));
	} else if(!(nv_uml_element_get_stereotypes((struct UMLElement *)o) &
	            (NV_STEREOTYPE_CREATE | NV_STEREOTYPE_DESTROY))) {
		nv_cpp_write_type(fp, NULL, NV_COMPOSITE, 1);
	}

	nv_cpp_write_function_name(fp, m, c, o, qualified);
	fprintf(fp, "(");
	for(l = nv_uml_operation_get_parameters(o);l ;l = l->next) {
		struct UMLParameter *p = (struct UMLParameter *) l->data;
		if (nv_uml_parameter_get_direction(p) != NV_RETURN) {
			fprintf(fp, "%s", comma);
			nv_cpp_write_function_argument(fp, p);
			comma[0] = ',';
			comma[1] = ' ';
		}
	}

	/* If nothing was printed insert a void */
	if( comma[0] == '\0' ) {
		fprintf(fp, "void");
	}

	fprintf(fp, ")");
}

static void nv_cpp_write_function_header(FILE *fp, struct UMLModel *m, struct UMLClass *c, struct UMLOperation *o)
{
	if(nv_uml_operation_get_qualifier(o, NV_STATIC)) {
		fprintf(fp, "static ");
	}
	nv_cpp_write_function_head(fp, m, c, o, 0);
	fprintf(fp, ";\n");
}

static void nv_cpp_write_function(FILE *fp, struct UMLModel *m, struct UMLClass *c, struct UMLOperation *o)
{
	nv_cpp_write_function_head(fp, m, c, o, 1);
#if 0
	/* Set the object kind in the IIR constructor */
	if(nv_uml_element_get_stereotypes((struct UMLElement *)o) & NV_STEREOTYPE_CREATE) {
		const char *name = nv_get_name(c);
		fprintf(fp, " : IIR(");
		name+=4;
		fprintf(fp, "IR");
		while(*name) {
			if(isupper(*name)) {
				fprintf(fp, "_");
			}
			fprintf(fp, "%c", toupper(*name));
			name++;
		}
		fprintf(fp, ")");
	}
#endif
	fprintf(fp, "\n");
	fprintf(fp, "{\n");
#if 0
	/* Set the object kind */
	if(nv_uml_element_get_stereotypes((struct UMLElement *)o) & NV_STEREOTYPE_CREATE) {
		const char *name = nv_get_name(c);
		fprintf(fp, "\tthis->_kind = ");
		name+=4;
		fprintf(fp, "IR");
		while(*name) {
			if(isupper(*name)) {
				fprintf(fp, "_");
			}
			fprintf(fp, "%c", toupper(*name));
			name++;
		}
		fprintf(fp, ";\n");
	}
#endif
#if 0
	/* generate code if this is a getter or setter */
	{
		const char *fn = nv_get_name(o);
		if(!strncmp(fn, "set_", 4)) {
			const char *an = fn + 4;
			fprintf(fp, "\tthis->_%s = %s;\n", an, an);
		} else if(!strncmp(fn, "get_", 4)) {
			const char *an = fn + 4;
			fprintf(fp, "\treturn this->_%s;\n", an);
		}
	}
#endif
	fprintf(fp, "}\n\n");
}

static void nv_cpp_write_literal(FILE *fp, struct UMLLiteral *l)
{
	fprintf(fp, "\t%s", nv_get_name(l));
}

static void nv_cpp_write_enumeration(FILE *fp, struct UMLEnumeration *e)
{
	List *work;
	int comma = 0;
	work = nv_uml_enumeration_get_literals(e);
	fprintf(fp, "enum %s\n{", nv_get_name(e));
	while(work != NULL) {
		struct UMLLiteral *l = (struct UMLLiteral *) work->data;
		if (comma == 1) {
			fprintf(fp, ",");
		}
		fprintf(fp, "\n");
		nv_cpp_write_literal(fp, l);
		comma = 1;
		work = work->next;
	}
	fprintf(fp, "\n};\n\n");
}

static void nv_cpp_write_guard_end(FILE *fp, const char *mn, const char *n)
{
	char *g = (char *) malloc(sizeof(char) *(strlen(mn) + strlen(n) + 6));
	sprintf(g, "_%s_%s_H_", mn, n);
	nv_str_toupper(g);
	fprintf(fp, "\n#endif /*!%s*/\n", g);
	free(g);
}

static void nv_cpp_write_guard_start(FILE *fp, const char *mn, const char *n)
{
	char *g = (char *) malloc(sizeof(char) * (strlen(mn) + strlen(n) + 6));
	sprintf(g, "_%s_%s_H_", mn, n);
	nv_str_toupper(g);
	fprintf(fp, "#ifndef %s\n", g);
	fprintf(fp, "#define %s\n\n", g);
	free(g);
}

static void nv_cpp_write_function_prototypes(FILE *fp, struct UMLModel *m, struct UMLClass *c)
{
	List *l;
	const char *prefix = "public:\n";
	for(l = nv_uml_class_get_operations(c); l; l = l->next) {
		struct UMLOperation *o = (struct UMLOperation *) l->data;
		if(nv_uml_element_get_visibility((struct UMLElement *)o) == NV_PUBLIC) {
			fprintf(fp, "%s\t", prefix);
			prefix = "";
			nv_cpp_write_function_header(fp, m, c, o);
		}
	}

	prefix = "public:\n";
	for(l = nv_uml_class_get_operations(c); l; l = l->next) {
		struct UMLOperation *o = (struct UMLOperation *) l->data;
		if(nv_uml_element_get_visibility((struct UMLElement *)o) == NV_PROTECTED) {
			fprintf(fp, "%s\t", prefix);
			prefix = "";
			nv_cpp_write_function_header(fp, m, c, o);
		}
	}

	prefix = "private:\n";
	for(l = nv_uml_class_get_operations(c); l; l = l->next) {
		struct UMLOperation *o = (struct UMLOperation *) l->data;
		if(nv_uml_element_get_visibility((struct UMLElement *)o) == NV_PRIVATE) {
			fprintf(fp, "%s\t", prefix);
			prefix = "";
			nv_cpp_write_function_header(fp, m, c, o);
		}
	}
}

static void nv_cpp_write_attributes(FILE *fp, struct UMLClass *c)
{
	const char *prefix = "public:\n";
	{
		struct UMLListLink *iter;
		for(iter = nv_uml_list_front(&c->attributes); iter; iter = nv_uml_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			if(nv_uml_element_get_visibility((struct UMLElement *)a) == NV_PUBLIC) {
				fprintf(fp, "%s", prefix);
				prefix = "";
				nv_cpp_write_attribute(fp, a);
			}
		}
	}

	prefix = "protected:\n";
	{
		struct UMLListLink *iter;
		for(iter = nv_uml_list_front(&c->attributes); iter; iter = nv_uml_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			if(nv_uml_element_get_visibility((struct UMLElement *)a) == NV_PROTECTED) {
				fprintf(fp, "%s", prefix);
				prefix = "";
				nv_cpp_write_attribute(fp, a);
			}
		}
	}

	prefix = "private:\n";
	{
		struct UMLListLink *iter;
		for(iter = nv_uml_list_front(&c->attributes); iter; iter = nv_uml_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			if(nv_uml_element_get_visibility((struct UMLElement *)a) == NV_PRIVATE) {
				fprintf(fp, "%s", prefix);
				prefix = "";
				nv_cpp_write_attribute(fp, a);
			}
		}
	}
}

static void nv_cpp_write_class_declaration(FILE *fp, struct UMLModel *m, struct UMLClass *c)
{
	List *l;
	const char *comma = " : ";
	fprintf(fp, "class %s", nv_get_name(c));
	for(l = nv_uml_type_get_supers((struct UMLType *) c); l; l = l->next) {
		struct UMLType *ty = (struct UMLType *) l->data;
		fprintf(fp, "%spublic %s", comma, nv_get_name(ty));
		comma = ", ";
	}
	fprintf(fp, "\n{\n");

	nv_cpp_write_attributes(fp, c);
	nv_cpp_write_function_prototypes(fp, m, c);

	fprintf(fp, "};\n\n");
}

#define FORWARD_DECLARATION (1)
#define INCLUDE (2)

static void nv_cpp_write_header_includes(FILE *fp, struct UMLClass *c)
{
	List *l;
	HashTable *includes;

	fprintf(fp, "#include \"primitive_types.h\"\n");
	includes = hash_table_new(pointer_hash, pointer_equal);

	/* Class generalization includes */
	for(l = nv_uml_type_get_supers((struct UMLType *) c); l; l = l->next) {
		struct UMLType *t = (struct UMLType *) l->data;
		fprintf(fp, "#include \"%s.h\"\n", nv_get_name(t));
		hash_table_insert(includes, t, (void *)INCLUDE);
	}

	/* Class attribute includes */
	{
		struct UMLListLink *iter;
		for(iter = nv_uml_list_front(&c->attributes); iter; iter = nv_uml_list_next(iter)) {
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
	}

	/* Class operation parameter includes */
	for( l = nv_uml_class_get_operations(c); l; l = l->next ) {
		List *l2;
		struct UMLOperation *op = (struct UMLOperation *) l->data;
		for( l2 = nv_uml_operation_get_parameters(op); l2; l2 = l2->next ) {
			struct UMLParameter *p = (struct UMLParameter *) l2->data;
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
	{
		struct UMLListLink *iter;
		for(iter = nv_uml_list_front(&c->attributes); iter; iter = nv_uml_list_next(iter)) {
			struct UMLAttribute *a = NV_UML_LIST_GET_DATA( iter, struct UMLAttribute, link );
			struct UMLType *t = nv_uml_attribute_get_type(a);
			if( NV_CLASS == nv_uml_type_get_base_type(t) ||
			    NV_ENUMERATION == nv_uml_type_get_base_type(t) ) {
				if( NV_COMPOSITE != nv_uml_attribute_get_aggregation(a) &&
				    !hash_table_lookup(includes, t) ) {
					fprintf(fp, "class %s;\n", nv_get_name(t));
					hash_table_insert(includes, t, (void *)FORWARD_DECLARATION);
				}
			}
		}
	}

	/* Class operation parameter forward declarations */
	for( l = nv_uml_class_get_operations(c); l; l = l->next ) {
		List *l2;
		struct UMLOperation *op = (struct UMLOperation *) l->data;
		for( l2 = nv_uml_operation_get_parameters(op); l2; l2 = l2->next ) {
			struct UMLParameter *p = (struct UMLParameter *) l2->data;
			struct UMLType *t = nv_uml_parameter_get_type(p);
			/* TODO: Assume classes are call by reference for now */
			if( NV_CLASS == nv_uml_type_get_base_type(t) &&
			    !hash_table_lookup(includes, t) ) {
				fprintf(fp, "class %s;\n", nv_get_name(t));
				hash_table_insert(includes, t, (void *)FORWARD_DECLARATION);
			}
		}
	}
	fprintf(fp, "\n");
	hash_table_free(includes);
}

static void nv_cpp_write_class_header(const char *dir, struct UMLModel *m, struct UMLClass *c)
{
	FILE *fp;
	char *header = (char *) malloc(sizeof(char) * (strlen(dir) + strlen(nv_get_name(c)) + 4));

	sprintf(header, "%s/%s.h", dir, nv_get_name(c));
	fp = fopen(header, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", header);
		exit(1);
	}

	nv_cpp_write_guard_start(fp, nv_get_name(m), nv_get_name(c));
	nv_cpp_write_header_includes(fp, c);
	/*nv_cpp_write_forward_declarations();
	  nv_cpp_write_enumerations();*/
	nv_cpp_write_class_declaration(fp, m, c);
	nv_cpp_write_guard_end(fp, nv_get_name(m), nv_get_name(c));
	fclose(fp);
	free(header);
}

static void nv_cpp_write_source_includes(FILE *fp, struct UMLClass *c)
{
	fprintf(fp, "#include \"%s.h\"\n\n", nv_get_name(c));
}

static void nv_cpp_write_functions(FILE *fp, struct UMLModel *m, struct UMLClass *c)
{
	List *l = nv_uml_class_get_operations(c);
	for(; l; l = l->next) {
		nv_cpp_write_function(fp, m, c, (struct UMLOperation *) l->data);
	}
}

static void nv_cpp_write_class_source(const char *dir, struct UMLModel *m, struct UMLClass *c)
{
	FILE *fp;
	char *source = (char *) malloc(sizeof(char) *
					    (strlen(dir)
					     + strlen(nv_get_name(c))
					     + 6));

	sprintf(source, "%s/%s.cpp", dir, nv_get_name(c));
	fp = fopen(source, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", source);
		exit(1);
	}

	nv_cpp_write_source_includes(fp, c);
	nv_cpp_write_functions(fp, m, c);
	fclose(fp);
	free(source);
}

static void nv_cpp_write_class(const char *dir, struct UMLModel *m, struct UMLClass *c)
{
	nv_cpp_write_class_header(dir, m, c);
	nv_cpp_write_class_source(dir, m, c);
}

static void nv_cpp_write_model_enumeration(const char *dir, struct UMLModel *m, struct UMLEnumeration *e)
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

	nv_cpp_write_guard_start(fp, nv_get_name(m), nv_get_name(e));
	nv_cpp_write_enumeration(fp, e);
	nv_cpp_write_guard_end(fp, nv_get_name(m), nv_get_name(e));
	fclose(fp);
	free(header);
}

static void nv_cpp_write_primitive_types(const char *dir, struct UMLModel *m)
{
	List *l;
	FILE *fp;
	const char *types_name = "primitive_types";
	char *source = (char *) malloc(sizeof(char) * (strlen(dir) + strlen(types_name) + 4));

	sprintf(source, "%s/%s.h", dir, types_name);
	fp = fopen(source, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", source);
		exit(1);
	}
	free(source);

	nv_cpp_write_guard_start(fp, nv_get_name(m), types_name);
	for(l = nv_uml_model_get_primitivetypes(m);l ;l = l->next) {
		fprintf(fp, "typedef unsigned int %s; ", nv_get_name(l->data));
		fprintf(fp, "/* TODO: select correct C type. */\n");
	}
	nv_cpp_write_guard_end(fp, nv_get_name(m), types_name);

	fclose(fp);
}

static void nv_cpp_write_makefile(const char *dir, struct UMLModel *m)
{
	List *l;
	FILE *fp;
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
	fprintf(fp, "CXX := g++\n");
	fprintf(fp, "WARNINGFLAGS := -Wall -Wextra --pedantic\n");
	fprintf(fp, "CXXFLAGS := $(WARNINGFLAGS) -g\n");
	fprintf(fp, "LIBS :=\n");
	fprintf(fp, "OBJDIR := .\n");
	fprintf(fp, "SRCDIR := .\n");
	fprintf(fp, "INCDIR := .\n");
	fprintf(fp, "INCLUDE := -I $(INCDIR)\n");
	fprintf(fp, "OBJECTS :=");
	for(l = nv_uml_model_get_classes(m);l ;l = l->next) {
		fprintf(fp, "\\\n$(OBJDIR)/%s.o", nv_get_name(l->data));
	}
	fprintf(fp, "\n");
	fprintf(fp, "DEPS = $(OBJECTS:.o=.d)\n");
	fprintf(fp, "\n");

	fprintf(fp, "$(LIBNAME) : $(OBJECTS)\n");
	fprintf(fp, "\tar rcs lib$(LIBNAME).a $(OBJECTS)\n\n");
	fprintf(fp, "-include $(DEPS)\n\n");
	fprintf(fp, "$(OBJDIR)/%%.o : $(SRCDIR)/%%.cpp\n");
	fprintf(fp, "\t$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@\n");
	fprintf(fp, "\t$(CXX) $(CXXFLAGS) -MM -MF $(patsubst %%.o,%%.d,$@) $<\n");

	fprintf(fp, "\n");
	fprintf(fp, "clean:\n");
	fprintf(fp, "\trm -f *.o *.a *.d *~\n");
	fprintf(fp, "\trm -f lib$(LIBNAME).a\n");

	fclose(fp);
}

static void nv_cpp_write_library_header(const char *dir, struct UMLModel *m)
{
	FILE *fp;
	List *l;
	char *header = (char *) malloc(sizeof(char) * (strlen(dir) + strlen(nv_get_name(m)) + 4));

	sprintf(header, "%s/%s.h", dir, nv_get_name(m));
	fp = fopen(header, "w");
	if( fp == NULL ) {
		fprintf(stderr, "Unable to open file %s for writing\n", header);
		exit(1);
	}
	free(header);

	nv_cpp_write_guard_start(fp, nv_get_name(m), "");
	fprintf(fp, "#include \"primitive_types.h\"\n");
	for(l = nv_uml_model_get_classes(m);l ;l = l->next) {
		struct UMLClass *c = (struct UMLClass *) l->data;
		if( !nv_uml_class_get_qualifier(c, NV_ABSTRACT) )
		{
			fprintf(fp, "#include \"%s.h\"\n", nv_get_name(c));
		}
	}
	nv_cpp_write_guard_end(fp, nv_get_name(m), "");
	fclose(fp);
}

void nv_cpp_write_model(struct UMLModel *m, const char *dir)
{
	List *l;
	for(l = nv_uml_model_get_classes(m);l ;l = l->next) {
		nv_cpp_write_class(dir, m, (struct UMLClass *) l->data);
	}

	/*for(l = nv_uml_model_get_datatypes(m);l ;l = l->next) {
		nv_cpp_write_datatype(dir, m, (struct UMLDataType *) l->data);
		}*/

	for(l = nv_uml_model_get_enumerations(m);l ;l = l->next) {
		nv_cpp_write_model_enumeration(dir, m, (struct UMLEnumeration *) l->data);
	}

	nv_cpp_write_primitive_types(dir, m);
	nv_cpp_write_makefile(dir, m);
	nv_cpp_write_library_header(dir, m);
}
