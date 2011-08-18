#include <stdio.h>

#include "xml_reader.h"
#include "uml_model.h"
#include "c_writer.h"
#include "graphviz_writer.h"

void usage(char *prog)
{
	printf("Usage:\n");
	printf("\t%s filename outputdir\n", prog);
}

int main(int argc, char **argv)
{
	struct UMLModel *m;
	if (argc != 3) {
		usage(argv[0]);
		return 1;
	}

	m = nv_xmlr_read_file(argv[1]);
	if (m == NULL) {
		return 1;
	}

	/*nv_c_write_model(m, argv[2]);*/
	nv_cpp_write_model(m, argv[2]);
	nv_graphviz_write_model(m, stdout);
	nv_uml_model_delete(m);
	return 0;
}
