EXTRA_DIST = Makefile.win

INCLUDES = -g -I$(top_srcdir) $(LIBRARIES_CFLAGS) -DVERSION=\""$(VORTEX_VERSION)"\" \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\" -DPACKAGE_TOP_DIR=\""$(top_srcdir)"\" -Wall \
	-DVERSION=\"$(VORTEX_VERSION)\"


libvortexincludedir = $(includedir)/vortex

lib_LTLIBRARIES = libvortex.la

libvortex_la_SOURCES =	\
	vortex.c \
	vortex_connection.c \
	vortex_listener.c  \
	vortex_handlers.c  \
	vortex_frame_factory.c \
	vortex_greetings.c \
	vortex_profiles.c \
	vortex_channel.c \
	vortex_reader.c \
	vortex_dtds.c \
	vortex_hash.c \
	vortex_queue.c \
	vortex_sequencer.c \
	vortex_writer.c  \
	vortex_semaphore.c \
	vortex_thread_pool.c \
	vortex_channel_pool.c \
	vortex_errno.c
libvortexinclude_HEADERS = vortex.h \
	vortex_connection.h \
	vortex_listener.h \
	vortex_handlers.h \
	vortex_types.h \
	vortex_frame_factory.h \
	vortex_greetings.h \
	vortex_profiles.h \
	vortex_channel.h \
	vortex_reader.h \
	vortex_dtds.h \
	vortex_hash.h \
	vortex_queue.h \
	vortex_sequencer.h \
	vortex_writer.h \
	vortex_semaphore.h \
	vortex_thread_pool.h \
	vortex_channel_pool.h \
	vortex_errno.h
libvortex_la_LIBADD = \
	$(LIBRARIES_LIBS)

