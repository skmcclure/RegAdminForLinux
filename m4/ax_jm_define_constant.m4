# subst's and defines a constant

AC_DEFUN([AX_JM_DEFINE_CONSTANT], [

  AC_MSG_NOTICE([Setting constant $1 to $2])

  AC_DEFINE($1, $2, $3)

  define([MY_NAME],[$1])

  eval $1="$2"

  AC_SUBST(MY_NAME)

  undefine([MY_NAME])
])

AC_DEFUN([AX_JM_DEFINE_CONSTANT_UNQUOTED], [

  AC_MSG_NOTICE([Setting constant $1 to $2])

  AC_DEFINE_UNQUOTED($1, $2, $3)

  define([MY_NAME],[$1])

  eval $1="$2"

  AC_SUBST(MY_NAME)

  undefine([MY_NAME])
])
