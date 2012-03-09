#! /bin/sh

#set -x
set -e

if test -z "$TOP_BUILDDIR" ; then
    TOP_BUILDDIR=../..
fi

GROUP_SCRIPT="$TOP_BUILDDIR/test/bin/ls_example_registries"

VAL_FILE="shared-values"

rm -f $VAL_FILE

# Get common values
for i in `$GROUP_SCRIPT` ; do

    if test -d $i ; then
        if test ! -e $VAL_FILE ; then
            if $TOP_BUILDDIR/test/bin/ls_reg_values $i > $VAL_FILE
            then
                :
            else
                rm $VAL_FILE
                exit 1
            fi
        else
            if cat $VAL_FILE | $TOP_BUILDDIR/test/bin/filter_values $i > $VAL_FILE.foo
            then
                mv $VAL_FILE.foo $VAL_FILE
            else
                rm $VAL_FILE
                exit 1
            fi
        fi
    fi
done
