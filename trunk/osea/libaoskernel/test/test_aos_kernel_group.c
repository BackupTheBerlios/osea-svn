#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <src/aos_kernel.h>

#define MODULE_TEST_NAME "aos_kernel_group"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;

gboolean init_function_var = FALSE;

gboolean test1_exit_var = FALSE;
gint     test1_group_created = 0;

gboolean test1_function (OseaClientSimpleData * data, gpointer usr_data)
{
	if (data->state == OSEACLIENT_OK) {
		printf ("Group created: %d\n", data->id);
		test1_group_created = data->id;
		test1_exit_var = TRUE;	
	}else {
		printf ("Fail to create group\n");
		exit (-1);
	}

	return TRUE;
}

gboolean test1_function2 (OseaClientNulData * data, gpointer usr_data)
{
	gboolean edited = GPOINTER_TO_INT (usr_data);
	if (data->state == OSEACLIENT_OK) {
		if (edited)
			printf ("Group edited\n");
		else 
			printf ("Group deleted\n");

	}else {
		if (edited)
			printf ("Fail to edit group: %s\n", data->text_response);
		else 
			printf ("Fail to delete group: %s\n", data->text_response);
		exit (-1);
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

	// create a new group
	if (!aos_kernel_group_new ( "foo group", "Foo group test",
				      test1_function, NULL)) {
		return FALSE;
	}
	
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	// edit the group
	test1_exit_var = FALSE;
	if (!aos_kernel_group_edit (test1_group_created, "new foo group", "This is foo group test", 
				      test1_function2, GINT_TO_POINTER (TRUE))) {
		
		return FALSE;
	}

	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	// delete the group
	test1_exit_var = FALSE;
	if (!aos_kernel_group_remove (test1_group_created,
				       test1_function2, GINT_TO_POINTER (FALSE))) {
		
		return FALSE;
	}

	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	return TRUE;

}


gboolean    test2_exit_var = FALSE;
GList    *  test2_group_to_remove = NULL;

gboolean test2_function2 (OseaClientNulData * data, gpointer usr_data)
{
	gboolean is_the_last = GPOINTER_TO_INT (usr_data);
	printf ("We got a response..\n");
	if (data->state == OSEACLIENT_OK) {
		printf ("Group deleted\n");
	}else {
		printf ("Fail to delete group\n");
		exit (-1);
	}

	if (is_the_last)
		test2_exit_var = TRUE;

	return TRUE;
}

gboolean test2_foreach_function (gpointer key, gpointer value, gpointer data) 
{
	AosKernelGroup * group;
	group = (AosKernelGroup *) value;

	printf ("id: %d name: %s description: %s\n",
		group->id, group->name, group->description);
	
	test2_group_to_remove = g_list_append (test2_group_to_remove, GINT_TO_POINTER(group->id));

	return FALSE;
}



gboolean test2_function (OseaClientData * data, gpointer group_data) {
	
	if (data->state == OSEACLIENT_OK) {
		printf ("Recieved correct response: %s\n", data->text_response);
		printf ("Number of nodes..%d\n", oseaclient_list_length (data->data));
		
	}else {
		printf ("Recieved incorrect response: %s\n", data->text_response);
		exit (-1);
	}

	oseaclient_list_foreach (data->data, test2_foreach_function, group_data);
	test2_exit_var = TRUE;	
	return TRUE;
}


gboolean test2 (void) {
	
	GList  * aux_list = NULL;
	

	// We are goint to create several groups. After that list them to delete them finally. 
	if (!aos_kernel_group_new ("foo group 1", "This is a test",
				     NULL, NULL)) {
		return FALSE;
	}

	if (!aos_kernel_group_new ("foo group 2", "This is a test",
				     NULL, NULL)) {
		return FALSE;
	}

	if (!aos_kernel_group_new ("foo group 3", "This is a test",
				     NULL, NULL)) {
		return FALSE;
	}
	
	if (!aos_kernel_group_list (0, 0, test2_function, NULL)) {
		return FALSE;
	}
	
	while (test2_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}
	
	// Remove all created groups..
	test2_exit_var = FALSE;
	for (aux_list = g_list_first (test2_group_to_remove); aux_list; aux_list = g_list_next (aux_list)) {
		if (!g_list_next (aux_list)) {
			if (!aos_kernel_group_remove (GPOINTER_TO_INT (aux_list->data), test2_function2, 
						       GINT_TO_POINTER(TRUE))) {
				printf ("Unable to remove listed group\n");
				exit (-1);
			}
		}
		else
			if (!aos_kernel_group_remove (GPOINTER_TO_INT (aux_list->data), test2_function2, 
						       GINT_TO_POINTER(FALSE))) {
				printf ("Unable to remove listed group\n");
				exit (-1);
			}
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
	{"aos_kernel_group_new/remove/edit", test1}, 
	{"aos_kernel_group_list/remove",     test2}, 
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


