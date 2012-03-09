
if HAVE_CTYPESLIB

check_LTLIBRARIES = libt2.la

libt2_la_SOURCES = t2.c t2.h
libt2_la_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) -I.
libt2_la_LDFLAGS = -rpath /usr/lib

wrapper_pys = \
	t2_wrapper.py \
	enum_t1_wrapper.py

all-local: libt2.la $(wrapper_pys)

include $(top_srcdir)/python/wrap-rules.am

h2xml_cflags += \
	-I$(top_srcdir)/$(subdir) \
	-I$(top_builddir)/$(subdir) \
	-I.

t2_wrapper.py:: t2_wrapper.conf t2.h libt2.la

enum_t1_wrapper.py:: enum_t1_wrapper.conf enum-t1.h

EXTRA_DIST = \
	t2_wrapper.conf \
	enum_t1_wrapper.conf \
	enum-t1.h

noinst_PYTHON = \
	t1.py \
	t1_fail_no_ob.py \
	t1_fail_non_struct.py \
	t1_fail_none_ob.py \
	t1_no_funcs.py \
	t1_too_many_constructors.py \
	t1_too_many_destructors.py

dist_check_SCRIPTS = \
	test_c_obj \
	test_c_obj_t2 \
	test_c_enum_obj

TESTS_ENVIRONMENT = \
	ABS_TOP_BUILD_DIR=$(abs_top_builddir) \
	ABS_TOP_SRC_DIR=$(abs_top_srcdir) \
	SCRIPT_DIR=$(abs_top_srcdir)/$(subdir) \
	REGISTRY_LIST_PROG=$(abs_top_builddir)/test/bin/ls_example_registries \
	EF_VALUES_FILE=$(abs_top_builddir)/test/data/shared-values \
	EF_KEYS_FILE=$(abs_top_builddir)/test/data/shared-keys \
	RRA_TEST_PROGRAM_OPTS="--verbose" \
	PYTHONPATH=$(abs_top_builddir)/python:$(abs_top_builddir)/python/_test/lib:$(abs_top_srcdir)/python:$(abs_top_srcdir)/python/_test/lib:$$PYTHONPATH \
	LD_LIBRARY_PATH=$(abs_top_builddir)/rregadmin/.libs:$(abs_top_builddir)/$(subdir)/.libs:$$LD_LIBRARY_PATH

TESTS = \
	test_c_obj \
	test_c_obj_t2 \
	test_c_enum_obj

CLEANFILES = *.pyc *.pyo $(wrapper_pys)

endif
