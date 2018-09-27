#!/bin/bash

INC_DIR="$1"
LIB_VERSION="$2"
TARGET_DIR="$3"

PKG_NAME="libmorfeusz2-dev"

TMP_DIR=`mktemp -d`
PKG_DIR="$TMP_DIR"/"$PKG_NAME"_"$LIB_VERSION"
TARGET_INC_DIR="$PKG_DIR/usr/include"
mkdir -p "$TARGET_INC_DIR"
cp "$INC_DIR/morfeusz2.h" "$INC_DIR/morfeusz2_c.h" "$TARGET_INC_DIR"

DIR="$( cd "$( dirname "$0" )" && pwd )"

"$DIR/createDeb.sh" "$PKG_NAME" \
    "Runtime library for Morfeusz - header files." \
    "Runtime library for Morfeusz - header files." \
    libmorfeusz2 "" "$LIB_VERSION" all libdevel "$PKG_DIR" "$TARGET_DIR"
