#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <src/afdal_kernel_register.h>
#include <coyote/coyote.h>

#define MODULE_TEST_NAME "afdal_kernel_register"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


gint test1_exit_var = 0;
gboolean test1_exit_var2 = FALSE;

gboolean test1_user_function (gpointer data, gpointer user_data)
{
	AfDalKernelRegister * register_data;

	register_data = (AfDalKernelRegister *) data;

	if (register_data->state == AFDAL_KERNEL_OK) {
		printf ("Recieved an ok msg\n");
		test1_exit_var++;
	}else {
		printf ("ERROR: Recieved an error msg\n");
		exit (-1);
	}
	
	return TRUE;
}

gboolean test1_user_function2 (gpointer data, gpointer user_data)
{
	AfDalKernelRegister * register_data;
	gint i, j;

	register_data = (AfDalKernelRegister *) data;

	printf ("Active server list:\n");
	if (register_data->state == AFDAL_KERNEL_OK) {
		for (i = 0; i < coyote_dataset_get_height (register_data->active_servers); i++) {
			printf ("Active server name:[%s] host:[%s] port:[%s]\n",
				coyote_dataset_get (register_data->active_servers, i, 0),
				coyote_dataset_get (register_data->active_servers, i, 1),
				coyote_dataset_get (register_data->active_servers, i, 2));
		}
		test1_exit_var2 = TRUE;
	}else {
		printf ("ERROR: Recieved an error msg\n");
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
	RRConnection * connection;
	GError * error = NULL;


	coyote_init (NULL, NULL, &error);

	connection = coyote_connection_new ("localhost", "55000", TYPE_COYOTE_SIMPLE);
	if (!connection)
		return FALSE;

	if (!afdal_kernel_register ("test service 1", "localhost", "40000", 
				    connection, test1_user_function, NULL)) {
		return FALSE;
	}

	if (!afdal_kernel_register ("test service 2", "localhost", "40000", 
				    connection, test1_user_function, NULL)) {
		return FALSE;
	}

	if (!afdal_kernel_register ("test service 3", "localhost", "40000", 
				    connection, test1_user_function, NULL)) {
		return FALSE;
	}

	while (test1_exit_var != 3) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}


gboolean test2 (void)
{
	RRConnection * connection;
	GError * error = NULL;


	coyote_init (NULL, NULL, &error);

	connection = coyote_connection_new ("localhost", "55000", TYPE_COYOTE_SIMPLE);
	if (!connection)
		return FALSE;

	if (!afdal_kernel_register_get_servers  (connection, test1_user_function2, NULL)) {
		return FALSE;
	}
	    
	while (!test1_exit_var2) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}

TestNode test_suite []  = { {"afdal_kernel_register", test1}, 
			   {"afdal_kernel_get_servers", test2},
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

