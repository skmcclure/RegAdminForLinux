dnl @synopsis AX_JM_BOOST_PYTHON
dnl
dnl This macro checks to see if the Boost.Python library is installed.
dnl It also attempts to guess the currect library name using several
dnl attempts. It tries to build the library name using a user supplied
dnl name or suffix and then just the raw library.
dnl
dnl If the library is found, HAVE_BOOST_PYTHON is defined and
dnl BOOST_PYTHON_LIB is set to the name of the library.
dnl
dnl This macro calls AC_SUBST(BOOST_PYTHON_LIB).
dnl
dnl In order to ensure that the Python headers are specified on the
dnl include path, this macro requires AX_JM_PYTHON to be called.
dnl
dnl @category InstalledPackages
dnl @author Michael Tindal <mtindal@paradoxpoint.com>
dnl @version 2004-09-20
dnl @license GPLWithACException

AC_DEFUN([AX_JM_BOOST_PYTHON],
[
AC_CACHE_CHECK(whether the Boost::Python library is available,
ac_cv_boost_python,
[AC_LANG_PUSH([C++])

 CPPFLAGS_SAVE=$CPPFLAGS
 if test x$PYTHON_CPPFLAGS != x; then
   CPPFLAGS="$PYTHON_CPPFLAGS $CPPFLAGS"
 fi
 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[
 #include <boost/python/module.hpp>
 using namespace boost::python;
 BOOST_PYTHON_MODULE(test) { throw "Boost::Python test."; }]],
 			   [[return 0;]]),
  			   ac_cv_boost_python=yes, ac_cv_boost_python=no)
 AC_LANG_POP([C++])
 CPPFLAGS=$CPPFLAGS_SAVE
])
if test "$ac_cv_python" = "yes"; then
  AC_DEFINE(HAVE_BOOST_PYTHON,[1],[define if the Boost::Python library is available])
  ax_python_lib=boost_python
  AC_ARG_WITH([boost-python],AS_HELP_STRING([--with-boost-python],[specify the boost python library or suffix to use]),
  [if test "x$with_boost_python" != "xno"; then
     ax_python_lib=$with_boost_python
     ax_boost_python_lib=boost_python-$with_boost_python
   fi])
  for ax_lib in $ax_python_lib $ax_boost_python_lib boost_python; do
    AC_CHECK_LIB($ax_lib, main, [BOOST_PYTHON_LIB=$ax_lib break])
  done
  AC_SUBST(BOOST_PYTHON_LIB)
fi
])dnl
