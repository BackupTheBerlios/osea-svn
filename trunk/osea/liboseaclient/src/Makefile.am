INCLUDES = -g -I$(top_srcdir) $(LIBRARIES_CFLAGS) -DCOMPILATION_DATE=`date +%s` -Wall

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

libafdalincludedir = $(includedir)/afdal

lib_LTLIBRARIES = liboseaclient.la

liboseaclient_la_SOURCES =	\
	oseaclient-detaileddatanode.c \
	oseaclient-datanode.c \
	oseaclient.c \
	oseaclient_list.c \
	oseaclient_request.c \
	oseaclient_session.c \
        oseaclient_event_source.c \
	oseaclient_keys.c \
	oseaclient_password.c \
	oseaclient_support.c \
	oseaclient_decimal.c

libafdalinclude_HEADERS =	\
	oseaclient-datanode.h \
	oseaclient-detaileddatanode.h \
	oseaclient.h	\
	oseaclient_list.h \
	oseaclient_request.h \
	oseaclient_session.h \
	oseaclient_event_source.h \
	oseaclient_keys.h \
	oseaclient_password.h \
	oseaclient_support.h \
	oseaclient_decimal.h

liboseaclient_la_LIBADD = \
	$(LIBRARIES_LIBS)

EXTRA_DIST = oseaclient_data_node.gob
