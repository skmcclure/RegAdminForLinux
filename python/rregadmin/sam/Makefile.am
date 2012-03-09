
SUBDIRS = . _test

wrapper_pys = \
	sam_info_wrapper.py

all-local: assure_init_py $(wrapper_pys)

include $(top_srcdir)/python/wrap-rules.am

rregadminpy_PYTHON = \
	__init__.py \
	$(wrapper_pys)

rregadminpydir = $(pythondir)/rregadmin/sam

$(top_builddir)/$(subdir)/__init__.py: assure_init_py


sam_info_wrapper.py:: $(top_srcdir)/rregadmin/sam/sam_info.h \
                        $(REGISTRY_WRAPPER_DEPENDS)

EXTRA_DIST = \
	common-env.sh \
	sam_info_wrapper.conf

CLEANFILES = *.xml *.pyc *.pyo

MAINTAINERCLEANFILES = $(wrapper_pys)
