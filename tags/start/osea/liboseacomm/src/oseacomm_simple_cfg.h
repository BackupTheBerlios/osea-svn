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

#ifndef __COYOTE_SIMPLE_CFG_H__
#define __COYOTE_SIMPLE_CFG_H__
#include <glib.h>
#include <librr/rr.h>

typedef struct _CoyoteSimpleCfg CoyoteSimpleCfg;


// Low level callback function type definitions
typedef gboolean  (*CoyoteSimpleCfgFrameAvailable)       (RRChannel *channel, 
							  RRFrame *frame, 
							  GError **error);

typedef void      (* CoyoteSimpleCfgCloseConfirmation)   (RRChannel *channel, gint code,
							  const gchar *xml_lang, 
							  const gchar *diagnostic);

typedef gboolean  (* CoyoteSimpleCfgCloseIndication)     (RRChannel *channel, gint code,
							  const gchar *xml_lang, 
							  const gchar *diagnostic,
							  GError **error);

typedef gboolean  (* CoyoteSimpleCfgClientInit)          (RRChannel *channel, 
							  GError **error);

typedef gboolean  (* CoyoteSimpleCfgServerInit)          (RRChannel *channel, 
							  const gchar *piggyback, 
							  GError **error);

typedef void      (* CoyoteSimpleCfgClientConfirmation)  (RRChannel *channel, 
							  const gchar *piggyback);

typedef void      (* CoyoteSimpleCfgServerConfirmation)  (RRChannel *channel);

typedef void      (* CoyoteSimpleCfgFinalize)            (RRChannel *channel);


// High level callback function type definitions
typedef void      (* CoyoteSimpleCfgCompleteMessage)     (RRChannel *channel, 
							  RRFrame *frame,
							  GString *text, 
							  gpointer  data, 
							  gpointer custom_data );

struct _CoyoteSimpleCfg {
	CoyoteSimpleCfgFrameAvailable      frame_available_cb;
	
	CoyoteSimpleCfgCloseConfirmation   close_confirmation_cb;

	CoyoteSimpleCfgCloseIndication     close_indication_cb;

	CoyoteSimpleCfgClientInit          client_init_cb;

	CoyoteSimpleCfgServerInit          server_init_cb;

	CoyoteSimpleCfgClientConfirmation  client_confirmation_cb;

	CoyoteSimpleCfgServerConfirmation  server_confirmation_cb;

	CoyoteSimpleCfgFinalize            finalize_cb;


	CoyoteSimpleCfgCompleteMessage     complete_message_cb;
	
	gpointer  server_init_data;
	gpointer  complete_message_data;
	gpointer  complete_message_custom_data;
	
	GByteArray *buffer;
	gint buffer_len;
};


CoyoteSimpleCfg * oseacomm_simple_cfg_new     (void);

void		  oseacomm_simple_cfg_destroy (CoyoteSimpleCfg *config);

void oseacomm_simple_cfg_set_frame_available  (CoyoteSimpleCfg *config,
					     CoyoteSimpleCfgFrameAvailable cb);

void oseacomm_simple_cfg_set_close_indication (CoyoteSimpleCfg *config,
 					     CoyoteSimpleCfgCloseIndication cb);

void oseacomm_simple_cfg_set_close_confirmation (CoyoteSimpleCfg *config,
					       CoyoteSimpleCfgCloseConfirmation cb);

void oseacomm_simple_cfg_set_finalize         (CoyoteSimpleCfg *config,
					     CoyoteSimpleCfgFinalize cb);

void oseacomm_simple_cfg_set_server_init      (CoyoteSimpleCfg *config,
					     CoyoteSimpleCfgServerInit cb,
					     gpointer data);

void oseacomm_simple_cfg_set_server_confirm   (CoyoteSimpleCfg *config,
					     CoyoteSimpleCfgServerConfirmation cb);

void oseacomm_simple_cfg_set_complete_message (CoyoteSimpleCfg * config,
					     CoyoteSimpleCfgCompleteMessage cb,
					     gpointer data,
					     gpointer custom_data);


#endif
