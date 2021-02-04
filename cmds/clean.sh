#!/bin/sh
set -e
. ./cmds/config.sh
 
echo "Cleaning QuarkOS..."

for PROJECT in $PROJECTS; do
  (cd src/$PROJECT && $MAKE clean -s)
done
 
rm src/sysroot.img
