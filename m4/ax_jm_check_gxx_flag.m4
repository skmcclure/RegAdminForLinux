AC_DEFUN([AX_JM_CHECK_GCC_FLAG],[
  AC_LANG_PUSH(C++)
  option="$1"
  add_to_name="$2"
  SAVE_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $option"
  AC_MSG_CHECKING([whether gcc understands $option])
  AC_TRY_COMPILE([], [],
                 has_option=yes,
                 has_option=no,)
  CXXFLAGS="$SAVE_CXXFLAGS"
  AC_MSG_RESULT($has_option)
  if test $has_option = yes; then
    eval $add_to_name="$`echo $add_to_name`\ $option"
  fi
  unset has_option
  unset SAVE_CXXFLAGS
  unset option
  AC_LANG_POP(C++)
])
