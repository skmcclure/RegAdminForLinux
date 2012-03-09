AC_DEFUN([AX_JM_CHECK_EXTRA_GXX_OPTIONS], [

  # test to see if -fvisibility-inlines-hidden works.
  option="-fvisibility-inlines-hidden"
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $option"
  AC_MSG_CHECKING([whether gcc understands $option])
  AC_TRY_COMPILE([], [],
                 has_option=yes,
                 has_option=no,)
  CFLAGS="$SAVE_CFLAGS"
  AC_MSG_RESULT($has_option)
  if test $has_option = yes; then
    CXXFLAGS="$CXXFLAGS $option"
    AC_DEFINE_UNQUOTED([HAVE_FVISIBILITY_INLINES_HIDDEN], [1],
                       [Whether -fvisibility-inlines-hidden works with the compiler])
  fi
  unset has_option
  unset SAVE_CFLAGS
  unset option

  CFLAGS_VISIBILITY=""
  # test to see if -fvisibility-inlines-hidden works.
  option="-fvisibility=hidden"
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $option"
  AC_MSG_CHECKING([whether gcc understands $option])
  AC_TRY_COMPILE([], [],
                 has_option=yes,
                 has_option=no,)
  CFLAGS="$SAVE_CFLAGS"
  AC_MSG_RESULT($has_option)
  if test $has_option = yes; then
    CFLAGS_VISIBILITY="$CFLAGS_VISIBILITY $option"
    AC_DEFINE_UNQUOTED([HAVE_FVISIBILITY_HIDDEN], [1],
                       [Whether -fvisibility=hidden works with the compiler])
  fi
  AC_SUBST(CFLAGS_VISIBILITY)
  unset has_option
  unset SAVE_CFLAGS
  unset option

])
