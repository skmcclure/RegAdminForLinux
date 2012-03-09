AC_DEFUN([AX_CONSTANT], [

  AC_DEFINE($1, $2, $3)

  define([MY_NAME],[$1])

  eval "$1=\"$2\""

  AC_SUBST(MY_NAME)

  undefine([MY_NAME])
])

AC_DEFUN([AX_CONSTANT_UNQUOTED], [

  AC_DEFINE_UNQUOTED($1, $2, $3)

  define([MY_NAME],[$1])

  eval "$1=\"$2\""

  AC_SUBST(MY_NAME)

  undefine([MY_NAME])
])
