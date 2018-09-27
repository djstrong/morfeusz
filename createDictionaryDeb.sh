#!/bin/bash

set -e

DICT_DIR="$1"
DICT_NAME="$2"
DICT_VERSION="$3"
TARGET_DIR="$4"

PKG_NAME="morfeusz2-dictionary-${DICT_NAME}"

TMP_DIR=`mktemp -d`
PKG_DIR="$TMP_DIR"/"$PKG_NAME"_"$DICT_VERSION"
TARGET_DICT_DIR="$PKG_DIR/usr/share/morfeusz/dictionaries"
mkdir -p "$TARGET_DICT_DIR"
cp "$DICT_DIR/$DICT_NAME-a.dict" "$TARGET_DICT_DIR"
cp "$DICT_DIR/$DICT_NAME-s.dict" "$TARGET_DICT_DIR"

DIR="$( cd "$( dirname "$0" )" && pwd )"

"$DIR/createDeb.sh" "$PKG_NAME" \
    "$DICT_NAME dictionary for Morfeusz." \
    "$DICT_NAME dictionary for the morphological analyzer Morfeusz." \
    "" "libmorfeusz2" "$DICT_VERSION" all misc "$PKG_DIR" "$TARGET_DIR"
