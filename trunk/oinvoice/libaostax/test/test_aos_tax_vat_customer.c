#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib.h>
#include <src/aos_tax.h>


#define MODULE_TEST_NAME "aos_tax_vat_customer"

typedef struct _TestNode {
	gchar     * test_description;
	gboolean (* test_function) (void);
}TestNode;

gboolean init_function_var = FALSE;

gboolean test1_exit_var = FALSE;
gboolean test2_exit_var = FALSE;
gboolean test3_exit_var = FALSE;

gint     test3_identifier = 0;

gboolean print_vat_customer (gpointer key, gpointer value, gpointer data)
{
	AosTaxVatCustomer *customer = (AosTaxVatCustomer *) value;
	

	printf("%d\t%s\t%s\t%s\n", customer->id, customer->reference, customer->name, customer->description );
	
	return FALSE;
}

gboolean test1_user_function (OseaClientData * data, gpointer user_data)
{

	if (data->state == OSEACLIENT_OK) {
		oseaclient_list_foreach (data->data, print_vat_customer, NULL);
		test1_exit_var = TRUE;
	}else {
		printf ("ERROR: Recieved an error msg\n");
		exit (-1);
	}

	return TRUE;
}


gboolean test2_user_function (OseaClientSimpleData * data, gpointer user_data)
{
	
	if (data->state == OSEACLIENT_OK) {
		printf ("ID  = %d\n", data->id);
		test2_exit_var = TRUE;
	}else {
		g_print ("ERROR: Recieved an error msg\n");
		exit (-1);
	}

	return TRUE;
}

gboolean test2_user_function2 (OseaClientSimpleData * data, gpointer user_data)
{
	
	if (data->state == OSEACLIENT_ERROR) {
		test3_exit_var = TRUE;
	}else {
		g_print ("ERROR: Recieved an ok msg while expecting error\n");
		exit (-1);
	}

	return TRUE;
}

gboolean test3_user_function (OseaClientSimpleData * data, gpointer user_data)
{
	
	if (data->state == OSEACLIENT_OK) {
		test3_identifier = data->id;
		g_print ("ID = %d\n", data->id);
		test3_exit_var = TRUE;
	}else {
		g_print ("ERROR: Recieved an error msg\n");
		exit (-1);
	}

	return TRUE;
}

gboolean test3_user_function2 (OseaClientNulData * data, gpointer user_data)
{
	
	if (data->state == OSEACLIENT_OK)
		g_print ("OK: Value changed\n");
	else
		g_print ("ERROR: Recieved an error msg\n");
	
	test3_exit_var = TRUE;
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



// List customer
	test1_exit_var = FALSE;
	if (!aos_tax_vat_customer_list ( 0, 0, test1_user_function, NULL)) {
		return FALSE;
	}
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// New customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_new ("ref", "name", "description", 
					 test3_user_function, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// List customer	
	test1_exit_var = FALSE;
	if (!aos_tax_vat_customer_list ( 0, 0, test1_user_function, NULL)) {
		return FALSE;
	}
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// Remove customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_remove (test3_identifier, test3_user_function2, NULL)) {
		return FALSE;
	} 
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// List customer
	test1_exit_var = FALSE;
	if (!aos_tax_vat_customer_list ( 0, 0, test1_user_function, NULL)) {
		return FALSE;
	}
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}




gboolean test2 (void)
{
	gint identifier;

// New customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_new ("ref", "name", "description", 
					 test3_user_function, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}
	identifier = test3_identifier;

// New customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_new ("ref_2", "name_2", "description_2", 
					 test3_user_function, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}
	
// New customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_new ("ref_2", "name_2", "description_2", 
					 test2_user_function2, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// List customer
	test1_exit_var = FALSE;
	if (!aos_tax_vat_customer_list ( 0, 0, test1_user_function, NULL)) {
		return FALSE;
	}
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// Remove customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_remove (identifier, test3_user_function2, NULL)) {
		return FALSE;
	} 
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// List customer
	test1_exit_var = FALSE;
	if (!aos_tax_vat_customer_list ( 0, 0, test1_user_function, NULL)) {
		return FALSE;
	}
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}


// Remove customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_remove (test3_identifier, test3_user_function2, NULL)) {
		return FALSE;
	} 
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// List customer
	test1_exit_var = FALSE;
	if (!aos_tax_vat_customer_list ( 0, 0, test1_user_function, NULL)) {
		return FALSE;
	}

	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}

gboolean test3 (void)
{
	gint identifier;


// New customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_new ("ref", "name", "description", 
					 test3_user_function, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}
	identifier = test3_identifier;
	
// New customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_new ("ref_2", "name_2", "description_2", 
					  test3_user_function, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}
	
// Edit customer
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_edit (identifier, "new_ref", "new_name", "new_description", 
					   test3_user_function2, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// Edit customer
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_edit (identifier-3, "new_ref", "new_name", "new_description", 
					  test3_user_function2, NULL)) {
		return FALSE;
	}
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// List customer
	test1_exit_var = FALSE;

	if (!aos_tax_vat_customer_list ( 0, 0, test1_user_function, NULL)) {
		return FALSE;
	}

	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}
	
// Remove customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_remove (identifier, test3_user_function2, NULL)) {
		return FALSE;
	} 
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// Remove customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_remove (identifier, test3_user_function2, NULL)) {
		return FALSE;
	} 
	while (test3_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

// Remove customer	
	test3_exit_var = FALSE;
	if (!aos_tax_vat_customer_remove (test3_identifier, test3_user_function2, NULL)) {
		return FALSE;
	} 
	while (test3_exit_var == FALSE) {
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



TestNode test_suite []  = { {"aos_tax_vat_customer_list/new/remove LVL 1", test1},
			    {"aos_tax_vat_customer_list/new/remove LVL 2", test2},
			    {"aos_tax_vat_customer_list/new/remove/edit LVL 3", test3},
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

