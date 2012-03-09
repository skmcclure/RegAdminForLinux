
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_builddir)/test/lib \
	-I$(top_srcdir)/test/lib

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

LDADD = \
	../librregadmin_hive.la \
	../../secdesc/librregadmin_secdesc.la \
	../../util/librregadmin_util.la \
	$(top_builddir)/test/lib/librregadmin_test.la

if HAVE_MELUNIT_CXX

# Uncomment the following to get more output from the tests.
# Change "message" to "info" or "debug" for even more output.
#TESTS_ENVIRONMENT = \
#	RRA_UTIL_OPTIONS="-l debug"

check_PROGRAMS = \
	test_security_descriptor_cell \
	test_cell \
	test_foreach \
	test_hive \
	test_hive2 \
	test_hive3 \
	test_hive4 \
	test_bin \
	test_key_cell \
	test_keycmp \
	test_value_key_cell \
	test_hive_check

TESTS = $(check_PROGRAMS)

test_security_descriptor_cell_SOURCES = test_security_descriptor_cell.cpp
test_bin_SOURCES = test_bin.cpp
test_foreach_SOURCES = test_foreach.cpp
test_hive_SOURCES = test_hive.cpp
test_hive_check_SOURCES = test_hive_check.cpp
test_hive2_SOURCES = test_hive2.cpp
test_hive3_SOURCES = test_hive3.cpp
test_hive4_SOURCES = test_hive4.cpp
test_cell_SOURCES = test_cell.cpp
test_key_cell_SOURCES = test_key_cell.cpp
test_value_key_cell_SOURCES = test_value_key_cell.cpp
test_keycmp_SOURCES = test_keycmp.cpp

endif

EXTRA_DIST = \
	samba_regf.h.info \
	samba_regf.idl.info
