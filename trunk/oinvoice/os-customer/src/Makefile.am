# os_gen_do_not_touch_this_file
INCLUDES = \
	$(OS_CUSTOMER_CFLAGS) -DSYSCONFDIR=\""$(sysconfdir)"\" -DCOMPILATION_DATE=`date +%s` -Wall -g

bin_PROGRAMS = os-customer

os_customer_SOURCES = \
	os_customer.c \
	os_customer_enum_values.c  os_customer_enum_values.h  \
	os_customer_address.c  os_customer_address.h  \
	os_customer_telephone.c  os_customer_telephone.h  \
	os_customer_inet_data.c  os_customer_inet_data.h  \
	os_customer_contact.c  os_customer_contact.h  \
	os_customer_customer.c  os_customer_customer.h  

afgen: ../os-customer.idl
	os-gen -b ../../ -y "2002,2003" -a "Advanced Software Production Line, S.L." ../os-customer.idl

os_customer_LDADD = 	$(OS_CUSTOMER_LIBS)

