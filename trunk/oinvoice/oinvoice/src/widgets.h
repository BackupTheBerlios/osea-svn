//  ASPL Fact: invoicing client program for ASPL Fact System
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

#ifndef __WIDGETS_H__
#define __WIDGETS_H__


#include <gnome.h>
#include <glade/glade.h>

void       widgets_set_permission_disabling (gboolean active);

GladeXML * widgets_load (const gchar *fname, const gchar *root, const gchar *domain);

void       widgets_set_sensitive (const gchar *xpd_base_name, GtkWidget *widget, gboolean value);

#endif
