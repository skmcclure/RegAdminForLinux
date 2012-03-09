dnl @synopsis AX_JM_BOOST_SIGNALS
dnl
dnl This macro checks to see if the Boost.Signals library is installed.
dnl It also attempts to guess the currect library name using several
dnl attempts. It tries to build the library name using a user supplied
dnl name or suffix and then just the raw library.
dnl
dnl If the library is found, HAVE_BOOST_SIGNALS is defined and
dnl BOOST_SIGNALS_LIB is set to the name of the library.
dnl
dnl This macro calls AC_SUBST(BOOST_SIGNALS_LIB).
dnl
dnl @category InstalledPackages
dnl @author Michael Tindal <mtindal@paradoxpoint.com>
dnl @version 2004-09-20
dnl @license GPLWithACException

AC_DEFUN([AX_JM_BOOST_SIGNALS],
[AC_REQUIRE([AX_JM_CXX_NAMESPACES])dnl
AC_CACHE_CHECK(whether the Boost::Signal library is available,
ax_cv_boost_signal,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[#include <boost/signal.hpp>]],
			   [[boost::signal<void ()> sig; return 0;]]),
  			   ax_cv_boost_signal=yes, ax_cv_boost_signal=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_boost_signal" = yes; then
  AC_DEFINE(HAVE_BOOST_SIGNALS,[1],[define if the Boost::Signal library is available])
fi
  dnl Now determine the appropriate file names
  AC_ARG_WITH([boost-signals],AS_HELP_STRING([--with-boost-signals],
  [specify the boost signals library or suffix to use]),
  [if test "x$with_boost_signals" != "xno"; then
    ax_signals_lib=$with_boost_signals
    ax_boost_signals_lib=boost_signals-$with_boost_signals
  fi])
  for ax_lib in $ax_signals_lib $ax_boost_signals_lib boost_signals; do
    AC_CHECK_LIB($ax_lib, main, [BOOST_SIGNALS_LIB=$ax_lib break])
  done
  AC_SUBST(BOOST_SIGNALS_LIB)
])dnl
