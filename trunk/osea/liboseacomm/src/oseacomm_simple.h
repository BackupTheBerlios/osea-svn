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

#ifndef __OSEACOMM_SIMPLE_H__
#define __OSEACOMM_SIMPLE_H__

G_BEGIN_DECLS

#define OSEACOMM_SIMPLE_URI "http://fact.aspl.es/profiles/oseacomm_profile"

#include <glib.h>
#include <librr/rr-channel.h>
#include "oseacomm_simple_cfg.h"

typedef struct _OseaCommSimple OseaCommSimple;
typedef struct _OseaCommSimpleClass OseaCommSimpleClass;

#define TYPE_OSEACOMM_SIMPLE (oseacomm_simple_get_type ())
#define OSEACOMM_SIMPLE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_OSEACOMM_SIMPLE, OseaCommSimple))
#define OSEACOMM_SIMPLE_CLASS(class) (G_TYPE_CHECK_CLASS_CAST ((class), TYPE_OSEACOMM_SIMPLE, OseaCommSimpleClass))
#define IS_OSEACOMM_SIMPLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_OSEACOMM_SIMPLE))
#define IS_OSEACOMM_SIMPLE_CLASS(class) (G_TYPE_CHECK_CLASS_TYPE ((class), TYPE_OSEACOMM_SIMPLE))
#define OSEACOMM_SIMPLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_OSEACOMM_SIMPLE, OseaCommSimpleClass))

struct _OseaCommSimple {
	RRChannel parent_object;
	GByteArray * buffer;
	gboolean finish_connection;
};

struct _OseaCommSimpleClass {
	RRChannelClass parent_class;
};

GType oseacomm_simple_get_type (void);


OseaCommSimple * oseacomm_simple_start    (RRConnection * connection, 
				       OseaCommSimpleCfg * config, 
				       GError ** error);

gboolean       oseacomm_simple_close    (OseaCommSimple * channel, 
				       GError ** error);

gboolean       oseacomm_simple_send     (OseaCommSimple * channel, 
				       gchar * data, 
				       gint len, gint free,
				       GError ** error);

gboolean       oseacomm_simple_reply    (OseaCommSimple * channel,
				       gchar * data, gint len,
				       gint msg_no, gint free,
				       GError ** error);


gboolean       oseacomm_simple_reply_error    (OseaCommSimple * channel,
					     gchar * data, gint len,
					     gint msg_no, gint free,
					     GError ** error);

G_END_DECLS

#endif 














