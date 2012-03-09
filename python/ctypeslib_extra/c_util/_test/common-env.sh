#! /bin/sh

if test -z "$abs_top_builddir" ; then
    abs_top_builddir=`pwd`/../../../..
fi

PYTHONPATH="$abs_top_srcdir/python:$abs_top_builddir/python:$PYTHONPATH"
export PYTHONPATH

LIBRARY_PATH="$abs_top_builddir/rregadmin/.libs:./.libs:$LIBRARY_PATH"
export LIBRARY_PATH

LD_LIBRARY_PATH="$abs_top_builddir/rregadmin/.libs:./.libs:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

# LD_PRELOAD="$LD_PRELOAD:$abs_top_builddir/rregadmin/.libs/librregadmin.so.0"
# export LD_PRELOAD

xml2py_args="-v -k efst"
export xml2py_args

xml2py_libs=""
export xml2py_libs

xml2py_extra_modules=""
export xml2py_extra_modules

xml2py_include_patterns=""
export xml2py_include_patterns



h2xml_args="$h2xml_cflags -I."
export h2xml_args

h2xml_sources=""
export h2xml_sources
