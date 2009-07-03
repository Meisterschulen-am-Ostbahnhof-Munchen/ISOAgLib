#!/bin/sh
exec "$(cd "$(dirname "$0")/../../bin" && pwd)/update_makefile.sh" "$@"

