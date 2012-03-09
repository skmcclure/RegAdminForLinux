dnl @synopsis AX_JM_PRINT_TO_FILE([FILE],[DATA])
dnl
dnl Writes the specified data to the specified file.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_JM_PRINT_TO_FILE],[
  AC_REQUIRE([AX_JM_FILE_ESCAPES])
  printf "$2" > "$1"
])

dnl @synopsis AX_JM_FILE_ESCAPES
dnl
dnl Writes the specified data to the specified file.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_JM_FILE_ESCAPES],[
  AX_JM_DOLLAR="\$"
  AX_JM_SRB="\\135"
  AX_JM_SLB="\\133"
  AX_JM_BS="\\\\"
  AX_JM_DQ="\""
])

dnl @synopsis AX_JM_APPEND_TO_FILE([FILE],[DATA])
dnl
dnl Appends the specified data to the specified file.
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_JM_APPEND_TO_FILE],[
  AC_REQUIRE([AX_JM_FILE_ESCAPES])
  printf "$2" >> "$1"
])
