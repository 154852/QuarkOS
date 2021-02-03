#!/bin/sh
set -e
. ./config.sh

echo "Initialising headers..."
 
rm -rf sysroot
cp -r static sysroot
 
for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers -s)
done