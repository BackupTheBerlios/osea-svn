INCLUDES = -g $(LIBRARIES_CFLAGS) \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\" \
	-DCOMPILATION_DATE=`date +%s` -Wall

liboseatypeincludedir = $(includedir)/liboseatype

lib_LTLIBRARIES = liboseatype.la

liboseatype_la_SOURCES = oseatype.c

liboseatypeinclude_HEADERS = oseatype.h 

liboseatype_la_LIBADD = \
	$(LIBRARIES_LIBS)

