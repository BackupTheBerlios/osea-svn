//  LibVortex:  A BEEP (RFC3080/RFC3081) implementation.
//  Copyright (C) 2005 Advanced Software Production Line, S.L.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 2.1 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free
//  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
//  02111-1307 USA
//  
//  You may find a copy of the license under this software is released
//  at COPYING file. This is LGPL software: you are wellcome to
//  develop propietary applications using this library withtout any
//  royalty or fee but returning back any change, improvement or
//  addition in the form of source code, project image, documentation
//  patches, etc. 
//
//  You can also contact us to negociate other license term you may be
//  interested different from LGPL. Contact us at:
//          
//      Postal address:
//         Advanced Software Production Line, S.L.
//         C/ Dr. Michavila Nº 14
//         Coslada 28820 Madrid
//         Spain
//
//      Email address:
//         info@aspl.es - http://fact.aspl.es
//
#ifndef __VORTEX_CHANNEL_H__
#define __VORTEX_CHANNEL_H__

#include <vortex.h>

VortexChannel     * vortex_channel_new                         (VortexConnection * connection, 
								gint channel_num, 
								gchar * profile,
								VortexOnCloseChannel    close,
								gpointer                close_user_data,
								VortexOnFrameReceived   received,
								gpointer                received_user_data,
								VortexOnChannelCreated  on_channel_created,
								gpointer user_data);

gboolean           vortex_channel_close                        (VortexChannel * channel,
								VortexOnClosedNotification on_closed);

VortexChannel    * vortex_channel_empty_new                    (gint channel_num,
								gchar * profile,
								VortexConnection * connection);

void               vortex_channel_set_close_handler            (VortexChannel * channel,
								VortexOnCloseChannel close,
								gpointer user_data);

void               vortex_channel_set_received_handler         (VortexChannel * channel, 
								VortexOnFrameReceived received,
								gpointer user_data);

void               vortex_channel_set_complete_flag            (VortexChannel * channel,
								gboolean value);

gboolean           vortex_channel_have_previous_frame          (VortexChannel * channel);

VortexFrame      * vortex_channel_get_previous_frame           (VortexChannel * channel);

void               vortex_channel_set_previous_frame           (VortexChannel * channel, 
								VortexFrame * frame);

gboolean           vortex_channel_have_complete_flag           (VortexChannel * channel);

void               vortex_channel_update_status                (VortexChannel * channel, 
								gint frame_size,
								WhatUpdate update);

void               vortex_channel_update_status_received       (VortexChannel * channel, gint frame_size,
								WhatUpdate update);

gint               vortex_channel_get_next_msg_no              (VortexChannel * channel);

gint               vortex_channel_get_next_expected_msg_no     (VortexChannel * channel);

gint               vortex_channel_get_number                   (VortexChannel * channel);

guint32            vortex_channel_get_next_seq_no              (VortexChannel * channel);

guint32            vortex_channel_get_next_expected_seq_no     (VortexChannel * channel);

gint               vortex_channel_get_next_reply_no            (VortexChannel * channel);

gint               vortex_channel_get_next_expected_reply_no   (VortexChannel * channel);

gint               vortex_channel_get_window_size              (VortexChannel * channel);

gchar            * vortex_channel_get_profile                  (VortexChannel * channel);

VortexConnection * vortex_channel_get_connection               (VortexChannel * channel);

void               vortex_channel_queue_frame                     (VortexChannel * channel, 
								   VortexWriterData * data);

gboolean           vortex_channel_queue_is_empty                  (VortexChannel * channel);

VortexWriterData * vortex_channel_queue_next_msg                  (VortexChannel * channel);

gint               vortex_channel_queue_length                    (VortexChannel * channel);

void               vortex_channel_set_data                        (VortexChannel * channel,
								   gpointer key,
								   gpointer value);

gpointer           vortex_channel_get_data                        (VortexChannel * channel,
								   gpointer key);

gboolean           vortex_channel_send_msg                        (VortexChannel * channel,
								   gchar         * message,
								   gint            message_size,
								   gint          * msg_no);

gboolean           vortex_channel_send_msgv                       (VortexChannel * channel,
								   gint          * msg_no,
								   gchar         * format,
								   ...);

gboolean           vortex_channel_send_msg_and_wait               (VortexChannel   * channel,
								   gchar           * message,
								   gint              message_size,
								   gint            * msg_no,
								   WaitReplyData   * wait_reply);

gboolean           vortex_channel_send_msg_and_waitv              (VortexChannel   * channel,
								   gint            * msg_no,
								   WaitReplyData   * wait_reply,
								   gchar           * format,
								   ...);

gboolean           vortex_channel_send_rpy                        (VortexChannel *channel,  
								   gchar * message,
								   gint    message_size,
								   gint    msg_no_rpy);

gboolean           vortex_channel_send_rpyv                       (VortexChannel *channel,
								   gint    msg_no_rpy,
								   gchar * format,
								   ...);

gboolean           vortex_channel_send_err                       (VortexChannel *channel,  
								  gchar * message,
								  gint    message_size,
								  gint    msg_no_err);

gboolean           vortex_channel_send_errv                      (VortexChannel * channel,
								  gint    msg_no_err,
								  gchar * format, 
								  ...); 
						
gboolean           vortex_channel_is_opened                      (VortexChannel * channel);

gboolean           vortex_channel_is_being_closed                (VortexChannel * channel);


void               vortex_channel_free                           (VortexChannel * channel);

gboolean           vortex_channel_is_defined_received_handler    (VortexChannel * channel);      

gboolean           vortex_channel_invoke_received_handler        (VortexChannel * channel, 
								  VortexFrame * frame);

gboolean           vortex_channel_is_defined_close_handler       (VortexChannel * channel);

gboolean           vortex_channel_invoke_close_handler           (VortexChannel  * channel);

VortexFrame      * vortex_channel_wait_reply                     (VortexChannel * channel, 
								  gint msg_no,
								  WaitReplyData * wait_reply);

WaitReplyData *    vortex_channel_create_wait_reply              ();

void               vortex_channel_free_wait_reply                (WaitReplyData * wait_reply);

gboolean           vortex_channel_is_ready                       (VortexChannel * channel);

// internal vortex function

gboolean           vortex_channel_is_up_to_date                  (VortexChannel * channel);

void               vortex_channel_lock_to_update_received        (VortexChannel * channel);

void               vortex_channel_unlock_to_update_received      (VortexChannel * channel);

void               vortex_channel_lock_to_receive                (VortexChannel * channel);

void               vortex_channel_unlock_to_receive              (VortexChannel * channel);

void               vortex_channel_0_frame_received               (VortexChannel    * channel,
								  VortexConnection * connection,
								  VortexFrame      * frame,
								  gpointer           user_data);

void           	   vortex_channel_signal_on_close_blocked        (VortexChannel    * channel);

void               vortex_channel_flag_reply_processed           (VortexChannel * channel, 
								  gboolean flag);

void               vortex_channel_install_waiter                 (VortexChannel * channel,
								  gchar * rpy);

void               vortex_channel_wait_until_sent                (VortexChannel * channel,
								  gchar * rpy);

void               vortex_channel_signal_rpy_sent               (VortexChannel * channel,
		 						 gchar * rpy);

void               vortex_channel_signal_reply_sent_on_close_blocked (VortexChannel * channel);

void               vortex_channel_set_pool                       (VortexChannel * channel,
								  VortexChannelPool * pool);

#endif
