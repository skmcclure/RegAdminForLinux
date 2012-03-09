#! /bin/sh

DEBUG_OUT="true"

assert_true() {

    if "$@" ; then
        if test "$ASSERT_VERBOSE" = "1" ; then
          echo "Success: $@"
        fi
    else
        echo "Failed: $@"
        exit 1
    fi
}

assert_false() {

    if "$@" ; then
        echo "Failed: not $@"
        exit 1
    else
        if test "$ASSERT_VERBOSE" = "1" ; then
            echo "Success: not $@"
        fi
    fi
}
