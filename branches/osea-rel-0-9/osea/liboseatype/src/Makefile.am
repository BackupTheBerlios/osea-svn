INCLUDES = -g $(LIBRARIES_CFLAGS) \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\" \
	-DCOMPILATION_DATE=`date +%s` -Wall

liboseatypeincludedir = $(includedir)/liboseatype

lib_LTLIBRARIES = liboseatype.la

liboseatype_la_SOURCES = oseatype.c \
	oseatype-object.c \
	oseatype-message.c oseatype-message-query.c


liboseatypeinclude_HEADERS = oseatype.h \
	oseatype-object.h \
	oseatype-message.h oseatype-message-query.h

liboseatype_la_LIBADD = \
	$(LIBRARIES_LIBS)

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

