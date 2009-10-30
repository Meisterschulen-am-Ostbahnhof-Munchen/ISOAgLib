#!/bin/sh

cd `dirname $0`
cd ..

# A) remove old DOC
find doc/ -name "*.html" -o -name "*.png" -o -name "*.md5" -o -name "*.map" -exec rm -f {} \;

# B) Create Doc
doxygen doc/DoxyfileAll
doxygen doc/DoxyfileApi
