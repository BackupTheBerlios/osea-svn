# af_gen_do_not_touch_this_file
INCLUDES = \
	$(AF_CUSTOMER_CFLAGS) -DSYSCONFDIR=\""$(sysconfdir)"\" -DCOMPILATION_DATE=`date +%s` -Wall -g

bin_PROGRAMS = af-customer

af_customer_SOURCES = \
	af_customer.c \
	af_customer_enum_values.c  af_customer_enum_values.h  \
	af_customer_address.c  af_customer_address.h  \
	af_customer_telephone.c  af_customer_telephone.h  \
	af_customer_inet_data.c  af_customer_inet_data.h  \
	af_customer_contact.c  af_customer_contact.h  \
	af_customer_customer.c  af_customer_customer.h  

afgen: ../af-customer.idl
	af-gen -b ../../ -y "2002,2003" -a "Advanced Software Production Line, S.L." ../af-customer.idl

af_customer_LDADD = 	$(AF_CUSTOMER_LIBS)

