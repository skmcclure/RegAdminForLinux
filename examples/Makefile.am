
LDADD = \
	$(top_builddir)/rregadmin/librregadmin.la

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_PROGRAMS = \
	hiveinfo \
	hive_check \
	find_classname \
	lshive \
	get_subkey_orders \
	find_large_values \
	find_flagged_values \
	print_acls \
	print_all_hive_strings \
	print_all_multisz \
	gen_pw \
	print_root_secdesc \
	convert_secdesc

hiveinfo_SOURCES = hiveinfo.c
hive_check_SOURCES = hive_check.c
lshive_SOURCES = lshive.c
print_acls_SOURCES = print_acls.c
get_subkey_orders_SOURCES = get_subkey_orders.c
find_classname_SOURCES = find_classname.c
find_large_values_SOURCES = find_large_values.c
find_flagged_values_SOURCES = find_flagged_values.c
print_all_hive_strings_SOURCES = print_all_hive_strings.c
print_all_multisz_SOURCES = print_all_multisz.c
print_root_secdesc_SOURCES = print_root_secdesc.c
convert_secdesc_SOURCES = convert_secdesc.c
convert_secdesc_LDFLAGS = -static

gen_pw_SOURCES = gen_pw.c
gen_pw_LDFLAGS = $(SSL_LDFLAGS) $(SSL_LIBS)
gen_pw_CFLAGS = $(SSL_CFLAGS)
