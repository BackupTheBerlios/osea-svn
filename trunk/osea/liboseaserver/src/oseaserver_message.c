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

#include "oseaserver_message.h"

/**
 * oseaserver_message_error_answer:
 * @channel: Channel used to send @message
 * @msg_no: message number which is going to be replied
 * @message: message content
 * @status_code: a code from coyote_code module
 * 
 * Creates and sends a negative response message by using the given data. @message is the 
 * message content, sent throught @channel replying to an incoming message numbered as @msg_no.  
 **/
void            oseaserver_message_error_answer (RRChannel * channel, gint msg_no, gchar * message,
					   CoyoteCodeType status_code)
{
	CoyoteXmlObject  * coyote_xml_object = NULL;
	CoyoteXmlMessage * coyote_xml_message = NULL;
	GError           * error = NULL;

	coyote_xml_object = coyote_xml_new_object ();
	
	coyote_xml_add_response_service (coyote_xml_object,
					 status_code,
					 message, NULL);
	
	coyote_xml_message = coyote_xml_build_message (coyote_xml_object);
	
	
	coyote_simple_reply_error (COYOTE_SIMPLE (channel), 
				   coyote_xml_message->content,
				   coyote_xml_message->len,
				   msg_no, TRUE, &error);
	
	coyote_xml_destroy_message (coyote_xml_message);
	coyote_xml_destroy_object (coyote_xml_object);
	return;
}

/**
 * oseaserver_message_ok_answer:
 * @channel: Channel used to send @message
 * @msg_no: message number which is going to be replied
 * @message: message content
 * @status_code: a code from coyote_code module
 * @Varargs: Optional datasets 
 * 
 * Creates and sends an afirmative response message by using the given data. @message is the 
 * message content, sent throught @channel replying to an incoming message numbered as @msg_no. 
 * @Varargs represents a dataset list. 
 * If we want to send an afirmative reply with two datasets we will write something like:+
 *
 * oseaserver_message_ok_answer (channel, msg_no, "Take this data", COYOTE_CODE_OK, 
 *                         dataset1, dataset2, NULL);
 **/
void            oseaserver_message_ok_answer    (RRChannel * channel, gint msg_no, gchar * message, 
					   CoyoteCodeType status_code, ...)
{
	CoyoteXmlObject  * coyote_xml_object = NULL;
	CoyoteXmlMessage * coyote_xml_message = NULL;
	GError           * error = NULL;
	va_list            args;

	coyote_xml_object = coyote_xml_new_object ();

	va_start (args, status_code);

	coyote_xml_add_vresponse_service (coyote_xml_object,
					  status_code,
					  message, args);

	va_end (args);
	
	coyote_xml_message = coyote_xml_build_message (coyote_xml_object);
	
	
	coyote_simple_reply (COYOTE_SIMPLE (channel), 
				   coyote_xml_message->content,
				   coyote_xml_message->len,
				   msg_no, TRUE, &error);
	
	coyote_xml_destroy_message (coyote_xml_message);
	coyote_xml_destroy_object (coyote_xml_object);
	return;
}

/**
 * oseaserver_message_check_params:
 * @data: data structure which represents an incoming service request message
 * @Varargs: list of strings composed by parameters names to check for.
 * 
 * Function that check incoming message parameters. It checks
 * parameter name and its order, so if you want to check that @data has
 * the following parameters: param1, param2, param3 in that order you
 * have to do oseaserver_message_check_params (data, param1, param2, param3, NULL).
 * 
 * Once all paramteres are checked this function returns a
 * GList which contains all the values associated with.
 * 
 * Return value: A GList or NULL if parameters doesn't match
 * in name or order.
 **/
GList * oseaserver_message_check_params (CoyoteXmlServiceData * data, ...)
{
	va_list   arguments;
	gchar   * param      = NULL;
	GList   * list_aux   = NULL;
	GList   * result     = NULL;

	CoyoteXmlServiceNode * node = NULL;

	g_return_val_if_fail (data, FALSE);

	va_start (arguments, data);
	
	list_aux = g_list_first(data->item_list);

	while ((param = va_arg (arguments, gchar *))) {
		node = (CoyoteXmlServiceNode *) list_aux->data;

		if (node) {
			if (g_strcasecmp (node->attrib, param) != 0) {
				g_list_free (result);
				return NULL;
			}else
				result = g_list_append (result, node->value);
		}else { 
			g_list_free (result);
			return NULL;
		}

		list_aux = g_list_next (list_aux);
	}

	va_end (arguments);
	return result;
	
}

