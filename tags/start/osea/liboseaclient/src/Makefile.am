INCLUDES = -g -I$(top_srcdir) $(LIBRARIES_CFLAGS) -DCOMPILATION_DATE=`date +%s` -Wall

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

libafdalincludedir = $(includedir)/afdal

lib_LTLIBRARIES = libafdal.la

libafdal_la_SOURCES =	\
	afdal-datanode.c \
	afdal-detaileddatanode.c \
	afdal.c \
	afdal_list.c \
	afdal_request.c \
	afdal_session.c \
        afdal_event_source.c \
	afdal_afkeys.c \
	afdal_password.c \
	afdal_support.c \
	afdal_decimal.c

libafdalinclude_HEADERS =	\
	afdal-datanode.h \
	afdal-detaileddatanode.h \
	afdal.h	\
	afdal_list.h \
	afdal_request.h \
	afdal_session.h \
	afdal_event_source.h \
	afdal_afkeys.h \
	afdal_password.h \
	afdal_support.h \
	afdal_decimal.h

libafdal_la_LIBADD = \
	$(LIBRARIES_LIBS)

EXTRA_DIST = afdal_data_node.gob
