
dist_check_SCRIPTS = \
	test_add_key

TESTS_ENVIRONMENT = \
	ABS_TOP_BUILD_DIR=$(abs_top_builddir) \
	ABS_TOP_SRC_DIR=$(abs_top_srcdir) \
	SCRIPT_LIB_DIR=$(abs_top_srcdir)/$(subdir)/.. \
	CL_PROG=$(abs_top_builddir)/bin/rra_basic_cl \
	DIFF_PROG=$(abs_top_builddir)/bin/rra_registry_diff \
	REGISTRY_LIST_PROG=$(abs_top_builddir)/test/bin/ls_example_registries \
	EF_VALUES_FILE=$(abs_top_builddir)/test/data/shared-values \
	EF_KEYS_FILE=$(abs_top_builddir)/test/data/shared-keys \
	BUILD_TYPE=$(BUILD_TYPE) \
	PYTHONPATH=$$PYTHONPATH:$(srcdir)/..

# Uncomment the following for extra hive integrity checks to be done
# before the hives are written
# TESTS_ENVIRONMENT = $(TESTS_ENVIRONMENT) \
#	RRA_CLI_OPTIONS="--hive-check-write" \
#       RRA_UTIL_OPTIONS="-l info"

CLEANFILES = *.pyc *.pyo gmon_*.out gmon.out callgrind.out*

clean-local:
	rm -rf test-data

.PHONY:	valgrind-check
valgrind-check:
	VALGRIND_CHECK=true $(MAKE) check

TESTS = \
	test_add_key

#AM_CPPFLAGS = \
#	$(MELUNIT_CXX_CFLAGS) \
#	-I$(top_srcdir) \
#	-I$(top_builddir) \
#	-I$(top_builddir)/test/lib \
#	-I$(top_srcdir)/test/lib

#AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

#LDADD = \
#	$(top_builddir)/rregadmin/util/librregadmin_util.la \
#	$(top_builddir)/test/lib/librregadmin_test.la
