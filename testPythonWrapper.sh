#!/bin/bash
echo "testing python wrapper"
PROJECT_BINARY_DIR=$1
export LD_LIBRARY_PATH="$PROJECT_BINARY_DIR/morfeusz:$PROJECT_BINARY_DIR/morfeusz/wrappers/python/build/lib.linux-x86_64-2.7"
export PYTHONPATH=`find "$PROJECT_BINARY_DIR" -name _morfeusz2.so -print -quit | xargs dirname`
echo $PYTHONPATH
python test.py
