#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <src/aos_kernel.h>

#define MODULE_TEST_NAME "aos_kernel_user"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;

gboolean init_function_var = FALSE;

gboolean test1_exit_var = FALSE;
gint     test1_user_created = 0;

gboolean test1_function (AfDalSimpleData * data, gpointer usr_data)
{
	if (data->state == AFDAL_OK) {
		printf ("User created: %d\n", data->id);
		test1_user_created = data->id;
		test1_exit_var = TRUE;	
	}else {
		printf ("Fail to create user\n");
		exit (-1);
	}

	return TRUE;
}

gboolean test1_function2 (AfDalNulData * data, gpointer usr_data)
{
	gboolean edited = GPOINTER_TO_INT (usr_data);
	if (data->state == AFDAL_OK) {
		if (edited)
			printf ("User edited\n");
		else 
			printf ("User deleted\n");
		test1_exit_var = TRUE;	
	}else {
		if (edited)
			printf ("Fail to edit user\n");
		else 
			printf ("Fail to delete user\n");
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

	// create a new user
	if (!aos_kernel_user_new ("test", "foo_pass", "This is a test",
				    test1_function, NULL)) {
		return FALSE;
	}
	
	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	// edit the user
	test1_exit_var = FALSE;
	if (!aos_kernel_user_edit (test1_user_created, "new test", "new foo_pass", "This is not a test", 
				     test1_function2, GINT_TO_POINTER (TRUE))) {
		
		return FALSE;
	}

	while (test1_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}

	// delete the user
	test1_exit_var = FALSE;
	if (!aos_kernel_user_remove (test1_user_created, 
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
GList    *  test2_user_to_remove = NULL;

gboolean test2_function2 (AfDalNulData * data, gpointer usr_data)
{
	gboolean is_the_last = GPOINTER_TO_INT (usr_data);
	printf ("We got a response..\n");
	if (data->state == AFDAL_OK) {
		printf ("User deleted\n");
	}else {
		printf ("Fail to delete user\n");
		exit (-1);
	}

	if (is_the_last)
		test2_exit_var = TRUE;

	return TRUE;
}

gboolean test2_foreach_function (gpointer key, gpointer value, gpointer data) 
{
	AfDalKernelUser * user;
	user = (AfDalKernelUser *) value;

	printf ("id: %d nick: %s description: %s\n",
		user->id, user->nick, user->description);
	
	test2_user_to_remove = g_list_append (test2_user_to_remove, GINT_TO_POINTER(user->id));

	return FALSE;
}



gboolean test2_function (AfDalData * data, gpointer user_data) {
	
	if (data->state == AFDAL_OK) {
		printf ("Recieved correct response: %s\n", data->text_response);
		printf ("Number of nodes..%d\n", afdal_list_length (data->data));
		
	}else {
		printf ("Recieved incorrect response: %s\n", data->text_response);
		exit (-1);
	}

	afdal_list_foreach (data->data, test2_foreach_function, user_data);
	test2_exit_var = TRUE;	
	return TRUE;
}


gboolean test2 (void) {
	
	GList  * aux_list = NULL;

	// We are goint to create several users. After that list them to delete them finally. 
	if (!aos_kernel_user_new ("test", "foo_pass", "This is a test",
				    NULL, NULL)) {
		return FALSE;
	}

	if (!aos_kernel_user_new ("test2", "foo_pass", "This is a test",
				    NULL, NULL)) {
		return FALSE;
	}

	if (!aos_kernel_user_new ("test3", "foo_pass", "This is a test",
				   NULL, NULL)) {
		return FALSE;
	}
	
	if (!aos_kernel_user_list (0, 0, test2_function, NULL)) {
		return FALSE;
	}
	
	while (test2_exit_var == FALSE) {
		sleep (1);
		printf (".");
	}
	
	// Remove all created users..
	test2_exit_var = FALSE;
	for (aux_list = g_list_first (test2_user_to_remove); aux_list; aux_list = g_list_next (aux_list)) {
		if (!g_list_next (aux_list)) {
			if (!aos_kernel_user_remove (GPOINTER_TO_INT (aux_list->data), test2_function2, 
						       GINT_TO_POINTER(TRUE))) {
				printf ("Unable to remove listed user\n");
				exit (-1);
			}
		}
		else
			if (!aos_kernel_user_remove (GPOINTER_TO_INT (aux_list->data), test2_function2, 
						       GINT_TO_POINTER(FALSE))) {
				printf ("Unable to remove listed user\n");
				exit (-1);
			}
	}
	
	while (test2_exit_var == FALSE) {
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
	{"aos_kernel_user_new/remove/edit", test1}, 
	{"aos_kernel_user_list/remove",     test2}, 
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


