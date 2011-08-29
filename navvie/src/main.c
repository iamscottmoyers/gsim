#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "xml_reader.h"
#include "uml_model.h"
#include "c_writer.h"
#include "cpp_writer.h"
#include "graphviz_writer.h"

/* Supported operations */
enum operation {
	READ_UML,      /* Create a UMLModel from a .uml file */
	WRITE_C,       /* Write a UMLModel as a C library */
	WRITE_CPP,     /* Write a UMLModel as a C++ library */
	WRITE_GRAPHVIZ /* Write a UMLModel as a graphviz file */
};

/* Maps argument strings to their operation. */
struct arg_op_map {
	const char *str;
	enum operation op;
};

/* Argument string -> operation mappings for read operations */
struct arg_op_map read_map[] = {
	{"uml", READ_UML}
};

/* Argument string -> operation mappings for write operations. */
struct arg_op_map write_map[] = {
	{"c", WRITE_C},
	{"cpp", WRITE_CPP},
	{"graphviz", WRITE_GRAPHVIZ}
};

/* An operation / path pair. This is the format all read / write arguments come in as. */
struct arg {
	enum operation op;
	const char *path;
};

/* All the arguments passed on the command line. Supports a single read argument and many write arguments. */
struct args {
	unsigned int num_read_args;
	struct arg read_arg;
	unsigned int num_write_args;
	struct arg write_args[NELEMS(write_map)];
};

/* Status machine for parsing the command line. */
enum arg_state {
	MATCH_READ_WRITE,
	MATCH_READ_OP,
	MATCH_WRITE_OP,
	MATCH_READ_PATH,
	MATCH_WRITE_PATH
};

/* Global arguments structure. */
static struct args arguments = {0, {READ_UML, NULL}, 0, {{READ_UML, NULL}}};

/* Prints a message and usage information when argument parsing fails. */
void usage(const char *prog, const char *msg)
{
	unsigned int i;
	printf("%s\n\n", msg);
	printf("Usage:\n");
	printf("\t%s --read operation input_path (--write operation output_path)*\n\n", prog);
	printf("Where you can have a single --read operation from one one of:\n");
	for(i = 0; i < NELEMS(read_map); i++) {
		printf("%s\n", read_map[i].str);
	}
	printf("\n");
	printf("and you can have 0 to %d --write operations which can be:\n", NELEMS(write_map));
	for(i = 0; i < NELEMS(write_map); i++) {
		printf("%s\n", write_map[i].str);
	}
	exit(1);
}

void parse_arguments(int argc, char **argv)
{
	int i;
	enum arg_state state = MATCH_READ_WRITE;
	for(i = 1; i < argc; i++) {
		switch(state) {
			case MATCH_READ_WRITE:
				if(!strcmp("--read", argv[i])) {
					if(arguments.num_read_args > 0) {
						usage(argv[0], "Too many read arguments.");
					}
					state = MATCH_READ_OP;
				} else if(!strcmp("--write", argv[i])) {
					if(arguments.num_write_args >= NELEMS(write_map)) {
						usage(argv[0], "Too many write arguments." );
					}
					state = MATCH_WRITE_OP;
				} else {
					usage(argv[0], "Unknown argument.");
				}
				break;
			case MATCH_READ_OP:
			{
				unsigned int j;
				for(j = 0; j < NELEMS(read_map); j++) {
					if(!strcmp(read_map[j].str, argv[i])) {
						arguments.read_arg.op = read_map[j].op;
						break;
					}
				}

				if( j == NELEMS(read_map) ) {
					usage(argv[0], "Unknown read operation.");
				}
				state = MATCH_READ_PATH;
				break;
			}
			case MATCH_WRITE_OP:
			{
				unsigned int j;
				for(j = 0; j < NELEMS(write_map); j++) {
					if(!strcmp(write_map[j].str, argv[i])) {
						arguments.write_args[arguments.num_write_args].op = write_map[j].op;
						break;
					}
				}

				if( j == NELEMS(write_map) ) {
					usage(argv[0], "Unknown write operation.");
				}
				state = MATCH_WRITE_PATH;
				break;
			}
			case MATCH_READ_PATH:
				arguments.read_arg.path = argv[i];
				arguments.num_read_args++;
				state = MATCH_READ_WRITE;
				break;
			case MATCH_WRITE_PATH:
				arguments.write_args[arguments.num_write_args].path = argv[i];
				arguments.num_write_args++;
				state = MATCH_READ_WRITE;
				break;
			default:
				assert(0 && "Invalid argument state." );
				break;
		}
	}

	if(arguments.num_read_args == 0) {
		usage(argv[0], "You must specify at least one read argument.");
	}
}

static void write_model(struct UMLModel *m)
{
	unsigned int i;
	for(i = 0; i < arguments.num_write_args; i++) {
		const char *path = arguments.write_args[i].path;
		switch(arguments.write_args[i].op) {
			case WRITE_C:
				nv_c_write_model(m, path);
				break;
			case WRITE_CPP:
				nv_cpp_write_model(m, path);
				break;
			case WRITE_GRAPHVIZ:
				nv_graphviz_write_model(m, path);
				break;
			default:
				assert(0 && "Unknown write argument." );
				break;
		}
	}
}

int main(int argc, char **argv)
{
	parse_arguments(argc, argv);

	switch(arguments.read_arg.op) {
		case READ_UML:
		{
			struct UMLModel *m = nv_xmlr_read_file(arguments.read_arg.path);
			write_model(m);
			nv_uml_model_delete(m);
			break;
		}
		default:
			assert(0 && "Unknown read argument." );
			break;
	}

	return 0;
}
