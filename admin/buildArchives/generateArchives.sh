#!/bin/bash

# Collect the main files of IsoAgLib which are
# relevant for setups

VERSION="2.1"

START=`pwd`
cd `dirname $0`
SCRIPT_DIR=`pwd`

cd ../..
ISOAGLIB_ROOT=`pwd`

echo "StartDir: $START, Script: $SCRIPT_DIR, Root: $ISOAGLIB_ROOT"

SVN_PART="-path '*/.svn/*'"
DOC_PART="-path '*/doc/*'"

TEMP1_PART="-name '*~'"
TEMP2_PART="-name '*.bak'"
TEMP3_PART="-name '*.o'"
TEMP4_PART="-name '*.tmp'"
EXEC_PART="-and -not \( -path '*/compiler_projects/kdevelop_make/*' -and -perm -111 \)"
COMM_BIOS_PART="-and -not \( -path '*/commercial_BIOS/*' -and -not -name '*.ilo' -and -not -name '*.txt' \)"

NEVER_WANTED_PART=`echo "-and -not \( $TEMP1_PART -o $TEMP2_PART -o $TEMP3_PART -o $TEMP4_PART \) $EXEC_PART $COMM_BIOS_PART"`


SUPPL_ALL_PART="-path '*/supplementary_driver/*'"
SUPPL_ACTOR_PART="-path '*/supplementary_driver/*/actor/*'"
SUPPL_RS232_PART="-path '*/supplementary_driver/*/rs232/*'"
SUPPL_SENSOR_PART="-path '*/supplementary_driver/*/sensor/*'"
SUPPL_DATASTREAMS_PART="-path '*/supplementary_driver/*/datastreams/*'"

PROC_PART="-path '*/comm/Part7_ProcessData/*'"
TC_PART="-path '*/comm/Part10_TaskController_Client/*'"
BASE_PART="-path '*/comm/Part7_ApplicationLayer/*'"

echo "Build File List for main core"
rm -f $SCRIPT_DIR/coreList.txt
CMDLINE=`echo "find . -type f -and -not \( $SVN_PART -o $SUPPL_ALL_PART -o $DOC_PART -o $SUPPL_ALL_PART -o $PROC_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/coreList.txt"`
echo $CMDLINE | sh
# add core bitmaps for doc
find ./doc/html/Diagramme/ -type f -not -path "*/.svn/*" >> $SCRIPT_DIR/coreList.txt
# add directories for Dev-C++
find examples/compiler_projects/Dev-C++/ -type d -name "objects"
echo "./library/commercial_BIOS/README_LICENSED_PARTS.txt" >> $SCRIPT_DIR/coreList.txt

echo "Build File List for Proces Module"
rm -f $SCRIPT_DIR/procList.txt
CMDLINE=`echo "find . -type f -and \( $PROC_PART -o $TC_PART \) -and -not \( $SVN_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/procList.txt"`
echo $CMDLINE | sh

echo "Build File List for Base Module"
rm -f $SCRIPT_DIR/baseList.txt
CMDLINE=`echo "find . -type f -and $BASE_PART -and -not \( $SVN_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/baseList.txt"`
echo $CMDLINE | sh

echo "Build File List for Supplementary Actor"
rm -f $SCRIPT_DIR/supplActorList.txt
CMDLINE=`echo "find . -type f -and $SUPPL_ACTOR_PART -and -not \( $SVN_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/supplActorList.txt"`
echo $CMDLINE | sh
echo "library/xgpl_src/supplementary_driver/hal/actor.h" >> "$SCRIPT_DIR/supplActorList.txt"
echo "library/xgpl_src/supplementary_driver/hal/readme.txt" >> "$SCRIPT_DIR/supplActorList.txt"

echo "Build File List for Supplementary RS232"
rm -f $SCRIPT_DIR/supplRs232List.txt
CMDLINE=`echo "find . -type f -and $SUPPL_RS232_PART -and -not \( $SVN_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/supplRs232List.txt"`
echo $CMDLINE | sh
echo "library/xgpl_src/supplementary_driver/hal/rs232.h" >> "$SCRIPT_DIR/supplRs232List.txt"
echo "library/xgpl_src/supplementary_driver/hal/readme.txt" >> "$SCRIPT_DIR/supplRs232List.txt"

echo "Build File List for Supplementary Sensor"
rm -f $SCRIPT_DIR/supplSensorList.txt
CMDLINE=`echo "find . -type f -and $SUPPL_SENSOR_PART -and -not \( $SVN_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/supplSensorList.txt"`
echo $CMDLINE | sh
echo "library/xgpl_src/supplementary_driver/hal/sensor.h" >> "$SCRIPT_DIR/supplSensorList.txt"
echo "library/xgpl_src/supplementary_driver/hal/readme.txt" >> "$SCRIPT_DIR/supplSensorList.txt"

echo "Build File List for Supplementary Datastreams"
rm -f $SCRIPT_DIR/supplDatastreamsList.txt
CMDLINE=`echo "find . -type f -and $SUPPL_DATASTREAMS_PART -and -not \( $SVN_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/supplDatastreamsList.txt"`
echo $CMDLINE | sh
echo "library/xgpl_src/supplementary_driver/hal/datastreams.h" >> "$SCRIPT_DIR/supplDatastreamsList.txt"
echo "library/xgpl_src/supplementary_driver/hal/readme.txt" >> "$SCRIPT_DIR/supplDatastreamsList.txt"


echo "Build File List for generated Docu"
rm -f $SCRIPT_DIR/generatedHtmlDocuList.txt
find ./doc/html -type f -not -path "*/.svn/*" -not -path "*/Diagramme/*" > $SCRIPT_DIR/generatedHtmlDocuList.txt


echo "Build Archives"

echo "Build coreArchiv.zip/.tar.gz"
rm -f $SCRIPT_DIR/coreArchiv.zip
cat $SCRIPT_DIR/coreList.txt | zip -9 $SCRIPT_DIR/coreArchiv-$VERSION.zip -@
tar -cf $SCRIPT_DIR/coreArchiv-$VERSION.tar -T $SCRIPT_DIR/coreList.txt
gzip $SCRIPT_DIR/coreArchiv-$VERSION.tar

echo "Build procArchiv.zip/.tar.gz"
rm -f $SCRIPT_DIR/procArchiv.zip
cat $SCRIPT_DIR/procList.txt | zip -9 $SCRIPT_DIR/procArchiv-$VERSION.zip -@
tar -cf $SCRIPT_DIR/procArchiv-$VERSION.tar -T $SCRIPT_DIR/procList.txt
gzip $SCRIPT_DIR/procArchiv-$VERSION.tar

echo "Build baseArchiv.zip/.tar.gz"
rm -f $SCRIPT_DIR/baseArchiv.zip
cat $SCRIPT_DIR/baseList.txt | zip -9 $SCRIPT_DIR/baseArchiv-$VERSION.zip -@
tar -cf $SCRIPT_DIR/baseArchiv-$VERSION.tar -T $SCRIPT_DIR/baseList.txt
gzip $SCRIPT_DIR/baseArchiv-$VERSION.tar

echo "Build supplActorArchiv.zip/.tar.gz"
rm -f $SCRIPT_DIR/supplActorArchiv.zip
cat $SCRIPT_DIR/supplActorList.txt | zip -9 $SCRIPT_DIR/supplActorArchiv-$VERSION.zip -@
tar -cf $SCRIPT_DIR/supplActorArchiv-$VERSION.tar -T $SCRIPT_DIR/supplActorList.txt
gzip $SCRIPT_DIR/supplActorArchiv-$VERSION.tar

echo "Build supplRs232Archiv.zip/.tar.gz"
rm -f $SCRIPT_DIR/supplRs232Archiv.zip
cat $SCRIPT_DIR/supplRs232List.txt | zip -9 $SCRIPT_DIR/supplRs232Archiv-$VERSION.zip -@
tar -cf $SCRIPT_DIR/supplRs232Archiv-$VERSION.tar -T $SCRIPT_DIR/supplRs232List.txt
gzip $SCRIPT_DIR/supplRs232Archiv-$VERSION.tar

echo "Build supplSensorArchiv.zip/.tar.gz"
rm -f $SCRIPT_DIR/supplSensorArchiv.zip
cat $SCRIPT_DIR/supplSensorList.txt | zip -9 $SCRIPT_DIR/supplSensorArchiv-$VERSION.zip -@
tar -cf $SCRIPT_DIR/supplSensorArchiv-$VERSION.tar -T $SCRIPT_DIR/supplSensorList.txt
gzip $SCRIPT_DIR/supplSensorArchiv-$VERSION.tar

echo "Build supplDatastreamsArchiv.zip/.tar.gz"
rm -f $SCRIPT_DIR/supplDatastreamsArchiv.zip
cat $SCRIPT_DIR/supplDatastreamsList.txt | zip -9 $SCRIPT_DIR/supplDatastreamsArchiv-$VERSION.zip -@
tar -cf $SCRIPT_DIR/supplDatastreamsArchiv-$VERSION.tar -T $SCRIPT_DIR/supplDatastreamsList.txt
gzip $SCRIPT_DIR/supplDatastreamsArchiv-$VERSION.tar

echo "Build generatedHtmlDocuList.zip"
rm -f $SCRIPT_DIR/generatedHtmlDocuList.zip
cat $SCRIPT_DIR/generatedHtmlDocuList.txt | zip -9 $SCRIPT_DIR/generatedHtmlDocuList.zip -@
