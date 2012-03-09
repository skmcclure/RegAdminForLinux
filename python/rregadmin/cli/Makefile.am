
SUBDIRS = . _test

wrapper_pys = \
	basic_result_wrapper.py \
	basic_wrapper.py \
	init_wrapper.py

all-local: assure_init_py $(wrapper_pys)

include $(top_srcdir)/python/wrap-rules.am

rregadminpy_PYTHON = \
	__init__.py \
	$(wrapper_pys)


rregadminpydir = $(pythondir)/rregadmin/cli

TYPES_DEPS = ../registry/types_wrapper.py $(top_builddir)/$(subdir)/__init__.py

$(top_builddir)/$(subdir)/__init__.py: assure_init_py

basic_result_wrapper.py:: $(top_srcdir)/rregadmin/cli/basic_result.h \
                       $(HIVE_WRAPPER_DEPENDS) \
                       $(TYPES_DEPS)

basic_wrapper.py:: $(top_srcdir)/rregadmin/cli/basic.h \
                    $(HIVE_WRAPPER_DEPENDS) \
                    $(TYPES_DEPS) \
                    basic_result_wrapper.py

init_wrapper.py:: $(top_srcdir)/rregadmin/cli/init.h

EXTRA_DIST = \
	common-env.sh \
	basic_result_wrapper.conf \
	basic_wrapper.conf \
	init_wrapper.conf

CLEANFILES = *.xml *.pyc *.pyo

MAINTAINERCLEANFILES = $(wrapper_pys)
