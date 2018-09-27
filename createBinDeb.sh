#!/bin/bash

BIN_DIR="$1"
LIB_VERSION="$2"
ARCH="$3"
TARGET_DIR="$4"

PKG_NAME="morfeusz2"

TMP_DIR=`mktemp -d`
PKG_DIR="$TMP_DIR"/"$PKG_NAME"_"$LIB_VERSION"
TARGET_BIN_DIR="$PKG_DIR/usr/bin"
mkdir -p "$TARGET_BIN_DIR"
cp "$BIN_DIR/morfeusz_analyzer" "$BIN_DIR/morfeusz_generator" "$TARGET_BIN_DIR"
strip -s "$TARGET_BIN_DIR/morfeusz_analyzer" \
         "$TARGET_BIN_DIR/morfeusz_generator"

DIR="$( cd "$( dirname "$0" )" && pwd )"

"$DIR/createDeb.sh" "$PKG_NAME" \
    "Morphological analyzer for the Polish language." \
    "Morphological analyzer and generator for the Polish Language." \
    "libc6, libmorfeusz2" "morfeusz2-gui" "$LIB_VERSION" "$ARCH" misc "$PKG_DIR" "$TARGET_DIR"
