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
#include "os_tax_vat_value.h"


#define LOG_DOMAIN "os_tax_vat_value"


/**
 * os_tax_vat_value_get:
 * @data: customer_id, item_id
 * @user_data: 
 * @channel: 
 * @msg_no: 
 * 
 * Get de tax value for a customer type and an item type. 
 * 
 * Return value: OK and value, or ERROR.
 **/
gboolean os_tax_vat_value_get (OseaCommXmlServiceData *data, gpointer user_data, 
			       RRChannel * channel, gint msg_no)
{
	
	GList          * values;
	OseaCommDataSet  * value_dataset;

	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_type_id", "item_type_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	value_dataset = oseaserver_command_execute_single_query ("SELECT value FROM tax_vat_value_type \
                                                            WHERE customer_type_id = '%s', item_type_id = '%s';",
							   g_list_nth_data (values, 0),
							   g_list_nth_data (values, 1));
	
	if (!value_dataset) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to obtain vat value",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	oseaserver_message_ok_answer (channel, msg_no, "Vat value", OSEACOMM_CODE_OK, value_dataset, NULL);
	return TRUE;
}

/**
 * os_tax_vat_value_set:
 * @data: cutomer_id, item_id, value
 * @user_data: 
 * @channel: 
 * @msg_no: 
 * 
 * Set de value for a customer type and an item type
 * 
 * Return value: 
 **/
gboolean os_tax_vat_value_set (OseaCommXmlServiceData *data, 
			       gpointer user_data, 
			       RRChannel * channel, 
			       gint msg_no)
{
	GList    * values;
	gboolean   result = FALSE;

	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_type_id", "item_type_id", "value", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	result = oseaserver_command_execute_non_query ("UPDATE tax_vat_values SET value = %s/100 \
                                                  WHERE customer_type_id = '%s' AND item_type_id = '%s'",
						 g_list_nth_data (values, 2),
						 g_list_nth_data (values, 0),
						 g_list_nth_data (values, 1));
	
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to set value",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	oseaserver_message_ok_answer (channel, msg_no, "Value updated", OSEACOMM_CODE_OK, NULL);
	return TRUE;

}

gboolean os_tax_vat_value_list (OseaCommXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	
	OseaCommDataSet    * customer_type_dataset;
	OseaCommDataSet    * item_type_dataset;
	OseaCommDataSet    * values_dataset;
	OseaCommDataSet    * values_dataset2;
	gint               width;
	gint               rows;
	gint               i, j;

	customer_type_dataset = oseaserver_command_execute_single_query ("SELECT id, reference FROM tax_vat_customer_type ORDER BY id");
	if (!customer_type_dataset) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to get tax vat customer types", OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	item_type_dataset = oseaserver_command_execute_single_query ("SELECT id, reference FROM tax_vat_item_type ORDER BY id");
	if (!item_type_dataset) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to get tax vat item types", OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	values_dataset = oseaserver_command_execute_single_query ("SELECT ROUND(value * 100) FROM tax_vat_values ORDER BY customer_type_id, item_type_id");
	
	if (!values_dataset) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to get tax vat values", OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	width = oseacomm_dataset_get_height (customer_type_dataset);
	rows = oseacomm_dataset_get_height (item_type_dataset);
	
	values_dataset2 = oseacomm_dataset_new ();
	for (i = 0; i < rows; i++) {
		for (j = 0; j < width; j++) {
			oseacomm_dataset_add (values_dataset2, (gchar *) coyote_dataset_get (values_dataset, i + (j * rows), 0));
		}
		
		if (i == (rows - 1)) 
			break;
		oseacomm_dataset_new_row (values_dataset2);
	}

	oseacomm_dataset_free (values_dataset);
	

	oseaserver_message_ok_answer (channel, msg_no, "Tax vat values", OSEACOMM_CODE_OK, 
				customer_type_dataset, item_type_dataset, values_dataset2, NULL);
	
	return TRUE;	
}

