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

#ifndef __COYOTE_SIMPLE_H__
#define __COYOTE_SIMPLE_H__

G_BEGIN_DECLS

#define COYOTE_SIMPLE_URI "http://fact.aspl.es/profiles/oseacomm_profile"

#include <glib.h>
#include <librr/rr-channel.h>
#include "oseacomm_simple_cfg.h"

typedef struct _CoyoteSimple CoyoteSimple;
typedef struct _CoyoteSimpleClass CoyoteSimpleClass;

#define TYPE_COYOTE_SIMPLE (oseacomm_simple_get_type ())
#define COYOTE_SIMPLE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_COYOTE_SIMPLE, CoyoteSimple))
#define COYOTE_SIMPLE_CLASS(class) (G_TYPE_CHECK_CLASS_CAST ((class), TYPE_COYOTE_SIMPLE, CoyoteSimpleClass))
#define IS_COYOTE_SIMPLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_COYOTE_SIMPLE))
#define IS_COYOTE_SIMPLE_CLASS(class) (G_TYPE_CHECK_CLASS_TYPE ((class), TYPE_COYOTE_SIMPLE))
#define COYOTE_SIMPLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_COYOTE_SIMPLE, CoyoteSimpleClass))

struct _CoyoteSimple {
	RRChannel parent_object;
	GByteArray * buffer;
	gboolean finish_connection;
};

struct _CoyoteSimpleClass {
	RRChannelClass parent_class;
};

GType oseacomm_simple_get_type (void);


CoyoteSimple * oseacomm_simple_start    (RRConnection * connection, 
				       CoyoteSimpleCfg * config, 
				       GError ** error);

gboolean       oseacomm_simple_close    (CoyoteSimple * channel, 
				       GError ** error);

gboolean       oseacomm_simple_send     (CoyoteSimple * channel, 
				       gchar * data, 
				       gint len, gint free,
				       GError ** error);

gboolean       oseacomm_simple_reply    (CoyoteSimple * channel,
				       gchar * data, gint len,
				       gint msg_no, gint free,
				       GError ** error);


gboolean       oseacomm_simple_reply_error    (CoyoteSimple * channel,
					     gchar * data, gint len,
					     gint msg_no, gint free,
					     GError ** error);

G_END_DECLS

#endif 














