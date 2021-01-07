#!/bin/sh
set -e
. ./config.sh
 
echo "Cleaning QuarkOS..."

for PROJECT in $PROJECTS; do
  (cd $PROJECT && $MAKE clean -s)
done
 
# rm -rf sysroot
# rm sysroot.img