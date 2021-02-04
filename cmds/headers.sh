#!/bin/sh
set -e
. ./cmds/config.sh

echo "Initialising headers..."
 
rm -rf src/sysroot
cp -r src/static src/sysroot
 
for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  (cd src/$PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers -s)
done
