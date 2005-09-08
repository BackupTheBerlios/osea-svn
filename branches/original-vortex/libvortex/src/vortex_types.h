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
#ifndef __VORTEX_TYPES_H__
#define __VORTEX_TYPES_H__

// Vortex Types definition for Vortex frames

// Vortex type of frame definition:
/**
 * \defgroup vortex_types Vortex Types: Types definitions used across Vortex Library.
 */

/// \addtogroup vortex_types
//@{

/** 
 * @brief Describes the type allowed for a frame or the type a frame actually have.
 *
 * Every frame used inside Vortex Library have one of the following values.
 */
typedef enum {
	/** @brief The frame type is unknown, used to represent errors across the Vortex Library.
	 * 
	 * This only means an error have happen while receiving the frame or
	 * while creating a new one. This frame have no valid use inside the
	 * Vortex Library.
	 */
	VORTEX_FRAME_TYPE_UNKNOWN, 
	/**
	 * @brief Frame type is MSG. It is used to represent new Vortex messages.
	 *
	 * This frame type represent a new message or a frame which belongs to
	 * a set of frame conforming a message. This types of frames are
	 * generated while using the following function:
	 *
	 *   - \ref vortex_channel_send_msg
	 *   - \ref vortex_channel_send_msgv
	 *   - \ref vortex_channel_send_msg_and_wait
	 *   - \ref vortex_channel_send_msg_and_waitv
	 * 
	 */
	VORTEX_FRAME_TYPE_MSG,
	/** 
	 * @brief Frame type is RPY. It is used to represent reply frames to MSG frames called positive replies.
	 *
	 * This frame type represent replies produced to frame with type
	 * MSG. The following function allows to generate frame types for RPY:
	 * 
	 *  - \ref vortex_channel_send_rpy
	 *  - \ref vortex_channel_send_rpyv
	 * 
	 */
	VORTEX_FRAME_TYPE_RPY,
	/**
	 * @brief Frame type is ANS. It is used to represent reply frames to
	 * MSG frames using the type ANS.
	 *
	 * 
	 */
	VORTEX_FRAME_TYPE_ANS,
	/**
	 * @brief Frame type is ERR. It is used to represent reply frames to MSG frames called negative replies.
	 *
	 * This frame type is used to represent negative replies to received
	 * frames with type MSG. The following functions allows to generate ERR frames:
	 *    
	 *   - \ref vortex_channel_send_err
	 *   - \ref vortex_channel_send_errv
	 *
	 * 
	 */

	VORTEX_FRAME_TYPE_ERR,
	/**
	 * @brief Frame type is NUL. It is used to represent reply frames to MSG ending a set of ANS frame replies.
	 * 
	 */
	VORTEX_FRAME_TYPE_NUL
}VortexFrameType;

/** 
 * @brief Maximum sequence number allowed to be used for a channel created.
 * 
 * The sequencer number is used inside Vortex Library to keep up to
 * date synchronization inside a channel while receiving and sending
 * data. This number represent the byte where the stream the channel
 * is transporting is supposed to start for futher transmissions.
 * 
 */
#define MAX_SEQ_NO     G_GINT64_CONSTANT(4294967295)

/** 
 * @brief Module number used to rotate sequence number for a channel if it overcomes \ref MAX_SEQ_NO.
 *
 * While Vortex Library is operating with a channel, it may happen a
 * channel overcome \ref MAX_SEQ_NO so channel sequence number is
 * rotated using a modulo operation with \ref MAX_SEQ_MOD which is
 * just \ref MAX_SEQ_NO plus 1 unit.
 * 
 * 
 */
#define MAX_SEQ_MOD    G_GINT64_CONSTANT(4294967296)

/** 
 * @brief Maximum number of messages allowed to be sent for a especific channel instance.
 *
 * As defiend by RFC 3080 every channel created cannot repeat message
 * number sent. Every message sent have an unique identifier used to
 * reply to it and to differenciate them from each other.
 *
 * A channel cannot sent more messages that the number defined for
 * MAX_MSG_NO. Once a channel reach that number it cannot be used for
 * any new message. This doesn't mean this channel must be closed. The
 * channel still have pending to receive all replies genereated for
 * every message created.
 *
 */
#define MAX_MSG_NO     2147483647

/**
 * @brief Maximum number of channel allowed to be created inside a VortexConnection.
 * 
 * This number defines how many channels can be created inside a Vortex Connection.
 */
#define MAX_CHANNEL_NO 2147483647


/**
 * @brief The maximum number which can identify a message.
 * 
 */
#define MAX_MESSAGE_NO  2147483647


/**
 * @brief the maximum number of channel which can be created on an connection (or session).
 * 
 */
#define MAX_CHANNELS_NO 2147483647

/**
 * @brief The maximum number to be used on sequence frame payload 
 * 
 */
#define MAX_SEQUENCE_NO 4294967295

typedef enum {
 UPDATE_SEQ_NO = 1 << 0,
 UPDATE_MSG_NO = 1 << 1,
 UPDATE_RPY_NO = 1 << 2
} WhatUpdate;

/**
 * @brief A Vortex Connection object.
 *
 * This object represent a connection inside the Vortex Library. This
 * can be craeted using \ref vortex_connection_new and then validated every times is needed by using \ref vortex_connection_is_ok.
 *
 * Internal \ref VortexConnection representation is not exposed to
 * user code space to ensure the minimal impact while improving or
 * changing Vortex Library internals. 
 * 
 * To operate with a \ref VortexConnection object \ref vortex_connection "check out its API documentation".
 * 
 */
typedef struct _VortexConnection  VortexConnection;

/**
 * @brief A Vortex Frame object.
 *
 * This object represent a frame received or to be sent to remote Vortex (or BEEP enabled) nodes.
 *
 * Internal \ref VortexFrame representation is not exposed to user
 * code space to ensure the minimal impact while improving or changing Vortex Library internals. 
 *
 * To operate with a \ref VortexFrame object \ref vortex_frame "check out its API documentattion".
 * 
 */
typedef struct _VortexFrame       VortexFrame;

/**
 * @brief A Vortex Channel object.
 *
 * This object represents a channel which enables to send and receive
 * data. A channel must be created inside a \ref VortexConnection by using \ref vortex_channel_new. 
 *
 * Internal \ref VortexChannel representation is not exposed to user
 * code space to ensure minimal impact while improving or changing
 * Vortex Library internals.
 *
 * To operate with a \ref VortexChannel object \ref vortex_channel "check out its API documentattion".
 * 
 */
typedef struct _VortexChannel     VortexChannel;

/** 
 * @brief Thread safe hash table based on Glib Hash table intesively used across Vortex Library.
 *
 * Internal \ref VortexHash representation is not exposed to user
 * code space to ensure minimal impact while improving or changing
 * Vortex Library internals.
 *
 * To operate with a \ref VortexHash object \ref vortex_hash "check out its API documentattion".
 * 
 */
typedef struct _VortexHash        VortexHash;

/** 
 * @brief Thread safe queue table based on Glib Queue used across Vortex Library.
 *
 * Internal \ref VortexQueue representation is not exposed to user
 * code space to ensure minimal impact while improving or changing
 * Vortex Library internals.
 *
 * To operate with a \ref VortexQueue object \ref vortex_queue "check out its API documentattion".
 * 
 */
typedef struct _VortexQueue       VortexQueue;

/** 
 * @brief Thread safe semaphore definition based on Glib types used across Vortex Library.
 *
 * This semaphore representation is mainly used to synchronize the
 * Vortex Sequencer and the Vortex Writer. You can check more about this a \ref imp_notes "this section".
 *
 *
 * Internal \ref VortexSemaphore representation is not exposed to user
 * code space to ensure minimal impact while improving or changing
 * Vortex Library internals.
 *
 * To operate with a \ref VortexSemaphore object \ref vortex_semaphore "check out its API documentattion".
 * 
 */
typedef struct _VortexSemaphore   VortexSemaphore;

/** 
 * @brief Vortex Channel Pool definition.
 *
 * The \ref VortexChannelPool is an abstraction vhich allows managing several \ref vortex_channel "channels"
 *  using the same profile under the same \ref vortex_connection "connection". 
 *
 * The \ref VortexChannelPool allows to get better performance reducing the
 * impact of channel creation and channel closing because enforces
 * reusing the channels created in a manner the channels used from the
 * pool every time are allways ready. 
 *
 * Internal \ref VortexChannelPool representation is not exposed to user
 * code space to ensure minimal impact while improving or changing
 * Vortex Library internals.
 *
 * To operate with a \ref VortexChannelPool object \ref vortex_channel_pool "check out its API documentattion".
 * 
 */
typedef struct _VortexChannelPool VortexChannelPool;



struct _WaitReplyData {
	gint           msg_no_reply;
	GAsyncQueue  * queue;
};
/**
 * @brief Wait Reply data used for \ref wait_reply "Wait Reply Method".
 *
 * See \ref wait_reply "this section to know more about Wait Reply Method.
 * 
 */
typedef struct _WaitReplyData WaitReplyData;


// vortex status
/**
 * @brief Vortex Operation Status 
 *
 * This enum is used to represent different Vortex Library status. 
 * 
 */
typedef enum {
	/**
	 * @brief Represents an Error while Vortex Library was operating.
	 *
	 * The operation asked to be done by Vortex Library could be completed. 
	 * 
	 */
	VortexError,
	/** 
	 * @brief Represents the operation have been sucessfully complated.
	 *
	 * The operation asked to be done by Vortex Library have been completed.
	 * 
	 */
	VortexOk


} VortexStatus;


/**
 * @internal
 *
 * This type allows vortex_channel_send_msg and
 * vortex_channel_common_rpy to communicate the sequencer a new
 * message must be sent.  This a private structure for inter thread
 * communication and *MUST NOT* be used by vortex library consumer.
 *
 * 
 */
typedef struct _VortexSequencerData {
	VortexChannel   * channel;
	VortexFrameType   type;
	gint              channel_num;
	gint              msg_no;
	gint              first_seq_no;
	gchar           * message;
	gint              message_size;
}VortexSequencerData;



/**
 * @internal
 *
 * This type is used to transport the message to be sent into the
 * vortex writer process. At the end of the process, the vortex writer
 * relies on vortex_connection_do_a_sending_round to actually send
 * message waiting to be sent in a round robin fashion.
 * 
 * Because some internal vortex process needs to waits (and ensure)
 * for a message to be sent, this type enable that function, the
 * ..do_a_sending_round, to signal a posible thread waiting for a
 * message to be sent.
 */
typedef struct _VortexWriterData {
	VortexFrameType   type;
	gint              msg_no;
	gchar           * the_frame;
	gint              the_size;
}VortexWriterData;

#endif

// @}
