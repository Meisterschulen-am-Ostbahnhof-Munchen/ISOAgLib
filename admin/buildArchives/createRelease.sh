#!/bin/sh
# create a release of IsoAgLib

RELEASE_VERION="2.1.2"


START=`pwd`
cd `dirname $0`
SCRIPT_DIR=`pwd`

cd ../..
ISOAGLIB_ROOT=`pwd`

echo "StartDir: $START, Script: $SCRIPT_DIR, Root: $ISOAGLIB_ROOT, Release: $RELEASE_VERION"


# A) Adapt Doxyfile to create correct release string
CMDLINE=`echo "perl -p -i -e 's/(PROJECT_NUMBER\s*=)\s*[^\n]*/\1 $RELEASE_VERION/g' Doxyfile"`
echo $CMDLINE | sh

# B) adapt Download links to release version
CMDLINE=`echo "perl -p -i -e 's/([^0-9])[\-0-9\.]*(\.zip<\/td>)/\1-$RELEASE_VERION\2/g' readme.txt"`
echo $CMDLINE | sh

# C) remove old DOC
find $ISOAGLIB_ROOT -name "*.html" -o -name "*.png" -o -name "*.md5" -o -name "*.map" -exec rm -f {} \;


# B) Create Doc
doxygen doc/Doxyfile

# B) Build Packages
cd $START
sh "generateArchives.sh"

# C) create Download directory and move packages with current verison number to that location
mkdir -p "$ISOAGLIB_ROOT/doc/html/Download"
for Archive in `find . -name "*.zip"` ; do
	CombinedName=`basename $Archive .zip`
	CombinedName=$CombinedName"-$RELEASE_VERION.zip"
	echo "$Archive ---> $ISOAGLIB_ROOT/doc/html/Download/$CombinedName"
	mv $Archive "$ISOAGLIB_ROOT/doc/html/Download/$CombinedName"
done
