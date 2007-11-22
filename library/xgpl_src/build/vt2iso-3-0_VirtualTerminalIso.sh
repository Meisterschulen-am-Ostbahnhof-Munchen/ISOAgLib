#!/bin/bash


### INSTRUCTIONS:
##################
### When using this tutorial script for your own project, you'll have to 
### change the path in (2), 
### the name of your XML in (4) and 
### if your xerces-c is NOT located in /usr/local/lib and can't be found by vt2iso, change the path in (3) also.

### 1. Save path to where this script lies.
CURDIR=`pwd`
DIR=`dirname $0`

### 2. Change to where to XML definitions are...
# cd $CURDIR/$DIR/../examples/tutorial/3_VirtualTerminal_Client/3_0_VirtualTerminalIso/MaskDefinition

### 3. If vt2iso won't find your xerces-c lib, you can pass the path in the LD_LIBRARY_PATH environment variable:
# export LD_LIBRARY_PATH=/usr/local/lib

### 4. Now call vt2iso
# PAR="$CURDIR/$DIR/vt2iso/bin/vt2iso simpleVTIsoPool.xml"

PAR="$CURDIR/$DIR/vt2iso/bin/vt2iso $CURDIR/$DIR/../../../examples/src/Tutorials/3_VirtualTerminal_Client/3_0_VirtualTerminalIso/MaskDefinition/simpleVTIsoPool"

exec $PAR
