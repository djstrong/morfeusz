#!/bin/bash

GUI_DIR="$1"
LIB_VERSION="$2"
TARGET_DIR="$3"

PKG_NAME="morfeusz2-gui"
BIN_NAME="morfeusz-gui"

TMP_DIR=`mktemp -d`
PKG_DIR="$TMP_DIR"/"$PKG_NAME"_"$LIB_VERSION"
TARGET_BIN_DIR="$PKG_DIR/usr/bin"
mkdir -p "$TARGET_BIN_DIR"
cat > "$TARGET_BIN_DIR/$BIN_NAME" <<EOF
#!/bin/sh
java -jar /usr/share/java/morfeusz.jar
EOF
chmod +x "$TARGET_BIN_DIR/$BIN_NAME"
TARGET_JAR_DIR="$PKG_DIR/usr/share/java"
mkdir -p "$TARGET_JAR_DIR"
cp "$GUI_DIR/morfeusz.jar" "$TARGET_JAR_DIR"

TARGET_MENU_DIR="$PKG_DIR/usr/share/menu"
mkdir -p "$TARGET_MENU_DIR"
cat > "$TARGET_MENU_DIR/$PKG_NAME" <<EOF
?package($PKG_NAME):needs="X11" section="Applications/Education"\
  title="morfeusz2" command="/usr/bin/$BIN_NAME"
EOF

TARGET_APP_DIR="$PKG_DIR/usr/share/applications"
mkdir -p "$TARGET_APP_DIR"
cat > "$TARGET_APP_DIR/morfeusz.desktop" <<EOF
[Desktop Entry]
Encoding=UTF-8
Type=Application
Name=Morfeusz
Comment=The Morphological Analyzer Morfeusz
Comment[pl]=Analizator morfologiczny Morfeusz
Icon=/usr/share/morfeusz2/morfeusz.svg
Exec=/usr/bin/$BIN_NAME
Terminal=false
Categories=Office;Dictionary;GTK;
EOF

TARGET_SHARE_DIR="$PKG_DIR/usr/share/morfeusz2"
mkdir -p "$TARGET_SHARE_DIR"
cp "$GUI_DIR/morfeusz.svg" "$TARGET_SHARE_DIR"

mkdir "$PKG_DIR/DEBIAN"
cat > "$PKG_DIR/DEBIAN/postinst" <<EOF
#!/bin/sh
set -e
if [ "$1" = "configure" ] && [ -x "`which update-menus 2>/dev/null`" ]; then
    update-menus
fi
EOF
cat > "$PKG_DIR/DEBIAN/postrm" <<EOF
#!/bin/sh
set -e
if [ -x "`which update-menus 2>/dev/null`" ]; then update-menus ; fi
EOF
chmod +x "$PKG_DIR/DEBIAN"/post*


DIR="$( cd "$( dirname "$0" )" && pwd )"

"$DIR/createDeb.sh" "$PKG_NAME" \
    "GUI for Morfeusz." \
    "GUI for the morphological analyzer Morfeusz." \
    "java-runtime, libjmorfeusz-java" "" "$LIB_VERSION" all x11 "$PKG_DIR" "$TARGET_DIR"
