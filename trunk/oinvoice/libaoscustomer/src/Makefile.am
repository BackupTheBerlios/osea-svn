INCLUDES = -g -I$(top_srcdir) $(AFDALCUSTOMER_CFLAGS)  -DCOMPILATION_DATE=`date +%s` -Wall

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

libafdalcustomerincludedir = $(includedir)/afdalcustomer
lib_LTLIBRARIES = libafdalcustomer.la
libafdalcustomer_la_SOURCES = \
	afdal-customer-customer.c \
	afdal_customer_enum_values.c  \
	afdal_customer_address.c  \
	afdal_customer_telephone.c  \
	afdal_customer_inet_data.c  \
	afdal_customer_contact.c  \
	afdal_customer_customer.c  

libafdalcustomerinclude_HEADERS = \
	afdal_customer.h \
	afdal-customer-customer.h \
	afdal_customer_enum_values.h  \
	afdal_customer_address.h  \
	afdal_customer_telephone.h  \
	afdal_customer_inet_data.h  \
	afdal_customer_contact.h  \
	afdal_customer_customer.h  

libafdalcustomer_la_LIBADD = $(AFDALCUSTOMER_LIBS)
