INCLUDES = -g -I$(top_srcdir)/liboseacomm $(LIBRARIES_CFLAGS) -DCOMPILATION_DATE=`date +%s` -Wall

liboseaclientincludedir = $(includedir)/liboseaclient

lib_LTLIBRARIES = liboseaclient.la

liboseaclient_la_SOURCES =	\
	oseaclient-datanode.c \
	oseaclient-detaileddatanode.c \
	oseaclient.c \
	oseaclient_types.c \
	oseaclient_list.c \
	oseaclient_request.c \
	oseaclient_session.c \
        oseaclient_event_source.c \
	oseaclient_keys.c \
	oseaclient_password.c \
	oseaclient_support.c \
	oseaclient_decimal.c

liboseaclientinclude_HEADERS =	\
	oseaclient-datanode.h \
	oseaclient-detaileddatanode.h \
	oseaclient.h	\
	oseaclient_types.h \
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

EXTRA_DIST = oseaclient-datanode.gob oseaclient-detaileddatanode.gob

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

