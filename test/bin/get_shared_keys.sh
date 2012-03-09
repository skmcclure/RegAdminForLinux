#! /bin/bash

#set -x
set -e

if test -z "$TOP_BUILDDIR" ; then
    TOP_BUILDDIR=../..
fi

GROUP_SCRIPT="$TOP_BUILDDIR/test/bin/ls_example_registries"

KEY_FILE="shared-keys"

rm -f $KEY_FILE

# Get common keys
for i in `$GROUP_SCRIPT` ; do

    if test -d $i ; then
        if test ! -e $KEY_FILE ; then
            if $TOP_BUILDDIR/test/bin/ls_reg_keys $i > $KEY_FILE
            then
                :
            else
                rm $KEY_FILE
                exit 1
            fi
        else
            if cat $KEY_FILE | $TOP_BUILDDIR/test/bin/filter_keys $i > $KEY_FILE.foo
            then
                mv $KEY_FILE.foo $KEY_FILE
            else
                rm $KEY_FILE
                exit 1
            fi
        fi
    fi

done
