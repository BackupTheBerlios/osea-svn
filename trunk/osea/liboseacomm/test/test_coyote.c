#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <src/coyote.h>
#include <unistd.h>

#include <libintl.h>
#include <locale.h>


#define MODULE_TEST_NAME "coyote"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


/**
 * test1:
 * 
 * This test will perform some checks around initializing coyote.
 * 
 **/
gboolean test1 (void)
{
	gint argc = 1;
	gchar *argv[] = {"test_coyote"};
	GError *error;
	gboolean result;



	gchar **argv2 = (gchar **) argv;

	result = coyote_init(&argc, &argv2, &error);


	return result;
}


TestNode test_suite []  = {{"coyote_init", test1}, {NULL, NULL}};

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

