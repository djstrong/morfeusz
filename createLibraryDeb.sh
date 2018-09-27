#!/bin/bash

set -e

LIB_DIR="$1"
LIB_VERSION="$2"
ARCH="$3"
DICT="$4"
TARGET_DIR="$5"

if [ "$DICT" = polimorf ]; then
    exit
fi

#MAJOR_VERSION=`echo "$LIB_VERSION" | cut -f1 -d.`

PKG_NAME="libmorfeusz2"
LIB_NAME="$PKG_NAME.so"
#LIB_NAME_VERSION="$LIB_NAME.$MAJOR_VERSION"

TMP_DIR=`mktemp -d`
PKG_DIR="$TMP_DIR"/"$PKG_NAME"_"$LIB_VERSION"
TARGET_LIB_DIR="$PKG_DIR/usr/lib"
mkdir -p "$TARGET_LIB_DIR"
cp "$LIB_DIR/$LIB_NAME" "$TARGET_LIB_DIR"
chmod -x "$TARGET_LIB_DIR/$LIB_NAME"
strip --strip-unneeded "$TARGET_LIB_DIR/$LIB_NAME"
#ln -s "$LIB_NAME_VERSION" "$TARGET_LIB_DIR/$LIB_NAME"

mkdir "$PKG_DIR/DEBIAN"
#echo "$PKG_NAME $MAJOR_VERSION $PKG_NAME" > "$PKG_DIR/DEBIAN/shlibs"
cat > "$PKG_DIR/DEBIAN/postinst" <<EOF
#!/bin/sh
set -e
if [ "\$1" = "configure" ]; then
    ldconfig
fi
EOF
cat > "$PKG_DIR/DEBIAN/postrm" <<EOF
#!/bin/sh
set -e
if [ "\$1" = "remove" ]; then
    ldconfig
fi
EOF
chmod +x "$PKG_DIR/DEBIAN"/post*

DIR="$( cd "$( dirname "$0" )" && pwd )"

"$DIR/createDeb.sh" "$PKG_NAME" \
    "Runtime library for Morfeusz." \
    "Runtime library for the morphological analyzer Morfeusz." \
    "libc6, morfeusz2-dictionary-sgjp" "morfeusz2-dictionary-polimorf" "$LIB_VERSION" "$ARCH" libs \
    "$PKG_DIR" "$TARGET_DIR"
