INCLUDES = \
	$(AFTAX_CFLAGS) -DSYSCONFDIR=\""$(sysconfdir)"\" -DCOMPILATION_DATE=`date +%s` -Wall

bin_PROGRAMS = af-tax

af_tax_SOURCES = \
	af_tax.c \
	af_tax_vat_customer.c af_tax_vat_customer.h \
	af_tax_vat_item.c af_tax_vat_item.h \
	af_tax_vat_value.c af_tax_vat_value.h

af_tax_LDADD = \
	$(AFTAX_LIBS) -lpopt


