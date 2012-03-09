

SUBDIRS = . util secdesc hive registry cli diff sam

CLEANFILES = *.pyc *.pyo

rregadminpy_PYTHON = \
	__init__.py
rregadminpydir = $(pythondir)/rregadmin

all-local: assure_init_py

include $(top_srcdir)/python/wrap-rules.am
