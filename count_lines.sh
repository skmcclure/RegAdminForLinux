#! /bin/sh

# DIRS="`find . -type d \
#         | grep -v TEST \
#         | grep -v .deps \
#         | grep -v '^\.\/test' \
#         | grep -v '^\.$' \
#         `"

TMP_DIRS="rregadmin rregadminxx python bin examples"

DIRS=""
for i in $TMP_DIRS ; do
    if test -d $i ; then
        DIRS="$DIRS $i"
    fi
done

echo "DIRS=$DIRS"

echo "Running sloccount"
sloccount $DIRS

echo
echo
echo "----------------------------------------------------------------------"

CXX_SOURCE_FILES=""
for i in $DIRS ; do
    CXX_SOURCE_FILES="$CXX_SOURCE_FILES `find $i -name '*.[ch]' -o -name '*.[chi]pp'`"
done

echo "Running pmccabe"

pmccabe -v < /dev/null || true
pmccabe $CXX_SOURCE_FILES | sort -nr
