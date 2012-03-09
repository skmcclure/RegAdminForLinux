
AC_DEFUN([AX_JM_CHECK_ICU],[

  AC_MSG_NOTICE([checking for icu])

  AC_PATH_PROGS([ICU_CONFIG], [icu-config])

  ICU_VERSION=""
  ICU_UNICODE_VERSION=""
  ICU_CPPFLAGS=""
  ICU_LDFLAGS=""
  ICU_IO_LDFLAGS=""

  if test -n "$ICU_CONFIG" ; then
     ICU_VERSION="`$ICU_CONFIG --version`"
     ICU_UNICODE_VERSION="`$ICU_CONFIG --unicode-version`"

     ICU_CPPFLAGS="`$ICU_CONFIG --cppflags`"
     ICU_LDFLAGS="`$ICU_CONFIG --ldflags`"
     ICU_IO_LDFLAGS="`$ICU_CONFIG --ldflags-icuio`"
  fi

  AC_LANG_PUSH([C++])

  CPPFLAGS_save="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $ICU_CPPFLAGS"
  AC_CHECK_HEADERS([unicode/ustring.h])
  AC_CHECK_HEADERS([unicode/ustream.h])
  CPPFLAGS="$CPPFLAGS_save"
  unset CPPFLAGS_save

  LDFLAGS_save="$LDFLAGS"
  LDFLAGS="$LDFLAGS $ICU_LDFLAGS $ICU_IO_LDFLAGS"
  AC_CHECK_LIB([icui18n], [main], [:], [
    AC_MSG_ERROR([Required library icui18n not found])])
  AC_CHECK_LIB([icuuc], [main], [:], [
    AC_MSG_ERROR([Required library icuuc not found])])
  AC_CHECK_LIB([icudata], [main], [:], [
    AC_MSG_ERROR([Required library icudata not found])])
  AC_CHECK_LIB([icuio], [main], [:], [
    AC_MSG_ERROR([Required library icuio not found])])
  LDFLAGS="$LDFLAGS_save"
  unset LDFLAGS_save

  AC_LANG_POP([C++])

  AX_JM_DEFINE_CONSTANT_UNQUOTED([ICU_VERSION],
                                 [$ICU_VERSION],
                                 [ICU version])
  AX_JM_DEFINE_CONSTANT_UNQUOTED([ICU_UNICODE_VERSION],
                                 [$ICU_UNICODE_VERSION],
                                 [ICU unicode data version])

  AC_SUBST([ICU_CPPFLAGS])
  AC_SUBST([ICU_LDFLAGS])
  AC_SUBST([ICU_IO_LDFLAGS])
])
