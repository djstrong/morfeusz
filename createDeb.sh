#!/bin/bash

set -e

PKG_NAME="$1"
DESC="$2"
EXT_DESC="$3"
DEPENDS="$4"
SUGGESTED="$5"
VERSION="$6"
ARCH="$7"
SECTION="$8"
PKG_DIR="$9"
shift 9
TARGET_DIR="$1"

SIZE=`du -s $PKG_DIR | cut -f1`



mkdir -p "$PKG_DIR/DEBIAN"
CONTROL_FILE="$PKG_DIR/DEBIAN/control"
cat >"$CONTROL_FILE" <<EOL
Package: ${PKG_NAME}
Version: ${VERSION}
Section: ${SECTION}
Priority: optional
Architecture: ${ARCH}
Installed-Size: ${SIZE}
Original-Maintainer: Michał Lenart <michal.lenart@ipipan.waw.pl>
Maintainer: Słownik gramatyczny języka polskiego
Homepage: http://sgjp.pl/morfeusz
Depends: ${DEPENDS}
Suggests: ${SUGGESTED}
Description: ${DESC}
 ${EXT_DESC}
EOL

if [ -z "$DEPENDS" ]; then
    sed -i '/Depends: /d' "$CONTROL_FILE"
fi
if [ -z "$SUGGESTED" ]; then
    sed -i '/Suggests: /d' "$CONTROL_FILE"
fi

chmod 0644 "$CONTROL_FILE"
chmod g-w "$PKG_DIR" -R

fakeroot dpkg-deb --build "$PKG_DIR" "$TARGET_DIR"
