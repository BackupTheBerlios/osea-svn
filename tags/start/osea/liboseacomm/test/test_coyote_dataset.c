#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <src/coyote_dataset.h>

#define MODULE_TEST_NAME "coyote_dataset"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


/**
 * test1:
 * 
 * This test will create a CoyoteDataSet and then free it.
 * 
 **/
gboolean test1 (void)
{
	CoyoteDataSet * data_set;

	data_set = coyote_dataset_new ();
	
	if (!data_set) {
		printf ("Error: unable to create a dataset type\n");
		return FALSE;
	}
	

	return TRUE;
}

/**
 * test2:
 * 
 * This test will check coyote_dataset_free function
 * 
 **/
gboolean test2 (void)
{
	CoyoteDataSet * data_set;

	data_set = coyote_dataset_new ();
	

	coyote_dataset_add (data_set, "prueba");
	coyote_dataset_add (data_set, "caca");
	coyote_dataset_add (data_set, "pedo");

	coyote_dataset_new_row (data_set);
	
	coyote_dataset_add (data_set, "prueba2");
	coyote_dataset_add (data_set, "caca2");
	coyote_dataset_add (data_set, "pedo2");

	coyote_dataset_new_row (data_set);

	coyote_dataset_add (data_set, "prueba3");
	coyote_dataset_add (data_set, "caca3");
	coyote_dataset_add (data_set, "pedo3");

	coyote_dataset_new_row (data_set);

	coyote_dataset_free (data_set);

	return TRUE;
}

/**
 * test3:
 * 
 * This test will add several elements and then this elements will be
 * obtained thought the interface.
 * 
 **/
gboolean test3 (void)
{
	CoyoteDataSet * data_set;
	gint i;
	gint j;
	gchar * element = "hello";
	
	data_set = coyote_dataset_new ();
	
	for (i = 0; i < 3; i++) {

		for (j = 0; j < 3; j++) {
			coyote_dataset_add (data_set, element);
		}
		coyote_dataset_new_row (data_set);
	}

	for (i = 0; i < coyote_dataset_get_height (data_set); i++) {
		for (j = 0; j < coyote_dataset_get_width (data_set); j++) {
			
			if (coyote_dataset_get (data_set, i,j))
				if (g_strncasecmp ("hello", coyote_dataset_get (data_set, i, j), 5) != 0) 
					return FALSE;
		}
	}

	coyote_dataset_free (data_set);
	return TRUE;


}

/**
 * test4:
 * 
 * This test will check get_height and get_width functions
 * 
 **/
gboolean test4 (void)
{
	CoyoteDataSet * data_set;
	gint i;
	gint j;
	gchar * element = "hello";
	

	data_set = coyote_dataset_new ();
	
	for (i = 0; i < 3; i++) {

		for (j = 0; j < 3; j++) {
			coyote_dataset_add (data_set, element);
		}
		if (!(i == 2))
			coyote_dataset_new_row (data_set);
	}


	if (coyote_dataset_get_height (data_set) != 3)
		return FALSE;
	if (coyote_dataset_get_width (data_set) != 3)
		return FALSE;

	coyote_dataset_free (data_set);
	return TRUE;
}

/**
 * test5:
 * 
 * This test will check coyote_dataset_add_row function
 * 
 **/
gboolean test5 (void)
{
	CoyoteDataSet * data_set;

	data_set = coyote_dataset_new ();
	

	// Create the firt row
	coyote_dataset_add_nth (data_set, "prueba", "caca", "pedo", NULL);

	coyote_dataset_new_row (data_set);

	// Create the second row
	coyote_dataset_add_nth (data_set, "prueba2", "caca2", "pedo2", NULL);
	
	coyote_dataset_new_row (data_set);

	// Create the third and final row. Notice that there is no
	// coyote_dataset_new row call because we want a 3x3 table.
	coyote_dataset_add_nth (data_set, "prueba3", "caca3", "pedo3", NULL);

	coyote_dataset_free (data_set);

	return TRUE;
}

/**
 * test6:
 * 
 * This test will check coyote_dataset_lookup_row function
 * 
 **/
gboolean test6 (void)
{
	CoyoteDataSet * data_set;

	data_set = coyote_dataset_new ();
	

	// Create the firt row
	coyote_dataset_add_nth (data_set, "prueba", "caca", "pedo", NULL);

	coyote_dataset_new_row (data_set);

	// Create the second row
	coyote_dataset_add_nth (data_set, "prueba2", "caca2", "pedo2", NULL);
	
	coyote_dataset_new_row (data_set);

	// Create the third and final row. Notice that there is no
	// coyote_dataset_new row call because we want a 3x3 table.
	coyote_dataset_add_nth (data_set, "prueba3", "caca3", "pedo3", NULL);


	if (coyote_dataset_lookup_row (data_set, 0, "prueba") != 0) {
		printf ("return %d\n", coyote_dataset_lookup_row (data_set, 0, "prueba"));
		return FALSE;
	}
	if (coyote_dataset_lookup_row (data_set, 1, "caca") != 0)
		return FALSE;
	if (coyote_dataset_lookup_row (data_set, 2,"pedo") != 0)
		return FALSE;

	if (coyote_dataset_lookup_row (data_set, 0,"prueba2") != 1)
		return FALSE;
	if (coyote_dataset_lookup_row (data_set, 1,"caca2") != 1)
		return FALSE;
	if (coyote_dataset_lookup_row (data_set, 2, "pedo2") != 1)
		return FALSE;

	if (coyote_dataset_lookup_row (data_set, 0, "prueba3") != 2)
		return FALSE;
	if (coyote_dataset_lookup_row (data_set, 1, "caca3") != 2)
		return FALSE;
	if (coyote_dataset_lookup_row (data_set, 2, "pedo3") != 2)
		return FALSE;

	
	coyote_dataset_free (data_set);

	return TRUE;
}


TestNode test_suite []  = {{"coyote_dataset_new", test1}, 
			   {"coyote_dataset_destroy", test2}, 
			   {"coyote_dataset_add/new_row/get", test3}, 
			   {"coyote_dataset_get_width/get_height", test4}, 
			   {"coyote_dataset_add_nth", test5},
			   {"coyote_dataset_lookup_row", test6},
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

