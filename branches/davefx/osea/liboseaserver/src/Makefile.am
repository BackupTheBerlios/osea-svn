INCLUDES = -g -I$(top_srcdir) $(LIBRARIES_CFLAGS) \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\" \
	-DCOMPILATION_DATE=`date +%s` -Wall

libafgsincludedir = $(includedir)/afgs

lib_LTLIBRARIES = libafgs.la

libafgs_la_SOURCES = afgs.c \
		     afgs_config.c \
		     afgs_database.c \
	             afgs_reg.c \
		     afgs_message.c \
		     afgs_command.c \
		     afgs_afkeys.c \
		     afgs_services.c \
		     afgs_log.c \
		     afgs_signal_handler.c \
		     afgs_main.c

libafgsinclude_HEADERS = afgs.h \
                         afgs_config.h \
			 afgs_database.h  \
			 afgs_reg.h \
			 afgs_message.h \
			 afgs_command.h \
		         afgs_afkeys.h \
			 afgs_services.h \
			 afgs_log.h \
		         afgs_signal_handler.h \
			 afgs_main.h
libafgs_la_LIBADD = \
	$(LIBRARIES_LIBS)

