
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_builddir)/test/lib \
	-I$(top_srcdir)/test/lib

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

LDADD = \
	../librregadmin_cli.la \
	../../registry/librregadmin_registry.la \
	../../hive/librregadmin_hive.la \
	../../secdesc/librregadmin_secdesc.la \
	../../util/librregadmin_util.la \
	$(top_builddir)/test/lib/librregadmin_test.la

noinst_HEADERS = \
	helpers.h

clean-local:
	rm -rf test-data

if HAVE_MELUNIT_CXX

# Uncomment the following for extra hive integrity checks to be done
# before the hives are written
# TESTS_ENVIRONMENT = \
#	RRA_CLI_OPTIONS="--hive-check-write"

run_checks = \
	test_options \
	test_basic_result \
	test_basic \
	test_basic_cd \
	test_basic_exit \
	test_basic_has_key \
	test_basic_show_key \
	test_basic_show_key_parseable \
	test_basic_delete_key \
	test_basic_add_key \
	test_basic_has_value \
	test_basic_show_value \
	test_basic_cat_value \
	test_basic_delete_value \
	test_basic_add_value \
	test_basic_modify_value

check_PROGRAMS = \
	$(run_checks)

#TESTS_ENVIRONMENT = \
#	RRA_UTIL_OPTIONS="-l info"

TESTS = $(run_checks)

test_basic_SOURCES = test_basic.cpp
test_basic_result_SOURCES = test_basic_result.cpp
test_basic_cd_SOURCES = test_basic_cd.cpp
test_basic_exit_SOURCES = test_basic_exit.cpp
test_basic_show_key_SOURCES = test_basic_show_key.cpp
test_basic_show_key_parseable_SOURCES = test_basic_show_key_parseable.cpp
test_basic_has_key_SOURCES = test_basic_has_key.cpp
test_basic_delete_key_SOURCES = test_basic_delete_key.cpp
test_basic_add_key_SOURCES = test_basic_add_key.cpp
test_basic_has_value_SOURCES = test_basic_has_value.cpp
test_basic_show_value_SOURCES = test_basic_show_value.cpp
test_basic_cat_value_SOURCES = test_basic_cat_value.cpp
test_basic_delete_value_SOURCES = test_basic_delete_value.cpp
test_basic_add_value_SOURCES = test_basic_add_value.cpp
test_basic_modify_value_SOURCES = test_basic_modify_value.cpp
test_options_SOURCES = test_options.cpp

endif
