//  Af-Tax server: 
//  Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.
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


#include <glib.h>
#include <oseacomm/coyote.h>
#include <oseaserver/afgs.h>
#include "os_tax_vat_item.h"


#define LOG_DOMAIN "os_tax_vat_item"


gboolean os_tax_vat_item_list (OseaCommXmlServiceData *data, 
			       gpointer user_data, 
			       RRChannel * channel, 
			       gint msg_no)
{
	GList         * values;
	OseaCommDataSet * response;
	gint            initial, row_number;
	gchar         * aux;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial", "row_number", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Convert initial and row_number to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for initial parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for row_number parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	// Check out request information
	if (row_number == 0)
		response = oseaserver_command_execute_single_query ("SELECT * FROM tax_vat_item_type WHERE id >= %d", 
							      initial);
	else
		response = oseaserver_command_execute_single_query ("SELECT * FROM tax_vat_item_type WHERE id >= %d LIMIT %d", 
							      initial, row_number);
	
	
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available users", OSEACOMM_CODE_OK, response, NULL);

	return TRUE;	

}

gboolean os_tax_vat_item_new (OseaCommXmlServiceData *data, 
				  gpointer user_data, 
				  RRChannel * channel, 
				  gint msg_no)
{
	GList          * values;
	OseaCommDataSet  * customer_type_id;
	gboolean         result = FALSE;
	

	// Check for correct params
	values = oseaserver_message_check_params (data, "reference", "name", "description", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Create the new vat customer type
	result = oseaserver_command_execute_non_query ("INSERT INTO tax_vat_item_type (reference, name, description) \
                                                 VALUES ('%s','%s','%s')",
						g_list_nth_data (values, 0),
						g_list_nth_data (values, 1),
						g_list_nth_data (values, 2));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to create new vat item type",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}	

	// Check out the newly created vat customer type identifier
	customer_type_id = oseaserver_command_execute_single_query ("SELECT id from tax_vat_item_type WHERE reference = '%s'",
						     g_list_nth_data (values,  0));
	
	// For the new inserted customer, we need to define its  relation with all exiting item_type
	// in order to make life easy at the vat_value_list service. This behaivour will be also followed 
	// in new_item_type service. 
	result = oseaserver_command_execute_non_query ("INSERT INTO tax_vat_values (customer_type_id, item_type_id, value) \
                                                  ((SELECT id, %s, 0 FROM tax_vat_customer_type))",
						 oseacomm_dataset_get (customer_type_id, 0, 0));
	
	
	oseaserver_message_ok_answer (channel, msg_no, "Vat item type created", 
				OSEACOMM_CODE_OK, customer_type_id, NULL);

	return TRUE;
	
}

gboolean os_tax_vat_item_edit (OseaCommXmlServiceData *data, 
				   gpointer user_data, 
				   RRChannel * channel, 
				   gint msg_no)
{
	GList    * values;
	gboolean   result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "id", "new_reference", "new_name", "new_description", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Edit vat item type
	result = oseaserver_command_execute_non_query ("UPDATE tax_vat_item_type \
                                                  SET reference = '%s', name = '%s', description = '%s' \
                                                  WHERE id = '%s'",
						 g_list_nth_data (values, 1),
						 g_list_nth_data (values, 2),
						 g_list_nth_data (values, 3),
						 g_list_nth_data (values, 0));
	
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to update item type data",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	oseaserver_message_ok_answer (channel, msg_no, "Item type updated", OSEACOMM_CODE_OK, NULL);
	
	return TRUE;
}

gboolean os_tax_vat_item_remove (OseaCommXmlServiceData *data, 
				 gpointer user_data, 
				 RRChannel * channel, 
				 gint msg_no)
{
	GList     * values;
	gboolean    result;

	// Check for correct params
	values = oseaserver_message_check_params (data, "id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	result = oseaserver_command_execute_non_query ("DELETE FROM tax_vat_item_type WHERE id = '%s';",
						 g_list_nth_data (values,  0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to delete item type",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	oseaserver_message_ok_answer (channel, msg_no, "User item type", OSEACOMM_CODE_OK, NULL);
	
	return TRUE;

}
