#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <src/coyote_xml.h>
#include <src/coyote_dataset.h>

#define MODULE_TEST_NAME "coyote_xml"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


/**
 * test1:
 * 
 * This test will perform some checks around creating request service message.
 * 
 **/
gboolean test1 (void)
{
	CoyoteXmlObject      * object;
	CoyoteXmlMessage     * message;
	CoyoteDataSet * data_set = NULL;
		
	object = coyote_xml_new_object ();
	data_set = coyote_dataset_new ();


 	coyote_xml_add_request_service (object, "login", 
					"username", COYOTE_XML_ARG_STRING, "pepe", 
					"password", COYOTE_XML_ARG_STRING, "pp",
					NULL);

	coyote_xml_add_request_service (object, "give_me_a_present", 
					"minimum_cost", COYOTE_XML_ARG_STRING, "100000 $",
					NULL);
	

	coyote_dataset_add (data_set, "hola");
	coyote_dataset_add (data_set, "hola2");
	coyote_dataset_add (data_set, "hola3");

	coyote_xml_add_request_service (object, "permissions",
					"table1", COYOTE_XML_ARG_DATASET, data_set,
					"username", COYOTE_XML_ARG_STRING, "pepe", 
					"table2", COYOTE_XML_ARG_DATASET, data_set,
					NULL);
	
	message = coyote_xml_build_message (object);

	printf ("%s\n", message->content);

	if (message->len) {
		coyote_xml_destroy_message (message);
		return TRUE;
	}
	coyote_xml_destroy_message (message);
	coyote_dataset_free (data_set);

	return FALSE;
}

gboolean test2 (void) 
{
	CoyoteXmlObject * object;
	CoyoteXmlMessage * message;
	CoyoteDataSet * data_set = NULL;
	
	
	object = coyote_xml_new_object ();
	
	data_set = coyote_dataset_new ();

	coyote_dataset_add (data_set, "hola");
	coyote_dataset_add (data_set, "hola2");
	coyote_dataset_add (data_set, "hola3");

	coyote_xml_add_response_service (object, COYOTE_CODE_ERROR, "It can't be done", 
					 data_set, data_set, data_set, NULL);
	
	message = coyote_xml_build_message (object);
	
	if (message->len) {
		coyote_xml_destroy_message (message);
		return TRUE;
	}
		
	coyote_xml_destroy_message (message);

	coyote_dataset_free (data_set);
	return FALSE;
}

gboolean test3 (void)
{
	CoyoteXmlObject *object;
	CoyoteXmlMessage *message;
	CoyoteXmlServiceData * data;
	CoyoteXmlServiceNode * node;
	
	object = coyote_xml_new_object ();

 	coyote_xml_add_request_service (object, "login", 
					"username", COYOTE_XML_ARG_STRING, "pepe",
					"password", COYOTE_XML_ARG_STRING, "pp",
					NULL);

	coyote_xml_add_request_service (object, "give_me_a_present", 
					"minimum_cost", COYOTE_XML_ARG_STRING, "100000 $", 
					NULL);
	
	message = coyote_xml_build_message (object);

	data = coyote_xml_parse_message (message);
	if (!data) {
		coyote_xml_parse_destroy (data);
		return FALSE;
	}

	if ( ! (data->type == COYOTE_XML_SERVICE_REQUEST) ) 
		return FALSE;

	if ( g_strcasecmp (data->name, "login") )  
		return FALSE;
	
	
	if ( ! (data->item_list) ) 
		return FALSE;

	node = (CoyoteXmlServiceNode *) data->item_list->data;
	
	if ( g_strcasecmp (node->attrib, "username") ) 
		return FALSE;
	if ( g_strcasecmp (node->value, "pepe") ) 
		return FALSE;
	
	data->item_list = g_list_next (data->item_list);
	node = (CoyoteXmlServiceNode *) data->item_list->data;
	
	if ( g_strcasecmp (node->attrib, "password") ) 
		return FALSE;
	if ( g_strcasecmp (node->value, "pp" ) ) 
		return FALSE;

	coyote_xml_destroy_message (message);
	return TRUE;
}

gboolean test4 (void) 
{
	CoyoteXmlObject *object;
	CoyoteXmlMessage *message;
	CoyoteXmlServiceData * data;
	
	CoyoteDataSet * data_set = NULL;
	
	object = coyote_xml_new_object ();
	
	data_set = coyote_dataset_new ();

	coyote_dataset_add (data_set, "hola");
	coyote_dataset_add (data_set, "hola2");
	coyote_dataset_add (data_set, "hola3");

	coyote_xml_add_response_service (object, COYOTE_CODE_ERROR, "It can't be done", 
					 data_set, data_set, data_set, NULL);
	

	message = coyote_xml_build_message (object);

	coyote_dataset_free (data_set);
	data_set = NULL;


	data = coyote_xml_parse_message (message);
	if (!data) {
		coyote_xml_parse_destroy (data);
		return FALSE;
	}

	if ( ! (data->type == COYOTE_XML_SERVICE_RESPONSE) ) 
		return FALSE;

	if ( g_strcasecmp (data->status, "200") )
		return FALSE;
	
	if ( ! (data->item_list) ) 
		return FALSE;


	// Check for the first table which contains one row
	data_set = (CoyoteDataSet *) data->item_list->data;
	if (!data_set)
		return FALSE;


	if (!coyote_dataset_get (data_set, 0, 0)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 0), "hola"))
		return FALSE;

	if (!coyote_dataset_get (data_set, 0, 1)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 1), "hola2"))
		return FALSE;


	if (!coyote_dataset_get (data_set, 0, 2)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 2), "hola3"))
		return FALSE;
	
	// Check for the second table which contains one row
	data->item_list =  g_list_next (data->item_list);

	data_set = (CoyoteDataSet *) data->item_list->data;
	if (!data_set)
		return FALSE;

	if (!coyote_dataset_get (data_set, 0, 0)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 0), "hola"))
		return FALSE;

	
	if (!coyote_dataset_get (data_set, 0, 1)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 1), "hola2"))
		return FALSE;


	if (!coyote_dataset_get (data_set, 0, 2)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 2), "hola3"))
		return FALSE;


	// Check for the third row
	data->item_list = g_list_next (data->item_list);

	data_set = (CoyoteDataSet *) data->item_list->data;
	if (!data_set)
		return FALSE;
	

	if (!coyote_dataset_get (data_set, 0, 0)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 0), "hola"))
		return FALSE;

	
	if (!coyote_dataset_get (data_set, 0, 1)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 1), "hola2"))
		return FALSE;


	if (!coyote_dataset_get (data_set, 0, 2)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 2), "hola3"))
		return FALSE;

	coyote_xml_destroy_message (message);
	return TRUE;
}

gboolean test5 (void) 
{
	CoyoteXmlObject  * object;
	CoyoteXmlMessage * message;
	CoyoteDataSet    * data_set; 
	
	object = coyote_xml_new_object ();
	
	data_set = coyote_dataset_new ();

	coyote_dataset_add (data_set, "hola");
	coyote_dataset_add (data_set, "hola2");
	coyote_dataset_add (data_set, "hola3");	

	coyote_xml_add_response_service (object, COYOTE_CODE_ERROR, "It can't be done", 
					 data_set, data_set, data_set, NULL);
	
	message = coyote_xml_build_message (object);


	if (coyote_xml_validate_message (message)) {
		coyote_xml_destroy_message (message);
		coyote_xml_destroy_object (object);
		return TRUE;
	}
	
	coyote_xml_destroy_message (message);
	coyote_xml_destroy_object (object);
	return FALSE;
}

gboolean test6 (void) 
{
	CoyoteXmlObject *object;
	CoyoteXmlMessage *message;
	CoyoteXmlServiceData * data;
	CoyoteXmlServiceNode * node;
	
	CoyoteDataSet * data_set = NULL;
	
	object = coyote_xml_new_object ();
	
	data_set = coyote_dataset_new ();

	coyote_dataset_add (data_set, "hola");
	coyote_dataset_add (data_set, "hola2");
	coyote_dataset_add (data_set, "hola3");

	coyote_xml_add_request_service (object, "permission update",
					"table1", COYOTE_XML_ARG_DATASET, data_set,
					NULL);
	

	message = coyote_xml_build_message (object);

	coyote_dataset_free (data_set);
	data_set = NULL;


	data = coyote_xml_parse_message (message);
	if (!data) {
		coyote_xml_parse_destroy (data);
		return FALSE;
	}


	if ( ! (data->type == COYOTE_XML_SERVICE_REQUEST) ) 
		return FALSE;

	if ( g_strcasecmp (data->name, "permission update") )  
		return FALSE;

	if ( ! (data->item_list) ) 
		return FALSE;



	// Check for the table which contains one row
	node = (CoyoteXmlServiceNode *) data->item_list->data;
	data_set = (CoyoteDataSet *) node->value;

	if (!data_set)
		return FALSE;

	if (!coyote_dataset_get (data_set, 0, 0)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 0), "hola"))
		return FALSE;

	if (!coyote_dataset_get (data_set, 0, 1)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 1), "hola2"))
		return FALSE;


	if (!coyote_dataset_get (data_set, 0, 2)) 
		return FALSE;
	if ( g_strcasecmp (coyote_dataset_get (data_set, 0, 2), "hola3"))
		return FALSE;
	

	coyote_xml_destroy_message (message);
	return TRUE;
}

TestNode test_suite []  = {{"coyote_xml_add_request_service", test1}, 
			   {"coyote_xml_add_response_service", test2},
			   {"coyote_xml_add_request_service with strings correctness", test3},
			   {"coyote_xml_add_request_service with datasets correctness" ,test6},
			   {"coyote_xml_add_response_service correctness", test4},
			   {"coyote_xml_validate_message" ,test5},{NULL, NULL}};

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

