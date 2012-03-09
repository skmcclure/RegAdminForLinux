AC_DEFUN([AX_JM_CHECK_DOXYGEN_FORMAT], [
  FORMATS_UPP=`echo $2 | sed -e "y:m4_cr_letters:m4_cr_LETTERS[]:"`

  AC_MSG_CHECKING([whether to enable $1 doxygen doc generation])

  define([EN_NAME], [ENABLE_[]$1[]_FORMAT])

  echo $FORMATS_UPP | grep $1 > /dev/null 2>&1
  if test "$?" = "0" ; then
    eval EN_NAME=YES
  else
    eval EN_NAME=NO
  fi
  AC_MSG_RESULT($EN_NAME)

  AC_SUBST(EN_NAME)

  AC_DEFINE_UNQUOTED(EN_NAME, ["$EN_NAME"],
                     [Whether to enable $1 doxygen format])

  undefine([EN_NAME])
])
