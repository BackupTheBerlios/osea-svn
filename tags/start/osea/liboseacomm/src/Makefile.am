INCLUDES = -g -I$(top_srcdir) $(LIBRARIES_CFLAGS) \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\"  -Wall

libcoyoteincludedir = $(includedir)/coyote

lib_LTLIBRARIES = libcoyote.la

libcoyote_la_SOURCES =	\
	coyote_simple.c coyote_simple_cfg.c coyote_xml.c  \
        coyote_connection.c coyote.c coyote_code.c coyote_dataset.c

libcoyoteinclude_HEADERS =	\
	coyote_simple.h coyote_simple_cfg.h coyote_xml.h \
	coyote_connection.h coyote.h coyote_code.h coyote_dataset.h

libcoyote_la_LIBADD = \
	$(LIBRARIES_LIBS)

