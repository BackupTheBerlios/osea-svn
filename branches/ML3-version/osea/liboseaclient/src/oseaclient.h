//
//  LibOseaClient: common functions to liboseaclient* level and architectural functions.
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

#ifndef __OSEACLIENT_H__
#define __OSEACLIENT_H__

#include <glib.h>
#include <librr/rr.h>
#include <liboseacomm/oseacomm.h>
#include "oseaclient_types.h"
#include "oseaclient_list.h"
#include "oseaclient_request.h"
#include "oseaclient_session.h"
#include "oseaclient_event_source.h"
#include "oseaclient_password.h"
#include "oseaclient_support.h"
#include "oseaclient-datanode.h"
#include "oseaclient-detaileddatanode.h"
#include "oseaclient_decimal.h"
#include "oseaclient_keys.h"


void    oseaclient_print_version_info ();

void    oseaclient_init ();


#endif
