#include "osea_datalist.h"

typedef struct __AsplDataListNode {
	gpointer        data;
	gchar         * data_name;
	GDestroyNotify  free_func;
} AsplDataListNode;


gint __osea_datalist_key_compare (gconstpointer a,
				  gconstpointer b,
				  gpointer user_data)
{
	GQuark qa = GPOINTER_TO_INT(a);
	GQuark qb = GPOINTER_TO_INT(b);
	
	if (qa < qb)
		return -1;
	
	if (qa > qb)
		return 1;
	return 0;
}

void __osea_datalist_node_destroy (gpointer data)
{
	AsplDataListNode * node = (AsplDataListNode *) data;

	g_return_if_fail (node);
	
	if (node->free_func)
		node->free_func (node->data);

	g_free (node->data_name);
	g_free (node);
	

	return;
}


AsplDataList * osea_datalist_new      ()
{
	return g_tree_new_full (__osea_datalist_key_compare, NULL, NULL, __aspl_datalist_node_destroy) ;
}

void           osea_datalist_free     (AsplDataList * adl)
{
	g_tree_destroy (adl);
	return;
}

void           osea_datalist_set_full (AsplDataList * adl, const gchar * name, gpointer data, GDestroyNotify free_func)
{
	AsplDataListNode * node = g_new0 (AsplDataListNode, 1);

	node->data = data;
	node->data_name = g_strdup (name);
	node->free_func = free_func;

	g_tree_insert (adl, GINT_TO_POINTER(g_quark_from_string(name)), node);

	return;
}

void           osea_datalist_set      (AsplDataList * adl, const gchar * name, gpointer data)
{
	AsplDataListNode * node = g_new0 (AsplDataListNode, 1);

	node->data = data;
	node->data_name = g_strdup (name);

	g_tree_insert (adl, GINT_TO_POINTER(g_quark_from_string(name)), node);

	return;
}

gpointer       osea_datalist_get      (AsplDataList * adl, const gchar * name)
{
	AsplDataListNode * node;

	node = (AsplDataListNode *) g_tree_lookup (adl, GINT_TO_POINTER(g_quark_from_string(name)));

	return node->data;
}

gboolean    __osea_datalist_print_node (gpointer key,
					gpointer value,
					gpointer data)
{
	AsplDataListNode * node = (AsplDataListNode *) value;
	g_print ("name: %s value: 0x%x\n", node->data_name, GPOINTER_TO_INT (node->data));
	return FALSE;
}
void           osea_datalist_print    (AsplDataList * adl)
{
	g_return_if_fail (adl);

	g_print ("Aspl Data List content:\n");
	g_tree_foreach (adl, __osea_datalist_print_node, NULL);
	return;
}
