//  LibOseaComm:  Support library with xml and communication functions.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US

#include "oseacomm_simple_cfg.h"

#define LOG_DOMAIN "OSEACOMM_SIMPLE_CFG"

/**
 * oseacomm_simple_cfg_destroy:
 * @config: a config object
 * 
 * This function frees up all allocated resources by @config.
 **/
void oseacomm_simple_cfg_destroy (OseaCommSimpleCfg *config)
{
	g_return_if_fail (config);
	g_free (config);
	
	return;
}

/**
 * oseacomm_simple_cfg_set_frame_available:
 * @config: a config object
 * @cb: function to be executed when event arises
 * 
 * This function sets a callback function for "frame available"
 * event. This event arises when a frame arrives from the other side.
 *
 * If this callback is set, the "complete_message" event will never be
 * generated.
 *
 **/
void oseacomm_simple_cfg_set_frame_available  (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgFrameAvailable cb)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);

	config->frame_available_cb = cb;
	return;
}

/**
 * oseacomm_simple_cfg_set_close_indication:
 * @config: a config object
 * @cb: function to be executed when event arises
 * 
 * This function sets a callback function for "close indication"
 * event. This event arises when a client wants to close an already
 * opened channe.
 **/
void oseacomm_simple_cfg_set_close_indication (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgCloseIndication cb)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);

	config->close_indication_cb = cb;
	return; 
}




/**
 * oseacomm_simple_cfg_set_close_confirmation:
 * @config: a config object
 * @cb: function to be executed when event arises
 * 
 * This function sets a callback function for "close confirmation"
 * event. 
 **/
void oseacomm_simple_cfg_set_close_confirmation (OseaCommSimpleCfg *config,
					       OseaCommSimpleCfgCloseConfirmation cb)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);

	config->close_confirmation_cb = cb;
	return; 
}


void oseacomm_simple_cfg_set_finalize         (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgFinalize cb)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);
	
	return;
}

/**
 * oseacomm_simple_cfg_set_server_init:
 * @config: a config object 
 * @cb: function to be executed when event arises
 * @data: data to be passed to @cb function
 * 
 * This function sets a callback function for "server init" event. This
 * event arises when an incoming client connection is going to be
 * started.
 **/
void oseacomm_simple_cfg_set_server_init      (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgServerInit cb,
					     gpointer data)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);

	config->server_init_cb = cb;

	return;
}

void oseacomm_simple_cfg_set_server_confirm   (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgServerConfirmation cb)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);
	
	return;
}

void oseacomm_simple_cfg_set_client_confirm   (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgClientConfirmation cb)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);
	
	return;
}

/**
 * oseacomm_simple_cfg_set_complete_message:
 * @config: a config object
 * @cb: function to be executed when a complete_message arrives.
 * @data: data to be passed to @cb function
 * @custom_data: data to be passed to @cb function
 * 
 * This functions sets a callback function for "complete message"
 * event. This event arises when a complete message arrives. A
 * complete message is composed of low level frames, so if you don't
 * want to manage all incoming frames to join them in a complete
 * message, use this callback.
 *
 **/
void oseacomm_simple_cfg_set_complete_message (OseaCommSimpleCfg * config,
					     OseaCommSimpleCfgCompleteMessage cb,
					     gpointer data, gpointer custom_data)
{
	g_return_if_fail (config);
	g_return_if_fail (cb);
	
	config->complete_message_cb = cb;
	config->complete_message_data = data;
	config->complete_message_custom_data = custom_data;
	return;
}


/**
 * oseacomm_simple_cfg_new:
 * 
 * This function creates a new config object. This object will guide
 * all communication process telling to "oseacomm simple" profile which
 * function should be called when any event arises.  Moreover, this
 * object will guide communication process in a high or low level way,
 * ie: using comple_message_cb you will be called when a complete
 * message has arrived. However, using frame_available_cb you will be
 * called when a single frame has arrived.  Therefore, you need to use
 * oseacomm_simple_cfg_set* functions to set up which functions are
 * going to be called when something happens.
 * 
 * Return value: The config object.
 **/
OseaCommSimpleCfg * oseacomm_simple_cfg_new     (void)
{
	OseaCommSimpleCfg *config;
	
	config = g_new0 (OseaCommSimpleCfg, 1);
	config->buffer = g_byte_array_new ();
	config->buffer_len = 0;

	return config;
}
