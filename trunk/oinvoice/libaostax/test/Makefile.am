noinst_PROGRAMS = test_aos_tax_vat_customer test_aos_tax_vat_item \
		  test_aos_tax_vat_value

INCLUDES = -I.. -I $(top_srcdir) $(AOSTAX_CFLAGS) -Wall

test_aos_tax_vat_customer_SOURCES = test_aos_tax_vat_customer.c

test_aos_tax_vat_customer_LDADD = $(AOSTAX_LIBS) $(top_builddir)/src/libaostax.la

test_aos_tax_vat_item_SOURCES = test_aos_tax_vat_item.c

test_aos_tax_vat_item_LDADD = $(AOSTAX_LIBS) $(top_builddir)/src/libaostax.la

test_aos_tax_vat_value_SOURCES = test_aos_tax_vat_value.c

test_aos_tax_vat_value_LDADD = $(AOSTAX_LIBS) $(top_builddir)/src/libaostax.la
