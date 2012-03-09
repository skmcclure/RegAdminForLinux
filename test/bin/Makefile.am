
LDADD = \
	$(top_builddir)/rregadmin/librregadmin.la \
	$(top_builddir)/test/lib/librregadmin_test.la

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_srcdir)/test/lib \
	-I$(top_builddir)/test/lib

EXTRA_DIST = \
	get_shared_keys.sh \
	get_shared_values.sh

noinst_SCRIPTS = \
	get_shared_keys.sh \
	get_shared_values.sh

noinst_PROGRAMS = \
	ls_example_registries \
	ls_reg_keys \
	filter_keys \
	ls_reg_values \
	filter_values

ls_example_registries_SOURCES = ls_example_registries.cpp
filter_keys_SOURCES = filter_keys.c
filter_values_SOURCES = filter_values.c
ls_reg_keys_SOURCES = ls_reg_keys.c
ls_reg_values_SOURCES = ls_reg_values.c
