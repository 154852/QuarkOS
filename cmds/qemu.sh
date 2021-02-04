#!/bin/sh
set -e
. ./cmds/build.sh
echo "Invoking QEMU..."
. ./cmds/start.sh "$@"
. ./cmds/clean.sh
