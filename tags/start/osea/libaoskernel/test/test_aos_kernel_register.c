#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <src/aos_kernel.h>
#include <coyote/coyote.h>

#define MODULE_TEST_NAME "aos_kernel_register"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


gint     test1_exit_var = 0;
gboolean test1_exit_var2 = FALSE;
gint     test3_exit_var = 0;

gboolean test1_user_function (AfDalNulData * register_data, gpointer user_data)
{

	if (register_data->state == AFDAL_OK) {
		printf ("Recieved an ok msg\n");
		test1_exit_var++;
	}else {
		printf ("ERROR: Recieved an error msg\n");
		exit (-1);
	}
	
	return TRUE;
}


gboolean print_active_server (gpointer key, gpointer value, gpointer data)
{
	AfDalKernelServer *server = (AfDalKernelServer *) value;
	
	printf("Active server name:[%s] host:[%s] port:[%s]\n", server->name, server->host, server->port);
	
	return FALSE;
}


gboolean test1_user_function2 (AfDalData * register_data, gpointer user_data)
{
	printf ("Active server list:\n");
	if (register_data->state == AFDAL_OK) {
		g_tree_foreach (register_data->data, print_active_server, NULL);
		test1_exit_var2 = TRUE;
	}else {
		printf ("ERROR: Recieved an error msg\n");
		exit (-1);
	}
	return TRUE;
}

gboolean test1_user_function3 (AfDalNulData * register_data, gpointer user_data)
{
 	if (register_data->state == AFDAL_OK) { 
 		printf ("Recieved an ok msg\n"); 
 	}else { 
 		printf ("ERROR: Recieved an error msg\n"); 
 	} 
	test3_exit_var++; 
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

	if (!aos_kernel_register ("test service 1", 1, "localhost", "40000", 
				    connection, test1_user_function, NULL)) {
		return FALSE;
	}

	if (!aos_kernel_register ("test service 2", 1, "localhost", "40000", 
				    connection, test1_user_function, NULL)) {
		return FALSE;
	}

	if (!aos_kernel_register ("test service 3", 1, "localhost", "40000", 
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

	if (!aos_kernel_register_get_servers  (connection, test1_user_function2, NULL)) {
		return FALSE;
	}
	    
	while (!test1_exit_var2) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}


gboolean test3 (void)
{
	RRConnection * connection;
	GError * error = NULL;

	coyote_init (NULL, NULL, &error);

	connection = coyote_connection_new ("localhost", "55000", TYPE_COYOTE_SIMPLE);
	if (!connection)
		return FALSE;

	aos_kernel_register ("test service 1", 1, "localhost", "40000", connection, test1_user_function, NULL);
		
	aos_kernel_register ("test service 2", 1, "localhost", "40000", connection, test1_user_function, NULL); 

	aos_kernel_register ("test service 3", 1, "localhost", "40000", connection, test1_user_function, NULL);


	if (!aos_kernel_unregister ("test service 2", "localhost", "40000",
				    connection, test1_user_function3, NULL)) {
		return FALSE;
	}
	if (!aos_kernel_unregister ("test service 3", "localhost", "40000",
				    connection, test1_user_function3, NULL)) {
		return FALSE;
	}
	if (!aos_kernel_unregister ("test service 1", "localhost", "40000",
				    connection, test1_user_function3, NULL)) {
		return FALSE;
	}


	while (test3_exit_var != 3) {
		sleep (1);
		printf (".");
	}

	return TRUE;
}


TestNode test_suite []  = { 
	{"aos_kernel_register", test1}, 
	{"aos_kernel_get_servers", test2},
	{"aos_kernel_unregister", test3},
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


