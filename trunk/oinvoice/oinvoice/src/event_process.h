//  ASPL Fact: invoicing client program for ASPL Fact System
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

#ifndef __EVENT_PROCESS_H__
#define __EVENT_PROCESS_H__


#include <glib.h>
#include <liboseaclient/oseaclient.h>

gboolean event_process_oseaclient_multidata  (OseaClientMultiData * data, gpointer datalist_aux);

gboolean event_process_oseaclient_data       (OseaClientData * data, gpointer datalist_aux);

gboolean event_process_oseaclient_simpledata (OseaClientSimpleData * data, gpointer datalist_aux);

gboolean event_process_oseaclient_nuldata    (OseaClientNulData * data, gpointer datalist_aux);


#endif
