#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "xml_reader.h"
#include "uml_model.h"
#include "c_writer.h"
#include "cpp_writer.h"
#include "graphviz_writer.h"

/* Typedefs for read and write execution functions. */
typedef struct UMLModel *(*read_function)(const char *);
typedef void (*write_function)(struct UMLModel *, const char *);

/* Maps read argument strings to their operation. */
struct read_op_map {
	const char *str;
	read_function pfn;
};

/* Maps write argument strings to their operation. */
struct write_op_map {
	const char *str;
	write_function pfn;
};

/* Argument string -> operation mappings for read operations */
struct read_op_map read_map[] = {
	{"uml", nv_xmlr_read_file}
};

/* Argument string -> operation mappings for write operations. */
struct write_op_map write_map[] = {
	{"c", nv_c_write_model},
	{"cpp", nv_cpp_write_model},
	{"graphviz", nv_graphviz_write_model}
};

/* Status machine for parsing the command line. */
enum arg_state {
	MATCH_READ_WRITE,
	MATCH_READ_OP,
	MATCH_WRITE_OP,
	MATCH_READ_PATH,
	MATCH_WRITE_PATH
};

/* Prints a message and usage information when argument parsing fails. */
void usage(const char *prog, const char *msg)
{
	unsigned int i;
	printf("ERROR: %s\n\n", msg);
	printf("Usage:\n");
	printf("\t%s ( --read operation input_path ( --write operation output_path )* )*\n\n", prog);
	printf("Where --read operation can be any of:\n");
	for(i = 0; i < NELEMS(read_map); i++) {
		printf("%s\n", read_map[i].str);
	}
	printf("\n");
	printf("and where --write operation can be any of:\n");
	for(i = 0; i < NELEMS(write_map); i++) {
		printf("%s\n", write_map[i].str);
	}
	exit(1);
}

void interpret_arguments(int argc, char **argv)
{
	enum arg_state state = MATCH_READ_WRITE;
	struct UMLModel *current_model = NULL;
	read_function read = NULL;
	write_function write = NULL;
	int i;

	for(i = 1; i < argc; i++) {
		switch(state) {
			case MATCH_READ_WRITE:
				if(!strcmp("--read", argv[i])) {
					state = MATCH_READ_OP;
				} else if(!strcmp("--write", argv[i])) {
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
						read = read_map[j].pfn;
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
						write = write_map[j].pfn;
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
				assert(read);
				nv_uml_model_delete(current_model);
				current_model = read(argv[i]);
				state = MATCH_READ_WRITE;
				break;
			case MATCH_WRITE_PATH:
				assert(write);
				if(NULL == current_model) {
					usage(argv[0], "No existing model. Encountered a --write before a --read.");
				}

				write(current_model, argv[i]);
				state = MATCH_READ_WRITE;
				break;
			default:
				assert(0 && "Invalid argument state." );
				break;
		}
	}
	nv_uml_model_delete(current_model);
}

int main(int argc, char **argv)
{
	interpret_arguments(argc, argv);
	return 0;
}
