#!/bin/bash

WORKDIR=$1
WHAT=$2
CMD=$WORKDIR/morfeusz_$WHAT
DIR=$3

echo "build test FSA for $DIR"

TMP_DICTIONARY=`mktemp -d`
DICT_NAME=test

python fsabuilder/morfeusz_builder \
    --only-$WHAT \
    --input-files $DIR/dictionary.tab \
    --dict $DICT_NAME \
    --dict-dir $TMP_DICTIONARY \
    --tagset-file=$DIR/tagset.dat \
    --segments-file=$DIR/segmentation.dat

echo "testing $DIR"

INPUT=$DIR/input.txt
OUTPUT=$DIR/output.txt
TMP_OUTPUT=`mktemp`
ARGS=`cat $DIR/ARGS`

$CMD --dict-dir $TMP_DICTIONARY --dict $DICT_NAME $ARGS < $INPUT > $TMP_OUTPUT

if [ $? -ne 0 ]
then
    echo "command '$ARGS < $INPUT > $TMP_OUTPUT' returned non-zero exit status" >&2
    exit 1
fi
diff=`diff --brief $OUTPUT $TMP_OUTPUT`
if [ "$diff" != "" ]
then
    diff -u $OUTPUT $TMP_OUTPUT
    echo "#### Output for '$INPUT' differs from '$OUTPUT' contents" >&2
    exit 1
fi
