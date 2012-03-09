
SUBDIRS = . _test

wrapper_pys = \
	bin_wrapper.py \
	cell_wrapper.py \
	foreach_wrapper.py \
	hive_wrapper.py \
	init_wrapper.py \
	key_cell_wrapper.py \
	key_list_cell_wrapper.py \
	types_wrapper.py \
	security_descriptor_cell_wrapper.py \
	value_cell_wrapper.py \
	value_key_cell_wrapper.py \
	value_list_cell_wrapper.py \
	hive_check_wrapper.py \
	hcheck_wrapper.py

all-local: assure_init_py $(wrapper_pys)

include $(top_srcdir)/python/wrap-rules.am

rregadminpy_PYTHON = \
	__init__.py \
	$(wrapper_pys)

rregadminpydir = $(pythondir)/rregadmin/hive

TYPES_DEPS = types_wrapper.py $(top_builddir)/$(subdir)/__init__.py

$(top_builddir)/$(subdir)/__init__.py: assure_init_py

init_wrapper.py:: $(top_srcdir)/rregadmin/hive/init.h

types_wrapper.py:: $(top_srcdir)/rregadmin/hive/types.h \
                         $(UTIL_WRAPPER_DEPENDS)

bin_wrapper.py:: $(top_srcdir)/rregadmin/hive/bin.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

cell_wrapper.py:: $(top_srcdir)/rregadmin/hive/cell.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

hive_wrapper.py:: $(top_srcdir)/rregadmin/hive/hive.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

foreach_wrapper.py:: $(top_srcdir)/rregadmin/hive/foreach.h \
                       $(UTIL_WRAPPER_DEPENDS) \
                       $(TYPES_DEPS)

key_cell_wrapper.py:: $(top_srcdir)/rregadmin/hive/key_cell.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

key_list_cell_wrapper.py:: $(top_srcdir)/rregadmin/hive/key_list_cell.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

security_descriptor_cell_wrapper.py:: $(top_srcdir)/rregadmin/hive/security_descriptor_cell.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

value_cell_wrapper.py:: $(top_srcdir)/rregadmin/hive/value_cell.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

value_key_cell_wrapper.py:: $(top_srcdir)/rregadmin/hive/value_key_cell.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

value_list_cell_wrapper.py:: $(top_srcdir)/rregadmin/hive/value_list_cell.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

hive_check_wrapper.py:: $(top_srcdir)/rregadmin/hive/hive_check.h \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

hcheck_wrapper.py:: $(top_srcdir)/rregadmin/hive/hcheck_decl.h \
                     hive_check_wrapper.py \
                     $(UTIL_WRAPPER_DEPENDS) \
                     $(TYPES_DEPS)

EXTRA_DIST = \
	common-env.sh \
	bin_wrapper.conf \
	cell_wrapper.conf \
	foreach_wrapper.conf \
	hive_wrapper.conf \
	init_wrapper.conf \
	key_cell_wrapper.conf \
	key_list_cell_wrapper.conf \
	types_wrapper.conf \
	security_descriptor_cell_wrapper.conf \
	value_cell_wrapper.conf \
	value_key_cell_wrapper.conf \
	value_list_cell_wrapper.conf \
	hive_check_wrapper.conf \
	hcheck_wrapper.conf

CLEANFILES = *.xml *.pyc *.pyo

MAINTAINERCLEANFILES = $(wrapper_pys)
