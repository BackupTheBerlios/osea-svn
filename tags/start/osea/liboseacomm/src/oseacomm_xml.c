//  LibCoyote:  Support library with xml and communication functions.
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or   
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <stdarg.h>
#include "oseacomm_xml.h"
#include "oseacomm_dataset.h"

#define LOG_DOMAIN "COYOTE_XML"

typedef struct _CoyoteAttrib CoyoteAttrib;

struct _CoyoteAttrib {
	gchar *attrib_name;
	gchar *attrib_value;
};

struct _CoyoteXmlObject {
	xmlDocPtr doc;
};



xmlNodePtr __oseacomm_xml_create_table (CoyoteDataSet * data_set, xmlDocPtr doc)
{
	gint i;
	gint j;
	xmlNodePtr result;
	xmlNodePtr cursor;
	xmlNodePtr cursor_aux;
	
	
	g_return_val_if_fail (data_set, NULL);

	// Create the initial node table
	result = xmlNewDocRawNode (doc, NULL, "table", NULL);
	cursor = result;

	for (i = 0; i < oseacomm_dataset_get_height (data_set); i++) {

		// Create a row
		if (i == 0) {
			// if first param is set, the following node must be inserted as a child
			cursor->children = xmlNewDocRawNode (doc, NULL, "row", NULL);		
			cursor = cursor->children;
		}else {
			cursor->next = xmlNewDocRawNode (doc, NULL, "row", NULL);
			cursor = cursor->next;
		}

		
		cursor_aux = cursor;
		for (j = 0; j < oseacomm_dataset_get_width (data_set); j++) {
			// insert each param
			if (j == 0) {
				// if is the first param, this must be inserted as a child
				cursor_aux->children = xmlNewDocRawNode (doc, NULL, "data", 
									 (xmlChar *)oseacomm_dataset_get (data_set, i,j));
				cursor_aux = cursor_aux->children;
			} else {
				// in other case, the insertion is made as a siblin
				cursor_aux->next = xmlNewDocRawNode (doc, NULL, "data", 
								     (xmlChar *)oseacomm_dataset_get (data_set, i,j));
				cursor_aux = cursor_aux->next;
			}
		}


	}
	
	return result;
}



xmlNodePtr __oseacomm_xml_insert_arg (xmlDocPtr doc, gchar *attrib, CoyoteXmlArgType type, gpointer value)
{
	xmlNodePtr node = NULL;

	switch (type) {
	case COYOTE_XML_ARG_STRING:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, " param: %s, %s", attrib, (gchar *)value);
		node = xmlNewDocRawNode (doc, NULL, "param", NULL);
		xmlSetProp (node, "attrib", (xmlChar *)attrib);
		xmlSetProp (node, "value", (xmlChar *)value);
		break;
	case COYOTE_XML_ARG_DATASET:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, " paramtable: %s, DATASET", attrib);
		node = xmlNewDocRawNode (doc, NULL, "paramtable", NULL);
		xmlSetProp (node, "attrib", (xmlChar *)attrib);
		node->children = __oseacomm_xml_create_table ((CoyoteDataSet *)value, doc);
		break;
	}
	
	return node;
}


CoyoteXmlServiceData *  __oseacomm_xml_parse_service_request (xmlNodePtr cursor, 
							    xmlDocPtr document, 
							    CoyoteXmlServiceData * object )
{
	xmlChar               * string;
	GString               * row_log;
	CoyoteXmlServiceNode  * node;
	xmlNodePtr              cursor_table;
	xmlNodePtr              cursor_row;
	xmlNodePtr              cursor_data;
	CoyoteDataSet         * data_set;
	gint                    i, j;
	
	object->type = COYOTE_XML_SERVICE_REQUEST;
	
	// For each request service node
	for (; cursor; cursor = cursor->next) {
		// Create a node for containing the information
		node = g_new0 (CoyoteXmlServiceNode, 1);

		// Set the request service name.
		node->attrib = g_strdup ((gchar *) xmlGetProp (cursor, "attrib"));
		
		// Check for param or paramtable
		if (!xmlStrcmp (cursor->name, (const xmlChar *) "param")) {		

			// We are at a param
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Parsing a param.");
			node->type = COYOTE_XML_ARG_STRING;
			node->value =  g_strdup ((gchar *) xmlGetProp (cursor, "value"));

		} else {

			// We are at a paramtable	
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Parsing a paramtable.");
			node->type = COYOTE_XML_ARG_DATASET;

			// Set cursor_table to the table element (there is only one)
			cursor_table = cursor->children;
			data_set = oseacomm_dataset_new ();
			
			// Set cursor_row to the fist row of the table
			cursor_row = cursor_table->children;

			// For each row element 
			for (; cursor_row; cursor_row = cursor_row->next) {
				
				// Set cursor_data to the fist data of the row
				cursor_data = cursor_row->children;
				
				// For each data element
				for (; cursor_data; cursor_data = cursor_data->next) {
					// Get the information of the table position
					string = xmlNodeListGetString (document, cursor_data->children, 1);
					oseacomm_dataset_add (data_set, (gchar *) string);
					g_free (string);
				}

				// Ask for last row. If not, add a new row
				if (cursor_row->next)
					oseacomm_dataset_new_row (data_set);
			}
			// I've got the dataset yet

			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Receiving dataset:");
			
			for (i = 0; i < oseacomm_dataset_get_height (data_set); i++) {
				
				row_log = g_string_new (NULL);	       
				
				for (j = 0; j < oseacomm_dataset_get_width (data_set); j++)
					g_string_sprintfa (row_log, ":%s", oseacomm_dataset_get (data_set, i, j));
				
				g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Row: %s", row_log->str);
				
				g_string_free (row_log, TRUE);
			}
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "End of dataset.");
			node->value = data_set;
		}
		
		// Insert the node at the result list
		
		object->item_list = g_list_append (object->item_list, (gpointer) node);
	}
	return object;
}


CoyoteXmlServiceData *  __oseacomm_xml_parse_service_response (xmlNodePtr cursor, 
							    xmlDocPtr document, 
							    CoyoteXmlServiceData * object )
{
	xmlNodePtr              cursor_aux;
	xmlNodePtr              cursor_aux2;
	xmlChar               * string;
	xmlChar               * string2;
	CoyoteDataSet         * data_set;
	gint                    i, j;
	GString               * string_aux;
	
	object->type = COYOTE_XML_SERVICE_RESPONSE;

	// The first element is the status
	string = xmlGetProp (cursor, "code");
	string2 = xmlGetProp (cursor, "text");
	
	object->status = (gchar *) string;
	object->status_message = (gchar *) string2;
	object->item_list = NULL;

	// Set cursor to the first row element
	cursor = cursor->next;
	
	// For each table elment 
	for (; cursor; cursor = cursor->next) {


		data_set = oseacomm_dataset_new ();

		cursor_aux = cursor->children;
		// For each row element 

		for (; cursor_aux; cursor_aux = cursor_aux->next) {

			cursor_aux2 = cursor_aux->children;

			// For each data element
			for (; cursor_aux2; cursor_aux2 = cursor_aux2->next) {
				
				// Get the request service name.
				string = xmlNodeListGetString (document, cursor_aux2->children, 1);
				
				oseacomm_dataset_add (data_set, (gchar *) string);
				
				g_free (string);

			}
			

			// Ask for last row. If not, add a new row
			if (cursor_aux->next)
				oseacomm_dataset_new_row (data_set);
			
		}

		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Receiving dataset:");

		for (i = 0; i < oseacomm_dataset_get_height (data_set); i++) {
			
			string_aux = g_string_new (NULL);	       

			for (j = 0; j < oseacomm_dataset_get_width (data_set); j++)
				g_string_sprintfa (string_aux, ":%s", oseacomm_dataset_get (data_set, i, j));
			
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Row: %s", string_aux->str);
			
			g_string_free (string_aux, TRUE);
		}
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "End of dataset.");

		object->item_list = g_list_append (object->item_list, data_set);
	}
	return object;
}




xmlNodePtr __oseacomm_xml_create_row (GList * row_data, xmlDocPtr doc)
{
	gboolean first_param = TRUE;
	GList * list_aux;
	xmlNodePtr result;
	xmlNodePtr cursor;
	
	
	g_return_val_if_fail (row_data, NULL);

	// Create the initial node
	result = xmlNewDocRawNode (doc, NULL, "row", NULL);

	cursor = result;

	for (list_aux = g_list_first (row_data); list_aux; list_aux = g_list_next (list_aux)) {
		
		// insert each param
		if (first_param) {
			// if is the first param, this must be inserted as a child
			cursor->children = xmlNewDocRawNode (doc, NULL, "data", (xmlChar *)list_aux->data);
			cursor = cursor->children;
			first_param = FALSE;
		} else {
			// in other case, the insertion is made as a siblin
			cursor->next = xmlNewDocRawNode (doc, NULL, "data", (xmlChar *)list_aux->data);
			cursor = cursor->next;
		}
	}
	
	return result;
}


void __oseacomm_xml_dataset_destroy (gpointer data, gpointer usr_data)
{
	oseacomm_dataset_free ((CoyoteDataSet *) data);
}



/**
 * oseacomm_xml_new_object:
 * 
 * Creates a new xml object that will be filled with the appropiate data  (opaque type).
 * 
 * Return value: A new xml object used to build the message containing all xml information.
**/

CoyoteXmlObject * oseacomm_xml_new_object (void)
{
	CoyoteXmlObject *object;

	object = g_new (CoyoteXmlObject, 1);
	object -> doc = xmlNewDoc ("1.0");
	
	object->doc->children = xmlNewDocRawNode (object->doc, NULL, "general_message", NULL);
	object->doc->children->children = xmlNewDocRawNode (object->doc, NULL, "protocol_version", "1.0");

	return object;
}


/**
 * oseacomm_xml_destroy_object:
 * @object: object to be freed
 * 
 * Free up the structure CoyoteXmlObject and all the content inside it.
 **/
void oseacomm_xml_destroy_object (CoyoteXmlObject * object)
{
	xmlFreeDoc (object->doc);
	g_free (object);
}



/**
 * oseacomm_xml_add_request_service:
 * @object: the object to add the request service
 * @name_of_service: the name of the service requested
 * @Varargs: a list of strings corresponding to the params, ended by
 * NULL.  It should look like ("attrib1", STRING|DATASET, "value1", "attrib2",
 * STRING|DATASET, "value2", ...., NULL) 
 * 
 * Attach to the object a service request, filling it with the given params.
 **/
void oseacomm_xml_add_request_service  (CoyoteXmlObject * object, gchar * name_of_service, ...)
{
	va_list args;

	va_start (args, name_of_service);

	oseacomm_xml_add_vrequest_service (object, name_of_service, args);

	va_end (args);
	
	return;
}


void oseacomm_xml_add_vrequest_service (CoyoteXmlObject *object, gchar *name_of_service, va_list args)
{
	xmlNodePtr cursor;
	gboolean first_param = TRUE;
	gchar *attrib = NULL;
	CoyoteXmlArgType type;
	gpointer value = NULL;
	
	g_return_if_fail (object);
	g_return_if_fail (name_of_service);
	
	// set the cursor to the right position
	cursor = object->doc->children->children;
	while (cursor->next) {
		cursor = cursor->next;
	}
	
	// insert the node of the service request
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "New request service: %s", name_of_service);
	cursor->next = xmlNewDocRawNode (object->doc, NULL, "request_service", NULL);
	xmlSetProp (cursor->next, "name", (xmlChar *)name_of_service);
	
 	// prepare for insert params
	cursor = cursor->next;
	
	// parse the unknown-number-of params
	while ((attrib = va_arg (args, gchar *))) {
		type = va_arg (args, CoyoteXmlArgType);

		switch (type) {
		case COYOTE_XML_ARG_STRING:
			value = va_arg (args, gchar *);
			break;
		case COYOTE_XML_ARG_DATASET:
			value = va_arg (args, CoyoteDataSet *);
			break;
		}
		
		if (first_param) {
			// if this is the first, this must be inserted as a child
			cursor->children = __oseacomm_xml_insert_arg (object->doc, attrib, type, value);
			cursor = cursor->children;
			first_param = FALSE;
		} else {
			// in other case, the insertion is made as a siblin
			cursor->next = __oseacomm_xml_insert_arg (object->doc, attrib, type, value);
			cursor = cursor->next;
		}
	}
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Exiting from oseacomm_xml_add_vrequest");
	return;
}



/**
 * oseacomm_xml_validate_message:
 * @message: 
 * 
 * Validates the message against the main dtd of LibCoyote.
 * 
 * Return value: TRUE if the validation was succesful, FALSE if not.
 **/
gboolean oseacomm_xml_validate_message (CoyoteXmlMessage * message)
{

	xmlDtdPtr dtd;
	gchar *dtd_file_name = NULL;
	xmlValidCtxtPtr context;
	xmlDocPtr doc;

	g_return_val_if_fail (message, FALSE);

	dtd_file_name = g_build_filename (PACKAGE_DTD_DIR, "liboseacomm", "fact.dtd", NULL );
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Using dtd file: %s", dtd_file_name);
	if (!dtd_file_name) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Cannot find the dtd file to validate messages");
		g_free (dtd_file_name);
		return FALSE;
	}

	dtd = xmlParseDTD (NULL, dtd_file_name);

	g_free (dtd_file_name);

	if (!dtd) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Cannot parse the dtd file");
		return FALSE;
	}

	// dtd correct

	doc = xmlParseMemory ((xmlChar *) message->content, (message->len * sizeof (char)));
	if (!doc) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Cannot parse xml message to validate it");
		xmlFreeDtd (dtd);
		return FALSE;
	}

	// doc correct

	context = g_new0 (xmlValidCtxt, 1);

	if (xmlValidateDtd (context, doc, dtd)) {
		// Validation succesful
		xmlFreeDtd (dtd);
		xmlFreeDoc (doc);
		g_free (context);
		return TRUE;
	} 
	
	// Validation failed
	xmlFreeDtd (dtd);
	xmlFreeDoc (doc);
	g_free (context);
	return FALSE;
	

}



/**
 * oseacomm_xml_parse_message:
 * @message: raw xml data.
 * 
 * This function returns a structure that represents a service request or a
 * service response.  This function expects a correct xml message so
 * it would be a good idea to call oseacomm_xml_validate_message before
 * calling this one.  
 * Moreover, this function parses a xml message
 * containing only one request or response service.
 *
 * In the other hand, each field contained in a CoyoteXmlServiceData
 * is explained: 
 *
 * "name" : this field represents the service request which is been
 * answered to. 
 *
 * "protocol_version" : message's protocol version.
 *
 * "item_list" : a list where each element is a CoyoteDataSet
 * elment. See oseacomm_dataset module for more detail.
 *
 * Return value: a new CoyoteXmlServiceData structure or NULL if something fails.
 **/
CoyoteXmlServiceData * oseacomm_xml_parse_message (CoyoteXmlMessage *message)
{

	xmlDocPtr document;
	xmlNodePtr cursor;
	xmlChar * string;
	CoyoteXmlServiceData * result;
	
	
	g_return_val_if_fail (message, NULL);

	// Create the result object
	result = g_new0 (CoyoteXmlServiceData, 1);

	// Load the xml document
	document = xmlParseMemory ((xmlChar *) message->content, (message->len * sizeof (char)));
	if (!document) {
		g_printerr ("Unable to parse the given document\n");
		g_free (result);
		return NULL;
	}

	// Get the root element
	cursor = xmlDocGetRootElement (document);
	
	// Get the position of the protocol_version element
	cursor = cursor->children;
	string = xmlNodeListGetString (document, cursor->children, 1);
	result->protocol_version = g_strdup ((gchar *) string);

	// Get the firts element from the xml message.
	cursor = cursor->next;
	
	// Get the node content
	if (!xmlStrcmp (cursor->name, (const xmlChar *) "response_service")) {		
		// We are at response_service case
		result->name = NULL;
		return __oseacomm_xml_parse_service_response (cursor->children, document, result);
	}
        // We are at request_service case	

	string = xmlGetProp (cursor, "name");
	result->name = g_strdup ((gchar *) string);
	g_free (string);

        return __oseacomm_xml_parse_service_request (cursor->children, document, result);
}



/**
 * oseacomm_xml_build_message:
 * @object: the object to convert 
 * 
 * Builds a message from @object, returning a string with the generated xml message.
 * 
 * Return value: xml message to be sent
 **/
CoyoteXmlMessage * oseacomm_xml_build_message (CoyoteXmlObject *object)
{
	CoyoteXmlMessage *message;
	g_return_val_if_fail (object, FALSE);
	g_return_val_if_fail (object->doc, FALSE);

	message = g_new (CoyoteXmlMessage, 1);
	xmlDocDumpMemory (object->doc, (xmlChar **) &(message->content), &(message->len));

	return message;
}


/**
 * oseacomm_xml_destroy_message:
 * @message: message to be freed
 * 
 * Free up the message. The char pointer called 'content' is not freed and must be freed later.
 **/
void oseacomm_xml_destroy_message (CoyoteXmlMessage *message )
{
	g_free (message);
}



/**
 * oseacomm_xml_add_response_service:
 * @object: 
 * @status_code: 
 * @explanation: 
 * @Varargs: CoyoteDataSet list
 *
 * This function adds a response service named by @name_of_service to
 * the @object. @status_code and @explanation vars are the state
 * result from a previous request service.  @Varags is a list of list
 * of strings, so a call to this function should look like
 * ..,explanation, CoyoteDataSet, CoyoteDataSet, NULL) where each
 * CoyoteDateSet contains a single table. This tables will be sent as
 * logically separeted set of data.
 * 
 **/
void oseacomm_xml_add_response_service (CoyoteXmlObject * object,
				      CoyoteCodeType status_code,
				      gchar * explanation,
				      ...)
{
	va_list args;

	va_start (args, explanation);

	oseacomm_xml_add_vresponse_service (object, status_code, explanation, args);

	va_end (args);
	
	return;
}

void oseacomm_xml_add_vresponse_service (CoyoteXmlObject * object,
				       CoyoteCodeType status_code,
				       gchar * explanation,
				       va_list args)
{
	xmlNodePtr cursor;
	CoyoteDataSet * table = NULL;
	CoyoteCode *code_information = NULL;

	// Check for incorrect params.
	g_return_if_fail (object);
	g_return_if_fail (explanation);

	// set the cursor to the right position
	cursor = object->doc->children->children;
	while (cursor->next) {
		cursor = cursor->next;
	}

	// insert the node of the service response
	cursor->next = xmlNewDocRawNode (object->doc, NULL, "response_service", NULL);

	// prepare for insert data, access to myself
	cursor = cursor->next;

	// Insert the response status 
	cursor->children = xmlNewDocRawNode (object->doc, NULL, "status", NULL);
	
	// Create the correct code information
	code_information = oseacomm_code_new (status_code, (const gchar *) explanation);

	xmlSetProp (cursor->children, "code", (xmlChar *) code_information->status_code);
	xmlSetProp (cursor->children, "text", (xmlChar *) code_information->status_text);

	oseacomm_code_free (code_information);

	cursor = cursor->children;

	// parse the unknown-number-of params
		
	// For each recieved table
	while ( (table = va_arg (args, CoyoteDataSet *))) {
		
		cursor->next = __oseacomm_xml_create_table (table, object->doc);
		cursor = cursor->next;
	}

	return;
}



/**
 * oseacomm_xml_parse_destroy:
 * @data: data to be freed
 * 
 * Function to return the memory allocated by @data object. 
 *
 **/
void oseacomm_xml_parse_destroy (CoyoteXmlServiceData * data)
{
	g_free (data->name);
	g_free (data->protocol_version);
	g_free (data->status);
	g_free (data->status_message);
	g_list_foreach (data->item_list, __oseacomm_xml_dataset_destroy, NULL);

	g_free (data);
}
