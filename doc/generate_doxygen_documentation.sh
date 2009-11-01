#!/bin/sh

# A) make sure the current path is where this script lies.
cd `dirname $0`

# B) remove old DOC
find ./ -name "*.html" -o -name "*.png" -o -name "*.gif" -o -name "*.md5" -o -name "*.map" -exec rm -f {} \;

# C) copy images to api/all
mkdir -p html/all/images
mkdir -p html/api/images

cp graphics/*.png graphics/*.gif html/all/images
cp graphics/*.png graphics/*.gif html/api/images

# D) create Doc
doxygen DoxyfileAll
doxygen DoxyfileApi
