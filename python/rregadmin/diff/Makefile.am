
SUBDIRS = . _test

wrapper_pys = \
	diff_info_wrapper.py \
	hive_wrapper.py \
	registry_wrapper.py

all-local: assure_init_py $(wrapper_pys)

include $(top_srcdir)/python/wrap-rules.am

rregadminpy_PYTHON = \
	__init__.py \
	$(wrapper_pys)

rregadminpydir = $(pythondir)/rregadmin/diff

DIFF_INFO_DEPS = diff_info_wrapper.py $(top_builddir)/$(subdir)/__init__.py

$(top_builddir)/$(subdir)/__init__.py: assure_init_py


diff_info_wrapper.py:: $(top_srcdir)/rregadmin/diff/diff_info.h \
                        $(REGISTRY_WRAPPER_DEPENDS)

hive_wrapper.py:: $(top_srcdir)/rregadmin/diff/hive.h \
                   $(REGISTRY_WRAPPER_DEPENDS) \
                   $(DIFF_INFO_DEPS)

registry_wrapper.py:: $(top_srcdir)/rregadmin/diff/registry.h \
                       $(REGISTRY_WRAPPER_DEPENDS) \
                       $(DIFF_INFO_DEPS)

EXTRA_DIST = \
	common-env.sh \
	diff_info_wrapper.conf \
	hive_wrapper.conf \
	registry_wrapper.conf

CLEANFILES = *.xml *.pyc *.pyo

MAINTAINERCLEANFILES = $(wrapper_pys)
