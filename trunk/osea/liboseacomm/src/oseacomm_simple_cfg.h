//  LibOseaComm:  Support library with xml and communication functions.
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

#ifndef __OSEACOMM_SIMPLE_CFG_H__
#define __OSEACOMM_SIMPLE_CFG_H__
#include <glib.h>
#include <librr/rr.h>

typedef struct _OseaCommSimpleCfg OseaCommSimpleCfg;


// Low level callback function type definitions
typedef gboolean  (*OseaCommSimpleCfgFrameAvailable)       (RRChannel *channel, 
							  RRFrame *frame, 
							  GError **error);

typedef void      (* OseaCommSimpleCfgCloseConfirmation)   (RRChannel *channel, gint code,
							  const gchar *xml_lang, 
							  const gchar *diagnostic);

typedef gboolean  (* OseaCommSimpleCfgCloseIndication)     (RRChannel *channel, gint code,
							  const gchar *xml_lang, 
							  const gchar *diagnostic,
							  GError **error);

typedef gboolean  (* OseaCommSimpleCfgClientInit)          (RRChannel *channel, 
							  GError **error);

typedef gboolean  (* OseaCommSimpleCfgServerInit)          (RRChannel *channel, 
							  const gchar *piggyback, 
							  GError **error);

typedef void      (* OseaCommSimpleCfgClientConfirmation)  (RRChannel *channel, 
							  const gchar *piggyback);

typedef void      (* OseaCommSimpleCfgServerConfirmation)  (RRChannel *channel);

typedef void      (* OseaCommSimpleCfgFinalize)            (RRChannel *channel);


// High level callback function type definitions
typedef void      (* OseaCommSimpleCfgCompleteMessage)     (RRChannel *channel, 
							  RRFrame *frame,
							  GString *text, 
							  gpointer  data, 
							  gpointer custom_data );

struct _OseaCommSimpleCfg {
	OseaCommSimpleCfgFrameAvailable      frame_available_cb;
	
	OseaCommSimpleCfgCloseConfirmation   close_confirmation_cb;

	OseaCommSimpleCfgCloseIndication     close_indication_cb;

	OseaCommSimpleCfgClientInit          client_init_cb;

	OseaCommSimpleCfgServerInit          server_init_cb;

	OseaCommSimpleCfgClientConfirmation  client_confirmation_cb;

	OseaCommSimpleCfgServerConfirmation  server_confirmation_cb;

	OseaCommSimpleCfgFinalize            finalize_cb;


	OseaCommSimpleCfgCompleteMessage     complete_message_cb;
	
	gpointer  server_init_data;
	gpointer  complete_message_data;
	gpointer  complete_message_custom_data;
	
	GByteArray *buffer;
	gint buffer_len;
};


OseaCommSimpleCfg * oseacomm_simple_cfg_new     (void);

void		  oseacomm_simple_cfg_destroy (OseaCommSimpleCfg *config);

void oseacomm_simple_cfg_set_frame_available  (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgFrameAvailable cb);

void oseacomm_simple_cfg_set_close_indication (OseaCommSimpleCfg *config,
 					     OseaCommSimpleCfgCloseIndication cb);

void oseacomm_simple_cfg_set_close_confirmation (OseaCommSimpleCfg *config,
					       OseaCommSimpleCfgCloseConfirmation cb);

void oseacomm_simple_cfg_set_finalize         (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgFinalize cb);

void oseacomm_simple_cfg_set_server_init      (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgServerInit cb,
					     gpointer data);

void oseacomm_simple_cfg_set_server_confirm   (OseaCommSimpleCfg *config,
					     OseaCommSimpleCfgServerConfirmation cb);

void oseacomm_simple_cfg_set_complete_message (OseaCommSimpleCfg * config,
					     OseaCommSimpleCfgCompleteMessage cb,
					     gpointer data,
					     gpointer custom_data);


#endif
