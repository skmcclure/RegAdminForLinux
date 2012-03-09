
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_builddir)/test/lib \
	-I$(top_srcdir)/test/lib

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

LDADD = \
	$(top_builddir)/test/lib/librregadmin_test.la \
	../librregadmin_registry.la \
	../../hive/librregadmin_hive.la \
	../../secdesc/librregadmin_secdesc.la \
	../../util/librregadmin_util.la

noinst_HEADERS = \
	helpers.h

if HAVE_MELUNIT_CXX

run_checks = \
	test_registry_win_dir \
	test_foreach \
	test_check \
	test_registry2 \
	test_registry_key \
	test_regkeygen \
	test_hive_info \
	test_registry

make_checks = \
	test_registry3

check_PROGRAMS = \
	$(run_checks) \
	$(make_checks)

#TESTS_ENVIRONMENT = \
#	RRA_UTIL_OPTIONS="-l info"

TESTS = $(run_checks)

test_check_SOURCES = test_check.cpp
test_foreach_SOURCES = test_foreach.cpp
test_registry_SOURCES = test_registry.cpp
test_registry2_SOURCES = test_registry2.cpp
test_registry3_SOURCES = test_registry3.cpp
test_registry_win_dir_SOURCES = test_registry_win_dir.cpp
test_registry_key_SOURCES = test_registry_key.cpp
test_regkeygen_SOURCES = test_regkeygen.cpp
test_hive_info_SOURCES = test_hive_info.cpp

endif
