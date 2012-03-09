
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_builddir)/test/lib \
	-I$(top_srcdir)/test/lib

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

LDADD = \
	../librregadmin_diff.la \
	../../registry/librregadmin_registry.la \
	../../hive/librregadmin_hive.la \
	../../secdesc/librregadmin_secdesc.la \
	../../util/librregadmin_util.la \
	$(top_builddir)/test/lib/librregadmin_test.la

if HAVE_MELUNIT_CXX

# TESTS_ENVIRONMENT = \
#	RRA_UTIL_OPTIONS="-l trace"

run_checks = \
	test_diff_info \
	test_key_cell_diff \
	test_value_key_cell_diff \
	test_hive_compare \
	test_registry_compare

check_PROGRAMS = \
	$(run_checks)

TESTS = $(run_checks)

test_diff_info_SOURCES = test_diff_info.cpp
test_key_cell_diff_SOURCES = test_key_cell_diff.cpp
test_value_key_cell_diff_SOURCES = test_value_key_cell_diff.cpp
test_hive_compare_SOURCES = test_hive_compare.cpp
test_registry_compare_SOURCES = test_registry_compare.cpp

endif
