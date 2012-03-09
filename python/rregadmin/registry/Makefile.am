
SUBDIRS = . _test

wrapper_pys = \
	types_wrapper.py \
	registry_wrapper.py \
	registry_key_wrapper.py \
	hive_info_wrapper.py \
	init_wrapper.py

all-local: assure_init_py $(wrapper_pys)

include $(top_srcdir)/python/wrap-rules.am

rregadminpy_PYTHON = \
	__init__.py \
	$(wrapper_pys)

rregadminpydir = $(pythondir)/rregadmin/registry

TYPES_DEPS = types_wrapper.py $(top_builddir)/$(subdir)/__init__.py

$(top_builddir)/$(subdir)/__init__.py: assure_init_py

types_wrapper.py:: $(top_srcdir)/rregadmin/registry/types.h \
                    $(HIVE_WRAPPER_DEPENDS)

registry_wrapper.py:: $(top_srcdir)/rregadmin/registry/registry.h \
                       $(HIVE_WRAPPER_DEPENDS) \
                       $(TYPES_DEPS)

registry_key_wrapper.py:: $(top_srcdir)/rregadmin/registry/registry_key.h \
                       $(HIVE_WRAPPER_DEPENDS) \
                       $(TYPES_DEPS)

hive_info_wrapper.py:: $(top_srcdir)/rregadmin/registry/hive_info.h \
                       $(HIVE_WRAPPER_DEPENDS) \
                       $(TYPES_DEPS)

init_wrapper.py:: $(top_srcdir)/rregadmin/registry/init.h

EXTRA_DIST = \
	common-env.sh \
	types_wrapper.conf \
	registry_wrapper.conf \
	registry_key_wrapper.conf \
	hive_info_wrapper.conf \
	init_wrapper.conf

CLEANFILES = *.xml *.pyc *.pyo

MAINTAINERCLEANFILES = $(wrapper_pys)
