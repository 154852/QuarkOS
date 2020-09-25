#!/bin/sh
set -e
. ./config.sh

echo "Initialising headers..."
 
rm -rf sysroot
mkdir -p "$SYSROOT"
 
for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers -s)
done