#!/bin/bash

set -e

JAVA_DIR="$1"
LIB_VERSION="$2"
ARCH="$3"
TARGET_DIR="$4"

PKG_NAME="libjmorfeusz-java"
JNI_NAME="libjmorfeusz.so"
JAR_NAME="jmorfeusz.jar"

TMP_DIR=`mktemp -d`
PKG_DIR="$TMP_DIR"/"$PKG_NAME"_"$LIB_VERSION"
TARGET_JNI_DIR="$PKG_DIR/usr/lib/jni"
TARGET_JAR_DIR="$PKG_DIR/usr/share/java"
mkdir -p "$TARGET_JNI_DIR"
mkdir -p "$TARGET_JAR_DIR"
cp "$JAVA_DIR/$JNI_NAME" "$TARGET_JNI_DIR"
cp "$JAVA_DIR/$JAR_NAME" "$TARGET_JAR_DIR"
chmod -x "$TARGET_JNI_DIR/$JNI_NAME"
strip --strip-unneeded "$TARGET_JNI_DIR/$JNI_NAME"

DIR="$( cd "$( dirname "$0" )" && pwd )"

"$DIR/createDeb.sh" "$PKG_NAME" \
    "Java bindings for Morfeusz." \
    "Java bindings for the morphological analyzer Morfeusz." \
    "libc6, libmorfeusz2" "" "$LIB_VERSION" "$ARCH" java \
    "$PKG_DIR" "$TARGET_DIR"
