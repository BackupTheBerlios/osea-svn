INCLUDES = -g -I$(top_srcdir) $(LIBRARIES_CFLAGS) \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\" \
	-DCOMPILATION_DATE=`date +%s` -Wall

liboseaserverincludedir = $(includedir)/oseaserver

lib_LTLIBRARIES = liboseaserver.la

liboseaserver_la_SOURCES = oseaserver.c \
		     oseaserver_config.c \
		     oseaserver_database.c \
	             oseaserver_reg.c \
		     oseaserver_message.c \
		     oseaserver_command.c \
		     oseaserver_keys.c \
		     oseaserver_services.c \
		     oseaserver_log.c \
		     oseaserver_signal_handler.c \
		     oseaserver_main.c

liboseaserverinclude_HEADERS = oseaserver.h \
                         oseaserver_config.h \
			 oseaserver_database.h  \
			 oseaserver_reg.h \
			 oseaserver_message.h \
			 oseaserver_command.h \
		         oseaserver_keys.h \
			 oseaserver_services.h \
			 oseaserver_log.h \
		         oseaserver_signal_handler.h \
			 oseaserver_main.h
liboseaserver_la_LIBADD = \
	$(LIBRARIES_LIBS)

