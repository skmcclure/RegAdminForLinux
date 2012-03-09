
AC_DEFUN([AX_JM_CHECK_DOXYGEN_FEATURE], [
  ax_cdf_FEATURE=$1

  dnl AC_MSG_CHECKING([if doxygen supports $ax_cdf_FEATURE])
  if test -n "$DOXYGEN_CONF_OUT" ; then
    ax_cdf_DOXYGEN_CONF="$DOXYGEN_CONF_OUT"
  else
    ax_cdf_DOXYGEN_CONF="`$DOXYGEN -s -g -`"
  fi
  ax_cdf_GREP_OUTPUT=`echo $ax_cdf_DOXYGEN_CONF | grep $ax_cdf_FEATURE`

  if test -n "$ax_cdf_GREP_OUTPUT" ; then
    dnl AC_MSG_RESULT([yes])
    $2
    :
  else
    dnl AC_MSG_RESULT([no])
    $3
    :
  fi
])
