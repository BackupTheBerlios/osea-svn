//  oinvoice: support file
//  Copyright (C) 2002, 2003  Francis Brosnan Blázquez
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
#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include <gtk/gtk.h>
#include <glade/glade.h>

#include "event_source.h"
#include "osea_datalist.h"
#include "dialog.h"
#include "event_process.h"
#include "event_source.h"

typedef void (* SupportFunc) (AsplDataList * datalist);

AsplDataList *  support_create_callback (SupportFunc source_func);


#endif
