INCLUDES = -g -I$(top_srcdir) $(LIBRARIES_CFLAGS) \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\"  -Wall

liboseacommincludedir = $(includedir)/liboseacomm

lib_LTLIBRARIES = liboseacomm.la

liboseacomm_la_SOURCES =	\
	oseacomm_simple.c oseacomm_simple_cfg.c oseacomm_xml.c  \
        oseacomm_connection.c oseacomm.c oseacomm_code.c oseacomm_dataset.c

liboseacomminclude_HEADERS =	\
	oseacomm_simple.h oseacomm_simple_cfg.h oseacomm_xml.h \
	oseacomm_connection.h oseacomm.h oseacomm_code.h oseacomm_dataset.h

liboseacomm_la_LIBADD = \
	$(LIBRARIES_LIBS)

