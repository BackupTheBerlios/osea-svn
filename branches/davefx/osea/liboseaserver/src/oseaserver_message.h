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

#ifndef __AFGS_MESSAGE_H__
#define __AFGS_MESSAGE_H__

#include <glib.h>
#include <coyote/coyote.h>
#include "oseaserver_config.h"

void            oseaserver_message_error_answer (RRChannel * channel, gint msg_no, gchar * message,
					   CoyoteCodeType status_code);

void            oseaserver_message_ok_answer    (RRChannel * channel, gint msg_no, gchar * message, 
					   CoyoteCodeType status_code, ...);

GList         * oseaserver_message_check_params (CoyoteXmlServiceData * data, ...);

#endif

