INCLUDES = \
	$(OSTAX_CFLAGS) -DSYSCONFDIR=\""$(sysconfdir)"\" -DCOMPILATION_DATE=`date +%s` -Wall

bin_PROGRAMS = os-tax

os_tax_SOURCES = \
	os_tax.c \
	os_tax_vat_customer.c os_tax_vat_customer.h \
	os_tax_vat_item.c os_tax_vat_item.h \
	os_tax_vat_value.c os_tax_vat_value.h

os_tax_LDADD = \
	$(OSTAX_LIBS) -lpopt


