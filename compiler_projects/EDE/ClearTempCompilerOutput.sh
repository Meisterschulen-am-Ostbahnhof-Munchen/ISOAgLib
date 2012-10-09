#!/bin/sh
find IMI \( -iname "*.ti" -o -iname "*.ii" -o -iname "*.lst" -o -iname "*.obj" \) -exec rm -f {} \;

find IMI \( -iname "*.bak" -o -iname "*.inc" -o -iname "*.err" -o -iname "*.ilo" \) -exec rm -f {} \;
find IMI \( -iname "*.mak" -o -iname "*.src" -o -iname "*.ic" \) -exec rm -f {} \;
