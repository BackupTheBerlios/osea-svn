INCLUDES = -g -I$(top_srcdir) $(AOSCUSTOMER_CFLAGS)  -DCOMPILATION_DATE=`date +%s` -Wall

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

libaoscustomerincludedir = $(includedir)/aoscustomer
lib_LTLIBRARIES = libaoscustomer.la
libaoscustomer_la_SOURCES = \
	aos-customer-customer.c \
	aos_customer_enum_values.c  \
	aos_customer_address.c  \
	aos_customer_telephone.c  \
	aos_customer_inet_data.c  \
	aos_customer_contact.c  \
	aos_customer_customer.c  

libaoscustomerinclude_HEADERS = \
	aos_customer.h \
	aos-customer-customer.h \
	aos_customer_enum_values.h  \
	aos_customer_address.h  \
	aos_customer_telephone.h  \
	aos_customer_inet_data.h  \
	aos_customer_contact.h  \
	aos_customer_customer.h  

libaoscustomer_la_LIBADD = $(AOSCUSTOMER_LIBS)
