INCLUDES = -g -I$(top_srcdir) $(AOSTAX_CFLAGS)  -DCOMPILATION_DATE=`date +%s` -Wall

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

libaostaxincludedir = $(includedir)/aostax
lib_LTLIBRARIES = libaostax.la
libaostax_la_SOURCES =   aos-tax-vatcustomer.c aos-tax-vatitem.c \
			 aos_tax.c aos_tax_vat_customer.c aos_tax_vat_item.c \
			 aos_tax_vat_value.c
libaostaxinclude_HEADERS =   aos-tax-vatcustomer.h \
		             aos-tax-vatitem.h \
			     aos_tax.h \
	                     aos_tax_vat_customer.h aos_tax_vat_item.h \
			     aos_tax_vat_value.h 

libaostax_la_LIBADD = $(AOSTAX_LIBS)

