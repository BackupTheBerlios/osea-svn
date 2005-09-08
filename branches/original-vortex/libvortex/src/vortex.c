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
#include <vortex.h>
#include <signal.h>

#define LOG_DOMAIN "vortex"

gboolean     already_started     = FALSE;
GStaticMutex __vortex_exit_mutex = G_STATIC_MUTEX_INIT;
gboolean     __vortex_exit       = FALSE;


void __vortex_sigpipe_do_nothing (int signal)
{
	// do nothing sigpipe handler to be able to manage EPIPE error returned
	// by write. read calls do not fails because we use the vortex reader process
	// that is waiting for changes over a connection and that changes include remote peer
	// closing. So, EPIPE (or receive SIGPIPE) can't happen.
	return;
}

gboolean __vortex_log_handler_check_debug_section (gchar * section)
{
	gchar * sections = getenv ("VORTEX_DEBUG_CLASS");
	if ((section == NULL) || (sections == NULL))
		return TRUE;

	return (g_strrstr (sections, section) != NULL);
}


/** 
 * @brief Allows to get current status for log debug info to console.
 * 
 * @return TRUE if console debug is enabled. Otherwise FALSE.
 */
gboolean vortex_log_is_enabled ()
{
	return (getenv ("VORTEX_DEBUG") != NULL);
}

void __vortex_log_handler (const gchar *log_domain,
			  GLogLevelFlags log_level,
			  const gchar *message,
			  gpointer user_data)
{
	gchar  * filter_modules;
	gchar ** patterns;
	gint     iterator;
	

	// if not VORTEX_DEBUG FLAG, do not output anything
	if (!vortex_log_is_enabled ()) 
		return;

	// get filter module
	filter_modules = getenv ("VORTEX_DEBUG_FILTER");

	// filter list empty or not defined, show all
	if ((filter_modules == NULL) || (log_domain == NULL))
		goto show_log;

	patterns = g_strsplit (filter_modules, " ", 0);
	iterator = 0;
	while (patterns[iterator]) {
		if (g_pattern_match_simple (patterns[iterator], log_domain)) {
			g_strfreev (patterns);
			goto show_log;
		}
		iterator++;
	}
	g_strfreev (patterns);
	return;
	
	// filter defined, search modules list to show
	if ((log_domain && (!g_strrstr (filter_modules, log_domain))))
		return;

	// show module log
 show_log:
	log_domain = log_domain ? log_domain : "";
	switch (log_level) {
	case G_LOG_LEVEL_ERROR:
		if (!__vortex_log_handler_check_debug_section ("error"))
			break;

		if (getenv ("VORTEX_DEBUG_COLOR"))
			g_printerr ("\e[1;36m%s\e[0m:proc %d(\e[1;31merror\e[0m): %s\n", log_domain, getpid (), message);
		else
			g_printerr ("%s:proc %d(error): %s\n", log_domain, getpid (), message);
		break;
	case G_LOG_LEVEL_DEBUG:
		if (!__vortex_log_handler_check_debug_section ("debug"))
			break;

		if (getenv ("VORTEX_DEBUG_COLOR"))
			g_printerr ("\e[1;36m%s\e[0m:proc %d(\e[1;32mdebug\e[0m): %s\n", log_domain, getpid (), message);
		else
			g_printerr ("%s:proc %d(debug): %s\n", log_domain, getpid (), message);
		break;
	case G_LOG_LEVEL_CRITICAL:
		if (!__vortex_log_handler_check_debug_section ("critical"))
			break;

		if (getenv ("VORTEX_DEBUG_COLOR"))
			g_printerr ("\e[1;36m%s\e[0m:proc %d(\e[1;31mcritical\e[0m): %s\n", log_domain, getpid (), message);
		else
			g_printerr ("%s:proc %d(critical): %s\n", log_domain, getpid (), message);
		break;
	case G_LOG_LEVEL_WARNING:
		if (!__vortex_log_handler_check_debug_section ("warning"))
			break;

		if (getenv ("VORTEX_DEBUG_COLOR"))
			g_printerr ("\e[1;36m%s\e[0m:proc %d(\e[1;33mwarning\e[0m): %s\n", log_domain, getpid (), message);
		else
			g_printerr ("%s:proc %d(warning): %s\n", log_domain, getpid (), message);
		break;
	case G_LOG_LEVEL_MESSAGE:
		if (!__vortex_log_handler_check_debug_section ("message"))
			break;
		if (getenv ("VORTEX_DEBUG_COLOR"))
			g_printerr ("\e[1;36m%s\e[0m:proc %d(\e[1;32mmessage\e[0m): %s\n", log_domain, getpid (), message);
		else
			g_printerr ("%s:proc %d(message): %s\n", log_domain, getpid (), message);
		break;
	case G_LOG_LEVEL_INFO:
		if (!__vortex_log_handler_check_debug_section ("info"))
			break;
		if (getenv ("VORTEX_DEBUG_COLOR"))
			g_printerr ("\e[1;36m%s\e[0m:proc %d(\e[1;32minfo\e[0m): %s\n", log_domain, getpid (), message);
		break;
	default:
		g_printerr ("%s:proc %d: %s\n", log_domain, getpid (), message);
	}
	
	return;
}

void __vortex_show_stack (int value)
{
	g_print ("SIGNAL RECEIVED...\n");
	g_on_error_query ("vortex-listener");
}


/**
 * \defgroup vortex Vortex: Main Vortex Library Module (initialization and exit stuff)
 */

/// \addtogroup vortex
//@{



/** 
 * \brief Init vortex library.
 *
 * You should call this first before using and service of the vortex
 * library.
 */
void vortex_init ()
{

	gint thread_num;

	if (already_started)
		return;
	already_started = TRUE;

	if (!g_thread_supported ())
		g_thread_init (NULL);

#ifdef SIGPIPE
	// install sigpipe handler
	signal (SIGPIPE, __vortex_sigpipe_do_nothing);
#endif

	// install log handlers
	g_log_set_default_handler (__vortex_log_handler, NULL);

#ifdef G_OS_WIN32
	// init winsock api
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "init winsocket for windows");
	vortex_win32_init ();
#endif

	// init libXml
	LIBXML_TEST_VERSION
	
	// init dtds
	if (!vortex_dtds_load_channel_dtd ()) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to load dtd files");
		return;
	}

	// init vortex profile
	vortex_profiles_init ();

	// init listener module
	vortex_listener_init ();
	
	// init reader subsystem
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "starting vortex reader..");
	vortex_reader_run ();
	
	// init writer subsystem
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "starting vortex writer..");
	vortex_writer_run ();

	// init sequencer subsystem
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "starting vortex sequencer..");
	vortex_sequencer_run ();

	// init thread pool (for query receiving)
	thread_num = vortex_thread_pool_get_num ();
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "starting vortex thread pool: (%d threads the pool have)..",
	       thread_num);
	vortex_thread_pool_init (thread_num);

	// signal (SIGSEGV, __vortex_show_stack);
	// signal (SIGABRT, __vortex_show_stack);
	return;
}

/**
 * @brief Terminates vortex library function.
 * 
 * Stops all internal vortex process and all allocated resources. It
 * also close all channels for all connection that where not closed
 * until call this function. 
 *
 * This function is reentrant, allowing several threads to call \ref
 * vortex_exit function at the same time. Only one thread will
 * actually release resources allocated.
 *
 **/
void vortex_exit ()
{

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "shutting down vortex library");
	
	g_static_mutex_lock (&__vortex_exit_mutex);
	if (__vortex_exit) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "already shutted down from other thread..");
		g_static_mutex_unlock (&__vortex_exit_mutex);
		return;
	}

	// stop vortex sequencer
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "stoping vortex sequencer");
	vortex_sequencer_stop ();

	// vortex_writer_stop ();

	// vortex_reader_stop ();

#ifdef G_OS_WIN32
	WSACleanup ();
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "shutting down WinSock2(tm) api");
#endif


	// clean up vortex modules
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "shutting down vortex xml subsystem");
	vortex_dtd_cleanup ();

	// Cleanup function for the XML library.
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "shutting down xml library");
	xmlCleanupParser();

	__vortex_exit = TRUE;
	g_static_mutex_unlock (&__vortex_exit_mutex);
	
	// unlock listeners
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unlocking vortex listerners");
	vortex_listener_unlock ();

	return;
}

//@}


/**
 * \mainpage Vortex Library:  A BEEP Core (RFC3080/RFC3081) implementation.
 * \section intro Introduction 
 *
 * <b>Vortex Library</b> an implementation of the <b>RFC 3080 / RFC 3081</b>
 * standard definition for the <b>BEEP Core protocol mapped into TCP/IP</b>
 * layer written in c. It have been developed by <b>Advanced Software
 * Production Line, S.L.</b> (http://www.aspl.es) as a requirement for
 * the <B>Af-Arch</B> project (http://fact.aspl.es).
 *
 * Vortex Library have been implemented keeping in mind security and
 * DOS attack avoidance. It has a consitent and easy to use api which
 * will allow you to write application protocols really fast.
 *
 * The library will ensure the api backward compatibility in the
 * future and, at this moment, it is running on GNU/Linux platforms
 * and Microsoft Windows platforms.
 *
 * The Vortex library is intensively used by the <b>Af-Arch</b> project but
 * as RFC implementation <b>it can be used in a standalone way</b> apart from
 * <b>Af-Arch</b>.
 *
 * The following section represents documents you may find useful to
 * get an idea if Vortex Library is right for you. It talks about
 * features, some insights about its implementation and licenses issues.
 *
 * - \ref implementation
 * - \ref features
 * - \ref status
 * - \ref license
 *
 * The following documents are related to <b>how to use Vortex
 * Library</b>. Installing, using it and some tutorial to get Vortex
 * Library working for you sooner.
 * 
 * - \ref install
 * - \ref starting_to_program
 * - \ref profile_example
 *
 * You can also check the <b>Vortex Library API</b> documentation:
 * - \ref vortex
 * - \ref vortex_connection
 * - \ref vortex_channel 
 * - \ref vortex_frame
 * - \ref vortex_profiles
 * - \ref vortex_listener
 * - \ref vortex_handlers
 * - \ref vortex_channel_pool
 * - \ref vortex_thread_pool
 * - \ref vortex_types
 * - \ref vortex_hash
 * - \ref vortex_queue
 * - \ref vortex_semaphore
 *
 * \section miscellaneous Miscellaneous documents
 *
 * The following section holds all documents that didn't fall into any previous category:
 *
 *  - \ref image_logos
 * 
 * 
 * \section contact_aspl Contact Us
 *
 * You can reach us at the <b>Af-Arch mailing list:</b> at <a href="http://www.aspl.es/fact/index.php?id=26">aspl-fact-users</a>
 * for any question you may encounter. At this moment <b>Vortex
 * Library</b> is an smaller component inside the <b>Af-Arch</b>
 * project so questions about <b>Vortex Library</b> are also handled
 * on that list.
 *
 */

/**
 *
 * \page implementation Vortex Library implementation and dependencies
 *
 * \section dep_notes Vortex Library dependencies
 * 
 * Vortex Library have been implemented using only two dependencies:
 * libglib-2.0 (http://www.gtk.org) (at least 2.6) and libxml-2.0
 * (http://www.xmlsoft.org). They are really common across GNU/Linux
 * platforms and can be found easily for Microsoft Windows Platforms.
 *
 * Additionally, you can enable building <b>vortex-client</b>, a tool
 * which allows to perform custom operations against BEEP peer (not
 * only Vortex ones), by adding a new dependency: readline. 
 *
 * \subsection linux_install Vortex Library dependencies on GNU/Linux Environment
 *
 * In the case of GNU/Linux (unix environment) the package system will
 * provide you the libglib-2.0 and libxml-2.0 development package. On
 * debian system debpackage can be installed using:
 *
 * \code
 *   apt-get install libglib2.0-dev libxml2-dev
 * \endcode
 * 
 * If you know about the exact information about how to install Vortex
 * Library dependencies in your favorite system let us to know it. We
 * will add that information on this manual.
 *
 * \subsection mingw_install Vortex Library dependencies using Mingw Environment
 *
 * On Windows platform you can try to download the libglib-2.0 package
 * provided by Tor Liqvist at http://www.gimp.org/win32 . For
 * libxml-2.0 package we recomend to use the one provided by the
 * gnuwin32 project at http://gnuwin32.sf.net. 
 *
 * Do not use the package provided by Igor Zlatkovic on the same page
 * than libglib-2.0 (http://www.gimp.org/win32). It have a bug that
 * makes Vortex Library to break.
 *
 * Previous package I describe for windows DO NOT have pkg-config file
 * configuration. This means you will have to configure the
 * Makefile.win provided. Check the \ref install "install section" to
 * know more about this.
 *
 *
 * \subsection cygwin_install Vortex Library dependencies using Cygwin Environment
 *
 * If you try to use Vortex Library on Windows platform but using the
 * CygWin environment the previous information is not valid for your
 * case. 
 *
 * Previous package are compiled against the Microsoft Runtime
 * (msvcrt.dll) and every programm or library running inside the
 * cygwin environment is compiled agaisnt (or linked to)
 * cygwin1.dll. If you mix libraries or programms linked to both
 * libraries you will get extrange hangups.
 *
 * On cygwin enviroment use the libraries provided by the cygwin
 * package installing system. Keep in mind that running Vortex Library
 * over Cygwin will force you to chose the GPL license for your
 * application because the cygwin1.dll is license under the GPL. 
 *
 * No propietary development can be done using Cygwin environment.
 *
 * \section imp_notes Notes about Vortex Library implementation
 * 
 * The <b>BEEP Core</b> definition (<b>RFC3080</b> and <b>RFC3081</b>)
 * defines what must or should be supported by any implementation. At
 * this moment the Vortex library support all sections including must
 * and should requirements. But it still lacks of support for sasl
 * propile and profile tls.
 *
 * This means you can use the Vortex Library in production environments
 * because is really stable but, if you require security features
 * such user autentications or secure comunications then the Vortex
 * Library is still not prepare for you.
 *
 * Vortex Library have been build using asynchrouns sockets and a
 * thread model each one working for especific tasks. Once the Vortex
 * Library is started it creates 3 threads apart from the main thread
 * doing the following task:
 *
 * - <b>Vortex Sequencer: </b> its main purpose is to create the
 * package sequencing, split user level message into frames, queue
 * them into channel's buffer to be sent and signal Vortex Writer to do
 * round robin sendings.  This process allows user space to not get
 * blocked while sending message no mather how big they are.
 *
 * - <b>Vortex Writer: </b> its main purpose is to send already
 * created frames by the Vortex Sequencer in a round robing fashion so
 * starvation is avoided.
 *
 * - <b>Vortex Reader: </b> its main purpose is to read all frames
 * for all channels inside all connections. It checks all environment
 * conditions for a frame to be accepted: sequence number sync,
 * previous frame with more fragment flag activated, properly frame
 * format, and so on. Once the frame is accepted the Vortex Reader try
 * to dispatch it using the Vortex Library frame dispatch schema.
 * 
 * Appart from the 3 threads created plus the main one, the Vortex
 * Library creates a pool of threads already prepared to dispatch
 * incoming frames inside user space function.  The thread pool size
 * can be also tweaked.
 *
 * \section dispatch_schema The Vortex Library Frame receiving dispatch schema (or how incoming frames are read)
 *
 * Once a frame is received and validated the Vortex Reader try to
 * deliver it following next rules:
 *
 * - Invoke a second level handler for frame receive event. The second
 * level handler is a user space callback defined per channel. Several
 * channel on the same connections may have different second level
 * handlers (or the same) according to its purpose.
 *
 * - If second level handler for frame receive event where not
 * defined, the Vortex Reader tries to display the frame on the first
 * level handler for frame receive handler event. The first level
 * handler is defined at profile level. This means that channels using
 * the same profiles shares frame receive callback. This allows to
 * defined a general purpose callback at user space for every channel
 * created on every connection.
 *
 * - Finally, it may happen that a thread want to keep on waiting for
 * a especific frame to be received bypassing the second and first
 * level handlers. Its is usefull for that batch process that can't
 * continue if the frame response is not received. This is called \ref wait_reply "wait reply method".
 *
 * The second and first level handler dispatching method are called
 * asynchronous message receving because allows user code to keep on
 * doing other things and be notified only when frames are received.
 *
 * The wait reply method is also called synchronous dispatch method
 * because it blocks the caller thread until the frame is
 * received. The wait reply method disables the second and first
 * level handler execution. 
 *
 * Keep in mind that if no second level handler, first level handler
 * or wait reply method is defiened, the Vortex Reader will drop the
 * frame.
 *
 * As you note the Vortex Library support both method while receiving
 * data: asynchrouns method and synchronous method. This is also
 * applied to sending user data.
 *
 * \section sending_data Sending data method (or how frames can be sent)
 *
 * The Vortex Library also support both model while sending data: the
 * asynchronous and the synchronous.
 *
 * The asynchronous is the default and works in a transparent way for
 * the user space code. Every attempt to send data using the Vortex
 * Library will not block the user caller.  
 *
 * But in contrast the Vortex Library is designed to allow a sender to
 * be blocked until the Vortex Library can ensure the data (no mather
 * how many frames where created for that) have been sent.
 *
 * \section restrictions About restrictions the Vortex Library (DO NOT) have
 *
 * The Vortex Library have been build using another approach from
 * previous BEEP implementation the Af-Arch was using. The main
 * restriction it have disabled (several of them were strong reasons
 * to implement Vortex Library due to Af-Arch requirements):
 *
 * - <b>You can close</b> channels and connections (including that one
 * that is receiving the frame) inside the frame receive handler you
 * may define for both second and first level.
 *
 * - <b>You can reply any message you may want </b> (including the one
 * which have caused to be invoke the frame receive handler) or to
 * send any new message inside the frame receive handler you may
 * define.
 *
 * - <b>The close channel and close connection protocol</b> is fully
 * implemented according to all circumstance that may happen described
 * at RFC3080 standard. This means your code will not be required to
 * implement a close notification between peers in other to avoid
 * sending message that may cause the transport layer to break.  You
 * can actually close any channel (including the complete session)
 * without worry about if remote peer is done.
 * 
 * - <b>Asynchronous model and Synchronous model</b> is supported by
 * the Vortex Library. This means you are not required to use handler
 * or event notifications when you just want to keep on waiting for an
 * especific frame to be received. Of course, while programming
 * Graphical User Interfaces the asynchronous model will allow you to
 * get the better performance avoiding the my-application-gets-blank
 * effect on requests ;-)
 *
 * - The connection creation, channel creation, connection closing
 * and channel closing can be done in a synchronous way <b> but also
 * in an asynchronous way</b>. If you want a library that allows you
 * to start a connection creation process, keep on doing other things
 * and be nothified only when the connection have been created or
 * denied, the Vortex Library is for you.
 * 
 */

/**
 * \page features Vortex Library features
 * 
 * \section fectura_list Some features the Vortex Library have
 * The Vortex Library have some cool features you may find interesting
 * to chose it as your project data transport layer.
 * 
 * - It have a <b>consistent api</b> exposed by a strong function
 * module naming. Vortex Library doesn't expose to your code any
 * detail that may compromise your code in the future due to internal
 * Vortex changes or future releases.  This is really important to us:
 * backward compatibility will be ensured and no change to the
 * internal Vortex Library implementation will break your code.
 * 
 * - It have been <b>designed keeping in mind security</b> about
 * buffer overflows, DOS attacks, internal or external frame fragment
 * attack or channel starvation due to flooding attacks. Actually, It
 * is been used by the Af-Arch project (http://fact.aspl.es) which
 * requires transfering really large chunks of data by several
 * channels over several connection for several clients at the same
 * time.
 *
 * - It <b>supports both asynchrnous and synchronous programming
 * models</b> while sending and receiving data. This means you can
 * programm a handler to be executed on frame receive event (and keep
 * on doing other things) or write code that gets blocked until a
 * especific frame is received.
 *
 * - Operations such as <b>open a connection, create a channel, close a
 * channel can be made in an asynchronous way</b> avoiding you getting
 * blocking until this operation are completed. Of course synchronous
 * model ara also provided.
 *
 * - It support<b> a flexible</b> \ref dispatch_schema "schema" <b>to dispach
 * received frames and close channels</b> allowing you to tweak different
 * levels of handler to be executed including a wait reply method
 * which disables temporaly handlers installed. You can check more
 * about this issue at \ref dispatch_schema.
 *
 * - Vortex Library comes with a tool called <b>vortex-client</b>
 * which allows to make operations against BEEP-enabled peers. This
 * tool can help you on debuging your applications not only running
 * Vortex Library but also other BEEP enabled implementations. It have
 * support to:
 *     -# Create connections, create channels, check status for both:
 *     sequence numbers, next expected sequence numbers, reply
 *     numbers, next expected reply number, messge number status,
 *     profiles supported by remote side, etc...
 *     -# It allows to send message according to actual channel status or
 *     using the frame generator to create frames with user provided data.
 *     -# It allows to generate and send close messages and start message
 *     allowing you to check how your code works.
 *
 * - Vortex Library implements a new concept: <b>the channel
 * pool</b>. This allows you to create a pool of channels ready to be
 * used to send data. The library will handle what channel to use from
 * the pool when the next channel ready is required, avoiding running
 * thread get blocked due to posible previous operations over that
 * channel. It also have the ability to automatically increase the
 * channel pool and negociate for you the channel creation when the
 * next channel ready is requested but no one is ready. This concept
 * have increased the Af-Arch data transmission throutput up to 3
 * times than previous used implementation.
 *
 * - The Vortex Library <b>is well documented</b>, with examples of code, not
 * only about how to use Vortex Library but also how it have been
 * programmed internally. We understand this is a really important
 * point: documentation is the link between humans and tecnology,
 * without it, tecnology doesn't helps.
 *
 * - <b>License under Lesser General Public License</b> allowing to create
 * application not only to those open source project but also
 * propietary ones. See this \ref license "section" for more
 * information about license topic.
 */

/**
 * \page status Vortex Library status
 *
 * \section status_intro Introduction
 * The Vortex Library doesn't implements at this moment the sasl and
 * tls profiles. That's the only part of the standard defined for the
 * BEEP Core protocol at RFC3080 (http://www.ietf.org/rfc/rfc3080.txt) and RFC3081
 * (http://www.ietf.org/rfc/rfc3081.txt) not implemented yet.
 *
 * However the Vortex Library is really stable for production
 * environment. Actually it is been used for several projects developed
 * by Advanced Software Production Line which involves client and
 * servers nodes running GNU/Linux and Microsoft Windows XP platforms.
 *
 * Future release can only add security patches, sasl and tls
 * profiles implementations or bindings to other languages.
 *
 * \section actual_status Actual Vortex Library Status
 *
 * You can check the following to see what point from the RFC 3080 and
 * RFC 3081 are already covered by the Vortex Libray: 
 *
 * \verbinclude vortex-status.txt
 */

/**
 * \page license License issues
 *
 * \section licence_intro Vortex Library terms of use
 * The Vortex Library is release under the terms of the Lesser General
 * Public license (LGPL). You can find it at
 * http://www.gnu.org/licenses/licenses.html#LGPL.
 *
 * The main implication this license have is that you are allowed to
 * use the Vortex Library for commercial application as well on open
 * source application using GPL/LGPL compatible licenses. 
 *
 * Restrictions for propietary development are the following: 
 *
 * - You have to provide back to the main repository or to your
 * customers any change, modifications, improvements you may do to the
 * Vortex Library. Of course, we would appreciate to add to the main
 * Vortex Library repository any patch, idea or improvement you send
 * us but you are not required to do so.
 *
 * - You cannot use our company name or image, the Vortex Library name
 * or any trademark associated to that product or any product the
 * Advanced Software Production Line company may have in a way that
 * they could be damaged. Of course, we would appreciate your project, in
 * the case it is a propietary one, make a reference to us but you are
 * not required to do so. 
 *
 * \section other Getting Vortex Library under different license
 *
 * Additionally you can contact us if you are interested on getting
 * Vortex Library under other license than the LGPL. Use the following
 * contact information to reach us about this issue. 
 * 
 * \code
 *      Postal address:
 *         Advanced Software Production Line, S.L.
 *         C/ Dr. Michavila Nº 14
 *         Coslada 28820 Madrid
 *         Spain
 *      Email address:
 *         info@aspl.es
 *      Fax and Telephones:
 *         (00 34) 91 669 55 32 - (00 34) 91 231 44 50
 *         Pleope from outside Spain must use (00 34) prefix.
 * \endcode
 * 
 *
 */


/**
 * \page install Installing and Using Vortex Library
 * \section Index
 *  On this page you will find:
 *   - \ref vortex_source
 *   - \ref vortex_from_svn
 *   - \ref compile_linux
 *   - \ref compile_mingw
 *   - \ref using_linux
 *   - \ref using_mingw
 *
 * \section vortex_source Getting latest stable release 
 *
 * The <b>Vortex Library main repository</b> is included <b>inside the Af-Arch</b>
 * repository. Af-Arch project uses Vortex Library as its data
 * transport layer.  
 *
 * If you are planing to download Vortex Library and using it as a
 * standalone library apart from Af-Arch you can <b>get current stable
 * releases</b> at: http://www.sourceforge.net/projects/vortexlibrary
 * 
 * \section vortex_from_svn Getting latest sources from svn repository
 * 
 * You can also get the latest repository status, which is considered
 * to be stable, by checking out a svn copy ejecuting the following:
 *
 * \code
 *   svn co https://dolphin.aspl.es/svn/publico/af-arch/trunk/libvortex
 * \endcode
 * 
 * If you are using Microsoft Windows platform use the url
 * above. SubVersion client programs for windows can be found at:
 * http://tortoisesvn.tigris.org (really recommended) or
 * http://rapidsvn.tigris.org.
 *
 * Of course you will have to download the Vortex Library
 * dependencies. Check the \ref dep_notes "this section" to know
 * more about library dependencies.
 *
 * \section compile_linux Compiling Vortex Library on GNU/linux enviroments (including Cygwin)
 *
 * If you are running a POSIX (unix-like incluing cygwin) environment you can use
 * autotools to configure the project so you can compile it. Type the
 * following once you have downloaded the source code:
 *
 * \code
 *   bash:~$ cd libvortex
 *   bash:~/libvortex$ ./autogen.sh
 * \endcode
 *
 * This will configure your project trying to find the dependencies
 * needed. 
 *
 * Once the configure process is done you can type: 
 * 
 * \code
 *   bash:~/libvortex$ make install
 * \endcode
 *
 * The previous command will required permissions to write inside the
 * destination directory. If you have problems try to execute the
 * previous command as root.
 *
 * Because readline doesn't provide an standard way to get current
 * installation location, the following is provided to configure
 * readline installation. You have to use the <b>READLINE_PATH</b>
 * environment var as follows:
 * 
 * \code
 *   bash:~/libvortex$ READLINE_PATH=/some/path/to/readline make
 * \endcode
 *
 * <b>make</b> program will use the content of <b>READLINE_PATH</b> var to build an
 * include header directive and an include lib directive as follows:
 * \code
 *   READLINE_PATH=/some/path/to/readline
 *
 *   -I$(READLINE_PATH)/include
 *   -L$(READLINE_PATH)/lib
 * \endcode
 *
 * You <b>don't need to pay attention to this description if you don't
 * have problems with your readline</b> installation.
 * 
 * 
 *
 * \section compile_mingw Compiling Vortex Library on Windows Platforms using Mingw
 * 
 * On Windows system inside the src directory can be found a
 * Makefile.win file. This Makefile.win have been created to be run
 * under mingw environment. You will have to download the mingw
 * environment, the msys package and optionally the pkg-config program. 
 *
 * If you open the Makefile.win you will find the Makefile.win can
 * find the library dependencies throught pkg-config program or by a
 * directory provided by you. 
 *
 * The pkg-config method is preferred because is easy to use but it
 * have no diference (or special effect) on source code if you provide
 * the directory where libraries can be found. 
 *
 * If you chose the pkg-config method you have to ensure the
 * Makefile.win will find the include flags and import libraries while
 * executing:
 *
 * \code
 *   pkg-config --cflags --libs glib-2.0 libxml-2.0
 * \endcode
 *
 * Once you have ensured the pkg-config will provide the right
 * references to the Makefile.win type the following:
 * \code
 *  CC=gcc-3.3 vortex_dll=libvortex MODE=console make -f Makefile.win
 * \endcode
 *
 * Of course, the <b>CC</b> variable may point to another gcc, check the one
 * that is installed on your system but, make sure you are not using
 * the gcc provided by a cyginw installation. It will produce a faulty
 * libvortex.dll not usable by any native Microsoft Windows program.
 *
 * The <b>MODE</b> variable can be set to "windows". This will disable the
 * console output. "console" value will allow to enable vortex log
 * info to console.
 * 
 * The <b>vortex_dll</b> variable must not be changed. This variable is
 * controled by a top level Makefile.win inside the Af-Arch project so
 * that Makefile.win can control the library naming. To follow the
 * same convention naming will save you lot of problems in the future.
 *
 * Additionally, if you chose to provide the libraries directly you
 * will have to download the libraries the \ref dep_notes "Vortex
 * Library depends on" inside a directory, for example,
 * <i>"c:\libraries"</i>. Use lowercase letter for your files or directory, it
 * will save you lot of time. Now, edit the Makefile.win to comment
 * out those lines refering to pkg-config (LIBS and CFLAGS) and
 * uncomment those lines refering to BASE_DIR variable.
 *
 * Now type:
 * \code
 *  CC=gcc-3.3 vortex_dll=libvortex MODE=console BASE_DIR=c:/libraries make -f Makefile.win
 * \endcode
 *
 * This process will produce a libvortex.dll (actually the dinamic
 * libraries) and a import librariy called libvortex.dll.a. The import
 * library will be needed to compile your application under windows
 * against Vortex Library so it get linked to libvortex.dll.
 * 
 * \section using_linux Using Vortex on GNU/Linux platforms (including Cygwin)
 * 
 * Once you have installed the library you can type: 
 * \code
 *   gcc `pkg-config --cflags --libs vortex` your-program.c -o your-program
 * \endcode
 *
 * On windows platform using cygwin the previous example also
 * works. 
 *
 * \section using_mingw Using Vortex on Microsoft Windows with Mingw
 *
 * On mingw environments you should use something like:
 *
 * \code
 *  gcc your-program.c -o your-program
 *        -Ic:/libraries/include/vortex/  \
 *        -I"c:/libraries/include" \
 *        -I"c:/libraries/include/glib-2.0" \
 *        -I"c:/libraries/include/libxml2" \
 *        -I"c:/libraries/lib/glib-2.0/include" -I"." 
 *        -L"c:/libraries/lib" \
 *        -L"c:/libraries/bin" \
 *        -L"c:/libraries/lib/glib-2.0" \
 *        -lws2_32 \
 *        -lglib-2.0 \
 *        -lgmodule-2.0 \
 *        -lgobject-2.0 \
 *        -lgthread-2.0 \
 *        -lxml2 \
 *        -lintl  -liconv  -lm 
 * \endcode
 *
 * Where <b>c:/libraries</b> contains the installation of the Votex
 * Library (headers files installed on c:/libraries/include/vortex,
 * import library: libvortex.dll.a and dll: libvortex.dll), glib-2.0
 * installation and libxml-2.0 installation.
 *
 * Because <b>glib-2.0</b> also depends on libiconv and gettext you will have
 * to provide the -lintl and -liconv flags. The <b>-lws2_32</b> will provide
 * winsocks2 reference to your program.
 */

/** 
 * \page starting_to_program Vortex Library Manual
 * \section concepts Some concepts before starting to use Vortex
 * 
 * First of all some definitions about the protocol that <b>Vortex Library</b>
 * implements. This will help you to understand why Vortex Library
 * uses some names to refer to such things as: <b>frames, channels,
 * profiles</b>, etc.
 *
 * <b>Vortex library is an implementation of the RFC 3080/RFC 3081 protocol</b>,
 * also known as <b>BEEP: Block Extensible Exchange Protocol</b>. In the past it
 * was called as BXXP because the Xs of e(X)tensible and e(X)change. Due
 * to some marketing naming decision finally it was called BEEP because
 * the Es of the (E)xtensible and (E)xchange.
 *
 * \code
 *    BEEP:   the protocol
 *    Vortex: the implementation
 * \endcode
 *
 * From a simple point of view, the BEEP protocol defines how data
 * must be exanged between applications using some abstractions that
 * allows (or it should) programmers to write network applications
 * with stronger features such as asynchronous comunication, several
 * and concurrent message exchange over the same connection and so on,
 * without worring about to much on details.
 * 
 * The previous abstraction defined by the BEEP RFC are really
 * important. To understand the follow is a key to understand not only BEEP
 * but also Vortex library (and every BEEP implementation). 
 * 
 * \code
 *       BEEP abstraction layer
 *       -----------------------
 *       |       Message       |
 *       -----------------------
 *       |        Frame        |
 *       -----------------------
 *       |  Channel / Profiles |
 *       -----------------------
 *       |       Session       |
 *       -----------------------
 * \endcode
 * 
 * Previous table <b>is not the BEEP api stack</b> or the <b>Vortex library api
 * stack</b> but it represent the concepts you must use to be able to send and
 * receive data throught <b>BEEP</b>.
 * 
 * A <b>message is actually your application data</b> to be sent to a remote
 * peer or waiting to be received. It has no special meaning for a <b>BEEP</b>
 * implementation. Application using BEEP is the one who finally pay
 * attention to message format, correction and content meaning.
 * 
 * When you send a message (or a reply) <b>this messages are splited
 * into frames</b>. This frames have special info about payload
 * sequence, message type, channel used and many things more. BEEP
 * peers are able to keep track on comunication, error detection, sync
 * lost, etc due to this data. Under <b>Vortex Library</b> frames are
 * user transparent generated and check so application level only needs to use them.
 * 
 * When <b>you send a message, you select a channel to do this
 * comunication</b>. We have seen a message is actually splitted but from the
 * aplication view this is not important: <b>applications send messages over
 * channels</b>. 
 * 
 * Inside BEEP protocol definition, <b>every created channel</b> must
 * be done <b>using a profile definition</b>. This simply concept,
 * which usually confuse programers new to BEEP, is not anything
 * extrange or special. A profile <b>only defines what type of
 * messages will be exchanged</b> inside a channel.  It is just a way
 * of say to remote peers: <i>"Hey I'm a BEEP peer supporting the
 * following profiles"</i>. 
 * 
 * Actually, <b>to support a profile</b> means you have registered the
 * string which identifies the profile and you have implemented over
 * the Vortex Library that profile so you send and receive
 * message according to the format the profile defines. <b>A profile
 * inside Vortex Library is not</b>: a dll you have to implement or a
 * plugin or a set of especial callbacks.
 * 
 * In order to use the Vortex library and any BEEP implementation you
 * must define your own profile. Later on you can read: \ref profiles "defining a profile inside Vortex Library".
 * 
 * The last one concept to understand is <b>the BEEP
 * session</b>. According to BEEP RFC, a session is an abstraction
 * which allows to hold all channels created against a remote BEEP
 * peer (no mather what profiles where used). Because Vortex Library
 * is a BEEP implementation mapped into TCP/IP, a session is actually
 * a TCP connection (with some aditional data).
 * 
 * Now we know most of the concepts involving BEEP here goes how this
 * concepts get mapped into a concreate example using Vortex. Keep in
 * mind this is a simplyfied version on how could be used Vortex Library.
 * 
 * In order to send a message to a remote peer you'll have to create a Vortex
 * Connection to the remote peer.
 * 
 * \code
 *        gchar * host = "myhost.at.frobnicate.com";
 *        gchar * port = "55000";
 * 
 *        // Creates a Vortex Connection without providing a
 *        // OnConnected handler or user data. This will block us
 *        // until the connection is created or fails.
 *        VortexConnection * connection = vortex_connection_new (host, port, NULL, NULL);
 * \endcode
 * 
 * Once finished, you have actually created a BEEP session. Then
 * you'll have to create a Vortex Channel using a profile defined by you
 * or an existing one. Then we have to chose the channel number, let's
 * say we want to create the channel 2.
 * 
 * \code
 *        // Create a Vortex Channel over the given connection and using as channel number: 2.
 *        VortexChannel * new_channel = NULL;
 *        new_channel = vortex_channel_new (connection, 2,
 *                                          "http://my.profile.com",
 *                                          // do not provide on
 *                                          // channel close handlers.
 *                                          NULL, NULL, 
 *                                          // provide frame receive
 *                                          // handler (second level one)
 *                                          // Now, on_receiving_data
 *                                          // will be executed for every
 *                                          // frame received on this
 *                                          // channel unless wait reply
 *                                          // method is used.
 *                                          on_receiving_data, NULL,
 *                                          // do not provide a
 *                                          // OnChannelCreated
 *                                          // handler. This will block
 *                                          // us until the channel is
 *                                          // created.
 *                                          NULL, NULL);
 * \endcode
 * 
 * Once the channel is created (the remote peer have to accept its
 * creation because it is posible to deny its creation) you could send
 * messages to remove peer using this channel as follows:
 * 
 * \code
 *       // send a relly import message to remote peer.
 *       if (vortex_channel_send_msg (new_channel,
 * 				   "this a message to be sent",
 * 				   25,
 * 				   NULL)) {
 * 	   g_print ("Okey, my message was sent");
 *       }
 * \endcode
 * 
 * And finally, now you don't need the channel you close it.
 * 
 * \code
 *      // close the channel. This will block us until the channel is closed because
 *      // to close a channel can actually take longer time because the remote peer
 *      // may not accept close request until he is done.
 *      if (vortex_channel_close (new_channel, NULL)) {
 * 	   g_print ("Okey, my channel have been closed");
 *      }
 *
 *      // finally, finalize vortex running
 *      vortex_exit ();
 * \endcode
 * 
 *
 * That's all. You have created a simple Vortex Library client that
 * have connected, created a channel, send a message, close the
 * channel and terminated vortex running.
 * 
 * \section listener How a Vortex Listener works (or how to create one)
 *
 * To create a vortex listener, which waits for incomming beep connection
 * on a given port the following must be done:
 * 
 * \code  
 *   #include <vortex.h>
 *
 *   void on_ready (gchar * host, gint port, VortexStatus status,
 *                  gchar * message, gpointer user_data) {
 *        if (status != VortexOk) {
 *              g_print ("Unable to initlize vortex listener: %s\n", message);
 *              // do not exit from here using: exit or vortex_exit. This is actually
 *              // done by the main thread
 *        }
 *        g_print ("My vortex server is up and ready..\n");
 *        // do some stuff..
 *   }
 *
 *   gint main (gint argc, gchar ** argv) {
 *       // init vortex library
 *       vortex_init ();
 * 
 *       // register a profile
 *       vortex_profiles_register (SOME_PROFILE_URI,	
 *                                 // provide a first level start
 *                                 // handler (start handler can only be
 *                                 // provided at first level)
 *                                 start_handler, start_data, 
 *                                 // provide a first level close handler
 *                                 close_handler, close_data,
 *                                 // provide a first level frame receive handler
 *                                 frame_received_handler, frame_received_data);
 * 
 *       // create a vortex server using any name the running host may have.
 *       // the on_ready handler will be executed on vortex listener creation finish.
 *       vortex_listener_new ("0.0.0.0", "44000", on_ready);
 * 
 *       // wait for listeners
 *       vortex_listener_wait ();
 *
 *       // finalize vortex running
 *       vortex_exit ();
 *
 *        return 0;
 *   }
 *
 * \endcode
 * 
 * This four steps does the follow:
 *   <ul>
 *   <li>Initialize \ref vortex_init library and its allways running subsystem,
 *      such as vortex reader (a thread which is used to register threads to
 *      be watched waiting to incomming data). If vortex_init function is
 *      not called, unexpected behaviours will be get.</li>
 *
 *   <li>Register one (or more profiles) the listener being
 *      created will accept as valid profiles to create new channel over
 *      session using \ref vortex_profiles_register.</li>
 *
 *   <li>Create the listener using \ref vortex_listener_new, especifying the hostname to be used to listen
 *      incomming connection and the port. If hostname used is 0.0.0.0 all
 *      hostname found will be used. If 0 is used as port, an automatic
 *      port asigned by the SO will be used.</li>
 *
 *   <li>Finally, call to wait listener created using \ref vortex_listener_wait.</li>
 *   </ul>
 *
 * On the 2) step, which register a profile called SOME_PROFILE_URI to
 * be used on channel creation, it also set handlers to be called on
 * events happening for this listener.
 * 
 * This handlers are: <b>start handler</b>, <b>close handler</b>, and
 * <b>frame_received</b> handler. 
 * 
 * The <b>start handler</b> is executed, if defined, to notify that a
 * new petition to create a new channel over some session have
 * arrived. But this handler is also executed to know if vortex
 * listener agree to create the channel. If start handler returns TRUE
 * the channel will be created, otherwise not.
 * 
 * If you don't define a start handler a default one will be used
 * which allways returns TRUE. This means: all channel creation
 * petition will be accepted.
 * 
 * The <b>close handler</b> works the same as start handler but close is
 * executed to notify if a channel can be closed. Again, if close handler
 * returns TRUE channel will be closed, otherwise not.
 *
 * If you don't provide a close handler a default one will be used
 * which allways returns TRUE: all channel close petition will be
 * accepted.
 * 
 * The <b>frame_recevied handler</b> is executed to notify a new frame
 * have arrived over a channel. The frame before been delivered, have
 * been verified to be ok and so user space code doesn't have to check
 * anything.
 * 
 * All notification are run on newly created threads that are
 * ongoing threads created inside a thread pool. 
 *
 * You can use the <b>vortex-client</b> tool to check the Vortex
 * server listed above.
 * 
 * \section client How a vortex client works (or how to create a connection)
 * 
 * A vortex client peer works in a pretty different fashion than
 * listener. That's obvious, they have different function. In order to
 * connect to a vortex listener server (or a BEEP enabled peer) a
 * vortex client peer have to:
 * 
 * \code
 *   #include <vortex.h>
 *   
 *   gint main (argc gint, gchar ** argv) {
 *
 *     // init vortex library
 *     vortex_init ();
 * 
 *     // connect to remote vortex listener
 *     connection = vortex_connection_new (host, port, on_connected_handler);
 * 
 *     // check if everything is ok
 *     if (!vortex_connection_is_ok (connection, FALSE)) 
 *            g_print ("Connection have failed: %s\n", 
 * 		    vortex_connection_get_message (connection));
 *            vortex_connection_close (connection);
 *     }
 *   
 *     // connection ok, do some stuff
 *     
 *   }
 *   
 * \endcode
 * 
 * Previous steps stands for:
 *   <ul>
 *   
 *   <li> Initialize Vortex Library calling to \ref vortex_init. As we have
 *   see on vortex listener, if vortex library is not initialized
 *   unexpected behaviours will occur.</li>
 *    
 *   <li>Connect to remote peer located at host and port using
 *   \ref vortex_connection_new. This function will create a BEEP session
 *   (actually a connection with some additional info) to remote
 *   site. 
 *
 *   This function will block the caller until connection is created
 *   if you don't provide a on_connected_handler function (that is,
 *   passing a NULL value).
 *   
 *   This makes code easy to understand but, using on user interfaces
 *   this is not a good option.  If you provide a on_connected_handler
 *   the function will not block the caller and will do the connection
 *   process in a separated thread. Once the connection process have
 *   been finished vortex library will notify on the defined handler
 *   allowing caller thread to keep on doing other stuff than waiting
 *   for a connection.  
 *
 *   This initial connection creates not only a BEEP session it
 *   also creates the channel 0. This channel is used for session
 *   management functions such as new channel creation. </li>
 *
 *   <li>Finally, returned connection must be checked using \ref vortex_connection_is_ok. 
 *   This step must be done on both model: on block model and on
 *   threaded model.</li> </ul>
 * 
 * Once a vortex connection is successfully completed, is registered
 * on Vortex Reader thread. This allows Vortex Reader thread to
 * process and dispach all incoming frame to its correspondient
 * channel handler.
 * 
 * We have talked about channel handlers previously: the start, close
 * and frame received handler. Due to client peer nature, it will be
 * common to not pay attention to start and close events. If no
 * handler defined, default ones will be used. Of cource, if it is
 * needed to have more control over this process, event handler must
 * be defined.
 * 
 * The frame received handler must be defined for each channel
 * created. If no frame received handler is defined for a channel
 * used by a vortex client, virtually you won't receive any
 * frame. 
 *
 * This is because a vortex client is not required to register a
 * profile before creating Vortex Connections. Of course, if you
 * register a profile with the handlers before creating the connection
 * those ones will be used if not handlers are provided on channel
 * creation. See \ref dispatch_schema "this section" to understand how
 * the frame dispatch schema works.
 * 
 * \section sending_frames How an application must use Vortex Library to send and receive data
 * 
 * As defined on RFC 3080, any BEEP enabled aplication should define a
 * profile to be used for its message exchange and use one of the
 * message-exchange style defined. There are 3 message exchange style.
 * 
 * <ul>
 *
 * <li> <b>MSG/RPY</b>: this is a one-to-one message exchange style
 * and means a beep peer sends a message MSG and remote peer perform
 * some task to finally responds using a RPY message type.</li>
 *
 * <li><b>MSG/ERR</b>: this is a one-to-one message exchange style and
 * means the same as previous message exchange but remote peer have
 * responded with an error, so the task was not performed.</li>
 *
 * <li><b>MSG/ANS</b>: this is a one-to-many message exchange style
 * and works pretty much as MSG/RPY definition but defined to allows
 * remote peer to keep on send ANS frames during the task execution.</li>
 *
 * </ul>
 * 
 * While using Vortex Library you can send data to remote peer using three
 * function defined at the vortex channel api. 
 * 
 * \code
 * 1) vortex_channel_send_msg
 * 
 * 2) vortex_channel_send_rpy
 * 
 * 3) vortex_channel_send_err
 * \endcode
 *
 * As you may observe to generate the different types of message to be
 * sent a function is provided: 
 *
 * The first one allows you to send a new message. Once the message is
 * queued to be sent the function returns you the message number used
 * for this sending attempt. This function never block and actually do
 * not send the message directly. It just signal the Vortex Sequencer
 * to do the frame sequencing which finally will make Vortex Writer to
 * send the frames generated.
 * 
 * The second function allows to positive reply to a especific message
 * received. In order to be able to perform a positive reply using
 * vortex_channel_send_rpy or a negative reply using
 * vortex_channel_send_err you have to provide the message number to
 * reply to.
 * 
 * The third function allows to reply an error to a especifi message
 * recieved. As the previous function it is needed the message number
 * to reply to.
 * 
 * Things that cannot be done by Vortex applications is to send msg to
 * each other without using reply message. Actually you can use only
 * MSG type message to send data to other vortex (or beep) enable
 * application but this is not the point. Application have to think
 * about MSG type as a request message and RPY as a request reply
 * message. The point is: <b>do not use MSG to reply message received,
 * use RPY type.</b>
 *
 * Additionally, there are also function versions for the previous ones
 * which accepts a variable argument list so you can send message in a
 * printf like fashion.
 *
 * \code
 * 
 *   if (!vortex_channel_send_msgv (a_channel, NULL, 
 *                                  "Send this message with content: %s and size: %d", 
 *                                  content, content_size)) {
 *         g_print ("Unable to send my message\n");
 *   }
 * \endcode 
 * 
 * They are the same function names but appending a "v" at the end.
 * 
 * \section wait_reply Sending data and wait for a especific reply (or how get blocked until a reply arrives)
 *
 * We have see in previous section we can use several function to send
 * message in a non-blocking fashion no matter how big the message is:
 * calling to those function will never block. But, what if it is
 * needed to get blocked until a reply is received.
 * 
 * Vortex Library defines a <b>wait reply method</b> allowing to bypass the
 * second and first level handlers defined \ref dispatch_schema "inside the frame received dispatch schema".
 * 
 * Here is an example of code on how to use Wait Reply method:
 * 
 *  \code
 *   
 *    VortexFrame   * frame;
 *    gint            msg_no;
 *    WaitReplyData * wait_reply;
 * 
 *    // create a wait reply 
 *    wait_reply = vortex_channel_create_wait_reply ();
 *     
 *    // now send the message using msg_and_wait/v
 *    if (!vortex_channel_send_msg_and_wait (channel, "my message", 
 *                                           strlen ("my message"), 
 *                                           &msg_no, wait_reply)) {
 *        g_print ("Unable to send my message\n");
 *        vortex_channel_free_wait_reply (wait_reply);
 *    }
 *
 *    // get blocked until the reply arrives, the wait_reply object
 *    // must not be freed after this function because it already free it.
 *    frame = vortex_channel_wait_reply (channel, msg_no, wait_reply);
 *    if (frame == NULL) {
 *         g_print ("there was an error while receiving the reply or a timeout have occur\n");
 *    }
 *    g_print ("my reply have arrived: (size: %d):\n%s", 
 *             vortex_frame_get_payload_size (frame), 
 *             vortex_frame_get_payload (frame));
 *
 *    // that's all!
 * 
 *  \endcode
 * 
 * 
 * \section invocation_chain Invocation level for frames receive hander
 * 
 * Application designer have to keep in mind the following invocation
 * order for frame received handler:
 * <ul>
 *
 * <li> First of all, if the frame received is a reply one, that is, a
 * RPY or ERR frame, then Vortex Library look up for waiting thread
 * blocked on vortex_channel_wait_reply. If found, the frame is
 * delivered and invocation chain is stopped.</li>
 *
 * <li>If previous lookup was not sucessfull, vortex search for a
 * second level handler defined for the channel which is receiving the
 * frame. This second level handler have been defined by
 * vortex_channel_new at channel creation time. If frame received
 * handler is found for this level the invocation chain is
 * stopped.</li>
 *
 * <li>If previous lookup was not successfull, vortex search for a
 * first level handler. This handler have been defined by
 * vortex_profiles_register at profile registration.  If frame
 * received handler is found for this level the invocation chain is
 * stopped.</li>
 *
 * <li>Finally, if vortex do not find a way to deliver frame received,
 * then it is dropped (freeing the frame resources and registering a
 * log message)</li>
 *
 * </ul>
 *
 * As a consequence:
 * <ul>
 *
 * <li>If an application don't define any handler then it will only be
 * able to receive frames only throught the \ref
 * vortex_channel_wait_reply function. And of course, this function
 * only allows to receive replies frames so, any message received that
 * is not a reply will be dropped.</li>
 * 
 * <li>If an application defines the first level handler (using
 * \ref vortex_profiles_register) this handler will be executed for all
 * frames received for all channel defined under the profile
 * selected.</li>
 * 
 * <li>If an application defines the second level handler (using
 * \ref vortex_channel_new) this handler will be executed for all frames
 * received for a particular channel, the one created by
 * \ref vortex_channel_new which received the frame received handler.</li>
 *
 * </ul>
 * 
 * \section profiles Defining a profile inside Vortex (or How profiles concept confuse people)
 * 
 * Now we have to consider to spend some time learning more about
 * profiles. The profile concept inside the BEEP Core definition is
 * the most simple but at the same time the most confusing. Maybe
 * because the RFC doesn't define it in a clear and straightforward
 * way or maybe because profile sounds like something really dificult.
 *
 * Considerations apart, a profiles actually is a document that you
 * should define if you want your profile to be recognized and
 * registered by the IANA. In fact, the RFC 3080 section 6.4 have a
 * template form you should fill to define your profile and send it to
 * the IANA so they can add it to the BEEP profile track list.
 *
 * From the source code point of view, a profile only means:
 * <ul>
 *
 *  <li>To register a profile uri using \ref vortex_profiles_register and ...</li> 
 * 
 * <li>To implement the protocol described at the profile starting to
 * implement those pieces that face the send message description and
 * the receive message description:
 *
 *   <ul>
 * 
 *   <li>On frame receive handler, the programmer should implement the
 * part concerning about message to be received, errors to be detected
 * and so on.</li>
 *
 *   <li>And, while sending new messages or replies, over Vortex
 * Library (but not inside it) the programmer should implement the
 * protocol concerning about sending messages or replies.</li>
 *
 *  </ul>
 *  </li>
 * </ul>
 *
 * In other words, because the profile is only a definition on how you
 * should send and reply message and what types of message you will
 * have to recognize or what will happen on some circumstances it is
 * only needed by you to register the profile name and to implement
 * that behaviour on top of the Vortex Library to fulfill the profile
 * especification.
 *
 * Maybe the main problem a BEEP programmer have to face is the fact a
 * BEEP implementation doesn't allows him to starting to send and
 * receive message directly.
 *
 * This is because <b>the BEEP definition and Vortex Library
 * implementation is more a toolkit to build application protocols
 * than a application protocol itself</b>.
 *
 * Now see the tutorial about \ref profile_example "creating a simple profile" involving a simple server
 * creation with a simple client.
 */

/** \page profile_example Implementing a profile tutorial
 * 
 * \section profile_example_intro Introduction
 *
 * We are going to create a simple server which implements a simple
 * profile defined as: <i>every message it receives, is replyed with the payload
 * received appending "Received OK: ".</i>
 *
 * Additionally, we are going to create a simple client which
 * tries to connect to the server and then send a message to him
 * printing to the console the server's reply.
 *
 * The profile implemented is called
 * <i>"http://fact.aspl.es/profiles/plain_profile"</i>. While
 * implementing a profile you must chose a name based on a uri
 * style. You can't chose already defined profiles name so it is a
 * good idea to chose your profile name appending as prefix your
 * project name. An example of this can be:
 * <i>"http://your.project.org/profiles/profile_name"</i>.
 *
 * See how the <i>plain_profile</i> is implemented to get an idea on
 * how more complex, and useful profiles could be implemented.
 *
 * \section profile_example_server Implementing the server.
 *
 * First, we have to create the source code for the server:
 *
 * \include vortex-simple-listener.c
 * 
 * As you can see, the server code is fairly easy to understand. The following steps are done:
 * <ul>
 *  <li>First of all, Vortex Library is initialized using \ref vortex_init.</li>
 *
 *  <li>Then, <i>PLAIN_PROFILE</i> is registered inside Vortex Library
 *  using \ref vortex_profiles_register.  This means, the vortex
 *  listener we are building will recognize peer wanting to create new
 *  channels based on <i>PLAIN_PROFILE</i>.
 *
 *  <i><b>NOTE:</b> on connection startup every BEEP listener must
 *  report what profiles support. This allows BEEP initiators to
 *  figure out if the profile requested will be supported. Inside the
 *  Vortex Library implementation the registered profiles using \ref
 *  vortex_profiles_register will be used to create the supported
 *  profiles list sent to BEEP initiators.  
 *  
 *  The other interesting question the BEEP Core definition have is
 *  that BEEP initiators, the one which is actually connecting to a
 *  listener doesn't need to report its supported profiles. As a
 *  consecuence, you can create a Vortex Client connecting to a remote
 *  server without registering a profile. 
 *
 *  Obviously, this doesn't means you are not required to implement
 *  the profile, it just means you are not required, having the
 *  profile implemented, to register it against the vortex profiles
 *  api.</i>
 *
 *  While registering a profile, Vortex Library will allow you to
 *  register several callbacks to be called on event such us channel
 *  start, channel close and frame receive during the channel's
 *  life. This event will be called (actually registered handlers)
 *  only for those channels working under the semantic of PLAIN
 *  PROFILE.
 *
 *  As a conclusion, you can have several profiles implemented, having
 *  several channels opened on the same connection <i>"running"</i>
 *  different profiles at the same time.</li>
 *  
 *  <li>Next, a call to \ref vortex_listener_new creates a server
 * listener prepared to receive connection to any name the host may
 * have listening on port 44000. In fact, you can actually perform several
 * calls to \ref vortex_listener_new to listen several port at the
 * same time.</li>
 *
 *  <li>Before previous call, it is needed to call \ref vortex_listener_wait
 *  to block the main thread until Vortex Library is finished.</li>
 * 
 *  </ul> 
 *  
 *  That's all, you have created a Vortex Server supporting a
 *  user defined profile which replies to all message received
 *  appending "Received OK: " to them. To compile it you can \ref install "check out this section".
 *
 * \section profile_example_testing_server Testing the server created using the vortex-client tool and telnet command.
 *
 *  Now, you can run the server and test it using a telnet tool to
 *  check some interesting things. The output you should get should be
 *  somewhat similiar to the following:
 *
 * \code
 * (jobs:1)[acinom@barbol test]
 * $ ./vortex-simple-listener &
 * [2] 7397
 * 
 * (jobs:2)[acinom@barbol test]
 * $ telnet localhost 44000
 * Trying 127.0.0.1...
 * Connected to localhost.
 * Escape character is '^]'.
 * RPY 0 0 . 0 128
 * Content-Type: application/beep+xml
 * 
 * <greeting>
 *    <profile uri='http://fact.aspl.es/profiles/plain_profile' />
 * </greeting>
 * END
 * \endcode
 *
 * As you can see, the server replies inmediatly its availability
 * reporting the profiles it actually support. Inside the greeting
 * element we can observe the server support the <i>PLAIN_PROFILE</i>.
 *
 * Before starting to implement the vortex client, we can use
 * <b>vortex-client</b> tool to check our new server. Launch the
 * <b>vortex-client</b> tool and perform the following operations.
 *
 * <ul>
 *
 * <li> First, connect to the server located at localhost, port 44000
 * using <b>vortex-client</b> tool and once connected, show supported
 * profiles by the remote host. 
 *
 * \code
 * (jobs:0)[acinom@barbol libvortex]
 * $ vortex-client
 * Vortex-client v.0.8.3.b1498.g1498: a cmd tool to test vortex (and BEEP-enabled) peers
 * Copyright (c) 2005 Adavnced Software Production Line, S.L.
 * [=|=] vortex-client > connect
 * server to connect to: localhost
 * port to connect to: 44000
 * connecting to localhost:44000..ok: vortex message: session established and ready
 * [===] vortex-client > connection status
 * Created channel over this session:
 *  channel: 0, profile: not applicable
 * [===] vortex-client > show profiles
 * Supported remote peer profiles:
 *  http://fact.aspl.es/profiles/plain_profile
 * \endcode
 *
 * As you can observe, <b>vortex-client</b> tool is showing we are
 * already connected to remote peer and the connection created already
 * have a channel created with number 0. This channel number is the
 * BEEP administrative channel and every connection have it. This
 * channel is used to perform especial operations such as create new
 * channels, close them and channel tunning.
 * 
 * </li>
 * 
 * <li>Now, create a new channel chosing the plain profile as follows:
 * 
 * \code
 * [===] vortex-client > new channel
 * This procedure will request to create a new channel using Vortex api.
 * Select what profile to use to create for the new channel?
 * profiles for this peer: 1
 *  1) http://fact.aspl.es/profiles/plain_profile
 *  0) cancel process
 * you chose: 1
 * What channel number to create: you chose: 4
 * creating new channel..ok, channel created: 4
 * \endcode
 * </li> 
 *
 * <li>Now, send a test message and check if the server reply is
 * follwing the implementation, that is, the message should have
 * "Received OK: " preceding the text sent. Notify to vortex-client
 * you want to wait for the reply.
 *
 * \code
 * [===] vortex-client > send message
 * This procedure will send a message using the vortex api.
 * What channel do you want to use to send the message? you chose: 4
 * Type the message to send:
 * This is a test, reply my message
 * Do you want to wait for message reply? (Y/n) y
 * Message number 0 was sent..
 * waiting for reply...reply received: 
 * Received Ok: This is a test, reply my message
 * \endcode
 * </li> 
 *
 * <li>Now, check connection status and channel status to get more data about them. This will be
 * useful for you in the future is you want to debug BEEP peers.
 * \code
 * [===] vortex-client > connection status
 * Created channel over this session:
 *  channel: 0, profile: not applicable
 *  channel: 4, profile: http://fact.aspl.es/profiles/plain_profile
 * [===] vortex-client > channel status
 * Channel number to get status: 4
 * Channel 4 status is: 
 *  Profile definition: 
 *     http://fact.aspl.es/profiles/plain_profile
 *  Synchronization: 
 *     Last msqno sent:          0
 *     Next msqno to use:        1
 *     Last msgno received:      no message received yet
 *     Next reply to sent:       0
 *     Next reply exp. to recv:  1
 *     Next exp. msgno to recv:  0
 *     Next seqno to sent:       32
 *     Next seqno to receive:    45
 * [===] vortex-client > channel status
 * Channel number to get status: 0
 * Channel 0 status is: 
 *  Profile definition: 
 *     not applicable
 *  Synchronization: 
 *     Last msqno sent:          0
 *     Next msqno to use:        1
 *     Last msgno received:      no message received yet
 *     Next reply to sent:       0
 *     Next reply exp. to recv:  1
 *     Next exp. msgno to recv:  0
 *     Next seqno to sent:       185
 *     Next seqno to receive:    228
 * \endcode
 *</li>
 *
 *<li> Close the channel created as well as the connection.
 * \code
 * [===] vortex-client > close channel
 * closing the channel..
 * This procedure will close a channel using the vortex api.
 * What channel number to close: you chose: 4
 * Channel close: ok
 * [===] vortex-client > close
 * [=|=] vortex-client > quit
 * \endcode
 *</li>
 * </ul>
 * \section profile_example_implement_client Implementing a client for our server
 *
 * Now we have implemented our server supporting the <i>PLAIN
 * PROFILE</i>, we need some code to enable us sending data. 
 *
 * The following is the client implementation which connects, creates
 * a new channel and send a message:
 *
 * \include vortex-simple-client.c
 *
 * As you can observe the client is somewhat more complicated than the
 * server because it have to create not only the connection but also
 * the channel, sending the message and use the wait reply method to
 * read remote server reply.
 *
 * Due to the test nature, we have used wait reply method so the test
 * code gets linear in the sense <i>"I send the message and I get blocked
 * until the reply is received"</i> but this is not the preferred
 * method. 
 * 
 * The Vortex Library preferred method is to install a frame receive
 * handler and receive data replies or new message in an asynchronous
 * way. But, doing this on this example maybe will produce to increase
 * the complexity. If you want to know more about receiving data using
 * other methods, check this \ref dispatch_schema "section" to know more
 * about how can data is received.
 *
 *
 * \section profile_example_conclusion Conclusion
 *
 * We have seen how to create not only a profile but also a simple
 * Vortex Server and a Vortex Client. 
 *
 * We have also seen how we can use <b>vortex-client</b> tool to test
 * and perform operations against BEEP peers in general and against
 * Vortex peer in particular.
 *
 * We have also talked about the administrative channel: the channel
 * 0. This channel is present on every connection stablished and it is
 * used for especial operations about channel management. In fact, the
 * channel 0 is running under the definition of a profile defined at
 * the RFC 3080 called <i>Channel Management Profile</i>. This is
 * another example on how profiles are implemented: they only are
 * definition that must be implemented in other you can tell other
 * peer you actually support it. In this case, the <i>Channel Profile
 * Management</i> is compulsory.
 *
 * As another example to enforce the previous point is the <b>Coyote
 * Layer</b> inside the Af-Arch project. Coyote layer implement the profile:
 *
 * \code
 *  http://fact.aspl.es/profiles/coyote_profile
 * \endcode
 * 
 * On Af-Arch project, remote invoke procedure is done throught a
 * xml-rpc like message exchange defined and implemented on the Coyote
 * layer. 
 * 
 * If upper levels want to send a message to a remote Af-Arch enabled
 * node, they do it throught the Coyote layer. Coyote layer takes the
 * message and transform it into a coyote_profile compliant message so
 * remote peer, running also a Coyote layer, can recognize it. In
 * other word, the profile is registered using \ref
 * vortex_profiles_register and implemented on top of the Vortex
 * Library.
 *
 * <i><b>NOTE:</b> All code developed on this tutorial can be
 * found inside the Vortex Library repository, directory <b>test</b>.</i>
 *
 */

/**
 * \page image_logos Vortex Library Image logos
 * \section image_intro Logos available
 * 
 * You can use the following logs for your site or whatever you want
 * inside your project using Vortex Library. 
 * 
 * \image html vortex-big.png "A really big Vortex logo in PNG"
 * \image html vortex-200x200.png "A Vortex logo in PNG format (size 200x200)"
 * \image html vortex-100x100.png "A Vortex log in PNG format (size 100x100)"
 * \image html wide-logo-349x100.png "A wide logo suitable for a web site in PNG format (size 349x100)"
 * \image html vortex-has-you-420x100.png "Did Vortex catch you? A vortex logo in PNG format (size 420x100)"
 * \image html powered-by-vortex-361x100.png "A powred-by image (size 361x100)"
 * 
 */

 
// @}
