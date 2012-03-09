dnl Copyright 2006, 2007, 2008 James LewisMoss <jlm@racemi.com>
dnl
dnl This file is free software; as a special exception the author gives
dnl unlimited permission to copy and/or distribute it, with or without
dnl modifications, as long as this notice is preserved.
dnl
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
dnl implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT([RRegAdmin],
        [0.10.16],
        [James LewisMoss <jlm@racemi.com>],
        [rregadmin])

AM_INIT_AUTOMAKE([-Wall std-options dist-bzip2 check-news])

AC_CONFIG_HEADERS([rregadmin/config.h])
AC_CONFIG_HEADERS([rregadmin/util/macros.h])
AX_JM_PREFIX_CONFIG_H([rregadmin/rra_config.h], [RRA])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC

AC_AIX
AC_ISC_POSIX
AC_MINIX
AC_HEADER_STDC

AM_PROG_LIBTOOL

# Build settings
AX_JM_BUILD_SETTINGS([test])
AX_JM_COMPILE_WARNINGS_C([maximum])
AX_JM_COMPILE_WARNINGS_CXX([yes])
CFLAGS="$CFLAGS $GCC_WARN_FLAGS"
CXXFLAGS="$CXXFLAGS $GXX_WARN_FLAGS"

if test "$BUILD_TYPE" = "debug" ; then
  RRA_COMPILED_LOG_LEVEL="RRA_LOG_LEVEL_TRACE"
  RRA_INCLUDE_DEBUG_CODE="1"
elif test "$BUILD_TYPE" = "test" ; then
  RRA_COMPILED_LOG_LEVEL="RRA_LOG_LEVEL_DEBUG"
  RRA_INCLUDE_DEBUG_CODE="1"
elif test "$BUILD_TYPE" = "debug-release" ; then
  RRA_COMPILED_LOG_LEVEL="RRA_LOG_LEVEL_DEBUG"
  RRA_INCLUDE_DEBUG_CODE="0"
else
  RRA_COMPILED_LOG_LEVEL="RRA_LOG_LEVEL_MESSAGE"
  RRA_INCLUDE_DEBUG_CODE="0"
fi
AC_MSG_CHECKING([for log level compiled in])
AC_SUBST([RRA_COMPILED_LOG_LEVEL])
AC_DEFINE_UNQUOTED([RRA_COMPILED_LOG_LEVEL], [$RRA_COMPILED_LOG_LEVEL],
                   [Highest level of logging compiled into the code])
AC_MSG_RESULT([$RRA_COMPILED_LOG_LEVEL])

AC_MSG_CHECKING([for whether to include debugging code])
AC_SUBST([RRA_INCLUDE_DEBUG_CODE])
if test "$RRA_INCLUDE_DEBUG_CODE" == "1" ; then
  AC_DEFINE_UNQUOTED([RRA_INCLUDE_DEBUG_CODE], [1],
                     [Whether debug code should be included in the compile])
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

# Generic stuff
AC_CHECK_FUNCS([bzero memchr memmove memset])
AC_CHECK_FUNCS([strchr strerror strncasecmp strstr])
AC_CHECK_FUNCS([fwrite vsnprintf])
AC_CHECK_FUNCS([mkdtemp])

AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_PROG_AWK
AC_PROG_LN_S
AC_STRUCT_TM
AC_TYPE_SIZE_T

AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([locale.h iconv.h wchar.h])
AC_CHECK_HEADERS([malloc.h ctype.h errno.h signal.h])
AC_CHECK_HEADERS([syslog.h stdarg.h])
AC_CHECK_HEADERS([sys/cdefs.h])

AC_PATH_PROGS([REALPATH], [realpath], [\$(top_srcdir)/realpath.sh])
AC_PATH_PROGS([TAR], [gtar tar])

# Misc
AX_JM_CHECK_SSL
AX_JM_CHECK_READLINE

AM_WITH_DMALLOC
AM_WITH_MPATROL

# Boost
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([algorithm])
AC_CHECK_HEADERS([cstddef])
AC_CHECK_HEADERS([boost/crc.hpp])
AC_CHECK_HEADERS([boost/format.hpp])
AC_CHECK_HEADERS([boost/algorithm/string.hpp])
AC_CHECK_HEADERS([boost/cstdint.hpp])
AC_CHECK_HEADERS([boost/variant.hpp])
AC_LANG_POP([C++])
AC_PATH_PROGS([PYSTE], [pyste], [echo pyste])

# Efence
EFENCE_LIBS=""
AC_CHECK_LIB([efence], [main], [EFENCE_LIBS="-lefence"])
AC_SUBST(EFENCE_LIBS)

# Melunit
AX_PATH_MELUNIT_C
AX_PATH_MELUNIT_CXX

# Gettext
AC_CHECK_FUNCS([iconv iconv_open iconv_close])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.12])
AM_ICONV
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [Package name for gettext])
TRANSLATIONS="`for i in \`cat po/LINGUAS\` ; do echo -n \"$i \" ; done`"
AC_SUBST(TRANSLATIONS)
AC_DEFINE_UNQUOTED(TRANSLATIONS, ["$TRANSLATIONS"],
                   [The translations from po/LINGUAS])

# Doxygen
AX_JM_CHECK_DOXYGEN
AX_JM_CREATE_DOXYFILE([rregadmin], [], [
  INTERNAL_DOCS="YES"
  FULL_PATH_NAMES="YES"
  OPTIMIZE_OUTPUT_FOR_C="YES"
  TYPEDEF_HIDES_STRUCT="YES"
  OUTPUT_DIRECTORY="rregadmin/doxygen-docs"
  SHOW_DIRECTORIES="YES"
  DIRECTORY_GRAPH="YES"
  QUIET="NO"
])

# Glib
AM_PATH_GLIB_2_0
CPPFLAGS="$CPPFLAGS $GLIB_CFLAGS"
LIBS="$LIBS $GLIB_LIBS"
AC_CHECK_HEADERS([glib/gstdio.h])
AC_CHECK_FUNCS([g_file_set_contents])
AC_CHECK_FUNCS([g_slice_alloc])
AC_CHECK_FUNCS([g_sequence_new])
AC_CHECK_FUNCS([g_ascii_strtoll])
G_LOG_DOMAIN="RRegAdmin"
AC_SUBST([G_LOG_DOMAIN])
AC_DEFINE_UNQUOTED([G_LOG_DOMAIN], ["$G_LOG_DOMAIN"],
                   [Log domain for RRegAdmin])
AC_CHECK_FUNCS([g_utf8_collate_key])

AC_MSG_CHECKING([if g_utf8_collate_key works as expected])
AC_RUN_IFELSE(
  AC_LANG_PROGRAM([
#include <string.h>
#include <glib.h>
const char *test_val1 = "foobar";
const char *test_val2 = "FooBar";
], [
   gchar *collate_key1 = g_utf8_collate_key(test_val1, -1);
   gchar *collate_key2 = g_utf8_collate_key(test_val2, -1);
   if (0 < strcmp(collate_key1, collate_key2)) {
      return 0;
   }
   else {
      return 1;
   }
]), [
  AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_GOOD_G_UTF8_COLLATE_KEY], [1],
            [whether g_utf8_collate_key works as expected])
], [
  AC_MSG_RESULT([no])
], [
  AC_MSG_RESULT([cross-compiling so assuming no])
])


# Popt
AC_CHECK_HEADERS([popt.h])
AC_CHECK_LIB([popt], [main])

# Splint
AC_PATH_PROGS([SPLINT], [splint], [echo splint])

# Fuse
PKG_CHECK_MODULES([fuse], [fuse], [], [
  AC_MSG_NOTICE([Ignoring missing fuse])])
AM_CONDITIONAL([HAVE_FUSE], [test -n "$fuse_CFLAGS"])
AC_CHECK_FUNCS([setxattr])
AC_PATH_PROGS([FUSERMOUNT], [fusermount])

# pkg-config
RREGADMIN_REQUIRES="glib-2.0"
RREGADMIN_CFLAGS="$CPPFLAGS"
RREGADMIN_LIBS="$LIBS -lrregadmin"
RREGADMIN_VERSION="$VERSION"
AX_JM_CONFIG_PKGCONFIG_IN(
  [rregadmin],
  [Library supporting modification of hives and registries.])

# Python
AC_MSG_CHECKING([whether to include the python rregadmin library interface])
AC_ARG_ENABLE([python-interface],
AS_HELP_STRING([--disable-python-interface],
               [disable the python rregadmin library interface]), [
  if test x$enableval = xyes ; then
    INCLUDE_PYTHON_INTERFACE=yes
  else
    INCLUDE_PYTHON_INTERFACE=no
  fi
  ], [
    INCLUDE_PYTHON_INTERFACE=yes
])
AC_MSG_RESULT([$INCLUDE_PYTHON_INTERFACE])

if test x$INCLUDE_PYTHON_INTERFACE = xyes ; then

  AX_WITH_PYTHON([2.4], [python2.4 python-2.4 python])
  AC_PYTHON_DEVEL

  AM_PATH_PYTHON

  AC_PYTHON_MODULE([elementtree.ElementTree], [], [false])
  if test $HAVE_PYMOD_ELEMENTTREE_ELEMENTTREE = "0" ; then
     AC_PYTHON_MODULE([xml.etree.ElementTree], [], [false])
  else
     AC_PYTHON_MODULE([xml.etree.ElementTree], [], [false])
  fi

  AC_PYTHON_MODULE([ctypes], [], [false])

  AC_PYTHON_MODULE([ctypeslib], [], [false])
  AC_PYTHON_MODULE([xml.parsers.xmlproc], [dtdparser], [false])

  AX_JM_BOOST_PYTHON
fi

AC_PATH_PROGS([H2XML], [h2xml.py h2xml ctypeslib-h2xml], [echo h2xml])
AC_PATH_PROGS([XML2PY], [xml2py.py xml2py ctypeslib-xml2py], [echo xml2py])

AM_CONDITIONAL([HAVE_CTYPES], [test "x$HAVE_PYMOD_CTYPES" == "x1"])
AM_CONDITIONAL([HAVE_CTYPESLIB], [test "x$H2XML" != "xecho h2xml" -a "x$XML2PY" != "xecho xml2py"])
AM_CONDITIONAL([SHOULD_INCLUDE_PYTHON_INTERFACE],
               test -n "$PYTHON" -a "x$INCLUDE_PYTHON_INTERFACE" = "xyes")



# rpm
if test -e $srcdir/rpm-release ; then
  RPM_RELEASE=`cat $srcdir/rpm-release`
else
  RPM_RELEASE=1
fi
AC_SUBST(RPM_RELEASE)

AX_JM_RPM_INIT
AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)

INC_AMRPMINCLUDE="include \$(top_srcdir)/m4/rpm.am"
AC_SUBST(INC_AMRPMINCLUDE)

# Lib versioning
RRA_LIB_VERSION_CURRENT="1"
RRA_LIB_VERSION_REVISION="0"
RRA_LIB_VERSION_AGE="0"
RRA_LTLIB_VERSION="$RRA_LIB_VERSION_CURRENT:$RRA_LIB_VERSION_REVISION:$RRA_LIB_VERSION_AGE"
AC_SUBST([RRA_LIB_VERSION_CURRENT])
AC_SUBST([RRA_LIB_VERSION_REVISION])
AC_SUBST([RRA_LIB_VERSION_AGE])
AC_SUBST([RRA_LTLIB_VERSION])

AC_SUBST(abs_top_srcdir)
AC_SUBST(abs_top_builddir)

AC_CONFIG_FILES([ po/Makefile.in
        Makefile
        rregadmin.spec
        m4/Makefile
        rregadmin/util/Makefile
        rregadmin/util/_test/Makefile
        rregadmin/util/data/Makefile
        rregadmin/doc/Makefile
        rregadmin/hive/Makefile
        rregadmin/hive/_test/Makefile
        rregadmin/cli/Makefile
        rregadmin/cli/_test/Makefile
        rregadmin/registry/Makefile
        rregadmin/registry/_test/Makefile
        rregadmin/diff/Makefile
        rregadmin/diff/_test/Makefile
        rregadmin/secdesc/Makefile
        rregadmin/secdesc/_test/Makefile
        rregadmin/sam/Makefile
        rregadmin/sam/_test/Makefile
        rregadmin/fuse/Makefile
        rregadmin/fuse/_test/Makefile
        rregadmin/Makefile
        rregadmin/_test/Makefile
        bin/Makefile
        bin/_test/Makefile
        bin/_test/rra_basic_cl/Makefile
        bin/_test/rra_registry_diff/Makefile
        examples/Makefile
        docs/Makefile
        docs/bugs/Makefile
        docs/notes/Makefile
        docs/todo/Makefile
        python/Makefile
        python/_test/Makefile
        python/_test/lib/Makefile
        python/rregadmin/Makefile
        python/rregadmin/util/Makefile
        python/rregadmin/util/_test/Makefile
        python/rregadmin/cli/Makefile
        python/rregadmin/cli/_test/Makefile
        python/rregadmin/registry/Makefile
        python/rregadmin/registry/_test/Makefile
        python/rregadmin/hive/Makefile
        python/rregadmin/hive/_test/Makefile
        python/rregadmin/diff/Makefile
        python/rregadmin/diff/_test/Makefile
        python/rregadmin/secdesc/Makefile
        python/rregadmin/secdesc/_test/Makefile
        python/rregadmin/sam/Makefile
        python/rregadmin/sam/_test/Makefile
        python/ctypeslib_extra/Makefile
        python/ctypeslib_extra/c_util/Makefile
        python/ctypeslib_extra/c_util/_test/Makefile
        test/Makefile
        test/bin/Makefile
        test/lib/Makefile
        test/lib/sha1-0.2/Makefile
        test/data/Makefile
        test/data/hives/Makefile
        test/data/registries/Makefile
        test/data/registries/example2/Makefile
        test/data/registries/example2/system32/Makefile
        test/data/registries/example2/system32/config/Makefile
        test/data/special/Makefile
        test/data/special/example1/Makefile
        test/data/other/Makefile
        test/data/inf/Makefile
        test/data/inf/example1/Makefile
])

AC_OUTPUT
