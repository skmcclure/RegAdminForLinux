#! /bin/sh

IN_PATH=$1

cd `dirname $IN_PATH`
cd .

OUT_PATH=`pwd`/`basename $IN_PATH`

echo $OUT_PATH

#echo "Realpath.sh: $OUT_PATH" 1>&2
