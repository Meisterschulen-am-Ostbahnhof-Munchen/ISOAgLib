#!/bin/bash

# Collect the main files of IsoAgLib which are
# relevant for setups

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

PROC_PART="-path '*/comm/Part7_ProcessData/*'"
BASE_PART="-path '*/comm/Part7_ApplicationLayer/*'"
TERMINAL_ISO_PART="-path '*/comm/Part6_VirtualTerminal_Client/*'"
MULTIPACKET_PART="-path '*/comm/Part3_DataLink/*'"

echo "Build File List for main core"
rm -f $SCRIPT_DIR/coreList.txt
CMDLINE=`echo "find . -type f -and -not \( $SVN_PART -o $SUPPL_ALL_PART -o $DOC_PART -o $SUPPL_ALL_PART -o $PROC_PART -o $TERMINAL_ISO_PART -o $MULTIPACKET_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/coreList.txt"`
echo $CMDLINE | sh
# add core bitmaps for doc
find ./doc/html/Diagramme/ -type f -not -path "*/.svn/*" >> $SCRIPT_DIR/coreList.txt
# add directories for Dev-C++
find examples/compiler_projects/Dev-C++/ -type d -name "objects"
echo "./library/commercial_BIOS/README_LICENSED_PARTS.txt" >> $SCRIPT_DIR/coreList.txt

echo "Build File List for Proces Module"
rm -f $SCRIPT_DIR/procList.txt
CMDLINE=`echo "find . -type f -and $PROC_PART -and -not \( $SVN_PART \) $NEVER_WANTED_PART > $SCRIPT_DIR/procList.txt"`
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

echo "Build File List for generated Docu"
rm -f $SCRIPT_DIR/generatedHtmlDocuList.txt
find ./doc/html -type f -not -path "*/.svn/*" -not -path "*/Diagramme/*" > $SCRIPT_DIR/generatedHtmlDocuList.txt


echo "Build Archives"

echo "Build coreArchiv.zip"
rm -f $SCRIPT_DIR/coreArchiv.zip
cat $SCRIPT_DIR/coreList.txt | zip -9 $SCRIPT_DIR/coreArchiv.zip -@

echo "Build procArchiv.zip"
rm -f $SCRIPT_DIR/procArchiv.zip
cat $SCRIPT_DIR/procList.txt | zip -9 $SCRIPT_DIR/procArchiv.zip -@

echo "Build baseArchiv.zip"
rm -f $SCRIPT_DIR/baseArchiv.zip
cat $SCRIPT_DIR/baseList.txt | zip -9 $SCRIPT_DIR/baseArchiv.zip -@

echo "Build supplActorArchiv.zip"
rm -f $SCRIPT_DIR/supplActorArchiv.zip
cat $SCRIPT_DIR/supplActorList.txt | zip -9 $SCRIPT_DIR/supplActorArchiv.zip -@

echo "Build supplRs232Archiv.zip"
rm -f $SCRIPT_DIR/supplRs232Archiv.zip
cat $SCRIPT_DIR/supplRs232List.txt | zip -9 $SCRIPT_DIR/supplRs232Archiv.zip -@

echo "Build supplSensorArchiv.zip"
rm -f $SCRIPT_DIR/supplSensorArchiv.zip
cat $SCRIPT_DIR/supplSensorList.txt | zip -9 $SCRIPT_DIR/supplSensorArchiv.zip -@

echo "Build generatedHtmlDocuList.zip"
rm -f $SCRIPT_DIR/generatedHtmlDocuList.zip
cat $SCRIPT_DIR/generatedHtmlDocuList.txt | zip -9 $SCRIPT_DIR/generatedHtmlDocuList.zip -@
