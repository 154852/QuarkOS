#!/bin/sh
set -e
. ./cmds/headers.sh

echo "Building QuarkOS..."
 
for PROJECT in $PROJECTS; do
  (cd src/$PROJECT && DESTDIR="$SYSROOT" $MAKE install)
done

echo "Building filesystem..."
python3 cmds/buildtar.py
