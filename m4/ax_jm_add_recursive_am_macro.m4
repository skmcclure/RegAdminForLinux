dnl @synopsis AX_JM_ADD_RECURSIVE_AM_MACRO([TARGET],[RULE])
dnl
dnl Adds the specified rule to $AMINCLUDE along with a TARGET-recursive
dnl rule that will call TARGET for the current directory and TARGET-am
dnl recursively for each subdirectory
dnl
dnl @category Automake
dnl @author Tom Howard <tomhoward@users.sf.net>
dnl @version 2005-01-14
dnl @license AllPermissive

AC_DEFUN([AX_JM_ADD_RECURSIVE_AM_MACRO],[
  AX_JM_ADD_AM_MACRO([
$1-recursive:
	@set fnord ${AX_JM_DOLLAR}${AX_JM_DOLLAR}MAKEFLAGS; amf=${AX_JM_DOLLAR}${AX_JM_DOLLAR}2; \\
	dot_seen=no; \\
	list='${AX_JM_DOLLAR}(SUBDIRS)'; for subdir in ${AX_JM_DOLLAR}${AX_JM_DOLLAR}list; do \\
	  echo \"Making $1 in ${AX_JM_DOLLAR}${AX_JM_DOLLAR}subdir\"; \\
	  if test \"${AX_JM_DOLLAR}${AX_JM_DOLLAR}subdir\" = \".\"; then \\
	    dot_seen=yes; \\
	    local_target=\"$1-am\"; \\
	  else \\
	    local_target=\"$1\"; \\
	  fi; \\
	  (cd ${AX_JM_DOLLAR}${AX_JM_DOLLAR}subdir && ${AX_JM_DOLLAR}(MAKE) ${AX_JM_DOLLAR}(AM_MAKEFLAGS) ${AX_JM_DOLLAR}${AX_JM_DOLLAR}local_target) \\
	   || case \"${AX_JM_DOLLAR}${AX_JM_DOLLAR}amf\" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \\
	done; \\
	if test \"${AX_JM_DOLLAR}${AX_JM_DOLLAR}dot_seen\" = \"no\"; then \\
	  ${AX_JM_DOLLAR}(MAKE) ${AX_JM_DOLLAR}(AM_MAKEFLAGS) \"$1-am\" || exit 1; \\
	fi; test -z \"${AX_JM_DOLLAR}${AX_JM_DOLLAR}fail\"

$2
])
])
