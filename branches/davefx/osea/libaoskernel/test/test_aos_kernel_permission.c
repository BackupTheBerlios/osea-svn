#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <src/aos_kernel.h>
#include <coyote/coyote.h>

#define MODULE_TEST_NAME "aos_kernel_permission"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;

gboolean init_function_var = FALSE;

gboolean test1_exit_var = FALSE;
gint     test1_group_created = 0;


gboolean test1_foreach_function (gpointer key, gpointer value, gpointer data) 
{
	AfDalKernelPermission * permission;
	permission = (AfDalKernelPermission *) value;

	printf ("id: %-3d name: %-20s description: %s server_name: %s server_version: %s\n",
		permission->id, 
		permission->name, 
		permission->description, 
		permission->server_name,
		permission->server_version);
	
	return FALSE;
}

gboolean test1_function (AfDalData * data, gpointer usr_data)
{
	if (data->state == AFDAL_OK) {
		g_print ("Recieved an afirmative response\n");
	}else {
		g_print ("Recieved a negative response\n");
	}
	
	afdal_list_foreach (data->data, test1_foreach_function, NULL);
	
	test1_exit_var = TRUE;

	return TRUE;
}

gboolean test1_function2 (AfDalNulData * data, gpointer usr_data)
{
	if (data->state == AFDAL_OK) {
		
	}else {
		
	}

	test1_exit_var = TRUE;	

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

	if (!aos_kernel_permission_list (0, 0, test1_function, NULL)) {
		g_print ("Unable to get permission list\n");
		return FALSE;
	}
	
	

	test1_exit_var = FALSE;
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}


	return TRUE;
}



gboolean init_function_process (AfDalNulData * data, gpointer user_data)
{
	init_function_var = TRUE;
	return TRUE;
}


void init_function ()
{
	afdal_session_login ("aspl", "prueba", "localhost", "55000", init_function_process, NULL);

	while (init_function_var == FALSE) {
		sleep (1);
		printf (".");
	}

	return;
}



TestNode test_suite []  = { 
	{"aos_kernel_permission_list", test1}, 
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


