#!/bin/bash

set -exo pipefail

DIR=$1
shift
ARGS=`cat $DIR/ARGS`

DICT_DIR=`mktemp -d`
DICT=test

python fsabuilder/morfeusz_builder \
    --only-analyzer \
    --input-files $DIR/dictionary.tab \
    --dict-dir=$DICT_DIR \
    --dict=$DICT \
    --tagset-file=$DIR/tagset.dat \
    --segments-file=$DIR/segmentation.dat

build/morfeusz/morfeusz_analyzer --dict $DICT --dict-dir $DICT_DIR $ARGS < $DIR/input.txt > $DIR/output.txt
