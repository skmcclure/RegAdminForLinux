
dist_check_SCRIPTS = \
	test_path_wrapper \
	test_path

TESTS_ENVIRONMENT = \
	ABS_TOP_BUILD_DIR=$(abs_top_builddir) \
	ABS_TOP_SRC_DIR=$(abs_top_srcdir) \
	SCRIPT_DIR=$(abs_top_srcdir)/$(subdir) \
	REGISTRY_LIST_SCRIPT=$(abs_top_builddir)/test/bin/ls_example_registries \
	EF_VALUES_FILE=$(abs_top_builddir)/test/data/shared-values \
	EF_KEYS_FILE=$(abs_top_builddir)/test/data/shared-keys \
	RRA_TEST_PROGRAM_OPTS="--verbose" \
	PYTHONPATH=$(abs_top_builddir)/python:$(abs_top_builddir)/python/_test/lib:$(abs_top_srcdir)/python:$(abs_top_srcdir)/python/_test/lib:$$PYTHONPATH \
	LD_LIBRARY_PATH=$(abs_top_builddir)/rregadmin/.libs:$$LD_LIBRARY_PATH

TESTS = \
	test_path_wrapper

#	test_path
