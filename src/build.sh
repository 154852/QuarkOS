#!/bin/sh
set -e
. ./headers.sh

echo "Building QuarkOS..."
 
for PROJECT in $PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install)
done

echo "Building filesystem..."
python3 buildtar.py
