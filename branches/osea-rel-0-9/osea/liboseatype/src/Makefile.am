INCLUDES = -g $(LIBRARIES_CFLAGS) \
	-DPACKAGE_DTD_DIR=\""$(datadir)"\" \
	-DCOMPILATION_DATE=`date +%s` -Wall

liboseatypeincludedir = $(includedir)/liboseatype

lib_LTLIBRARIES = liboseatype.la

liboseatype_la_SOURCES = oseatype.c \
	oseatype-object.c oseatype-simple-object.c \
	oseatype-boolean.c oseatype-char.c oseatype-int.c\
	oseatype-message.c oseatype-message-query.c \
	oseatype-message-element-array.c \
	oseatype-positive-int.c oseatype-negative-int.c \
	oseatype-non-positive-int.c oseatype-non-negative-int.c \
	oseatype-decimal.c oseatype-float.c oseatype-double.c \
	oseatype-date.c oseatype-time.c oseatype-time-instant.c oseatype-time-duration.c \
	oseatype-month.c oseatype-year.c oseatype-recurring-date.c oseatype-recurring-day.c \
	oseatype-string.c oseatype-language.c \
	oseatype-compound-object.c


liboseatypeinclude_HEADERS = oseatype.h \
	oseatype-object.h oseatype-simple-object.h \
	oseatype-boolean.h oseatype-char.h oseatype-int.h\
	oseatype-message.h oseatype-message-query.h \
	oseatype-message-element-array.h \
	oseatype-positive-int.h oseatype-negative-int.h \
	oseatype-non-positive-int.h oseatype-non-negative-int.h \
	oseatype-decimal.h oseatype-float.h oseatype-double.h \
	oseatype-date.h oseatype-time.h oseatype-time-instant.h oseatype-time-duration.h \
	oseatype-month.h oseatype-year.h oseatype-recurring-date.h oseatype-recurring-day.h \
	oseatype-string.h oseatype-language.h \
	oseatype-compound-object.h


liboseatype_la_LIBADD = $(LIBRARIES_LIBS)

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

