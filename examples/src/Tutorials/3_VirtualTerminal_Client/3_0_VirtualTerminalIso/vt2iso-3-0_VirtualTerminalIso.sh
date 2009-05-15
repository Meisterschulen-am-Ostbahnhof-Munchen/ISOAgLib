#!/bin/sh

# Absolute path of the IsoAgLib base directory:
BASE="$(cd "$(dirname "$0")/../../../../.." && pwd)"

exec "$BASE/bin/x86linux/vt2iso" "$BASE/examples/src/Tutorials/3_VirtualTerminal_Client/3_0_VirtualTerminalIso/MaskDefinition/simpleVTIsoPool"
