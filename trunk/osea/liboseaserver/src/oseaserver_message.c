//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Marín Carreño
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
 * @status_code: a code from oseacomm_code module
 * 
 * Creates and sends a negative response message by using the given data. @message is the 
 * message content, sent throught @channel replying to an incoming message numbered as @msg_no.  
 **/
void            oseaserver_message_error_answer (RRChannel * channel, gint msg_no, gchar * message,
					  OseaCommCodeType status_code)
{
	OseaCommXmlObject  * oseacomm_xml_object = NULL;
	OseaCommXmlMessage * oseacomm_xml_message = NULL;
	GError           * error = NULL;

	oseacomm_xml_object = oseacomm_xml_new_object ();
	
	oseacomm_xml_add_response_service (oseacomm_xml_object,
					 status_code,
					 message, NULL);
	
	oseacomm_xml_message = oseacomm_xml_build_message (oseacomm_xml_object);
	
	
	oseacomm_simple_reply_error (OSEACOMM_SIMPLE (channel), 
				   oseacomm_xml_message->content,
				   oseacomm_xml_message->len,
				   msg_no, TRUE, &error);
	
	oseacomm_xml_destroy_message (oseacomm_xml_message);
	oseacomm_xml_destroy_object (oseacomm_xml_object);
	return;
}

/**
 * oseaserver_message_ok_answer:
 * @channel: Channel used to send @message
 * @msg_no: message number which is going to be replied
 * @message: message content
 * @status_code: a code from oseacomm_code module
 * @Varargs: Optional datasets 
 * 
 * Creates and sends an afirmative response message by using the given data. @message is the 
 * message content, sent throught @channel replying to an incoming message numbered as @msg_no. 
 * @Varargs represents a dataset list. 
 * If we want to send an afirmative reply with two datasets we will write something like:+
 *
 * oseaserver_message_ok_answer (channel, msg_no, "Take this data", OSEACOMM_CODE_OK, 
 *                         dataset1, dataset2, NULL);
 **/
void            oseaserver_message_ok_answer    (RRChannel * channel, gint msg_no, gchar * message, 
					  OseaCommCodeType status_code, ...)
{
	OseaCommXmlObject  * oseacomm_xml_object = NULL;
	OseaCommXmlMessage * oseacomm_xml_message = NULL;
	GError           * error = NULL;
	va_list            args;

	oseacomm_xml_object = oseacomm_xml_new_object ();

	va_start (args, status_code);

	oseacomm_xml_add_vresponse_service (oseacomm_xml_object,
					  status_code,
					  message, args);

	va_end (args);
	
	oseacomm_xml_message = oseacomm_xml_build_message (oseacomm_xml_object);
	
	
	oseacomm_simple_reply (OSEACOMM_SIMPLE (channel), 
				   oseacomm_xml_message->content,
				   oseacomm_xml_message->len,
				   msg_no, TRUE, &error);
	
	oseacomm_xml_destroy_message (oseacomm_xml_message);
	oseacomm_xml_destroy_object (oseacomm_xml_object);
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
GList * oseaserver_message_check_params (OseaCommXmlServiceData * data, ...)
{
	va_list   arguments;
	gchar   * param      = NULL;
	GList   * list_aux   = NULL;
	GList   * result     = NULL;

	OseaCommXmlServiceNode * node = NULL;

	g_return_val_if_fail (data, FALSE);

	va_start (arguments, data);
	
	list_aux = g_list_first(data->item_list);

	while ((param = va_arg (arguments, gchar *))) {
		node = (OseaCommXmlServiceNode *) list_aux->data;

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

