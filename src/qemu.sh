#!/bin/sh
set -e
. ./build.sh
echo "Invoking QEMU..."
qemu-system-i386 -kernel sysroot/boot/quarkos.kernel -serial stdio -drive file=test.img,format=raw "$@"
. ./clean.sh