#!/bin/sh
set -e
. ./build.sh
echo "Invoking QEMU..."
. ./start.sh "$@"
. ./clean.sh