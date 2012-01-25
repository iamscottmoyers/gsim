#include <gtk/gtk.h>

#include "gui.h"
#include "uml_class.h"
#include "uml_attribute.h"
#include "uml_operation.h"
#include "xml_reader.h"

enum {
	COLUMN = 0,
	NUM_COLS
};

static void tree_class( GtkTreeStore *tree, GtkTreeIter *parent, struct UMLClass *c )
{
	struct ListLink *iter;
	GtkTreeIter child, operation_child, attribute_child;

	gtk_tree_store_append(tree, &child, parent);
	gtk_tree_store_set(tree, &child, COLUMN, nv_get_name(c), -1);

	gtk_tree_store_append(tree, &attribute_child, &child);
	gtk_tree_store_set(tree, &attribute_child, COLUMN, "Attributes", -1);

	for(iter = nv_list_front(&c->attributes); iter; iter = nv_list_next(iter)) {
		GtkTreeIter att_child;
		struct UMLAttribute *a = NV_LIST_GET_DATA( iter, struct UMLAttribute, link );
		gtk_tree_store_append(tree, &att_child, &attribute_child);
		gtk_tree_store_set(tree, &att_child, COLUMN, nv_get_name(a), -1);
	}

	gtk_tree_store_append(tree, &operation_child, &child);
	gtk_tree_store_set(tree, &operation_child, COLUMN, "Operations", -1);

	for(iter = nv_list_front(&c->operations); iter; iter = nv_list_next(iter)) {
		GtkTreeIter op_child;
		struct UMLOperation *o = NV_LIST_GET_DATA( iter, struct UMLOperation, link );
		gtk_tree_store_append(tree, &op_child, &operation_child);
		gtk_tree_store_set(tree, &op_child, COLUMN, nv_get_name(o), -1);
	}
}

static void tree_model( GtkTreeStore *tree, struct UMLModel *model )
{
	struct ListLink *iter;
	GtkTreeIter child, class_child, package_child;

	gtk_tree_store_append(tree, &child, NULL);
	gtk_tree_store_set(tree, &child, COLUMN, nv_get_name(model), -1);

	gtk_tree_store_append(tree, &package_child, &child);
	gtk_tree_store_set(tree, &package_child, COLUMN, "Packages", -1);

	gtk_tree_store_append(tree, &class_child, &child);
	gtk_tree_store_set(tree, &class_child, COLUMN, "Classes", -1);

	for(iter = nv_list_front(&model->super.classes); iter; iter = nv_list_next(iter)) {
		struct UMLClass *c = NV_LIST_GET_DATA( iter, struct UMLClass, link );
		tree_class( tree, &class_child, c );
	}
}

static GtkTreeModel *create_and_fill_model(void)
{
	GtkTreeStore *tree;
	struct UMLModel *model;

	tree = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING);
	model = nv_xmlr_read_file("/Users/scott/Projects/gsim/aire/aire.uml");
	if (model == NULL) {
		return NULL;
	}

	tree_model(tree, model);
	nv_uml_model_delete(model);
	return GTK_TREE_MODEL(tree);
}

static GtkWidget *create_view_and_model (void)
{
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;
	GtkWidget *view;
	GtkTreeModel *model;

	view = gtk_tree_view_new();

	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "UML Elements");
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COLUMN);

	model = create_and_fill_model();
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
	g_object_unref(model);

	return view;
}

void nv_gui(int *argc, char ***argv)
{
	GtkWidget *window;
	GtkWidget *view;
	GtkWidget *vbox;

	gtk_init(argc, argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "XMI View");
	gtk_widget_set_size_request (window, 350, 300);

	vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	view = create_view_and_model();
	gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 1);
	g_signal_connect(G_OBJECT (window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);

	gtk_main();
}
