//  ASPL Clm: Command Line Manager for ASPL Fact system
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

#ifndef __GROUP_H__
#define __GROUP_H__

#include <liboseacomm/oseacomm.h>
#include <libaoskernel/aos_kernel.h>

gboolean group_new          (gchar * line, gpointer data);

gboolean group_remove       (gchar * line, gpointer data);

gboolean group_list         (gchar * line, gpointer data);

gboolean group_list_by_user (gchar * line, gpointer data);

gboolean group_add_user     (gchar * line, gpointer data);

gboolean group_del_user     (gchar * line, gpointer data);



#endif
