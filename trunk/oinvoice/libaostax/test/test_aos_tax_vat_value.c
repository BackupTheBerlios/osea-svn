#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib.h>
#include <src/aos_tax.h>
#include <liboseacomm/oseacomm.h>


#define MODULE_TEST_NAME "aos_tax_vat_value"

typedef struct _TestNode {
	gchar     * test_description;
	gboolean (* test_function) (void);
}TestNode;

gboolean init_function_var = FALSE;

gboolean test1_exit_var = FALSE;
gboolean test2_exit_var = FALSE;
gboolean test3_exit_var = FALSE;

gint     test3_identifier = 0;


gboolean test2_printout_index (gpointer key, gpointer value, gpointer data)
{
	AosTaxVatValueHeader * node;

	node = (AosTaxVatValueHeader *) value;

	g_print (" id:%-3d reference:%-10s \n", node->id, node->reference);
	
	return FALSE;
}

gboolean test2_printout_value (gpointer key, gpointer value, gpointer data)
{
	AosTaxVatValueData * node;

	node = (AosTaxVatValueData *) value;

	g_print (" id_customer:%-3d id_item:%-3d value:%-10ld \n", node->key->id_item, node->key->id_customer, node->value);
	
	return FALSE;
}

gboolean test1_user_function (OseaClientNulData * data, gpointer user_data)
{
	
	if (data->state == OSEACLIENT_OK) {
		test1_exit_var = TRUE;
	}else {
		g_print ("ERROR: Recieved an error msg\n");
		exit (-1);
	}

	return TRUE;
}

gboolean test1_user_function2 (OseaClientSimpleData * data, gpointer user_data)
{
	
	if (data->state == OSEACLIENT_OK) {
		test2_exit_var = TRUE;
	}else {
		g_print ("ERROR: Recieved an error msg\n");
		exit (-1);
	}

	return TRUE;
}
gboolean test2_user_function (OseaClientMultiData * data, gpointer user_data)
{
	GList * gtree_list = NULL;

	GTree * customer_index = NULL;
	GTree * item_index = NULL;
	GTree * value = NULL;

	if (data->state == OSEACLIENT_OK) {

		gtree_list = data->multi_data;
		customer_index = (GTree *) gtree_list->data;
		
		gtree_list = g_list_next (gtree_list);
		item_index = (GTree *) gtree_list->data;

		gtree_list = g_list_next (gtree_list);
		value = (GTree *) gtree_list->data;

		g_print ("Customer index content:\n");
		
		g_tree_foreach (customer_index, test2_printout_index, NULL);

		g_print ("Item index content:\n");
		
		g_tree_foreach (item_index, test2_printout_index, NULL);

		g_print ("Value content: ");
		g_print ("%d\n", g_tree_nnodes (value));
		
		g_tree_foreach (value, test2_printout_value, NULL);

		

		test2_exit_var = TRUE;
		
	}else {
		g_print ("ERROR: Recieved an error msg\n");
		exit (-1);
	}

	return TRUE;
}



/**
 * test1:
 * 
 * 
 * 
 **/
gboolean test1 (void)
{
	GError * error = NULL;


	oseacomm_init (NULL, NULL, &error);


// Set value	
	test1_exit_var = FALSE;
	if (!aos_tax_vat_value_set (143, 8, 30, 
				      test1_user_function, NULL)) {
		return FALSE;
	}
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// Get value	
	test2_exit_var = FALSE;
	if (!aos_tax_vat_value_get ( 143, 8, test1_user_function2, NULL)) {
		return FALSE;
	}
	while (test2_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}

gboolean test2 (void)
{
	GError * error = NULL;


        oseacomm_init (NULL, NULL, &error);


	if (!aos_tax_vat_value_list  (test2_user_function, NULL)) {
		return FALSE;
	}	

	while (test2_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}


gboolean init_function_process (OseaClientNulData * data, gpointer user_data)
{
	init_function_var = TRUE;
	return TRUE;
}


void init_function ()
{
	oseaclient_session_login ("aspl", "prueba", "localhost", "55000", init_function_process, NULL);

	while (init_function_var == FALSE) {
		sleep (1);
		printf (".");
	}

	return;
}



TestNode test_suite []  = {
//	{"aos_tax_vat_value_set/get", test1},
	{"aos_tax_vat_value_list", test2},
	{NULL, NULL}};

/**
 * main:
 * @argc: 
 * @argv: 
 * 
 * This function will test all the functions defined at test_suite array. 
 * DO NOT TOUCH THIS FUNCTION
 * 
 * Return value: 0 if everything was right or -1 if not.
 **/
int main(int argc, char **argv) 
{

	gint i;

	init_function ();

	for (i = 0; test_suite[i].test_description != NULL; i++) {
	
		if (test_suite[i].test_function ()) 
			g_print (" [  OK  ] [%s]: Testing: %s \n", MODULE_TEST_NAME, test_suite[i].test_description);
		else {
			g_print (" [ FAIL ] [%s]: Testing: %s \n", MODULE_TEST_NAME, test_suite[i].test_description);
			exit (-1);
		}
	}
	
	exit (0);

}


