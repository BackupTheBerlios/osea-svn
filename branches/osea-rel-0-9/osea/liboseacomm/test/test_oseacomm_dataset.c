#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <src/oseacomm_dataset.h>

#define MODULE_TEST_NAME "oseacomm_dataset"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


/**
 * test1:
 * 
 * This test will create a OseaCommDataSet and then free it.
 * 
 **/
gboolean test1 (void)
{
	OseaCommDataSet * data_set;

	data_set = oseacomm_dataset_new ();
	
	if (!data_set) {
		printf ("Error: unable to create a dataset type\n");
		return FALSE;
	}
	

	return TRUE;
}

/**
 * test2:
 * 
 * This test will check oseacomm_dataset_free function
 * 
 **/
gboolean test2 (void)
{
	OseaCommDataSet * data_set;

	data_set = oseacomm_dataset_new ();
	

	oseacomm_dataset_add (data_set, "prueba");
	oseacomm_dataset_add (data_set, "caca");
	oseacomm_dataset_add (data_set, "pedo");

	oseacomm_dataset_new_row (data_set);
	
	oseacomm_dataset_add (data_set, "prueba2");
	oseacomm_dataset_add (data_set, "caca2");
	oseacomm_dataset_add (data_set, "pedo2");

	oseacomm_dataset_new_row (data_set);

	oseacomm_dataset_add (data_set, "prueba3");
	oseacomm_dataset_add (data_set, "caca3");
	oseacomm_dataset_add (data_set, "pedo3");

	oseacomm_dataset_new_row (data_set);

	oseacomm_dataset_free (data_set);

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
	OseaCommDataSet * data_set;
	gint i;
	gint j;
	gchar * element = "hello";
	
	data_set = oseacomm_dataset_new ();
	
	for (i = 0; i < 3; i++) {

		for (j = 0; j < 3; j++) {
			oseacomm_dataset_add (data_set, element);
		}
		oseacomm_dataset_new_row (data_set);
	}

	for (i = 0; i < oseacomm_dataset_get_height (data_set); i++) {
		for (j = 0; j < oseacomm_dataset_get_width (data_set); j++) {
			
			if (oseacomm_dataset_get (data_set, i,j))
				if (g_strncasecmp ("hello", oseacomm_dataset_get (data_set, i, j), 5) != 0) 
					return FALSE;
		}
	}

	oseacomm_dataset_free (data_set);
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
	OseaCommDataSet * data_set;
	gint i;
	gint j;
	gchar * element = "hello";
	

	data_set = oseacomm_dataset_new ();
	
	for (i = 0; i < 3; i++) {

		for (j = 0; j < 3; j++) {
			oseacomm_dataset_add (data_set, element);
		}
		if (!(i == 2))
			oseacomm_dataset_new_row (data_set);
	}


	if (oseacomm_dataset_get_height (data_set) != 3)
		return FALSE;
	if (oseacomm_dataset_get_width (data_set) != 3)
		return FALSE;

	oseacomm_dataset_free (data_set);
	return TRUE;
}

/**
 * test5:
 * 
 * This test will check oseacomm_dataset_add_row function
 * 
 **/
gboolean test5 (void)
{
	OseaCommDataSet * data_set;

	data_set = oseacomm_dataset_new ();
	

	// Create the firt row
	oseacomm_dataset_add_nth (data_set, "prueba", "caca", "pedo", NULL);

	oseacomm_dataset_new_row (data_set);

	// Create the second row
	oseacomm_dataset_add_nth (data_set, "prueba2", "caca2", "pedo2", NULL);
	
	oseacomm_dataset_new_row (data_set);

	// Create the third and final row. Notice that there is no
	// oseacomm_dataset_new row call because we want a 3x3 table.
	oseacomm_dataset_add_nth (data_set, "prueba3", "caca3", "pedo3", NULL);

	oseacomm_dataset_free (data_set);

	return TRUE;
}

/**
 * test6:
 * 
 * This test will check oseacomm_dataset_lookup_row function
 * 
 **/
gboolean test6 (void)
{
	OseaCommDataSet * data_set;

	data_set = oseacomm_dataset_new ();
	

	// Create the firt row
	oseacomm_dataset_add_nth (data_set, "prueba", "caca", "pedo", NULL);

	oseacomm_dataset_new_row (data_set);

	// Create the second row
	oseacomm_dataset_add_nth (data_set, "prueba2", "caca2", "pedo2", NULL);
	
	oseacomm_dataset_new_row (data_set);

	// Create the third and final row. Notice that there is no
	// oseacomm_dataset_new row call because we want a 3x3 table.
	oseacomm_dataset_add_nth (data_set, "prueba3", "caca3", "pedo3", NULL);


	if (oseacomm_dataset_lookup_row (data_set, 0, "prueba") != 0) {
		printf ("return %d\n", oseacomm_dataset_lookup_row (data_set, 0, "prueba"));
		return FALSE;
	}
	if (oseacomm_dataset_lookup_row (data_set, 1, "caca") != 0)
		return FALSE;
	if (oseacomm_dataset_lookup_row (data_set, 2,"pedo") != 0)
		return FALSE;

	if (oseacomm_dataset_lookup_row (data_set, 0,"prueba2") != 1)
		return FALSE;
	if (oseacomm_dataset_lookup_row (data_set, 1,"caca2") != 1)
		return FALSE;
	if (oseacomm_dataset_lookup_row (data_set, 2, "pedo2") != 1)
		return FALSE;

	if (oseacomm_dataset_lookup_row (data_set, 0, "prueba3") != 2)
		return FALSE;
	if (oseacomm_dataset_lookup_row (data_set, 1, "caca3") != 2)
		return FALSE;
	if (oseacomm_dataset_lookup_row (data_set, 2, "pedo3") != 2)
		return FALSE;

	
	oseacomm_dataset_free (data_set);

	return TRUE;
}


TestNode test_suite []  = {{"oseacomm_dataset_new", test1}, 
			   {"oseacomm_dataset_destroy", test2}, 
			   {"oseacomm_dataset_add/new_row/get", test3}, 
			   {"oseacomm_dataset_get_width/get_height", test4}, 
			   {"oseacomm_dataset_add_nth", test5},
			   {"oseacomm_dataset_lookup_row", test6},
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

