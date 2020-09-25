#!/bin/sh
set -e
. ./headers.sh

echo "Building QuarkOS..."
 
for PROJECT in $PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install -s)
done