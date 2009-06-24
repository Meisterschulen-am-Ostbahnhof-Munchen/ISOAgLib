#!/bin/sh
# ####################################################################### #
#                         update_makefile.sh  -
#                          Script to create file list, configuration
#                          headers and project files for applications
#                          based on a feature set specifying config file
#                            -------------------
#   begin                 Mon Aug 02 2004
#   copyright            : (C) 2004 - 2006 by Dipl.-Inform. Achim Spangler
#   email                : a.spangler@osb-ag:de
# ####################################################################### #

# ####################################################################### #
#                                                                         #
# This file is part of the "IsoAgLib", an object oriented program library #
# to serve as a software layer between application specific program and   #
# communication protocol details. By providing simple function calls for  #
# jobs like starting a measuring program for a process data value on a    #
# remote ECU, the main program has not to deal with single CAN telegram   #
# formatting. This way communication problems between ECU's which use     #
# this library should be prevented.                                       #
# Everybody and every company is invited to use this library to make a    #
# working plug and play standard out of the printed protocol standard.    #
#                                                                         #
# Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  #
#                                                                         #
# The IsoAgLib is free software; you can redistribute it and/or modify it #
# under the terms of the GNU General Public License as published          #
# by the Free Software Foundation; either version 2 of the License, or    #
# (at your option) any later version.                                     #
#                                                                         #
# This library is distributed in the hope that it will be useful, but     #
# WITHOUT ANY WARRANTY; without even the implied warranty of              #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       #
# General Public License for more details.                                #
#                                                                         #
# You should have received a copy of the GNU General Public License       #
# along with IsoAgLib; if not, write to the Free Software Foundation,     #
# Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           #
#                                                                         #
# As a special exception, if other files instantiate templates or use     #
# macros or inline functions from this file, or you compile this file and #
# link it with other works to produce a work based on this file, this file#
# does not by itself cause the resulting work to be covered by the GNU    #
# General Public License. However the source code for this file must still#
# be made available in accordance with section (3) of the                 #
# GNU General Public License.                                             #
#                                                                         #
# This exception does not invalidate any other reasons why a work based on#
# this file might be covered by the GNU General Public License.           #
#                                                                         #
# Alternative licenses for IsoAgLib may be arranged by contacting         #
# the main author Achim Spangler by a.spangler@osb-ag:de                  #
# ####################################################################### #

# ####################################################################### #
# This file contains several helper functions to create
# project specific Makefiles based on several config variable
# settings. The vars are used to defined the features which are
# needed by the application
# ####################################################################### #

# global constants:
TAB="$(printf '\t')"
PATH_SEPARATOR1='=_=_'
PATH_SEPARATOR2='_=_='

main()
{
    set_default_values
    check_before_user_configuration "$@"
    
    # now source the config setting file
    . ./$CONF_FILE

    check_after_user_configuration
    
    # Create files to be used to build according to the users's
    # configuration settings
    create_buildfiles "$CONF_DIR" "$SCRIPT_DIR" "$START_DIR"
    
    report_summary
    
    make_doxygen_ready_comment_blocks
}

# Prefer these functions to echo due to incompatible versions (Bourne
# shell builtin, Bash builtin, command):
echo_() { printf '%s\n' "$*"; }
echo_e() { printf '%b\n' "$*"; }
echo_e_n() { printf '%b' "$*"; }
echo_n() { printf '%s' "$*"; }
# Compatible but slow variant as fallback:
eval 'echo() { '$(which echo)' "$@"; }'

# this function is used to verify and
# correct the several project configuration
# variables
# if one of the following variables isn't set
# the corresponding default values are used
# + USE_LITTLE_ENDIAN_CPU=1 --> most CPU types have little endian number variable representation -> number variable can be converted directly from int variable memory representation into CAN little endian string
# + USE_CAN_DRIVER="simulating"|"sys"|"msq_server"|"socket_server"|"socket_server_hal_simulator" -> select wanted driver connection for CAN
# + USE_CAN_DEVICE_FOR_SERVER="no_card"|"pcan"|"A1"|"rte" -> use this device for building the can_server
# + USE_RS232_DRIVER="simulating"|"sys"|"rte"|"hal_simulator" -> select wanted driver connection for RS232
# + CAN_BUS_CNT ( specify amount of available CAN channels at ECU; default 1 )
# + CAN_INSTANCE_CNT ( specify amount of CAN channels; default 1 )
# + PRT_INSTANCE_CNT ( specify amount of CAN channels to use for protocol; must be <= CAN_INSTANCE_CNT; default 1 )
# + RS232_INSTANCE_CNT ( specify amount of RS232 channels; default 1 )
# + PROJECT ( subdirectory name and executable filename defined by created Makefile )
# + REL_APP_PATH ( specify path to application of this project - error message if not given; use relative path!! )
# + APP_NAME ( optionally select single CC file for the main app - otherwise the whole given path is interpreted as part of this project )
#
# + PRJ_ISO11783=1 ( select ISO 11783, default not even ISO for internal CAN comm only )
# + PRJ_ISO_TERMINAL ( specify if ISO virtual terminal is wanted; default 0; only possible if PRJ_ISO11783=1 -> error message if not )
# + PRJ_BASE ( specify if Base data - main tractor information as PGN or LBS Base message - is wanted; default 0 )
# + PRJ_TRACTOR_GENERAL (only incorporate parts from BASE that provide tractor hitch and RPM information)
# + PRJ_TRACTOR_MOVE (only incorporate parts from BASE that provide tractor distance and speed information)
# + PRJ_TRACTOR_PTO (only incorporate parts from BASE that provide tractor PTO information)
# + PRJ_TRACTOR_LIGHT (only incorporate parts from BASE that provide tractor lighting information)
# + PRJ_TRACTOR_FACILITIES (only incorporate parts from BASE that provide tractor facilities information)
# + PRJ_TRACTOR_AUX (only incorporate parts from BASE that provide tractor auxiliary valve information)
# + PRJ_TRACTOR_GUIDANCE (only incorporate parts from BASE that provide tractor guidance information)
# + PRJ_TRACTOR_CERTIFICATION (only incorporate parts from BASE that provide tractor certification information)
# + PRJ_TIME_GPS (only incorporate parts from BASE that provide time and GPS information)
# + PRJ_PROCESS ( specify if process data should be used ; default 0 )
#   - PROC_LOCAL ( specify if local process data shall be used; must be activated for all types of local process data; default 0 )
#     o PROC_LOCAL_STD ( specify if full featured local process data shall be used; default 0 )
#     o PROC_LOCAL_SIMPLE_MEASURE ( specify if local process data with restricted measurement feature set shall be used; default 0 )
#     o PROC_LOCAL_SIMPLE_SETPOINT ( specify if local process data with restricted setpoint feature set shall be used; default 0 )
#     o PROC_LOCAL_SIMPLE_MEASURE_SETPOINT ( specify if process daza with maximum restricted feature set shall be used; default 0 )
#   - PROC_REMOTE ( specify if remote process data shall be used; must be activated for all types of remote process data; default 0 )
#     o PROC_REMOTE_STD ( specify if full featured remote process data shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_MEASURE ( specify if remote process data with restricted measurement feature set shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_SETPOINT ( specify if remote process data with restricted setpoint feature set shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_MEASURE_SETPOINT ( specify if process daza with maximum restricted feature set shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED ( specify if process daza with maximum restricted feature without any distinction between measurement and setpoint set shall be used; default 0 )
# + PRJ_DATASTREAMS ( specify if the input and output filestream should be accessed by IsoAgLib; provides target HAL for filestream handling; default 0 )
# + PRJ_EEPROM ( specify if the EEPROM should be accessed by IsoAgLib; provides important functions for local process data and local ISO items; default 0 )
# + PRJ_ACTOR ( specify if IsoAgLib extension for PWM access should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_SENSOR_DIGITAL ( specify if IsoAgLib extension for digital sensor data read should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_SENSOR_COUNTER ( specify if IsoAgLib extension for counting sensor data read should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_SENSOR_ANALOG ( specify if IsoAgLib extension for analog sensor data read should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_RS232 ( specify if IsoAgLib extension for C++ stream oriented I/O should be used; default 0 )

set_default_values()
{
    PARAMETER_TARGET_SYSTEM="UseConfigFile"
    PARAMETER_CAN_DRIVER="UseConfigFile"
    PARAMETER_CAN_DEVICE_FOR_SERVER="UseConfigFile"
    PARAMETER_RS232_DRIVER="UseConfigFile"
    USE_EMBED_LIB_DIRECTORY="library/commercial_BIOS/bios_esx"
    USE_EMBED_HEADER_DIRECTORY="library/commercial_BIOS/bios_esx"
    #USE_EMBED_LIBS="C756/Xos20l.lib Module/Xma20l.lib"
    #USE_EMBED_BIOS_SRC="Xos20go.asm Xos20err.c xos20esx.h XOS20EEC.H XOS20EEC.OBJ"
    #USE_EMBED_ILO="Xos20lcs.ilo"
    USE_EMBED_COMPILER_DIR="c:/programme/tasking/c166"
    USE_STLPORT_HEADER_DIRECTORY="C:/STLport/stlport"
    # USE_STLPORT_LIBRARY_DIRECTORY=""
    # if spec file defines this, the setting is simply overwritten
    APP_SEARCH_SRC_CONDITION="*.C *.cc *.c *.cpp *.cxx"
    APP_SEARCH_HDR_CONDITION="*.h *.hpp"

    APP_PATH_EXCLUDE=""
    APP_SRC_EXCLUDE=""
    ISOAGLIB_INSTALL_PATH="/usr/local"
    USE_SYSTEM_DEFINE=""
    GENERATE_FILES_ROOT_DIR="$PWD"

    PRJ_SEND_DEBUG=0
    USE_FLOAT_DATA_TYPE=0
    OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED=0
    USE_LITTLE_ENDIAN_CPU=1
    # set default USE_LITTLE_ENDIAN_CPU, so that quick number variable
    # to CAN string conversions are possible:
    USE_LITTLE_ENDIAN_CPU=1
    # set default USE_PCAN_LIB, so that quick number variable to CAN
    # string conversions are possible:
    USE_PCAN_LIB=0
    # set default USE_BIG_ENDIAN_CPU, so that quick number variable to
    # CAN string conversions are possible:
    USE_BIG_ENDIAN_CPU=0
    # option to not switch the system relais to ON, on startup
    # (default: set relais to ON on startup):
    PRJ_DO_NOT_START_RELAIS_ON_STARTUP=0
    USE_VT_UNICODE_SUPPORT=0
    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
    CAN_BUS_CNT=1
    CAN_BUS_USED=0
    CAN_INSTANCE_CNT=1
    PRT_INSTANCE_CNT=1
    RS232_INSTANCE_CNT=1
    APP_NAME=''
    PRJ_PROPRIETARY_PGN_INTERFACE=0
    PRJ_ISO11783=0
    PRJ_ISO11783=0
    PRJ_ISO_TERMINAL=0
    PRJ_ISO_FILESERVER_CLIENT=0
    PRJ_ISO_TERMINAL=0
    PRJ_ISO_FILESERVER_CLIENT=0
    PRJ_ISO_TERMINAL_SERVER=0
    PRJ_ISO_TERMINAL_LAYOUT_MANAGER=0
    PRJ_RS232_OVER_CAN=0
    PRJ_MULTIPACKET_STREAM_CHUNK=1
    PRJ_BASE=0
    PRJ_TRACTOR_GENERAL=0
    PRJ_TRACTOR_MOVE=0
    PRJ_TRACTOR_PTO=0
    PRJ_TIME_GPS=0
    PRJ_TRACTOR_LIGHT=0
    PRJ_TRACTOR_FACILITIES=0
    PRJ_TRACTOR_AUX=0
    PRJ_TRACTOR_GUIDANCE=0
    PRJ_TRACTOR_CERTIFICATION=0
    PRJ_TRACTOR_MOVE_SETPOINT=0
    PRJ_TRACTOR_PTO_SETPOINT=0
    PRJ_PROCESS=0
    PROC_LOCAL=0
    PROC_LOCAL_STD=0
    PROC_LOCAL_SIMPLE_MEASURE=0
    PROC_LOCAL_SIMPLE_SETPOINT=0
    PROC_LOCAL_SIMPLE_MEASURE_SETPOINT=0
    PROC_REMOTE=0
    PROC_REMOTE_STD=0
    PROC_REMOTE_SIMPLE_MEASURE=0
    PROC_REMOTE_SIMPLE_SETPOINT=0
    PROC_REMOTE_SIMPLE_MEASURE_SETPOINT=0
    PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED=0
    PRJ_EEPROM=0
    PRJ_DATASTREAMS=0
    PRJ_ACTOR=0
    PRJ_SENSOR_DIGITAL=0
    PRJ_SENSOR_ANALOG=0
    PRJ_SENSOR_COUNTER=0
    PRJ_RS232=0
    PRJ_MULTIPACKET=0
    # has to be overridden by configuration:
    USE_TARGET_SYSTEM='void'
    USE_ISO_TERMINAL_GRAPHICCONTEXT=0
    USE_ISO_TERMINAL_GETATTRIBUTES=0
    USE_ISO_TERMINAL_ATTRIBUTES=0
    PRJ_ISO_TERMINAL_OBJECT_SELECTION1=''
    PRJ_ISO_TERMINAL_OBJECT_SELECTION2=''
    PRJ_ISO_TERMINAL_OBJECT_SELECTION3=''
    PRJ_ISO_TERMINAL_OBJECT_SELECTION4=''
}

check_set_correct_variables()
{
    local CONF_DIR="$1"

    if [ "$CAN_BUS_CNT" -lt "$CAN_INSTANCE_CNT" ]; then
        echo_ "ERROR! The amount of available CAN BUS channels at ECU must be at least as high as wanted of used CAN instances"
        exit 2
    fi

    : ${ISO_AG_LIB_PATH:?"ERROR! Please specify the path to the root directory of IsoAgLib - i.e. where xgpl_src and IsoAgLibExamples are located"}
    ISO_AG_LIB_INSIDE="../../$ISO_AG_LIB_PATH"

    # check if ISO_AG_LIB_PATH valid
    if [ ! -d "$ISO_AG_LIB_PATH" ]; then
        echo_ "ERROR! ISO_AG_LIB_PATH is not a directory"
    else
        if [ ! -d "$ISO_AG_LIB_PATH/library" ]; then
            echo_ "ERROR! No library found within ISO_AG_LIB ($ISO_AG_LIB_PATH) - maybe your directory is not updated?"
        fi
    fi

    if [ "$PRT_INSTANCE_CNT" -gt "$CAN_INSTANCE_CNT" ]; then
        echo_ "ERROR! The Amount of protocol instances must be smaller or equal to amount of CAN instances"
        echo_ "Current false setting: PRT_INSTANCE_CNT=$PRT_INSTANCE_CNT \t CAN_INSTANCE_CNT=$CAN_INSTANCE_CNT"
        exit 2
    elif [ "$PRT_INSTANCE_CNT" -lt 1 ]; then
        echo_ "ERROR! There must be at least one protocol instance"
        exit 2
    fi

    : ${PROJECT:?"ERROR! Please set the variable PROJECT to the build sub-directory and executable filename"}
    : ${REL_APP_PATH:?"ERROR! Please set the variable REL_APP_PATH to the directory of the application sources"}

    if [ "$PRJ_ISO11783" -lt 1 -a "$PRJ_ISO_TERMINAL" -gt 0 ]; then
        echo_ "Warning overwrite setting of PRJ_ISO_TERMINAL == $PRJ_ISO_TERMINAL as ISO11783 is not activated"
        echo_ "Set PRJ_ISO11783 to 1 if you want ISO 11783 virtual terminal"
        PRJ_ISO_TERMINAL=0
    fi

    if test "$PRJ_ISO11783" -lt 1 -a "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ; then
        echo_ "Warning overwrite setting of PRJ_ISO_FILESERVER_CLIENT == $PRJ_ISO_FILESERVER_CLIENT as ISO11783 is not activated"
        echo_ "Set PRJ_ISO11783 to 1 if you want ISO 11783 fileserver"
        PRJ_ISO_FILESERVER_CLIENT=0
    fi

    if [ "$PRJ_RS232_OVER_CAN" -gt 0 ]; then
        PRJ_RS232=1 # force activation of RS232 driver class when this special RS232 HAL is activated
    fi

    if [ "$PRJ_BASE" -gt 0 ]; then
        # activate all base data sub-features, when PRJ_BASE is activated
        PRJ_TRACTOR_GENERAL=1
        PRJ_TRACTOR_MOVE=1
        PRJ_TRACTOR_PTO=1
        PRJ_TIME_GPS=1
        PRJ_TRACTOR_LIGHT=1
        PRJ_TRACTOR_FACILITIES=1
        PRJ_TRACTOR_AUX=1
        PRJ_TRACTOR_GUIDANCE=1
        PRJ_TRACTOR_CERTIFICATION=1
    fi

    if [ "$PRJ_TRACTOR_MOVE_SETPOINT" -ne 0 ]; then
        PRJ_TRACTOR_MOVE=1 # force basic trac move to compile in
    fi
    if [ "$PRJ_TRACTOR_PTO_SETPOINT" -gt 0 ]; then
        PRJ_TRACTOR_PTO=1 # force basic trac move to compile in
    fi

    # and also deactivate the features that are only available with the extension directories
    EXT_BASE_SRC="$ISO_AG_LIB_PATH/library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/ext"

    if [ ! -d "$EXT_BASE_SRC" ]; then
        # ISO is not active -> deactivate all ISO only features
        echo_ "Information:"; echo_
        echo_ "The directory $EXT_BASE_SRC with (optional) extension modules does not exist."
        echo_ "Deactivating features:"; echo_
        echo_ "PRJ_TRACTOR_LIGHT=0"
        echo_ "PRJ_TRACTOR_AUX=0"
        echo_ "PRJ_TRACTOR_GUIDANCE=0"
        echo_ "PRJ_TRACTOR_CERTIFICATION=0"; echo_
        echo_ "Please contact m.rothmund@osb-ag.de to gain access to this restriced area."
        PRJ_TRACTOR_LIGHT=0
        PRJ_TRACTOR_AUX=0
        PRJ_TRACTOR_GUIDANCE=0
        PRJ_TRACTOR_CERTIFICATION=0
        echo_
    fi

    # overwrite settings from config file with command line parameter settings
    if [ $PARAMETER_TARGET_SYSTEM != "UseConfigFile" ] ; then
        USE_TARGET_SYSTEM=$PARAMETER_TARGET_SYSTEM
        # echo_ "Use Parameter value for target system: $USE_TARGET_SYSTEM"
        # else
        # echo_ "Use Target System from config file: $USE_TARGET_SYSTEM"
    fi
    if [ $PARAMETER_CAN_DRIVER != "UseConfigFile" ] ; then
        USE_CAN_DRIVER=$PARAMETER_CAN_DRIVER
        #echo_ "Use Parameter value for can driver: $PARAMETER_CAN_DRIVER"
    fi
    if [ $PARAMETER_CAN_DEVICE_FOR_SERVER != "UseConfigFile" ] ; then
        USE_CAN_DEVICE_FOR_SERVER=$PARAMETER_CAN_DEVICE_FOR_SERVER
    fi
    if [ $PARAMETER_RS232_DRIVER != "UseConfigFile" ] ; then
        USE_RS232_DRIVER=$PARAMETER_RS232_DRIVER
        #echo_ "Use Parameter value for rs232 driver: $PARAMETER_RS232_DRIVER"
    fi

    case "$USE_TARGET_SYSTEM" in
        pc_win32)
            USE_SYSTEM_DEFINE="SYSTEM_PC"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/Dev-C++/"
            IDE_NAME="Visual Studio, Dev-C++"
            ;;        
        pc_linux)
            USE_SYSTEM_DEFINE="SYSTEM_PC"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/kdevelop_make/"
            IDE_NAME="KDevelop, make"
            ;;        
        esx)
            USE_SYSTEM_DEFINE="SYSTEM_ESX"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE/"
            IDE_NAME="Tasking EDE"
            ;;        
        esxu)
            USE_SYSTEM_DEFINE="SYSTEM_ESXu"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        c2c)
            USE_SYSTEM_DEFINE="SYSTEM_C2C"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        imi)
            USE_SYSTEM_DEFINE="SYSTEM_IMI"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        pm167)
            USE_SYSTEM_DEFINE="SYSTEM_PM167"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        Dj1)
            USE_SYSTEM_DEFINE="SYSTEM_DJ1"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        p1mc)
            USE_SYSTEM_DEFINE="SYSTEM_P1MC"
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/CCS/"
            IDE_NAME="Code Composer Studio"
            ;;
    esac
    
    INC_LOC_STD_MEASURE_ELEMENTS=$(expr "$PROC_LOCAL" \& \
        \( "$PROC_LOCAL_STD" \| "$PROC_LOCAL_SIMPLE_SETPOINT" \) )
    INC_LOC_STD_SETPOINT_ELEMENTS=$(expr "$PROC_LOCAL" \& \
        \( "$PROC_LOCAL_STD" \| "$PROC_LOCAL_SIMPLE_MEASURE" \) )
    INC_LOC_SIMPLE_SETPOINT_ELEMENTS=$(expr "$PROC_LOCAL" \& \
        \( "$PROC_LOCAL_SIMPLE_SETPOINT" \| "$PROC_LOCAL_SIMPLE_MEASURE_SETPOINT" \) )
    INC_REM_STD_MEASURE_ELEMENTS=$(expr "$PROC_REMOTE" \& \
        \( "$PROC_REMOTE_STD" \| "$PROC_REMOTE_SIMPLE_SETPOINT" \) )
    INC_REM_STD_SETPOINT_ELEMENTS=$(expr "$PROC_REMOTE" \& \
        \( "$PROC_REMOTE_STD" \| "$PROC_REMOTE_SIMPLE_MEASURE" \) )
    INC_REM_SIMPLE_SETPOINT_ELEMENTS=$(expr "$PROC_REMOTE" \& \
        \( "$PROC_REMOTE_SIMPLE_SETPOINT" \| "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" \) )
    INC_REM_SIMPLE_MEASURE_ELEMENTS=$(expr "$PROC_REMOTE" \& \
        \( "$PROC_REMOTE_SIMPLE_MEASURE" \| "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" \) )
    if [ "$PRJ_TRACTOR_LIGHT" -eq 0 -o "$PRJ_ISO11783" -eq 0 ]; then
        PRJ_TRACTOR_LIGHT=0
    fi
    if [ "$PRJ_TRACTOR_FACILITIES" -eq 0 -o "$PRJ_ISO11783" -eq 0 ]; then
        PRJ_TRACTOR_FACILITIES=0
    fi
    if [ "$PRJ_TRACTOR_AUX" -eq 0 -o "$PRJ_ISO11783" -eq 0 ]; then
        PRJ_TRACTOR_AUX=0
    fi
    if [ "$PRJ_TRACTOR_GUIDANCE" -eq 0 -o "$PRJ_ISO11783" -eq 0 ]; then
        PRJ_TRACTOR_GUIDANCE=0
    fi
    if [ "$PRJ_TRACTOR_CERTIFICATION" -eq 0 -o "$PRJ_ISO11783" -eq 0 ]; then
        PRJ_TRACTOR_CERTIFICATION=0
    fi
    if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]  ; then
        PRJ_MULTIPACKET=1
    fi

    if [ "$PRJ_ISO_TERMINAL_SERVER" -gt 0 ]; then
        PRJ_MULTIPACKET=1
    fi
    if [ "$PRJ_ISO_TERMINAL" -gt 0 ]; then
        PRJ_MULTIPACKET=1
    fi
    if [ "$PRJ_PROPRIETARY_PGN_INTERFACE" -gt 0 ]; then
        PRJ_MULTIPACKET=1
    fi

    case "$USE_CAN_DRIVER" in
        simulating)
            if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -gt 0 ]; then
                echo_ 'The selected CAN driver "simulating" does NOT provide the enhanced CAN processing.'
                echo_ 'Thus the project files will be generated without enhanced CAN processing'
                PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
            fi
            ;;
        msq_server|socket_server|socket_server_hal_simulator)
            ;;
        sys)
            if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -gt 0 ]; then
                echo_ 'The selected CAN driver "sys" on embedded targets does NOT provide the enhanced CAN processing.'
                echo_ 'Thus the project files will be generated without enhanced CAN processing'
                PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
            fi
            ;;
        *)
            echo_ 'ERROR! Please set the config variable "USE_CAN_DRIVER" to one of "simulating"|"sys"|"msq_server"|"socket_server"|"socket_server_hal_simulator"'
            echo_ 'Current Setting is $USE_CAN_DRIVER'
            exit 3
            ;;
    esac
}

# Take the first argument as variable name and append the remaining
# arguments to the variable's value.
append()
{
    local VARIABLE="$1"
    local FUNCTION="append_$VARIABLE"
    type "$FUNCTION" >/dev/null 2>&1 ||
        eval "$FUNCTION() { $VARIABLE=\"\$$VARIABLE\$*\"; }"
    shift
    "$FUNCTION" "$@"
}

# Write to FD3 what's needed for finding COMM features.
comm_features()
{
    printf '%s' " -path '*/IsoAgLib/typedef.h' -o -path '*/hal/"$HAL_PATH"/typedef.h' -o -name 'isoaglib_config.h' -o -path '*/hal/config.h'" >&3
    if [ "$PRJ_BASE" -gt 0 ]; then
        printf '%s' " -o -path '*/Part7_ApplicationLayer/*'" >&3
    else
        if test "$PRJ_TRACTOR_GENERAL" -gt 0 -o "$PRJ_TRACTOR_MOVE" -gt 0 -o "$PRJ_TRACTOR_FACILITIES" -gt 0 -o "$PRJ_TRACTOR_PTO" -gt 0 -o "$PRJ_TRACTOR_LIGHT" -gt 0 -o "$PRJ_TRACTOR_AUX" -gt 0 -o "$PRJ_TIME_GPS" -gt 0 -o "$PRJ_TRACTOR_GUIDANCE" -gt 0 -o "$PRJ_TRACTOR_CERTIFICATION" -gt 0; then
            printf '%s' " -o -name 'ibasetypes.h' -o -name 'basecommon_c*'" >&3
        fi
        if [ "$PRJ_TRACTOR_GENERAL" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracgeneral_c*' \)" >&3
        fi
        if test "$PRJ_TRACTOR_MOVE" -gt 0 -a "$PRJ_TRACTOR_MOVE_SETPOINT" -gt 0 ; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracmove*' \)" >&3
        elif test "$PRJ_TRACTOR_MOVE" -gt 0 -a "$PRJ_TRACTOR_MOVE_SETPOINT" -lt 1 ; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracmove_c.*' \)" >&3
        fi
        if test "$PRJ_TRACTOR_PTO" -gt 0 -a "$PRJ_TRACTOR_PTO_SETPOINT" -gt 0 ; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracpto*' \)" >&3
        elif test "$PRJ_TRACTOR_PTO" -gt 0 -a "$PRJ_TRACTOR_PTO_SETPOINT" -lt 1 ; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracpto_c.*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_LIGHT" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*traclight*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_FACILITIES" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            # tracfacilities is only defined for ISO 11783
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracfacilities_c*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_AUX" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            # tracaux is only defined for ISO 11783
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracaux*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_GUIDANCE" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            # tracguidance is only defined for ISO 11783
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracguidance*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_CERTIFICATION" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            # tracguidance is only defined for ISO 11783
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*traccert*' \)" >&3
        fi
        if [ "$PRJ_TIME_GPS" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*timeposgps*' \)" >&3
        fi
    fi

    if [ "$PRJ_PROPRIETARY_PGN_INTERFACE" -gt 0 ]; then
        printf '%s' " -o -path '*/ProprietaryCan/*'" >&3
    fi

    if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]  ; then
        if [ "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o -path '*/Part13_FileServer_Client/*'" >&3
        fi
    fi

    if [ "$PRJ_ISO_TERMINAL" -gt 0 ]; then
        if [ "$USE_ISO_TERMINAL_GRAPHICCONTEXT" -eq 0 ]; then
            # exclude graphicscontext_c
            if [ "$PRJ_ISO_TERMINAL_LAYOUT_MANAGER" -gt 0 ]; then
                printf '%s' " -o \( -path '*/Part6_VirtualTerminal_Client/*' -a -not -name '*graphicscontext_c*' \)" >&3
            else
                printf '%s' " -o \( -path '*/Part6_VirtualTerminal_Client/i*' -a -not -name '*graphicscontext_c*' \)" >&3
            fi
        else
            if [ "$PRJ_ISO_TERMINAL_LAYOUT_MANAGER" -gt 0 ]; then
                printf '%s' " -o -path '*/Part6_VirtualTerminal_Client/*'" >&3
            else
                printf '%s' " -o -path '*/Part6_VirtualTerminal_Client/i*'" >&3
            fi
        fi
    fi
    if [ "$PRJ_DATASTREAMS" -lt 1 ]; then
        if test "$PRJ_ISO_TERMINAL" -gt 0 -o "$PRJ_TIME_GPS" -gt 0 ; then
            printf '%s' " -o -path '*/driver/datastreams/volatilememory_c.*'" >&3
        fi
    fi
    if [ "$PRJ_ISO11783" -gt 0 ]; then
        printf '%s' " -o -path '*/Part3_DataLink/i*multi*' -o -path '*/Part3_DataLink/impl/stream_c.*' -o -path '*/Part3_DataLink/istream_c.*' -o -path '*/supplementary_driver/driver/datastreams/streaminput_c.h'  -o -path '*/IsoAgLib/convert.h'" >&3
        if [ "$PRJ_MULTIPACKET_STREAM_CHUNK" -gt 0 ]; then
            printf '%s' " -o -path '*/Part3_DataLink/impl/streamchunk_c.*' -o -path '*/Part3_DataLink/impl/chunk_c.*'" >&3
        else
            printf '%s' " -o -path '*/Part3_DataLink/impl/streamlinear_c.*'" >&3
        fi
    fi

    if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]  ; then
        if [ "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o -path '*/Part13_FileServer_Client/*'" >&3
        fi
    fi
}

# Write to FD3 what's needed for finding DRIVER features.
driver_features()
{
    printf '%s' " -path '*/hal/"$HAL_PATH"/can/can*.h'  -o  -path '*/hal/"$HAL_PATH"/can/hal_can*' -o -path '*/hal/can.h' -o -path '*/driver/system*'  -o \( -path '*/hal/"$HAL_PATH"/system*' -not -path '*hal_simulator*' \) -o -path '*/hal/system.h' -o -path '*/hal/"$HAL_PATH"/errcodes.h' -o -path '*/hal/"$HAL_PATH"/config.h'" >&3


    echo_ "IsoAgLib's Project-Generator running..."
    echo_
    echo_ "CAN driver:    $USE_CAN_DRIVER"
    case "$USE_CAN_DRIVER" in
        simulating)
            printf '%s' " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_simulating*'" >&3
            ;;
        msq_server)
            printf '%s' " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_client_msq.*' -o -path '*/hal/"$HAL_PATH"/can/msq_helper*'" >&3
            ;;
        socket_server)
            printf '%s' " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_client_sock.*'" >&3
            ;;
        socket_server_hal_simulator)
            printf '%s' " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_client_sock_hal_simulator.*'" >&3
            ;;
    esac
    if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -lt 1 ]; then
        #in NOT enhanced can hal -> we need to integrate the sources for the central CAN FIFO
        printf '%s' " -o -path '*/hal/generic_utils/can/*'" >&3
    fi

    if [ "$PRJ_EEPROM" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/eeprom/*' -o -path '*/hal/"$HAL_PATH"/eeprom/*' -o -path '*/hal/eeprom.h' -o -name 'eeprom_adr.h'" >&3
    fi
    if [ "$PRJ_DATASTREAMS" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/datastreams/*' -o -path '*/hal/"$HAL_PATH"/datastreams/*' -o -path '*/hal/datastreams.h'" >&3
    fi
    if [ "$PRJ_ACTOR" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/actor*' -o -path '*/hal/"$HAL_PATH"/actor/actor.h' -o -path '*/hal/"$HAL_PATH"/actor/actor_target_extensions.*' -o -path '*/hal/actor.h'" >&3
    fi
    if [ "$PRJ_SENSOR_DIGITAL" -gt 0 ]; then
        printf '%s' " -o -name '*digitali_c.*'" >&3
    fi
    if [ "$PRJ_SENSOR_ANALOG" -gt 0 ]; then
        printf '%s' " -o -name '*analogi*'" >&3
    fi
    if [ "$PRJ_SENSOR_COUNTER" -gt 0 ]; then
        printf '%s' " -o -name '*counteri*'" >&3
    fi
    if test "$PRJ_SENSOR_DIGITAL" -gt 0 -o "$PRJ_SENSOR_ANALOG" -gt 0 -o "$PRJ_SENSOR_COUNTER" -gt 0 ; then
        printf '%s' " -o -path '*/hal/"$HAL_PATH"/sensor/sensor.h' -path '*/hal/"$HAL_PATH"/sensor/sensor_target_extensions.*' -o -name '*sensorbase_c.*' -o -name '*sensor_c.*' -o -name '*sensori_c.*' -o -path '*/hal/sensor.h'" >&3
    fi
    if [ "$PRJ_RS232" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/rs232/*' -o -path '*/hal/rs232.h' -o -path '*/hal/"$HAL_PATH"/rs232/rs232.h'" >&3
        if [ "$PRJ_RS232_OVER_CAN" -gt 0 ]; then
            printf '%s' " -o -path '*/hal/virtualDrivers/rs232/*'" >&3
        else
            #-- The following line is wrong and never had any effect.
            #-- Just leaving in for informational reasons.
            #     printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/rs232/*'" >&3
            echo_ "RS232 driver:  $USE_RS232_DRIVER"
            case "$USE_RS232_DRIVER" in
                simulating)
                    printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_simulating*'" >&3
                    ;;
                hal_simulator)
                    printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_hal_simulator*'" >&3
                    ;;
                rte)
                    printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_rte*'" >&3
                    ;;
                sys)
                    if [ "$USE_TARGET_SYSTEM" = "pc_win32" ]; then
                        printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_w32_sys*'" >&3
                    else
                        printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_linux_sys*'" >&3
                    fi
                    ;;
                #     PRJ_DEFINES="$PRJ_DEFINES USE_REAL_RS232"
                #     printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_simulating*'" >&3
                *)
                    echo_ 'ERROR! Please set the config variable "USE_RS232_DRIVER" to one of "simulating"|"sys"|"rte"|"hal_simulator"'
                    echo_ 'Current Setting is $USE_RS232_DRIVER'
                    exit 3
            esac
        fi
        printf '%s' " -o -path '*/hal/"$HAL_PATH"/rs232/rs232_target_extensions.h'" >&3
    fi

    # add the standard driver directory sources for CAN
    if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -gt 0 ]; then
        printf '%s' " -o \( -path '*/driver/can/*' -a -not -name 'msgobj*' \)" >&3
    else
        printf '%s' " -o -path '*/driver/can/*'" >&3
    fi
}


#Write to FD3 what's needed for finding COMM PROC features.
comm_proc_features()
{
    if [ "$PRJ_PROCESS" -gt 0 ]; then
        if [ -n "$COMM_PROC_FEATURES" ]; then
            printf '%s' " -o " >&3
        fi
        printf '%s' " -name 'processdatachangehandler_c.*' -o -name 'iprocess_c.*' -o -name 'elementddi_s.h' -o -name 'proc_c.h' -o -path '*/Part7_ProcessData/impl/proc*' -o -path '*/Part7_ProcessData/iprocesscmd*' -o -path '*/Part7_ProcessData/impl/processcmd*' -o -path '*/Part7_ProcessData/*procdata*base_c.h'" >&3

        if [ "$PRJ_ISO11783" -gt 0 -a "$PROC_LOCAL" -gt 0 ]; then
            # allow DevPropertyHandler
            printf '%s' " -o -path '*/Part10_TaskController_Client/i*devproperty*'" >&3
        else
            printf '%s' " -o \( -path '*/Part10_TaskController_Client/i*devproperty*' -a -not -name 'devproperty*' \)" >&3
        fi

        if [ "$PROC_LOCAL" -gt 0 ]; then
            printf '%s' " -o -path '*/Part7_ProcessData/Local/impl/*'" >&3

            if [ "$PROC_LOCAL_STD" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Local/Std/*'" >&3
            fi
            if [ "$PROC_LOCAL_SIMPLE_MEASURE" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Local/SimpleMeasure/*'" >&3
            fi
            if [ "$PROC_LOCAL_SIMPLE_SETPOINT" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Local/SimpleSetpoint/*'" >&3
            fi
            if [ "$PROC_LOCAL_SIMPLE_MEASURE_SETPOINT" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Local/SimpleMeasureSetpoint/*'" >&3
            fi

            if [ "$INC_LOC_STD_MEASURE_ELEMENTS" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Local/StdMeasureElements/*'" >&3
            fi
            if [ "$INC_LOC_STD_SETPOINT_ELEMENTS" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Local/StdSetpointElements/*'" >&3
            fi
            if [ "$INC_LOC_SIMPLE_SETPOINT_ELEMENTS" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Local/SimpleSetpointElements/*'" >&3
            fi
        fi
        if [ "$PROC_REMOTE" -gt 0 ]; then
            printf '%s' " -o -path '*/Part7_ProcessData/Remote/impl/*'" >&3

            if [ "$PROC_REMOTE_STD" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/Std/*'" >&3
            fi
            if [ "$PROC_REMOTE_SIMPLE_MEASURE" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/SimpleMeasure/*'" >&3
            fi
            if [ "$PROC_REMOTE_SIMPLE_SETPOINT" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/SimpleSetpoint/*'" >&3
            fi
            if [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" -gt 0 -a "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/SimpleMeasureSetpoint/*'" >&3
            elif [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" -gt 0 ]; then
                printf '%s' " -o -name '*procdataremotesimplesetpointsimplemeasure_c.*'" >&3
            elif [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED" -gt 0 ]; then
                printf '%s' " -o -name '*procdataremotesimplesetpointsimplemeasurecombined_c.*'" >&3
            fi
            if [ "$INC_REM_STD_MEASURE_ELEMENTS" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/StdMeasureElements/*'" >&3
            fi
            if [ "$INC_REM_SIMPLE_MEASURE_ELEMENTS" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/SimpleMeasureElements/*'" >&3
            fi
            if [ "$INC_REM_STD_SETPOINT_ELEMENTS" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/StdSetpointElements/*'" >&3
            fi
            if [ "$INC_REM_SIMPLE_SETPOINT_ELEMENTS" -gt 0 ]; then
                printf '%s' " -o -path '*/Part7_ProcessData/Remote/SimpleSetpointElements/*'" >&3
            fi
        fi

        if test "$INC_LOC_STD_MEASURE_ELEMENTS" -gt 0 -o "$INC_REM_STD_MEASURE_ELEMENTS" -gt 0 ; then
            printf '%s' " -o -path '*/Part7_ProcessData/StdMeasureElements/*'" >&3
        fi
        if test "$INC_LOC_STD_SETPOINT_ELEMENTS" -gt 0 -o "$INC_REM_STD_SETPOINT_ELEMENTS" -gt 0 ; then
            printf '%s' " -o -path '*/Part7_ProcessData/StdSetpointElements/*'" >&3
        fi

    fi
}

# Calculate part of find command and write to FD3
find_part()
{
    local OUTER_OP="$1"; local INNER_OP="$2"
    shift 2
    [ $# -gt 0 ] || return
    printf '%s \( ' "$OUTER_OP" >&3
    local OR; local PART
    local FORMAT="%s${INNER_OP}"
    for PART in $*; do
        printf "$FORMAT" "$OR" "$PART" >&3
        : ${OR:= -or }
    done
    printf ' \)' >&3
}

# this function uses the "find" cmd
# to derive based on the selected features the
# corresponding file list into filelist_$PROJECT_PURE.txt
create_filelist( )
{
    local GENERATE_FILES_ROOT_DIR="$1"
    local SCRIPT_DIR="$2"

    #USE_TARGET_SYSTEM="pc"
    local HAL_PATH
    case "$USE_TARGET_SYSTEM" in
        pc*)
            HAL_PATH="pc"
            ;;
        *)
            HAL_PATH=$USE_TARGET_SYSTEM
            ;;
    esac

    {
        local COMM_PROC_FEATURES="$(comm_proc_features 3>&1 1>&9)"
        local COMM_FEATURES="$(comm_features 3>&1 1>&9)"
        local DRIVER_FEATURES="$(driver_features 3>&1 1>&9)"
    } 9>&1

    local LIB_ROOT="$ISO_AG_LIB_INSIDE/library/xgpl_src"
    local SRC_EXT="\( -name '*.c' -o -name '*.cc' -o -name '*.cpp' \)"
    #  HDR_UTEST_EXT="\( -name '*-test.h' \)"
    #  HDR_UTEST_MOCK_EXT="\( -name '*-mock.h' \)"
    #  TESTRUNNER_EXT="\( -name 'testrunner.cpp' \)"
    #  UTEST_DIRS_EXT="\( -name 'utest' -type d \)"


    # go back to directory where config file resides
    mkdir -p "$GENERATE_FILES_ROOT_DIR"
    cd "$GENERATE_FILES_ROOT_DIR"
    # echo_ "create filelist with 1 $1 and GENERATE_FILES_ROOT_DIR $GENERATE_FILES_ROOT_DIR"
    # mkdir tmpdir
    mkdir -p $PROJECT
    cd $PROJECT
    FILELIST_LIBRARY_HDR="filelist"'__'"$PROJECT_hdr.library.txt"
    FILELIST_LIBRARY_PURE="filelist"'__'"$PROJECT.library.txt"

    FILELIST_APP_HDR="filelist"'__'"$PROJECT_hdr.app.txt"
    FILELIST_APP_PURE="filelist"'__'"$PROJECT.app.txt"

    local FILELIST_COMBINED_HDR="filelist"'__'"$PROJECT_hdr.txt"
    FILELIST_COMBINED_PURE="filelist"'__'"$PROJECT.txt"

    if [ "A$DOXYGEN_EXPORT_DIR" = "A" ]; then
        FILELIST_DOXYGEN_READY="filelist"'__'"$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER-doc.txt"
    else
        FILELIST_DOXYGEN_READY="$DOXYGEN_EXPORT_DIR/filelist"'__'"$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER-doc.txt"
    fi
    local FIND_TEMP_PATH="$(printf '%s' "-path '*/scheduler/*'" \
        " -o -path '*/Part5_NetworkManagement/*'" \
        " -o -path '*/Part12_DiagnosticsServices/*' " \
        " -o -path '*/util/*'" \
        " -o -path '*/Part3_DataLink/i*can*' " \
        "${COMM_PROC_FEATURES:+ -o ${COMM_PROC_FEATURES}}" \
        "${COMM_FEATURES:+ -o ${COMM_FEATURES}}" \
        "${DRIVER_FEATURES:+ -o ${DRIVER_FEATURES}}")"
    { local EXCLUDE_PATH_PART1="$(find_part '-and -not' -path $APP_PATH_EXCLUDE 3>&1 1>&9)"; } 9>&1
    : ${EXCLUDE_PATH_PART1:=-a -not -path '*/xgpl_src/build/*'}

    eval "find $LIB_ROOT -follow $SRC_EXT -a \( $FIND_TEMP_PATH \) $EXCLUDE_FROM_SYSTEM_MGMT $EXCLUDE_PATH_PART1 -printf '%h/%f\n' > $FILELIST_LIBRARY_PURE"
    eval "find $LIB_ROOT -follow -name '*.h' -a \( $FIND_TEMP_PATH \) $EXCLUDE_FROM_SYSTEM_MGMT $EXCLUDE_PATH_PART1 -printf '%h/%f\n' > $FILELIST_LIBRARY_HDR"

    ##############################
    # find application files
    ##############################
    {
        local APP_SRC_PART="$(find_part -and "-name '%s'" $APP_SRC_FILE 3>&1 1>&9)"
        local EXCLUDE_PATH_PART2="$(find_part '-and -not' "-path '%s'" $APP_PATH_EXCLUDE 3>&1 1>&9)"
        local EXCLUDE_SRC_PART="$(find_part '-and -not' "-path '%s'" $APP_SRC_EXCLUDE 3>&1 1>&9)"
        # remove the joker '*' from the file type spec, as this causes only trouble
        local APP_SEARCH_SRC_CONDITION1="$(printf '%s' "$APP_SEARCH_SRC_CONDITION" | tr -d '*')"
        local APP_SEARCH_SRC_TYPE_PART="$(find_part '' "-name '*%s'" $APP_SEARCH_SRC_CONDITION1 3>&1 1>&9)"
        local APP_SEARCH_HDR_TYPE_PART="$(find_part '' "-name '%s'" $APP_SEARCH_HDR_CONDITION 3>&1 1>&9)"
    } 9>&1

    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ -follow $APP_SEARCH_SRC_TYPE_PART $APP_SRC_PART $EXCLUDE_PATH_PART2 $EXCLUDE_SRC_PART -printf '%h/%f\n' >&3"
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ -follow $APP_SEARCH_HDR_TYPE_PART $APP_SRC_PART $EXCLUDE_PATH_PART2 $EXCLUDE_SRC_PART -printf '%h/%f\n' >&4"
    done 3>"$FILELIST_APP_PURE" 4>"$FILELIST_APP_HDR"

    cat $FILELIST_LIBRARY_PURE $FILELIST_APP_PURE > $FILELIST_COMBINED_PURE
    cat $FILELIST_LIBRARY_HDR $FILELIST_APP_HDR > $FILELIST_COMBINED_HDR

    {
        # create list with suitable block definition for doxygen import
        # start main block for all files for project
        #echo_ "/**" > $FILELIST_DOXYGEN_READY
        #echo_ "* \section FileLists$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Files for $PROJECT ." >&3
        #echo_ "*/" >&3
        #echo_ "/*@{*/" >&3
        echo_e "\n\n @section FileLists$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Files for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" >&3

        # write block of source files
        #echo_ "/**" >&3
        #echo_ "* \section SrcList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Sourcefiles for $PROJECT ." >&3
        #echo_ "*/" >&3
        #echo_ "/*@{*/" >&3
        echo_e "\n\n @section SrcList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Sourcefiles for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" >&3
        echo_ "\code" >&3
        cat $FILELIST_COMBINED_PURE >&3
        echo_ "\endcode" >&3
        #echo_ "/*@}*/" >&3

        echo_ "" >&3

        # write block of header files
        #echo_ "/**" >&3
        #echo_ "* \section HdrList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Sourcefiles for $PROJECT ." >&3
        #echo_ "*/" >&3
        #echo_ "/*@{*/" >&3
        echo_e "\n\n @section HdrList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Headers for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" >&3
        echo_ "\code" >&3
        cat $FILELIST_COMBINED_HDR >&3
        echo_ "\endcode" >&3
        #echo_ "/*@}*/" >&3

        # write end of main block of all project files
        #echo_ "/*@}*/" >&3
    } 3>"$FILELIST_DOXYGEN_READY"
    cat $FILELIST_COMBINED_HDR >> $FILELIST_COMBINED_PURE
    rm -f $FILELIST_COMBINED_HDR

    # go back to directory where config file resides
    cd "$GENERATE_FILES_ROOT_DIR"

    return
}

# this function uses the "find" cmd
# to derive based on the selected features the
# corresponding file list into filelist_$FILELIST_UTEST_PURE.txt
# and filelist_$FILELIST_UTEST_MOCK_PURE.txt
create_utest_filelist()
{
    # UTESTs are disabled for now.
    return

    FILELIST_UTEST_PURE="filelist"'__'"$PROJECT.utest.txt"
    FILELIST_UTEST_MOCK_PURE="filelist"'__'"$PROJECT.utest.mock.txt"
    FILELIST_UTEST_MODSUT_PURE="filelist"'__'"$PROJECT.utest.modsut.txt"
    FILELIST_UTEST_RUNNER_PURE="filelist"'__'"$PROJECT.utest.runner.txt"

    DIRECTORYLIST_UTEST_PURE="directories"'__'"$PROJECT.utest.txt"

    # go to project directory
    cd $PROJECT
    # create new files containing the file lists
    rm -f "$FILELIST_UTEST_PURE" "$FILELIST_UTEST_MOCK_PURE" "$FILELIST_UTEST_MODSUT_PURE" "$FILELIST_UTEST_RUNNER_PURE" "$DIRECTORYLIST_UTEST_PURE"
    touch "$FILELIST_UTEST_PURE" "$FILELIST_UTEST_MOCK_PURE" "$FILELIST_UTEST_MODSUT_PURE" "$FILELIST_UTEST_RUNNER_PURE" "$DIRECTORYLIST_UTEST_PURE"

    # find unit test directories

    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ $UTEST_DIRS_EXT >> $DIRECTORYLIST_UTEST_PURE"
    done

    while read line; do
    # find unit test files (*-test.h)
        eval "find $line $HDR_UTEST_EXT -printf '%h/%f\n' >> $FILELIST_UTEST_PURE"

    # find mock object files (*-mock.h)
        eval "find $line $HDR_UTEST_MOCK_EXT -printf '%h/%f\n' >> $FILELIST_UTEST_MOCK_PURE"
    done 0<$DIRECTORYLIST_UTEST_PURE
    # determine modified software under test (MOD-SUT) files
    sed -e 's!\(.*\)/utest/\(.*\)-test\.h!\1/\2_c.h!' < $FILELIST_UTEST_PURE >> $FILELIST_UTEST_MODSUT_PURE

    # find the testrunner (testrunner.cpp)
    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ $TESTRUNNER_EXT -printf '%h/%f\n' >> $FILELIST_UTEST_RUNNER_PURE"
    done
}

# function to create the project specific autogenerated
# config header
# autogen_project_config.h
create_autogen_project_config()
{
    # go to project dir - below config dir
    cd $1/$PROJECT
    PLATFORM=$(uname)
    if [ $PLATFORM = "Linux" ] ; then
        ENDLINE="\n"
    else
        ENDLINE="\r\n"
    fi

    for FIRST_REL_APP_PATH in $REL_APP_PATH ; do
        # CONFIG_NAME=$ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/.config_$PROJECT.h
        CONFIG_NAME=$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/config_$PROJECT.h
        VERSION_FILE_NAME=$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/version.h
        [ -f $VERSION_FILE_NAME ] || {
            echo_    "#ifndef POOL_VERSION"            >&3
            echo_e "\t#define POOL_VERSION 1.0"     >&3
            echo_    "#endif"                         >&3
            echo_    "#ifndef FIRMWARE_VERSION"       >&3
            echo_e "\t#define FIRMWARE_VERSION 1.0" >&3
            echo_    "#endif"                         >&3
        } 3>$VERSION_FILE_NAME
        break;
    done

    if [ "A$DOXYGEN_EXPORT_DIR" = "A" ] ; then
        CONFIG_HEADER_DOXYGEN_READY="config_header"'__'"$PROJECT-doc.txt"
    else
        CONFIG_HEADER_DOXYGEN_READY="$DOXYGEN_EXPORT_DIR/config_header"'__'"$PROJECT-doc.txt"
    fi

  # first backup individual settings after line
  # START_INDIVIDUAL_PROJECT_CONFIG
    if [ -f $CONFIG_NAME ] ; then
        grep -A1000 "// START_INDIVIDUAL_PROJECT_CONFIG"  $CONFIG_NAME > $CONFIG_NAME.bak
    else
        touch $CONFIG_NAME.bak
    fi
    {
        # write INST counts
        CONFIG_HEADER_FILENAME=$(basename $CONFIG_NAME)
        echo_ "// File: $CONFIG_HEADER_FILENAME" >&3
        echo_ "// IMPORTANT: Never change the first block of this header manually!!!" >&3
        echo_ "//            All manual changes are overwritten by the next call of \"update_makefile.sh $CONF_FILE\" " >&3
        echo_ "//            Perform changes direct in the feature and project setup file $CONF_FILE"  >&3
        echo_ "//  ALLOWED ADAPTATION: Move the to be adapted defines from the middle block to the end after" >&3
        echo_ "//                      the line START_INDIVIDUAL_PROJECT_CONFIG and remove the comment indication there."  >&3
        echo_ "//                      All commented out defines in the middle block will be upated on next \"update_makefile.sh $CONF_FILE\" call,"  >&3
        echo_ "//                      if the corresponding value in isoaglib_config.h changed" >&3
    
        echo_e "\n\n// include an external file for definition of pool and firmware versions" >&3
        echo_    "#include \"version.h\""  >&3
    
        echo_e "$ENDLINE" >&3
        echo_    "#define PRJ_USE_AUTOGEN_CONFIG config_$PROJECT.h" >&3
        for SinglePrjDefine in $PRJ_DEFINES ; do
            SingleDefName="${SinglePrjDefine%%=*}"
            SingleDefRemainder="${SinglePrjDefine#$SingleDefName}"
            SingleDefValue="${SingleDefRemainder#=}"
            echo_ "#ifndef $SingleDefName" >&3
            echo_ "#define $SingleDefName $SingleDefValue" >&3
            echo_ "#endif // $SingleDefName" >&3
        done
        echo_e "$ENDLINE" >&3
    
    
        echo_e "#define CAN_BUS_CNT $CAN_BUS_CNT $ENDLINE" >&3
        echo_e "#define CAN_BUS_USED $CAN_BUS_USED $ENDLINE" >&3
        echo_e "#define CAN_INSTANCE_CNT $CAN_INSTANCE_CNT $ENDLINE" >&3
        echo_e "#define PRT_INSTANCE_CNT $PRT_INSTANCE_CNT $ENDLINE" >&3
        echo_e "#define RS232_INSTANCE_CNT $RS232_INSTANCE_CNT $ENDLINE" >&3
    
    
        echo_ "// Decide if the CPU stores number variables in BIG or LITTLE endian byte order in memory." >&3
        echo_ "// Most CPU will use LITTLE ENDIAN. Only some types of ARM, mostly 68k and PowerPC CPU types will use big endian." >&3
        echo_ "// Please check the manual of your targret cpu. This setting is used to activate some quick number conversion algorithms," >&3
        echo_ "// which provide quick conversion from number variable to CAN strings ( which are always little endian in ISO ) - and other way." >&3
        if [ "$USE_LITTLE_ENDIAN_CPU" -gt 0 ] ; then
            echo_e "#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN$ENDLINE" >&3
        else
            echo_e "// #define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN$ENDLINE" >&3
        fi
        if [ "$USE_BIG_ENDIAN_CPU" -gt 0 ] ; then
            echo_e "#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN$ENDLINE" >&3
        else
            echo_e "// #define OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN$ENDLINE" >&3
        fi
    
        if [ "$USE_PCAN_LIB" -gt 0 ] ; then
            echo_e "#define USE_PCAN_LIB$ENDLINE" >&3
        else
            echo_e "// #define USE_PCAN_LIB$ENDLINE" >&3
        fi
    
        if [ "$PRJ_DO_NOT_START_RELAIS_ON_STARTUP" -gt 0 ] ; then
            echo_e "#define CONFIG_DO_NOT_START_RELAIS_ON_STARTUP$ENDLINE" >&3
        else
            echo_e "// #define CONFIG_DO_NOT_START_RELAIS_ON_STARTUP$ENDLINE" >&3
        fi
    
        if [ "$PRJ_RS232_OVER_CAN" -gt 0 ] ; then
            echo_e "#define USE_RS232_OVER_CAN$ENDLINE" >&3
        fi
    
        if [ "$USE_VT_UNICODE_SUPPORT" -gt 0 ] ; then
            echo_e "#define USE_VT_UNICODE_SUPPORT$ENDLINE" >&3
        fi
    
    
        if [ "$PRJ_BASE" -gt 0 ] ; then
            echo_e "#ifndef USE_BASE $ENDLINE\t#define USE_BASE $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_GENERAL" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_GENERAL $ENDLINE\t#define USE_TRACTOR_GENERAL $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_MOVE" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_MOVE $ENDLINE\t#define USE_TRACTOR_MOVE $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_PTO" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_PTO $ENDLINE\t#define USE_TRACTOR_PTO $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_LIGHT" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_LIGHT $ENDLINE\t#define USE_TRACTOR_LIGHT $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_FACILITIES" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_FACILITIES $ENDLINE\t#define USE_TRACTOR_FACILITIES $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_AUX" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_AUX $ENDLINE\t#define USE_TRACTOR_AUX $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_GUIDANCE" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_GUIDANCE $ENDLINE\t#define USE_TRACTOR_GUIDANCE $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TRACTOR_CERTIFICATION" -gt 0 ; then
            echo_e "#ifndef USE_TRACTOR_CERTIFICATION $ENDLINE\t#define USE_TRACTOR_CERTIFICATION $ENDLINE#endif" >&3
        fi
        if test "$PRJ_TIME_GPS" -gt 0 ; then
            echo_e "#ifndef USE_TIME_GPS $ENDLINE\t#define USE_TIME_GPS $ENDLINE#endif" >&3
        fi
    
        if [ "$PROC_LOCAL" -gt 0 ] ; then
            echo_e "#ifndef USE_PROC_DATA_DESCRIPTION_POOL $ENDLINE\t#define USE_PROC_DATA_DESCRIPTION_POOL $ENDLINE#endif" >&3
        fi
    
        echo_ "// Decide if HEAP allocation strategy shall reduce size about 5K to 10K in favour of speed" >&3
        echo_ "// Strong Advice: Don't activate this, as long your target has not too tight memory restrictions" >&3
        echo_ "// Initialization of CAN filters and of local process data might get too slow under worst case conditions" >&3
        if [ "$OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED" -gt 0 ] ; then
            echo_e "#define OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED$ENDLINE" >&3
        else
            echo_e "// #define OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED$ENDLINE" >&3
        fi
    
        echo_ "/** allow configuration by parameter value YES */" >&3
        echo_ "#ifndef YES" >&3
        echo_ "  #define YES 1" >&3
        echo_ "#endif" >&3
        echo_ "/** allow configuration by parameter value NO */" >&3
        echo_ "#ifndef NO" >&3
        echo_ "  #define NO 0" >&3
        echo_ "#endif" >&3
    
    
        if [ "$USE_FLOAT_DATA_TYPE" -gt 0 ] ; then
            echo_ "// Decide if float shall be used for the project" >&3
            echo_e "#define USE_FLOAT_DATA_TYPE$ENDLINE" >&3
        fi
    
    
        if [ "$PRJ_PROCESS" -gt 0 ] ; then
            echo_e "#ifndef USE_PROCESS $ENDLINE  #define USE_PROCESS $ENDLINE#endif" >&3
            if [ "$PROC_REMOTE" -gt 0 ] ; then
                if [ "$PROC_REMOTE_STD" -gt 0 ] ; then
                    echo_e "#ifndef USE_PROC_REMOTE_STD $ENDLINE  #define USE_PROC_REMOTE_STD $ENDLINE#endif" >&3
                fi
                if [ "$PROC_REMOTE_SIMPLE_MEASURE" -gt 0 ] ; then
                    echo_e "#ifndef USE_PROC_REMOTE_SIMPLE_MEASURE $ENDLINE  #define USE_PROC_REMOTE_SIMPLE_MEASURE $ENDLINE#endif" >&3
                fi
                if [ "$PROC_REMOTE_SIMPLE_SETPOINT" -gt 0 ] ; then
                    echo_e "#ifndef USE_PROC_REMOTE_SIMPLE_SETPOINT $ENDLINE  #define USE_PROC_REMOTE_SIMPLE_SETPOINT $ENDLINE#endif" >&3
                fi
                if [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" -gt 0 ] ; then
                    echo_e "#ifndef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT $ENDLINE  #define USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT $ENDLINE#endif" >&3
                fi
                if [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED" -gt 0 ] ; then
                    echo_e "#ifndef USE_PROC_SIMPLE_REMOTE_MEASURE_SETPOINT_COMBINED $ENDLINE  #define USE_PROC_SIMPLE_REMOTE_MEASURE_SETPOINT_COMBINED $ENDLINE#endif" >&3
                fi
            fi
        else
        # the default in isoaglib_config.h is to activate
        # PROCESS as long as USE_PROCESS_YN unset
            echo_e "#ifndef USE_PROCESS_YN $ENDLINE\t#define USE_PROCESS_YN NO $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_EEPROM" -gt 0 ] ; then
            echo_e "#ifndef USE_EEPROM_IO $ENDLINE\t#define USE_EEPROM_IO $ENDLINE#endif" >&3
        else
        # the default in isoaglib_config.h is to activate
        # EEPROM as long as USE_EEPROM_IO_YN unset
            echo_e "#ifndef USE_EEPROM_IO_YN $ENDLINE\t#define USE_EEPROM_IO_YN NO $ENDLINE#endif" >&3
        fi
    
        if test "$PRJ_DATASTREAMS" -gt 0 -o $PRJ_ISO_TERMINAL -gt 0 -o $PRJ_TIME_GPS -gt 0 ; then
            echo_e "#ifndef USE_DATASTREAMS_IO $ENDLINE\t#define USE_DATASTREAMS_IO $ENDLINE#endif" >&3
        else
        # the default in isoaglib_config.h is to activate
        # DATASTREAMS as long as USE_DATASTREAMS_IO_YN unset
            echo_e "#ifndef USE_DATASTREAMS_IO_YN $ENDLINE\t#define USE_DATASTREAMS_IO_YN NO $ENDLINE#endif" >&3
        fi
    
        if [ "$PRJ_ISO11783" -gt 0 ] ; then
            echo_e "#ifndef USE_ISO_11783 $ENDLINE\t#define USE_ISO_11783 $ENDLINE#endif" >&3
            if [ "$PRJ_ISO_TERMINAL" -gt 0 ] ; then
                echo_e "#ifndef USE_ISO_TERMINAL $ENDLINE\t#define USE_ISO_TERMINAL $ENDLINE#endif" >&3
                if [ "$USE_ISO_TERMINAL_ATTRIBUTES" -ne 0 ] ; then
                    echo_e "#ifndef USE_ISO_TERMINAL_ATTRIBUTES $ENDLINE\t#define USE_ISO_TERMINAL_ATTRIBUTES $ENDLINE#endif" >&3
                fi
                if [ "$USE_ISO_TERMINAL_GRAPHICCONTEXT" -ne 0 ] ; then
                    echo_e "#ifndef USE_ISO_TERMINAL_GRAPHICCONTEXT $ENDLINE\t#define USE_ISO_TERMINAL_GRAPHICCONTEXT $ENDLINE#endif" >&3
                fi
                if [ -n "$PRJ_ISO_TERMINAL_OBJECT_SELECTION1" ] ; then
                    echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION1 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION1 $PRJ_ISO_TERMINAL_OBJECT_SELECTION1 $ENDLINE#endif" >&3
                fi
                if [ -n "$PRJ_ISO_TERMINAL_OBJECT_SELECTION2" ] ; then
                    echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION2 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION2 $PRJ_ISO_TERMINAL_OBJECT_SELECTION2 $ENDLINE#endif" >&3
                fi
                if [ -n "$PRJ_ISO_TERMINAL_OBJECT_SELECTION3" ] ; then
                    echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION3 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION3 $PRJ_ISO_TERMINAL_OBJECT_SELECTION3 $ENDLINE#endif" >&3
                fi
                if [ -n "$PRJ_ISO_TERMINAL_OBJECT_SELECTION4" ] ; then
                    echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION4 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION4 $PRJ_ISO_TERMINAL_OBJECT_SELECTION4 $ENDLINE#endif" >&3
                fi
            fi
            if [ "$PRJ_MULTIPACKET" -gt 0 ] ; then
                if [ "$PRJ_MULTIPACKET_STREAM_CHUNK" -gt 0 ] ; then
                    echo_e "#ifndef DEF_Stream_IMPL   $ENDLINE\t#define DEF_Stream_IMPL   StreamChunk   $ENDLINE#endif" >&3
                    echo_e "#ifndef DEF_Stream_c_IMPL $ENDLINE\t#define DEF_Stream_c_IMPL StreamChunk_c $ENDLINE#endif" >&3
                else
                    echo_e "#ifndef DEF_Stream_IMPL   $ENDLINE\t#define DEF_Stream_IMPL   StreamLinear   $ENDLINE#endif" >&3
                    echo_e "#ifndef DEF_Stream_c_IMPL $ENDLINE\t#define DEF_Stream_c_IMPL StreamLinear_c $ENDLINE#endif" >&3
                fi
            fi
            if [ "$PRJ_ISO_TERMINAL_SERVER" -gt 0 ] ; then
                echo_e "#ifndef USE_ISO_TERMINAL_SERVER $ENDLINE\t#define USE_ISO_TERMINAL_SERVER $ENDLINE#endif" >&3
                echo_ "with ISO_TERMINAL_SERVER!"
            fi
        fi
    
        if [ "$PRJ_SEND_DEBUG" -gt 0 ] ; then
            echo_e "#ifndef DEBUG $ENDLINE\t#define DEBUG $ENDLINE#endif" >&3
        fi
        # write overwriteable parts of isoaglib_config.h
        echo_e "$ENDLINE// The following configuration values can be overwritten." >&3
        echo_ "// These settings are initially defined in isoaglib_config.h ." >&3
        echo_ "// These settings are in commented-out, so that you can activate and adapt them by" >&3
        echo_e "// moving them below the line with START_INDIVIDUAL_PROJECT_CONFIG$ENDLINE"  >&3
    } 3>"$CONFIG_NAME"
    for conf_line in $(grep "#define CONFIG_" $ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/isoaglib_config.h | sed 's/#define \(CONFIG_[a-zA-Z0-9_]*\).*/\1/g') ; do
        conf_name=$(echo_ $conf_line | sed 's/#define \(CONFIG_[a-zA-Z0-9_]*\).*/\1/g')
        INDIV_CNT=$(grep -c $conf_name $CONFIG_NAME.bak)
        if [ "$INDIV_CNT" -lt 1 ] ; then
            grep -B1 "#define $conf_line" $ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/isoaglib_config.h >> $CONFIG_NAME
            sed "s|#define $conf_name|// #define $conf_name|g" $CONFIG_NAME > $CONFIG_NAME.1
            #     CMDLINE=$(echo_ "sed -e 's|#define $conf_name|// #define $conf_name|g' $CONFIG_NAME > $CONFIG_NAME.1")
            #     echo_ $CMDLINE | sh
            mv $CONFIG_NAME.1 $CONFIG_NAME
            echo_e_n "$ENDLINE" >> $CONFIG_NAME
        fi
    done
    {
        echo_e "$ENDLINE// DONT REMOVE THIS AND THE FOLLOWING LINE AS THEY ARE NEEDED TO DETECT YOUR PERSONAL PROJECT ADAPTATIONS!!!" >&3
        FRESH=$(grep -c "// START_INDIVIDUAL_PROJECT_CONFIG" $CONFIG_NAME.bak)
        if [ "$FRESH" -lt 1 ] ; then
            echo_ "// START_INDIVIDUAL_PROJECT_CONFIG" >&3
        fi
        cat $CONFIG_NAME.bak >&3
        rm -f $CONFIG_NAME.bak
    } 3>>"$CONFIG_NAME"
    sed -e 's|^[ \t]*//|//|g' $CONFIG_NAME > $CONFIG_NAME.1
    sed -e 's|^[ \t]*/\*|/\*|g' $CONFIG_NAME.1 > $CONFIG_NAME
    rm -f $CONFIG_NAME.1

    {
        echo_e "$ENDLINE$ENDLINE \section PrjConfig$PROJECT List of configuration settings for $PROJECT" >&3
        echo_ " This is only a copy with doxygen ready comment blocks from the original file $CONFIG_NAME " >&3
        echo_ " This header is included by <IsoAgLib/library/xgpl_src/IsoAgLib/isoaglib_config.h> based on the" >&3
        echo_ " project define #define PRJ_USE_AUTOGEN_CONFIG config_$PROJECT.h ( Important: place the directory of the application source files in the include search path )" >&3
        echo_ "" >&3
        echo_ " Use the file $CONF_FILE in the directory $1 as input file for $0 to create the project generation files." >&3
        echo_ "\code" >&3
        cat $CONFIG_NAME >&3
        echo_ "\endcode" >&3
    } 3>"$CONFIG_HEADER_DOXYGEN_READY"

    # go back to directory where config file resides
    cd $1
}

expand_definition()
{
    # Outputting the here document (cat <<) ist superior to other
    # means especially when RULE contains quotes:
    eval "cat <<END_OF_STRING
$RULE
END_OF_STRING"
}

expand_insert()
{
    printf '$(%s)' "$1"
}

expand_report()
{
    local NAME="$1"
    eval "printf '%s' \"\$REPORT_$NAME\""
}

define_insert_and_report()
{
    local NAME="$1"
    RULE="$2"
    local F=expand_insert # first expand for insertion
    eval "INSERT_${NAME}=\"\$(expand_definition)\""
    F=expand_report # then expand for report
    eval "REPORT_${NAME}=\"\$(expand_definition)\""
}

create_standard_makefile()
{
    MakefileName="Makefile"
    MakefileNameLong="Makefile"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER"

     : ${MAKEFILE_SKELETON_FILE:="$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_MakefileSkeleton.txt"}
    {
        # create Makefile Header
        echo_ "#############################################################################" >&3
        echo_ "# Makefile for building: $PROJECT" >&3
        echo_ "# Project:               $PROJECT" >&3
        echo_ "#############################################################################" >&3
        echo_ ""  >&3
        echo_ "####### Project specific variables" >&3
        echo_ "TARGET = $PROJECT" >&3
        echo_ "ISOAGLIB_PATH = $ISO_AG_LIB_INSIDE" >&3
        echo_ "INSTALL_PATH = $ISOAGLIB_INSTALL_PATH" >&3
        echo_n "APP_INC = " >&3
        KDEVELOP_INCLUDE_PATH="$ISO_AG_LIB_INSIDE/library;$ISO_AG_LIB_INSIDE/library/xgpl_src;"
        for EACH_REL_APP_PATH in $REL_APP_PATH ; do
            echo_n "-I$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH " >&3
            append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH;"
        done
        for SingleInclPath in $PRJ_INCLUDE_PATH ; do
            echo_n " -I$ISO_AG_LIB_INSIDE/$SingleInclPath" >&3
            append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$SingleInclPath;"
        done
        for SingleInclPath in $USE_LINUX_EXTERNAL_INCLUDE_PATH ; do
            echo_n " -I$SingleInclPath" >&3
            append KDEVELOP_INCLUDE_PATH " $SingleInclPath;"
        done
        echo_ "" >&3
        echo_n "LIBPATH = " >&3
        for SingleLibPath in $USE_LINUX_EXTERNAL_LIBRARY_PATH ; do
            echo_n " -L$SingleLibPath" >&3
        done
        echo_ "" >&3
        
        echo_n "EXTERNAL_LIBS = " >&3
        for SingleLibItem in $USE_LINUX_EXTERNAL_LIBRARIES ; do
            echo_n " $SingleLibItem" >&3
        done
        echo_ "" >&3
        
        echo_e "\n####### Include a version definition file into the Makefile context - when this file exists"  >&3
        echo_    "-include versions.mk" >&3
        
        echo_ "" >&3
        if [ "$USE_RS232_DRIVER" = "rte" -o "$USE_CAN_DEVICE_FOR_SERVER" = "rte" ] ; then
            define_insert_and_report BIOS_LIB '/usr/local/lib/librte_client.a /usr/local/lib/libfevent.a'
            echo_ "BIOS_LIB = $INSERT_BIOS_LIB" >&3
            # the new RTE library places the headers in /usr/local/include --> no special include paths are needed any more
            echo_n "BIOS_INC =" >&3
        fi
        local REPORT_VERSION_DEFINES=''
        define_insert_and_report PROJ_DEFINES "\$(\$F VERSION_DEFINES) -D$USE_SYSTEM_DEFINE -DPRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h"
        echo_e_n "\nPROJ_DEFINES = $INSERT_PROJ_DEFINES" >&3
        for SinglePrjDefine in $PRJ_DEFINES ; do
            echo_n " -D$SinglePrjDefine" >&3
        done
        if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
            echo_n " -DSYSTEM_WITH_ENHANCED_CAN_HAL" >&3
        fi
        
        if [ $USE_CAN_DRIVER = "msq_server" ] ; then
            echo_n " -DCAN_DRIVER_MESSAGE_QUEUE" >&3
        fi
        if [ $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
            echo_n " -DCAN_DRIVER_SOCKET" >&3
        fi
        
        echo_e "\n\n####### Definition of compiler binary prefix corresponding to selected target" >&3
        if [ "A$PRJ_COMPILER_BINARY_PRE" != "A" ] ; then
            echo_ "COMPILER_BINARY_PRE = \"$PRJ_COMPILER_BINARY_PRE\"" >&3
            
        else
            case $PRJ_DEFINES in
                *SYSTEM_A1*)
                    echo_ "COMPILER_BINARY_PRE = /opt/hardhat/devkit/arm/xscale_le/bin/xscale_le-" >&3
                    echo_ "SYSTEM_A1"
                    ;;
                *SYSTEM_MCC*)
                    echo_ "COMPILER_BINARY_PRE = /opt/eldk/usr/bin/ppc_6xx-" >&3
                    echo_ "SYSTEM_MCC"
                    ;;
                *)
                    echo_ "COMPILER_BINARY_PRE = " >&3
                    ;;
            esac
        fi

        list_source_files() {
            while [ 3 -le $# ]; do
                local INTRO="$1"
                local SOURCE_PATTERN="$2"
                local FILE_LIST="$3"
                shift 3
                printf '%s =' "$INTRO" >&3
                local FORMAT=' %s'
                grep -E "$SOURCE_PATTERN" <"$FILE_LIST" | 
                while read CcFile; do
                    printf "$FORMAT" "$CcFile" >&3
                    FORMAT='  \\\n\t\t%s'
                done
                printf '\n\n' >&3
            done
        }
        
        echo_e "\n\nfirst: all\n" >&3
        echo_ "####### Files" >&3
        list_source_files \
            SOURCES_LIBRARY '\.cc|\.cpp|\.c' "$MakefileFilelistLibrary" \
            SOURCES_APP '\.cc|\.cpp|\.c' "$MakefileFilelistApp" \
            HEADERS_APP '\.h|\.hpp|\.hh' "$MakefileFilelistAppHdr"
        
        # 'HEADERS_UTEST = "$MakefileFileListUTest"
        # 'HEADERS_UTEST_MOCKS = ' "$MakefileFileListUTestMock"
        # 'HEADERS_UTEST_MOD_SUT = ' "$MakefileFileListUTestMODSUT"
        # 'TESTRUNNER_SOURCES = ' "$MakefileFileListUTestRunner"
        
        ##### Library install header file gathering BEGIN
        
        grep    "/impl/" <"$MakefileFilelistLibraryHdr" > FileListInternal.txt
        grep    "/hal/"  <"$MakefileFilelistLibraryHdr" >> FileListInternal.txt
        grep -v "/impl/" <"$MakefileFilelistLibraryHdr" | grep -v /hal/ | grep -v ".cpp" > FileListInterface.txt
        
        # it's a good idea to get the several typedef.h headers to the install set
        grep typedef.h FileListInternal.txt >> FileListInterface.txt
        grep -E "driver/*/i*.h" FileListInternal.txt >> FileListInterface.txt
        
        # special exception to enable ISO-Request-PGN implementation in app scope
        grep -E "isorequestpgn_c.h" FileListInternal.txt >> FileListInterface.txt

        local INTERFACE_FILE
        while read INTERFACE_FILE; do
            local BASE_NAME
            sed -e '/#include/!d' \
                -e 's/.*#include[ \t\<\"]*\([^\>\"\t ]*\).*/\1/g' \
                -e 's|.*xgpl_src/IsoAgLib/\(.*\)|\1|g' \
                -e 's|\.\./||g' < "$INTERFACE_FILE" |
            while read BASE_NAME; do
                expr \( "$BASE_NAME" : '.*\.h' \) >/dev/null &&
                    ! grep -q -F "/$BASE_NAME" FileListInterface.txt &&
                    grep -F "$BASE_NAME" FileListInternal.txt >>FileListInterface.txt
            done
        done <FileListInterface.txt
        echo_n "INSTALL_FILES_LIBRARY = " >&3
        FIRST_LOOP="YES"
        for InterfaceFile in $(cat FileListInterface.txt) ; do
            if [ $FIRST_LOOP != "YES" ] ; then
                echo_e_n ' \\' >&3
                echo_e_n "\n\t\t" >&3
            else
                FIRST_LOOP="NO"
            fi
            echo_e_n "$InterfaceFile" >&3
        done
        echo_e "\n" >&3
        
        rm -f FileListInterface.txt FileListInternal.txt

        local REPORT_ISOAGLIB_PATH="$ISO_AG_LIB_INSIDE"
        local REPORT_APP_INC=DUMMY_REPORT_APP_INC
        local REPORT_BIOS_INC=DUMMY_REPORT_BIOS_INC
        local REPORT_LIBPATH=DUMMY_REPORT_LIBPATH
        local REPORT_EXTERNAL_LIBS=DUMMY_REPORT_EXTERNAL_LIBS

        define_insert_and_report EXTRA_CFLAGS '-Wextra -Winit-self -Wmissing-include-dirs'
        define_insert_and_report CXXFLAGS '-pipe -O -Wall -g $($F EXTRA_CFLAGS) -fno-builtin -fno-exceptions -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith $($F PROJ_DEFINES)'
        define_insert_and_report INCPATH '-I. -I$($F ISOAGLIB_PATH)/library -I$($F ISOAGLIB_PATH)/library/xgpl_src $($F APP_INC) $($F BIOS_INC)'
        define_insert_and_report CPP_PARAMETERS '$($F CXXFLAGS) $($F INCPATH)'
        define_insert_and_report LFLAGS '$($F LIBPATH)'
        define_insert_and_report SUBLIBS '-lrt'
        define_insert_and_report LIBS '$($F BIOS_LIB) $($F SUBLIBS) $($F EXTERNAL_LIBS)'
        define_insert_and_report LINKER_PARAMETERS_1 '$($F LFLAGS)'
        define_insert_and_report LINKER_PARAMETERS_2 '$($F LIBS)'

        printf '(Linux_specific_settings\n' >&5
        printf '  (COMPILER_PARAMETERS %s)\n' "$REPORT_CXXFLAGS" >&5
        printf '  (LINKER_PARAMETERS %s %s))\n' "$REPORT_LINKER_PARAMETERS_1" "$REPORT_LINKER_PARAMETERS_2" >&5
        
        ##### Library install header file gathering END
        expand_template "$MAKEFILE_SKELETON_FILE" >&3
        echo >&3
    } 3>"$MakefileNameLong"

    # NO UTESTs, no need to remove any testrunners
    #  # remove testrunner for A1
    #   case $PRJ_DEFINES in
    #       *SYSTEM_A1*)
    #       sed -e 's#all: \(.*\) testrunner\(.*\)#all: \1\2#g'  $MakefileNameLong > $MakefileNameLong.1
    #       mv $MakefileNameLong.1 $MakefileNameLong
    #       ;;
    #       *SYSTEM_A5*)
    #       sed -e 's#all: \(.*\) testrunner\(.*\)#all: \1\2#g'  $MakefileNameLong > $MakefileNameLong.1
    #       mv $MakefileNameLong.1 $MakefileNameLong
    #       ;;
    #   esac

    # add can_server creation to target "all"
    if [ $USE_CAN_DRIVER = "msq_server" -o $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
        cp $MakefileNameLong $MakefileNameLong.1
        sed -e 's#LFLAGS   =#LFLAGS   = -pthread#g' $MakefileNameLong.1 > $MakefileNameLong
    fi

    rm -f $MakefileNameLong.1

    # replace the install rules for version.h and the app config file
    sed -e "s#_PROJECT_CONFIG_REPLACE_#$CONFIG_NAME#g"  $MakefileNameLong > $MakefileNameLong.1
    sed -e "s#_PROJECT_VERSION_REPLACE_#$VERSION_FILE_NAME#g" $MakefileNameLong.1 > $MakefileNameLong
    rm -f $MakefileNameLong.1

    # replace any path items like "Bla/foo/../Blu" --> "Bla/Blu"
    while [ $(grep -c -e '/[0-9a-zA-Z_+\-]\+/\.\./' $MakefileNameLong) -gt 0 ] ; do
        sed -e 's|/[0-9a-zA-Z_+\-]\+/\.\./|/|g' $MakefileNameLong > $MakefileNameLong.1
        mv $MakefileNameLong.1 $MakefileNameLong
    done

    # create a symbolic link to get this individual MakefileNameLong referred as "Makefile"
    rm -f "Makefile"
    ln -s $MakefileNameLong "Makefile"
}

create_pure_application_makefile()
{
    # now create pure application makefile which is based upon an installed library
    MakefileName="MakefileApp"
    MakefileNameLong="MakefileApp"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER"

    if [ "A$MAKEFILE_APP_SKELETON_FILE" = "A" ] ; then
        MAKEFILE_APP_SKELETON_FILE="$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_MakefileAppSkeleton.txt"
    fi

    {
        # create Makefile Header
        echo_ "#############################################################################" >&3
        echo_ "# Makefile for building: $PROJECT" >&3
        echo_ "# Project:               $PROJECT" >&3
        echo_ "#############################################################################" >&3
        echo_ ""  >&3
        echo_ "####### Project specific variables" >&3
        echo_ "TARGET = $PROJECT" >&3
        echo_ "ISOAGLIB_INSTALL_PATH = $ISOAGLIB_INSTALL_PATH" >&3
        echo_n "APP_INC = " >&3
        KDEVELOP_INCLUDE_PATH="$ISO_AG_LIB_INSIDE/library;$ISO_AG_LIB_INSIDE/library/xgpl_src;"
        for EACH_REL_APP_PATH in $REL_APP_PATH ; do
            echo_n "-I$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH " >&3
            append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH;"
        done
        for SingleInclPath in $PRJ_INCLUDE_PATH ; do
            echo_n " -I$ISO_AG_LIB_INSIDE/$SingleInclPath" >&3
            append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$SingleInclPath;"
        done
        for SingleInclPath in $USE_LINUX_EXTERNAL_INCLUDE_PATH ; do
            echo_n " -I$SingleInclPath" >&3
            append KDEVELOP_INCLUDE_PATH " $SingleInclPath;"
        done
        echo_ "" >&3
        echo_n "LIBPATH = " >&3
        for SingleLibPath in $USE_LINUX_EXTERNAL_LIBRARY_PATH ; do
            echo_n " -L$SingleLibPath" >&3
        done
        echo_ "" >&3
        
        echo_n "EXTERNAL_LIBS = " >&3
        for SingleLibItem in $USE_LINUX_EXTERNAL_LIBRARIES ; do
            echo_n " $SingleLibItem" >&3
        done
        echo_ "" >&3
        
        echo_e "\n####### Include a version definition file into the Makefile context - when this file exists"  >&3
        echo_    "-include versions.mk" >&3
        
        
        echo_ "" >&3
        
        echo_e_n "\nPROJ_DEFINES = \$(VERSION_DEFINES) -D$USE_SYSTEM_DEFINE -DPRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h" >&3
        for SinglePrjDefine in $PRJ_DEFINES ; do
            echo_n " -D$SinglePrjDefine" >&3
        done
        if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
            echo_n " -DSYSTEM_WITH_ENHANCED_CAN_HAL" >&3
        fi
        
        echo_e "\n\n####### Definition of compiler binary prefix corresponding to selected target" >&3
        if [ "A$PRJ_COMPILER_BINARY_PRE" != "A" ] ; then
            echo_ "COMPILER_BINARY_PRE = \"$PRJ_COMPILER_BINARY_PRE\"" >&3
            
        else
            case $PRJ_DEFINES in
                *SYSTEM_A1*)
                    echo_ "COMPILER_BINARY_PRE = /opt/hardhat/devkit/arm/xscale_le/bin/xscale_le-" >&3
                    echo_ "SYSTEM_A1"
                    ;;
                *SYSTEM_MCC*)
                    echo_ "COMPILER_BINARY_PRE = /opt/eldk/usr/bin/ppc_6xx-" >&3
                    echo_ "SYSTEM_MCC"
                    ;;
                *)
                    echo_ "COMPILER_BINARY_PRE = " >&3
                    ;;
            esac
        fi
        
        echo_e "\n\nfirst: all\n" >&3
        echo_ "####### Files" >&3
        echo_n "SOURCES_APP = " >&3
        FIRST_LOOP="YES"
        for CcFile in $(grep -E "\.cc|\.cpp|\.c" $MakefileFilelistApp) ; do
            if [ $FIRST_LOOP != "YES" ] ; then
                echo_e_n '\\' >&3
                echo_e_n "\n\t\t" >&3
            else
                FIRST_LOOP="NO"
            fi
            echo_e_n "$CcFile  " >&3
        done
        echo_e "\n" >&3
        
        cat $MAKEFILE_APP_SKELETON_FILE >&3
    } 3>"$MakefileNameLong"

    # add can_server creation to target "all"
    if [ $USE_CAN_DRIVER = "msq_server" ] ; then
        cp $MakefileNameLong $MakefileNameLong.1
        sed -e 's#LFLAGS   =#LFLAGS   = -pthread#g' $MakefileNameLong.1 > $MakefileNameLong
    fi
    rm -f $MakefileNameLong.1

    # replace any path items like "Bla/foo/../Blu" --> "Bla/Blu"
    while [ $(grep -c -e '/[0-9a-zA-Z_+\-]\+/\.\./' $MakefileNameLong) -gt 0 ] ; do
        sed -e 's|/[0-9a-zA-Z_+\-]\+/\.\./|/|g' $MakefileNameLong > $MakefileNameLong.1
        mv $MakefileNameLong.1 $MakefileNameLong
    done

    # create a symbolic link to get this individual MakefileNameLong referred as "Makefile"
    rm -f "MakefileApp"
    ln -s $MakefileNameLong "MakefileApp"
}

create_kdevelop3_project_file()
{
    # now create a Kdevelop3 project file
    cp -a $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_kdevelop3Generic.kdevelop $PROJECT.kdevelop

    sed -e "s/REPLACE_AUTHOR/$PROJECT_AUTHOR/g" $PROJECT.kdevelop > $PROJECT.kdevelop.1
    sed -e "s/REPLACE_AUTHOR_EMAIL/$PROJECT_AUTHOR_EMAIL/g" $PROJECT.kdevelop.1 > $PROJECT.kdevelop
    sed -e "s/REPLACE_PROJECT/$PROJECT/g" $PROJECT.kdevelop > $PROJECT.kdevelop.1
    sed -e "s#REPLACE_INCLUDE#$KDEVELOP_INCLUDE_PATH#g" $PROJECT.kdevelop.1 > $PROJECT.kdevelop
    # mv $PROJECT.kdevelop.1 $PROJECT.kdevelop
    rm -f $PROJECT.kdevelop.1

    {
        echo_ "# KDevelop Custom Project File List" >&3
        cat filelist__$PROJECT.txt >&3
    } 3>"$PROJECT.kdevelop.filelist"
}

create_makefile()
{
    # go to project dir - below config dir
    DEV_PRJ_DIR="$1/$PROJECT"
    cd $DEV_PRJ_DIR
    mkdir -p "objects_library" "objects_app" "objects_testrunner" "objects_utest"
    MakefileFilelistLibrary="$1/$PROJECT/$FILELIST_LIBRARY_PURE"
    MakefileFilelistLibraryHdr="$1/$PROJECT/$FILELIST_LIBRARY_HDR"

    MakefileFilelistApp="$1/$PROJECT/$FILELIST_APP_PURE"
    MakefileFilelistAppHdr="$1/$PROJECT/$FILELIST_APP_HDR"

    #  MakefileFileListUTest="$1/$PROJECT/$FILELIST_UTEST_PURE"
    #  MakefileFileListUTestMock="$1/$PROJECT/$FILELIST_UTEST_MOCK_PURE"
    #  MakefileFileListUTestMODSUT="$1/$PROJECT/$FILELIST_UTEST_MODSUT_PURE"
    #  MakefileFileListUTestRunner="$1/$PROJECT/$FILELIST_UTEST_RUNNER_PURE"

    create_standard_makefile
    create_pure_application_makefile
    create_kdevelop3_project_file

    cd $1

    return
}

mangle_path1()
{
    if [ 0 -lt $# ]; then
        printf '%b' "$*" | mangle_path1
    else
        sed -e "s|\\\\|${PATH_SEPARATOR1}|g;s|/|${PATH_SEPARATOR1}|g"
    fi
}

demangle_path1()
{
    if [ 0 -lt $# ]; then
        printf '%b' "$*" | demangle_path1
    else
        sed -e "s|${PATH_SEPARATOR1}|\\\\|g"
    fi
}

mangle_path2()
{
    if [ 0 -lt $# ]; then
        printf '%b' "$*" | mangle_path2
    else
        sed -e "s|\\\\|${PATH_SEPARATOR2}|g;s|/|${PATH_SEPARATOR1}|g"
    fi
}

demangle_path2()
{
    if [ 0 -lt $# ]; then
        printf '%b' "$*" | demangle_path2
    else
        sed -e "s|${PATH_SEPARATOR2}|\\\\|g"
    fi
}

# Generate suitable project file for the Win32 IDE "Dev-C++"
# which is OpenSource and is based on MinGW - i.e. gcc for Win32
# URL http://www.bloodshed.net/dev/devcpp.html
create_DevCCPrj()
{
    DEV_PRJ_DIR="$1$PROJECT"

    # echo_ "Create Projekt file for Dev-C++ in $DEV_PRJ_DIR"


    mkdir -p $DEV_PRJ_DIR/objects
    cd $DEV_PRJ_DIR
    # remove some file lists, which are not used for Dev-C++
    rm -f $FILELIST_LIBRARY_PURE $FILELIST_APP_PURE

    # org test
    PROJECT_FILE_NAME="$PROJECT"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER.dev"
    DevCcPrjFilelist="$1/$PROJECT/$FILELIST_COMBINED_PURE"

    # echo_ "Erzeuge $PROJECT_FILE_NAME"
    PROJECT_EXE_NAME="$PROJECT"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER.exe"
    CONFIG_HDR_NAME="config_""$PROJECT.h"

    UNIT_CNT_CC=$(grep -c -E "\.cc|\.cpp|\.c" $DevCcPrjFilelist)
    UNIT_CNT_HDR=$(grep -c -E "\.h|\.hpp" $DevCcPrjFilelist)
    UNIT_CNT=$(expr $UNIT_CNT_CC + $UNIT_CNT_HDR)


    {
        echo_ "[Project]" >&3
        echo_ "FileName=$PROJECT_FILE_NAME" >&3
        echo_ "Name=$PROJECT" >&3
        echo_ "UnitCount=$UNIT_CNT" >&3
    
    
        cat <<'ENDOFHEADERA' >&3
Type=1
Ver=1
ObjFiles=
ENDOFHEADERA
    
        ISO_AG_LIB_PATH_WIN=$(echo_ "$ISO_AG_LIB_INSIDE" | mangle_path1)
    
        DEFINE_LINE='-D'"$USE_SYSTEM_DEFINE"'_@@_-DPRJ_USE_AUTOGEN_CONFIG='"$CONFIG_HDR_NAME"'_@@_'
        INCLUDE_DIR_LINE="$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}library;$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}library${PATH_SEPARATOR1}xgpl_src"
    
        LIB_DIR_LINE=""
        LIB_FILE_LINE="-lwinmm_@@_-lws2_32_@@_"
    
    
        USE_WIN32_EXTERNAL_INCLUDE_PATH=$(echo_ "$USE_WIN32_EXTERNAL_INCLUDE_PATH" | mangle_path1)
    
        USE_WIN32_EXTERNAL_LIBRARY_PATH=$(echo_ "$USE_WIN32_EXTERNAL_LIBRARY_PATH" | mangle_path1)
    
        USE_DEVCPP_EXTERNAL_LIBRARIES=$(echo_ "$USE_DEVCPP_EXTERNAL_LIBRARIES" | mangle_path1)
    
        REL_APP_PATH_WIN=$(echo_ "$REL_APP_PATH" | mangle_path1)
    
        PRJ_INCLUDE_PATH_WIN=$(echo_ "$PRJ_INCLUDE_PATH" | mangle_path1)
    
        for EACH_REL_APP_PATH in $REL_APP_PATH_WIN ; do
            append INCLUDE_DIR_LINE ";$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}$EACH_REL_APP_PATH"
        done
        for SingleInclPath in $PRJ_INCLUDE_PATH_WIN ; do
            append INCLUDE_DIR_LINE ";$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}$SingleInclPath"
        done
        for SingleInclPath in $USE_WIN32_EXTERNAL_INCLUDE_PATH ; do
            append INCLUDE_DIR_LINE ";$SingleInclPath"
        done
        for SingleLibItem in $USE_DEVCPP_EXTERNAL_LIBRARIES ; do
            append LIB_FILE_LINE "$SingleLibItem"'_@@_'
        done
        for SingleLibPath in $USE_WIN32_EXTERNAL_LIBRARY_PATH ; do
            if [ "M$LIB_DIR_LINE" != "M" ] ; then
                append LIB_DIR_LINE ";$SingleLibPath"
            else
                LIB_DIR_LINE="$SingleLibPath"
            fi
        done
    
    
        if  [ $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
            append DEFINE_LINE ""'-D__GNUWIN32___@@_-W_@@_-DWIN32_@@_-D_CONSOLE_@@_-D_MBCS_@@_-D_Windows_@@_-DCAN_DRIVER_SOCKET_@@_-DSYSTEM_WITH_ENHANCED_CAN_HAL_@@_'
        fi
    
        echo_n "Includes=$INCLUDE_DIR_LINE" >&3
        echo_ "" >&3
    
        echo_ "Libs=$LIB_DIR_LINE" >&3
        echo_ "Linker=$LIB_FILE_LINE" >&3
    
        cat <<'ENDOFHEADERA' >&3
PrivateResource=
ResourceIncludes=
MakeIncludes=
ENDOFHEADERA
    
    
    
        for SinglePrjDefine in $PRJ_DEFINES ; do
            append DEFINE_LINE ""'-D'"$SinglePrjDefine"'_@@_'
        done
        if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
            append DEFINE_LINE ""'-DSYSTEM_WITH_ENHANCED_CAN_HAL_@@_'
        fi
    
        echo_ "Compiler=$DEFINE_LINE" >&3
        echo_ "CppCompiler=$DEFINE_LINE" >&3
    } 3>"$PROJECT_FILE_NAME"
    while [ $(grep -c -e "${PATH_SEPARATOR1}[0-9a-zA-Z_+\\-]\\+${PATH_SEPARATOR1}\\.\\.${PATH_SEPARATOR1}" $PROJECT_FILE_NAME) -gt 0 ] ; do
        sed -e "s|${PATH_SEPARATOR1}[0-9a-zA-Z_+\\-]\\+${PATH_SEPARATOR1}\\.\\.${PATH_SEPARATOR1}|${PATH_SEPARATOR1}|g" $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
        mv $PROJECT_FILE_NAME.1 $PROJECT_FILE_NAME
    done

    demangle_path1 < $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
    mv $PROJECT_FILE_NAME.1 $PROJECT_FILE_NAME

    {
        cat <<'ENDOFHEADERB' >&3
IsCpp=1
Icon=
ExeOutput=
ObjectOutput=objects
OverrideOutput=0
ENDOFHEADERB

        echo_ "OverrideOutputName=$PROJECT_EXE_NAME" >&3
        cat <<'ENDOFHEADERC' >&3
HostApplication=
Folders=
CommandLine=
UseCustomMakefile=0
CustomMakefile=
IncludeVersionInfo=0
SupportXPThemes=0
CompilerSet=0
CompilerSettings=0000000000000000000000

[VersionInfo]
Major=0
Minor=1
Release=1
Build=1
LanguageID=1033
CharsetID=1252
CompanyName=
FileVersion=
FileDescription=Developed using the Dev-C++ IDE
InternalName=
LegalCopyright=
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=
AutoIncBuildNr=0

ENDOFHEADERC

        SOURCES=$(cat $DevCcPrjFilelist)
        unit_ind=0
        for i in $SOURCES
        do
            if [ $i = "" ] ; then
                continue
            fi
            unit_ind=$(expr $unit_ind + 1)
            echo_ "[Unit$unit_ind]" >&3
            echo_ "FileName=$i" >&3
            echo_ "CompileCpp=1" >&3
            echo_ "Folder=$PROJECT" >&3
            echo_ "Compile=1" >&3
            echo_ "Link=1" >&3
            echo_ "Priority=1000" >&3
            echo_ "OverrideBuildCmd=0" >&3
            echo_ "BuildCmd=" >&3
            echo_ "" >&3
        done
    } 3>>"$PROJECT_FILE_NAME"

    cd $1
}

unix_lines_to_windows_lines()
{
    local FROM_FILE=$1
    local TO_FILE=$2
    # New versions of SED understand \r directly, but older ones (e.g.
    # the current one of MSYS) don't. With printf it's OK for both:
    sed -e "$(printf 's|\r||g;s|$|\r|')" <"$FROM_FILE" >"$TO_FILE"
}

expand_template()
{
    TEMPLATE_FILE="$1"
    eval "cat <<END_OF_TEMPLATE
$(cat "$TEMPLATE_FILE")
END_OF_TEMPLATE" || {
        printf 'ERROR when processing template file %s\n' "$TEMPLATE_FILE" >&2
        exit 1
    }
}

create_EdePrj()
{
    DEV_PRJ_DIR="$1/$PROJECT"
    # echo_ "Create Projekt file for EDE in $DEV_PRJ_DIR"
    mkdir -p $DEV_PRJ_DIR/Template.dir
    PROJECT_FILE_NAME="$PROJECT"'_'"$USE_TARGET_SYSTEM.pjt"
    EdePrjFilelist="$1/$PROJECT/$FILELIST_COMBINED_PURE"
    CONFIG_HDR_NAME="config_""$PROJECT.h"


    if [ $USE_EMBED_LIB_DIRECTORY = "library/commercial_BIOS/bios_esx" ] ; then
        ## adopt the BIOS file, if $USE_EMBED_LIB_DIRECTORY and
        ## and $USE_EMBED_HEADER_DIRECTORY reflect default value which doesn't match to defined target
        case "$USE_TARGET_SYSTEM" in
            esx)
                ;;
            pc*)
                ;;
            *)
                USE_EMBED_LIB_DIRECTORY="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
                USE_EMBED_HEADER_DIRECTORY="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
                ;;
        esac
    fi

    if [ "M$USE_EMBED_LIBS" = "M" ] ; then
       # no setting in the config file -> derive based on target
        case "$USE_TARGET_SYSTEM" in
            c2c) USE_EMBED_LIBS="c2c10l.lib" ;;
            Dj1) USE_EMBED_LIBS="djbiosmvt.lib" ;;
            esx) USE_EMBED_LIBS="C756/Xos20l.lib Module/Xma20l.lib" ;;
            esxu) USE_EMBED_LIBS="Mos10l.lib" ;;
            imi) USE_EMBED_LIBS="adis10l.lib" ;;
            pm167) USE_EMBED_LIBS="mios1s.lib" ;;
        esac
    fi
    if [ "M$USE_EMBED_BIOS_SRC" = "M" ] ; then
        # no setting in the config file -> derive based on target
        case "$USE_TARGET_SYSTEM" in
            c2c) USE_EMBED_BIOS_SRC="c2c10cs.asm c2c10err.c  c2c10err.h c2c10osy.h" ;;
            Dj1) USE_EMBED_BIOS_SRC="DjBiosMVT.h" ;;
            esx) USE_EMBED_BIOS_SRC="Xos20go.asm Xos20err.c xos20esx.h XOS20EEC.H XOS20EEC.OBJ" ;;
            esxu) USE_EMBED_BIOS_SRC="MOS10ERR.C  MOS10ERR.H  MOS10GO.ASM MOS10OSY.H" ;;
            imi) USE_EMBED_BIOS_SRC="adis10go_cs.asm adis10.h Xos20eec.h XOS20EEC.OBJ" ;;
            pm167) USE_EMBED_BIOS_SRC="mios1.h mx1_0go.asm Xos20eec.h  XOS20EEC.OBJ Xos20err.c  Xos20err.h" ;;
        esac
    fi
    if [ "M$USE_EMBED_ILO" = "M" ] ; then
        # no setting in the config file -> derive based on target
        case "$USE_TARGET_SYSTEM" in
            c2c) USE_EMBED_ILO="c2c10osy.ilo" ;;
            Dj1) USE_EMBED_ILO="MiniVT.ilo" ;;
            esx) USE_EMBED_ILO="Xos20lcs.ilo" ;;
            esxu) USE_EMBED_ILO="MOS10L.ILO" ;;
            imi) USE_EMBED_ILO="adis10s_cs.ilo" ;;
            pm167) USE_EMBED_ILO="mx1_0s.ilo" ;;
        esac
    fi


    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        if [ "M$USE_APP_PATH" = "M" ] ; then
            USE_APP_PATH=$(echo_ "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')
        else
            append USE_APP_PATH ""$(echo_ ";$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')
        fi
    done
    echo_ "USE_APP_PATH: $USE_APP_PATH"

    INCLUDE_APP_PATH_TASKING="$ISO_AG_LIB_INSIDE/$USE_EMBED_HEADER_DIRECTORY"
    for SingleInclPath in $PRJ_INCLUDE_PATH ; do
        append INCLUDE_APP_PATH_TASKING ";$SingleInclPath"
    done

    USE_EMBED_HEADER_DIRECTORY=$(echo_ "$ISO_AG_LIB_INSIDE/$USE_EMBED_HEADER_DIRECTORY" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')
    USE_EMBED_LIB_DIRECTORY=$(echo_ "$ISO_AG_LIB_INSIDE/$USE_EMBED_LIB_DIRECTORY" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')


    USE_EMBED_ILO=$(echo_ "$USE_EMBED_LIB_DIRECTORY/$USE_EMBED_ILO" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')

    USE_DEFINES="__TSW_CPP_756__,$USE_SYSTEM_DEFINE,PRJ_USE_AUTOGEN_CONFIG=$CONFIG_HDR_NAME"

    for SinglePrjDefine in $PRJ_DEFINES ; do
        append USE_DEFINES ""','"$SinglePrjDefine"
    done


    USE_TARGET_LIB_LINE=""
    for LIBRARY in $USE_EMBED_LIBS ; do
        append USE_TARGET_LIB_LINE " -Wo $USE_EMBED_LIB_DIRECTORY/$LIBRARY"
    done
    # echo_ "Library line: $USE_TARGET_LIB_LINE"
    USE_TARGET_LIB_LINE=$(echo_ "$USE_TARGET_LIB_LINE" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')

    # avoid UNIX style directory seperator "/" as it can disturb Tasking during the link process ( during compile, everything runs fine with UNIX style - WMK seems to have problems with it durign link and hex gen )
    ISO_AG_LIB_PATH_WIN=$(echo_ "$ISO_AG_LIB_INSIDE" | mangle_path1)
    USE_EMBED_LIB_DIRECTORY=$(echo_ "$USE_EMBED_LIB_DIRECTORY" | mangle_path1)
    USE_EMBED_HEADER_DIRECTORY=$(echo_ "$USE_EMBED_HEADER_DIRECTORY" | mangle_path1)
    USE_TARGET_LIB_LINE=$(echo_ "$USE_TARGET_LIB_LINE" | mangle_path1)
    USE_EMBED_ILO=$(echo_ "$USE_EMBED_ILO" | mangle_path1)
    USE_APP_PATH=$(echo_ "$USE_APP_PATH" | mangle_path1)
    DEV_PRJ_DIR_WIN=$(echo_ "$DEV_PRJ_DIR" | mangle_path1)
    USE_DEFINES=$(echo_ "$USE_DEFINES" | mangle_path1)
    USE_EMBED_COMPILER_DIR=$(echo_ "$USE_EMBED_COMPILER_DIR" | mangle_path1)

    # remove some file lists, which are not used for Dev-C++
    rm -f "$1/$PROJECT/$FILELIST_LIBRARY_PURE" "$1/$PROJECT/$FILELIST_APP_PURE"

    {
        # Build Tasking Project File by: a) first stub part; b) file list c) second stub part
        cat $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_EDE.part1.pjt >&3

        sed -e "s|\\\\\\\\|${PATH_SEPARATOR1}|g" -e "s|/|${PATH_SEPARATOR1}|g" $EdePrjFilelist > $EdePrjFilelist.1

        mv $EdePrjFilelist.1 $EdePrjFilelist
        cat $EdePrjFilelist >&3
        rm -f $EdePrjFilelist.1

        # insert specific BIOS/OS sources
        for BiosSrc in $USE_EMBED_BIOS_SRC ; do
            echo_ "$USE_EMBED_LIB_DIRECTORY/$BiosSrc" >&3
        done


        cat $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_EDE.part2.pjt >&3
    } 3>"$DEV_PRJ_DIR/$PROJECT_FILE_NAME"
    cd $DEV_PRJ_DIR
    sed -e "s#INSERT_PROJECT#$PROJECT#g" -e "s#INSERT_TARGET_LIB_DIRECTORY#$USE_EMBED_LIB_DIRECTORY#g" $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
    sed -e "s#INSERT_ISO_AG_LIB_PATH#$ISO_AG_LIB_PATH_WIN#g" -e "s#INSERT_TARGET_HEADER_DIRECTORY#$USE_EMBED_HEADER_DIRECTORY#g" $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME

    sed -e "s#INSERT_TARGET_LIB#$USE_TARGET_LIB_LINE#g" -e "s#INSERT_TARGET_ILO#$USE_EMBED_ILO#g" $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1

    sed -e "s#INSERT_APP_PATH#$USE_APP_PATH#g" -e "s#INSERT_PRJ_PATH#$DEV_PRJ_DIR_WIN#g" -e "s#INSERT_DEFINES#$USE_DEFINES#g" -e "s#INSERT_EMBED_COMPILER_DIR#$USE_EMBED_COMPILER_DIR#g" $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME

    rm -f $PROJECT_FILE_NAME.1

    while [ $(grep -c -e "${PATH_SEPARATOR1}[0-9a-zA-Z_+\\-]\\+${PATH_SEPARATOR1}\\.\\.${PATH_SEPARATOR1}" $PROJECT_FILE_NAME) -gt 0 ] ; do
        sed -e "s|${PATH_SEPARATOR1}[0-9a-zA-Z_+\\-]\\+${PATH_SEPARATOR1}\\.\\.${PATH_SEPARATOR1}|${PATH_SEPARATOR1}|g" $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
        mv $PROJECT_FILE_NAME.1 $PROJECT_FILE_NAME
    done
    while [ $(grep -c -e "${PATH_SEPARATOR1}[0-9a-zA-Z_+\\-]\\+${PATH_SEPARATOR1}\\.\\.${PATH_SEPARATOR1}" $EdePrjFilelist) -gt 0 ] ; do
        sed -e "s|${PATH_SEPARATOR1}[0-9a-zA-Z_+\\-]\\+${PATH_SEPARATOR1}\\.\\.${PATH_SEPARATOR1}|${PATH_SEPARATOR1}|g" $EdePrjFilelist > $EdePrjFilelist.1
        mv $EdePrjFilelist.1 $EdePrjFilelist
    done

    demangle_path1 < $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
    demangle_path1 <$EdePrjFilelist > $EdePrjFilelist.1
    mv $EdePrjFilelist.1 $EdePrjFilelist

    # now set the target CPU if this varies from default CpuC167CS
    ## also adopt the BIOS file, if $USE_EMBED_LIB_DIRECTORY and
    ## and $USE_EMBED_HEADER_DIRECTORY reflect default value which doesn't match to defined target
    case "$USE_TARGET_SYSTEM" in
        Dj1)
            sed -e 's#CpuC167CS#CpuC167CR#g' $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME
            mv $PROJECT_FILE_NAME $PROJECT_FILE_NAME.1
            ;;
        esxu)
            sed -e 's#CpuC167CS#CpuF269#g' $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME
            mv $PROJECT_FILE_NAME $PROJECT_FILE_NAME.1
            ;;
        esx)
            # do nothing here
            ;;
    esac


    #echo_ "Converted UNIX to Windows Linebreak in $PROJECT_FILE_NAME"
    unix_lines_to_windows_lines "$PROJECT_FILE_NAME.1" "$PROJECT_FILE_NAME"
}

create_CcsPrj()
{
    cd "$1/$PROJECT"
    CCS_PROJECT_NAME="$PROJECT"
    CCS_PROJECT_DIR="$1/$PROJECT"
    CCS_PROJECT_FILE_NAME="$PROJECT"'_'"$USE_TARGET_SYSTEM.pjt"
    CCS_PROJECT_FILE_LIST="$1/$PROJECT/$FILELIST_COMBINED_PURE"
    CCS_CONFIG_HDR_NAME="config_""$PROJECT.h"
    CCS_PROJECT_TEMPLATE=$CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/library/xgpl_src/build/projectGeneration/update_makefile_CCSSkeleton.pjt
    CCS_COMMERCIAL_BIOS_PATH="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
    CCS_PROJECT_FILE_NAME_PATH="$CCS_PROJECT_DIR/$CCS_PROJECT_FILE_NAME"

    if [ "M$ISOAGLIB_INSTALL_PATH" = "M" ] ; then
        CCS_LIB_INSTALL_DIR="./install/lib"
    else
        CCS_LIB_INSTALL_DIR=$ISOAGLIB_INSTALL_PATH
    fi
    CCS_LIB_INSTALL_HEADER_DIR="$CCS_LIB_INSTALL_DIR/include"
    CCS_LIB_INSTALL_LIB_DIR="$CCS_LIB_INSTALL_DIR/lib"

    # check platform specific settings
    if test $PRJ_ACTOR -gt 0 -o $PRJ_SENSOR_DIGITAL -gt 0 -o $PRJ_SENSOR_ANALOG -gt 0 -o $PRJ_SENSOR_COUNTER -gt 0 ; then
        echo_
        echo_ "Misconfigured config file: P1MC has no sensor/actor capabilities!"
        exit 0
    fi

    if test $RS232_INSTANCE_CNT -gt 0 -a $PRJ_RS232_OVER_CAN -eq 0 ; then
        echo_
        echo_ "Misconfigured config file: P1MC has no native rs232. Please enable PRJ_RS232_OVER_CAN or disable RS232_INSTANCE_CNT"
        exit 0
    fi

    # copy project template
    cp $CCS_PROJECT_TEMPLATE $CCS_PROJECT_DIR/$CCS_PROJECT_FILE_NAME


    for EACH_REL_APP_PATH in $REL_APP_PATH; do
        local PART="$(echo_ "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|/[0-9a-zA-Z_+\-]+\\\.\.||g')"
        if [ -z "$USE_APP_PATH" ]; then
            USE_APP_PATH="$PART"
        else
            append USE_APP_PATH ";$PART"
        fi
    done

    # project defines
    CCS_PROJECT_DEFINES=

    #include pathes
    CCS_INCLUDE_PATH=""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$ISO_AG_LIB_INSIDE/$CCS_COMMERCIAL_BIOS_PATH\""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/\""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$ISO_AG_LIB_INSIDE/library\""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$ISO_AG_LIB_INSIDE/library/xgpl_src\""

    for EACH_EMBED_HEADER_DIR in $USE_EMBED_HEADER_DIRECTORY; do
        append CCS_INCLUDE_PATH " -i\"$EACH_EMBED_HEADER_DIR\""
    done

    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$CCS_LIB_INSTALL_HEADER_DIR\""

    # source files
    for EACH_SOURCE_FILE in $(cat $1/$PROJECT/$FILELIST_COMBINED_PURE | grep .*cpp$); do
        append CCS_SOURCE_FILE_LIST "\nSource=\"$EACH_SOURCE_FILE\""
    done

    sed -e "s#INSERT_PROJECT_DIR#$CCS_PROJECT_DIR#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_ISOAGLIB_DIR#$ISO_AG_LIB_INSIDE#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH
    sed -e "s#INSERT_SOURCE_FILE_LIST#$CCS_SOURCE_FILE_LIST#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_HEADER_SEARCH_PATH#$CCS_INCLUDE_PATH#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH
    sed -e "s#INSERT_CONFIG_HEADER_NAME#$CCS_CONFIG_HDR_NAME#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_PROJECT_DEFINES#$CCS_PROJECT_DEFINES#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH
    sed -e "s#INSERT_PROJECT_NAME#$CCS_PROJECT_NAME#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_LIB_INSTALL_DIR#$CCS_LIB_INSTALL_LIB_DIR#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH

    #mv $CCS_PROJECT_FILE_NAME_PATH.1 $CCS_PROJECT_FILE_NAME_PATH

    rm -f $CCS_PROJECT_FILE_NAME_PATH.1

    # create output
    mkdir -p $CCS_LIB_INSTALL_DIR
    mkdir -p $CCS_LIB_INSTALL_HEADER_DIR
    mkdir -p $CCS_LIB_INSTALL_LIB_DIR

    for FIRST_REL_APP_PATH in $REL_APP_PATH; do
        mv $CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/config_$PROJECT.h $CCS_LIB_INSTALL_HEADER_DIR
        mv $CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/version.h $CCS_LIB_INSTALL_HEADER_DIR
        break;
    done

    for EACH_INSTALL_HEADER in $(cat "$1/$PROJECT/$FILELIST_LIBRARY_HDR" | grep -v ".cpp"); do
        install -D $EACH_INSTALL_HEADER $CCS_LIB_INSTALL_HEADER_DIR/$(echo_ $EACH_INSTALL_HEADER | sed -e 's|.*xgpl_src/||')
    done
}

# print source paths in DSP format to FD3 and user information to FD1.
format_sources_for_dsp()
{
    for i in $@; do
        if [ -z "$i" ] ; then
            continue
        fi
        echo_ "# Begin Source File" >&3
        filename=$(echo_ "$i" |sed 's|/|\\|g')
        echo_ "SOURCE=$filename" >&3
        printf '\n    %s' "$filename"
        echo_ "# End Source File" >&3
        echo_ "" >&3
    done
}

create_VCPrj()
{

    DEV_PRJ_DIR=$(echo_ "$1/$PROJECT" | sed -e 's/Dev-C++/VC6/g')
    # echo_ "Create Projekt file for VC6 in $DEV_PRJ_DIR"
    mkdir -p $DEV_PRJ_DIR
    # Visual Studio will create the needed Debug and Release directories on its own.
    PROJECT_FILE_NAME="$PROJECT"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER.dsp"

    DspPrjFilelist="$1/$PROJECT/$FILELIST_COMBINED_PURE"
    CONFIG_HDR_NAME="config_""$PROJECT.h"

    # remove some file lists, which are not used for Dev-C++
    rm -f "$1/$PROJECT/$FILELIST_LIBRARY_PURE" "$1/$PROJECT/$FILELIST_APP_PURE"

    USE_DEFINES=$(echo_ " /D "'"'"$USE_SYSTEM_DEFINE"'"' " /D "'"'"PRJ_USE_AUTOGEN_CONFIG=$CONFIG_HDR_NAME"'"' | sed -e 's/SYSTEM_PC/SYSTEM_PC_VC/g')
    USE_d_DEFINES=$(echo_ $USE_DEFINES | sed -e 's#/D#/d#g')


    LIB_DIR_LINE=""
    LIB_FILE_LINE=""

    ISO_AG_LIB_PATH_WIN=$(echo_ "$ISO_AG_LIB_INSIDE" | mangle_path2)

    USE_STLPORT_HEADER_DIRECTORY=$(echo_ "$USE_STLPORT_HEADER_DIRECTORY" | mangle_path2)

    USE_WIN32_EXTERNAL_INCLUDE_PATH=$(echo_ "$USE_WIN32_EXTERNAL_INCLUDE_PATH" | mangle_path1)

    USE_WIN32_EXTERNAL_LIBRARY_PATH=$(echo_ "$USE_WIN32_EXTERNAL_LIBRARY_PATH" | mangle_path1)

    USE_MSVC_EXTERNAL_LIBRARIES=$(echo_ "$USE_MSVC_EXTERNAL_LIBRARIES" | mangle_path1)

    REL_APP_PATH_WIN=$(echo_ "$REL_APP_PATH" | mangle_path1)

    PRJ_INCLUDE_PATH_WIN=$(echo_ "$PRJ_INCLUDE_PATH" | mangle_path1)

    # echo_ "USE_CAN_DRIVER $USE_CAN_DRIVER; USE_CAN_DEVICE_FOR_SERVER $USE_CAN_DEVICE_FOR_SERVER"

    if  [ $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
        USE_INCLUDE_PATHS='/I "'"$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}library"'" /I "'"$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}library${PATH_SEPARATOR1}xgpl_src"'"'
        append USE_DEFINES ""' /D "CAN_DRIVER_SOCKET" /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
        append USE_d_DEFINES ""' /d "CAN_DRIVER_SOCKET" /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
    fi

    for EACH_REL_APP_PATH in $REL_APP_PATH_WIN ; do
        append USE_INCLUDE_PATHS ""' /I "'"$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}$EACH_REL_APP_PATH"'"'
    done

    for SingleInclPath in $PRJ_INCLUDE_PATH_WIN ; do
        append USE_INCLUDE_PATHS ""' /I "'"$ISO_AG_LIB_PATH_WIN${PATH_SEPARATOR1}$SingleInclPath"'"'
    done

    for SingleInclPath in $USE_WIN32_EXTERNAL_INCLUDE_PATH ; do
        append USE_INCLUDE_PATHS ""' /I "'"$SingleInclPath"'"'
    done
    for SingleLibItem in $USE_MSVC_EXTERNAL_LIBRARIES ; do
        append LIB_DIR_LINE ""' '"$SingleLibItem"
    done
    for SingleLibPath in $USE_WIN32_EXTERNAL_LIBRARY_PATH ; do
        append LIB_DIR_LINE ""' /libpath:"'"$SingleLibPath"'"'
    done

    demangle_path1 < $DspPrjFilelist > $DspPrjFilelist.1
    mv $DspPrjFilelist.1 $DspPrjFilelist

    #echo_ "Libs=$LIB_DIR_LINE"
    #echo_ "Linker=$LIB_FILE_LINE"

    for SinglePrjDefine in $PRJ_DEFINES ; do
        append USE_DEFINES ""' /D '"$SinglePrjDefine"
        append USE_d_DEFINES ""' /D '"$SinglePrjDefine"
    done
    if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
        append USE_DEFINES ""' /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
        append USE_d_DEFINES ""' /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
    fi

    USE_STLPORT_LIB_DIRECTORY=$(echo_ "$USE_STLPORT_HEADER_DIRECTORY" |sed 's|stlport|lib|g')
    SOURCES=$(grep -E "\.cc|\.cpp|\.c|\.lib" "$DspPrjFilelist")
    HEADERS=$(grep -E "\.h|\.hpp" "$DspPrjFilelist")

    local INSERT_PROJECT="$PROJECT"
    local INSERT_INCLUDE_PATHS="$(demangle_path1 "$USE_INCLUDE_PATHS")"
    local INSERT_DEFINES="$USE_DEFINES"
    local INSERT_d_DEFINES="$USE_d_DEFINES"
    local INSERT_CAN_LIB_PATH="$(demangle_path1 "$INSERT_CAN_LIB_PATH")"
    local INSERT_STLPORT_LIB_PATH="$(demangle_path1 "$USE_STLPORT_LIB_DIRECTORY")"
    
    local INSERT_DEBUG_USE_MFC=0
    local INSERT_DEBUG_USE_DEBUG_LIBRARIES=1
    local INSERT_DEBUG_OUTPUT_DIR=Debug
    local INSERT_DEBUG_INTERMEDIATE_DIR=Debug
    local INSERT_DEBUG_IGNORE_EXPORT_LIB=0
    local INSERT_DEBUG_TARGET_DIR=''
    local INSERT_DEBUG_CPP_PARAMETERS="/nologo /W3 /Gm /GX /ZI /Od ${INSERT_INCLUDE_PATHS} /D \"WIN32\" /D \"_DEBUG\" /D \"_CONSOLE\" /D \"_MBCS\" ${INSERT_DEFINES} /YX /FD /TP /GZ /c"
    local INSERT_DEBUG_RSC_PARAMETERS="/l 0x407 /d \"_DEBUG\" ${INSERT_d_DEFINES}"
    local INSERT_DEBUG_LINKER_PARAMETERS="kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept ${INSERT_CAN_LIB_PATH} /libpath:\"${INSERT_STLPORT_LIB_PATH}\""

    local INSERT_RELEASE_USE_MFC=0
    local INSERT_RELEASE_USE_DEBUG_LIBRARIES=0
    local INSERT_RELEASE_OUTPUT_DIR=Release
    local INSERT_RELEASE_INTERMEDIATE_DIR=Release
    local INSERT_RELEASE_IGNORE_EXPORT_LIB=0
    local INSERT_RELEASE_TARGET_DIR=''
    local INSERT_RELEASE_CPP_PARAMETERS="/nologo /W3 /GX /O2 ${INSERT_INCLUDE_PATHS} /D \"WIN32\" /D \"NDEBUG\" /D \"_CONSOLE\" /D \"_MBCS\" ${INSERT_DEFINES} /YX /FD /TP /c"
    local INSERT_RELEASE_RSC_PARAMETERS="/l 0x407 /d \"NDEBUG\" ${INSERT_d_DEFINES}"
    local INSERT_RELEASE_LINKER_PARAMETERS="kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:console /machine:I386 ${INSERT_CAN_LIB_PATH} /libpath:\"${INSERT_STLPORT_LIB_PATH}\""

    {
        expand_template "$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_vc6_prj_base.dsp" >&3

        printf '(VCPROJ_specific_settings\n' >&5
        printf '  (Project_name %s)\n' "$PROJECT" >&5
        printf '  (Debug\n' >&5
        printf '    (Use_MFC %s)\n' "$INSERT_DEBUG_USE_MFC" >&5
        printf '    (Use_debug_libraries %s)\n' "$INSERT_DEBUG_USE_DEBUG_LIBRARIES" >&5
        printf '    (Ignore_export_lib %s)\n' "$INSERT_DEBUG_IGNORE_EXPORT_LIB" >&5
        printf '    (Cpp_parameters %s)\n' "$INSERT_DEBUG_CPP_PARAMETERS" >&5
        printf '    (Rsc_parameters %s)\n' "$INSERT_DEBUG_RSC_PARAMETERS" >&5
        printf '    (Linker_parameters %s))\n' "$INSERT_DEBUG_LINKER_PARAMETERS" >&5
        printf '  (Release\n' >&5
        printf '    (Use_MFC %s)\n' "$INSERT_RELEASE_USE_MFC" >&5
        printf '    (Use_release_libraries %s)\n' "$INSERT_RELEASE_USE_RELEASE_LIBRARIES" >&5
        printf '    (Ignore_export_lib %s)\n' "$INSERT_RELEASE_IGNORE_EXPORT_LIB" >&5
        printf '    (Cpp_parameters %s)\n' "$INSERT_RELEASE_CPP_PARAMETERS" >&5
        printf '    (Rsc_parameters %s)\n' "$INSERT_RELEASE_RSC_PARAMETERS" >&5
        printf '    (Linker_parameters %s))\n' "$INSERT_RELEASE_LINKER_PARAMETERS" >&5

        cat <<'ENDOFHEADERB' >&3
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cc;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
ENDOFHEADERB

        printf '  (Modules %s)\n' "$(format_sources_for_dsp $SOURCES)" >&5

        cat <<'ENDOFHEADERB' >&3
# End Group

# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
ENDOFHEADERB

        printf '  (Headers %s))\n' "$(format_sources_for_dsp $HEADERS)" >&5

        echo_ "# End Group" >&3
        echo_ "# End Target" >&3
        echo_ "# End Project" >&3
    } 3>"$DEV_PRJ_DIR/$PROJECT_FILE_NAME.1"

    #echo_ "Convert UNIX to Windows Linebreak in $DEV_PRJ_DIR/$PROJECT_FILE_NAME"
    unix_lines_to_windows_lines "$DEV_PRJ_DIR/$PROJECT_FILE_NAME.1" "$DEV_PRJ_DIR/$PROJECT_FILE_NAME"
    rm -f "$DEV_PRJ_DIR/$PROJECT_FILE_NAME.1"
    cd "$DEV_PRJ_DIR"
    # org test
}

create_buildfiles()
{
    local CONF_DIR="$1"
    local SCRIPT_DIR="$2"
    local START_DIR="$3"
    # verify and correct the variables
    check_set_correct_variables "$CONF_DIR"
    GENERATE_FILES_ROOT_DIR=$(echo_ "$GENERATE_FILES_ROOT_DIR" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's/\\[0-9a-zA-Z_+\-]+\\\.\.//g')
    # echo_ "Create project for $USE_TARGET_SYSTEM in $GENERATE_FILES_ROOT_DIR"

    # now call the function create_filelist() which build
    # the file list based on the varibles defined above
    create_filelist "$GENERATE_FILES_ROOT_DIR" "$SCRIPT_DIR"

    # call function which build the file list for the unit
    # tests
    # UTESTs disabled right now! (even the create_utest_filelist simply returns at the beginning!)
    #  create_utest_filelist

    # call function to create project specific config file
    create_autogen_project_config $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
    case "$USE_TARGET_SYSTEM" in
        pc_linux)
           # call function to create the Makefile in the project dir
            create_makefile $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        # check if a win32 project file whould be created
        pc_win32)
            create_DevCCPrj $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            create_VCPrj $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        p1mc)
            create_CcsPrj $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        *)
            create_EdePrj $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
    esac

    # cleanup temporary files
    rm -f $GENERATE_FILES_ROOT_DIR/$PROJECT/directories__*.txt
    find $GENERATE_FILES_ROOT_DIR/$PROJECT -name "filelist__*.[alu]*.txt" -exec rm {} \;
    find $GENERATE_FILES_ROOT_DIR/$PROJECT -name "filelist__*-doc.txt" -exec rm {} \;
    #$GENERATE_FILES_ROOT_DIR/$PROJECT/filelist__*.txt

    # jump to initial directory on start of script
    cd "$START_DIR"
}





# Usage: usage
# Print the usage.
usage ()
{
    cat <<EOF
Usage: $0 [OPTION] project_config_file
Create filelist, Makefile and configuration settings for an IsoAgLib project.

-h, --help                        print this message and exit.
--report                          Print generation report.
--report=REPORT_FILE              print generation report to REPORT_FILE.
--doxygen-export-directory=DIR    write the filelist and configuration files with doxygen documentation
blocks to the given directory instead of the default directory of all generated files.
--IsoAgLib-root=DIR               use the given root directory instead of the entry in the selected configuration file.
--target-system=TARGET            produce the project definition files for the selected TARGET instead of the
target which is specified in the configuration file
("pc_linux"|"pc_win32"|"esx"|"esxu"|"c2c"|"imi"|"pm167"|"Dj1"|"mitron167").
--pc-can-driver=CAN_DRIVER        produce the project definition files for the selected CAN_DRIVER if the project shall run on PC
("simulating"|"sys"|"msq_server"|"socket_server"|"socket_server_hal_simulator").
--pc-can-device-for-server=CAN_DEVICE_FOR_SERVER
use this device for building the can_server
("no_card"|"pcan"|"A1"|"rte").
--pc-rs232-driver=RS232_DRIVER    produce the project definition files for the selected RS232_DRIVER if the project shall run on PC
("simulating"|"sys"|"rte"|"hal_simulator").
--little-endian-cpu               select configuration for LITTLE ENDIAN CPU type
--big-endian-cpu                  select configuration for BIG ENDIAN CPU type
--with-makefile-skeleton=filename define project specific MakefileSkeleton text file which is used as base for
Makefiles (default: MakefileSkeleton.txt in the same directory as this script)

$0 parses the selected project configuration file and overwrites the default values for all contained settings.
It then collects the corresponding files which can then be imported to an individual IDE.
Additionally a project specific configuration header is created in the directory xgpl_src/Application_Config with the
name scheme ".config_<project_name>.h". If the #define PRJ_USE_AUTOGEN_CONFIG is set to ".config_<project_name>.h",
the central configuration header xgpl_src/IsoAgLib/isoaglib_config.h will include this header.
Please set additionally the SYSTEM_FOO for the wanted platform - $0 will output the correct define at the end of the
run.
Thus with these two DEFINE settings, the compiler can generate a clean running executable / HEX.

Report bugs to <m.wodok@osb-ag.de>.
EOF
}


abs_dir_of(){ cd "$(dirname "$1")" && pwd; }

check_before_user_configuration()
{
    if [ $# -lt 1 ] ; then
        echo_ "ERROR! You must at least specify the configuration file for your project as parameter"
        echo_
        usage
        exit 1
    fi
    
    # Initialize file descriptor for reports (may be overridden by option --report):
    exec 5>/dev/null

    # Check the arguments.
    for option in "$@"; do
        case "$option" in
            -h | --help)
                usage
                exit 0 ;;
            '--doxygen-export-directory='*)
                DOXYGEN_EXPORT_DIR_1=$(echo_ "$option" | sed -e 's/--doxygen-export-directory=//')
                mkdir -p "$DOXYGEN_EXPORT_DIR_1"
                CALL_DIR=$PWD
                cd "$DOXYGEN_EXPORT_DIR_1"
                DOXYGEN_EXPORT_DIR=$PWD
                cd $CALL_DIR ;;
            '--IsoAgLib-root='*)
                ISOAGLIB_ROOT=$(echo_ "$option" | sed 's/--IsoAgLib-root=//')
                if [ ! -f "$ISOAGLIB_ROOT/library/xgpl_src/IsoAgLib/isoaglib_config.h" ] ; then
                    echo_ "Directory $ISOAGLIB_ROOT doesn't contain valid IsoAgLib directory root. The file xgpl_src/IsoAgLib/isoaglib_config.h can't be found." 1>&2
                    usage
                    exit 1
                else
                    CALL_DIR=$PWD
                    cd $ISOAGLIB_ROOT
                    ISOAGLIB_ROOT=$PWD
                    cd $CALL_DIR
                fi ;;
            '--target-system='*)
                PARAMETER_TARGET_SYSTEM=$(echo_ "$option" | sed 's/--target-system=//')
                ;;
            '--pc-can-driver='*)
                PARAMETER_CAN_DRIVER=$(echo_ "$option" | sed 's/--pc-can-driver=//')
                ;;
            '--pc-can-device-for-server='*)
                PARAMETER_CAN_DEVICE_FOR_SERVER=$(echo_ "$option" | sed 's/--pc-can-device-for-server=//')
                ;;
            '--pc-rs232-driver='*)
                PARAMETER_RS232_DRIVER=$(echo_ "$option" | sed 's/--pc-rs232-driver=//')
                ;;
            --little-endian-cpu)
                PARAMETER_LITTLE_ENDIAN_CPU=1
                ;;
            --big-endian-cpu)
                PARAMETER_LITTLE_ENDIAN_CPU=0
                USE_BIG_ENDIAN_CPU=1
                ;;
            '--with-makefile-skeleton='*)
                RootDir=$PWD
                MAKEFILE_SKELETON_FILE=$RootDir/$(echo_ "$option" | sed 's/--with-makefile-skeleton=//')
                ;;
            '--with-makefile-app-skeleton='*)
                RootDir=$PWD
                MAKEFILE_APP_SKELETON_FILE=$RootDir/$(echo_ "$option" | sed 's/--with-makefile-app-skeleton=//')
                ;;
            '--report'*) # Set file descriptor for report:
                local REPORTFILE="$(printf '%s' "$option" | sed -e 's|^--report\(=-\?\)\?||')"
                if [ -n "$REPORTFILE" ]; then
                    exec 5>"$REPORTFILE"
                else
                    exec 5>&1
                fi
                ;;
            -*)
                echo_ "Unrecognized option $option'" 1>&2
                usage
                exit 1
                ;;
            *)
                CONF_FILE=$option
                ;;
        esac
    done
    
    # check if configuration file exists
    if [ ! -f $CONF_FILE ] ; then
        echo_ "ERROR! configuration file $CONF_FILE does not exist!"
        exit 1
    elif [ ! -r $CONF_FILE ] ; then
        echo_ "ERROR! configuration file $CONF_FILE is not readable!"
        exit 1
    fi
    
    SCRIPT_DIR="$(abs_dir_of "$0")"
    CONF_DIR="$(abs_dir_of "$CONF_FILE")"
    
    # echo_ "StartDir $START_DIR"
    # echo_ "ScriptDir $SCRIPT_DIR"
    # echo_ "ConfDir $CONF_DIR"
}

check_after_user_configuration()
{
    # perform some checks based on user input
    # check for correct target system setting
    if [ $PARAMETER_TARGET_SYSTEM != "UseConfigFile" ] ; then
        USE_TARGET_SYSTEM=$PARAMETER_TARGET_SYSTEM
    fi
    case "$USE_TARGET_SYSTEM" in
        pc_linux | pc_win32 | esx | esxu | c2c | imi | p1mc | pm167 | Dj1 | mitron167)
            ;;
        *)
            echo_ "Unknown target system $USE_TARGET_SYSTEM" 1>&2
            usage
            exit 1 ;;
    esac
    
    # check for corrext CAN driver - and automatically adapt to embedded targets
    if [ $PARAMETER_CAN_DRIVER != "UseConfigFile" ] ; then
        USE_CAN_DRIVER=$PARAMETER_CAN_DRIVER
        IS_CAN_SERVER=$(echo_ $PARAMETER_CAN_DRIVER | grep -c "msq_server")
        if [ $IS_CAN_SERVER -gt 0 ] ; then
            USE_CAN_DEVICE_FOR_SERVER=$(echo_ $PARAMETER_CAN_DRIVER | sed 's/msq_server_//g')
            USE_CAN_DRIVER="msq_server"
            PARAMETER_CAN_DRIVER="msq_server"
        fi
    fi
    
    if [ $PARAMETER_CAN_DEVICE_FOR_SERVER != "UseConfigFile" ] ; then
        USE_CAN_DEVICE_FOR_SERVER=$PARAMETER_CAN_DEVICE_FOR_SERVER
    fi
    
    
    #default for not-can_server
    CAN_SERVER_FILENAME=$PARAMETER_CAN_DRIVER
    case "$USE_CAN_DRIVER" in
        simulating)
            case "$USE_TARGET_SYSTEM" in
                pc_linux | pc_win32)
                    # enhanced CAN HAL is not yet supported for simulating CAN HAL
                    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
                    ;;
                *)
                    echo_ "Override $USE_CAN_DRIVER CAN driver by system driver for embedded target $USE_TARGET_SYSTEM"
                    USE_CAN_DRIVER="sys"
                    PARAMETER_CAN_DRIVER="sys"
                    # enhanced CAN HAL is not yet supported for the known embedded targets
                    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
                    ;;
            esac
            ;;
        sys)
            case "$USE_TARGET_SYSTEM" in
                pc_linux | pc_win32)
                    echo_ "A selection of sys CAN_DRIVER is only applicable for embedded targets." 1>&2
                    usage
                    exit 1
                    ;;
                *)
                    ;;
            esac
            # enhanced CAN HAL is not yet supported for the known embedded targets
            PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
            ;;
        msq_server)
            case "$USE_TARGET_SYSTEM" in
                pc_linux)
                    # enhanced CAN HAL IS supported for the Linux can_server
                    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=1
                    ;;
                pc_win32)
                    echo_ "Server Client CAN driver can only used for target pc_linux. Overridden with socket_server" 1>&2
                    USE_CAN_DRIVER="socket_server"
                    USE_CAN_DEVICE_FOR_SERVER="no_card"
                    CAN_SERVER_FILENAME="can_server_sock"
                    ;;
                *)
                    echo_ "Override $USE_CAN_DRIVER CAN driver by system driver for embedded target $USE_TARGET_SYSTEM"
                    USE_CAN_DRIVER="sys"
                    PARAMETER_CAN_DRIVER="sys"
                    # enhanced CAN HAL is not yet supported for the known embedded targets
                    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
                    ;;
            esac
            # make sure, that USE_CAN_DEVICE_FOR_SERVER is automatically set, when not yet defined
            if [ "A$USE_CAN_DEVICE_FOR_SERVER" = "A" ] ; then
                case $PRJ_DEFINES in
                    *SYSTEM_A1*)
                        USE_CAN_DEVICE_FOR_SERVER="A1"
                        ;;
                    *)
                        USE_CAN_DEVICE_FOR_SERVER="no_card"
                        ;;
                esac
            fi
            if [ $USE_TARGET_SYSTEM != "pc_win32" ] ; then
                CAN_SERVER_FILENAME=${USE_CAN_DRIVER}_${USE_CAN_DEVICE_FOR_SERVER}
            fi
            ;;
        socket_server | socket_server_hal_simulator)
            # enhanced CAN HAL IS supported for the socket based can_server
            PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=1
            if [ "A$USE_CAN_DEVICE_FOR_SERVER" = "A" ] ; then
                case $PRJ_DEFINES in
                    *SYSTEM_A1*)
                        USE_CAN_DEVICE_FOR_SERVER="A1"
                        ;;
                    *)
                        USE_CAN_DEVICE_FOR_SERVER="no_card"
                        ;;
                esac
            fi
    
            CAN_SERVER_FILENAME=can_server_sock_${USE_CAN_DEVICE_FOR_SERVER}
            if test $USE_TARGET_SYSTEM = "pc_win32" ; then
                if test $USE_CAN_DEVICE_FOR_SERVER = "no_card" ; then
                    # skip extension "no_card"
                    CAN_SERVER_FILENAME=can_server_sock
                fi
            fi
            ;;
        *)
            echo_ "Unknown CAN driver $USE_CAN_DRIVER" 1>&2
            usage
            exit 1
            ;;
    esac
    
    case "$USE_CAN_DEVICE_FOR_SERVER" in
        rte)
            case "$USE_TARGET_SYSTEM" in
                pc_linux)
                    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=1
                    ;;
                pc_win32)
                    echo_ "RTE CAN driver can only used for target pc_linux" 1>&2
                    usage
                    exit 1
                    ;;
            esac
            ;;
    esac
    
    
    
    if [ $PARAMETER_RS232_DRIVER != "UseConfigFile" ] ; then
        USE_RS232_DRIVER=$PARAMETER_RS232_DRIVER
    fi
    case "$USE_RS232_DRIVER" in
        simulating)
            case "$USE_TARGET_SYSTEM" in
                pc_linux | pc_win32)
                    ;;
                *)
                    echo_ "Override $USE_RS232_DRIVER RS232 driver by system driver for embedded target $USE_TARGET_SYSTEM"
                    USE_RS232_DRIVER="sys"
                    PARAMETER_RS232_DRIVER="sys"
                    ;;
            esac
            ;;
        sys)
            ;;
        rte)
            case "$USE_TARGET_SYSTEM" in
                pc_linux)
                    ;;
                pc_win32)
                    USE_RS232_DRIVER="sys"
                    echo_ "RTE RS232 driver can only used for target pc_linux -> Override by sys"
                    ;;
                *)
                    echo_ "Override $USE_RS232_DRIVER RS232 driver by system driver for embedded target $USE_TARGET_SYSTEM"
                    USE_RS232_DRIVER="sys"
                    PARAMETER_RS232_DRIVER="sys"
                    ;;
            esac
            ;;
        *)
            echo_ "Unknown RS232 driver $USE_RS232_DRIVER" 1>&2
            usage
            exit 1
            ;;
    esac
    
    # check for little/big endian setting
    if [ "A$PARAMETER_LITTLE_ENDIAN_CPU" != "A" ] ; then
        USE_LITTLE_ENDIAN_CPU=$PARAMETER_LITTLE_ENDIAN_CPU
    fi
}

report_summary()
{
    if [ "$USE_LITTLE_ENDIAN_CPU" -lt 1 ] ; then
        echo_  "Endianess:     Big Endian CPU"
    else
        echo_  "Endianess:     Little Endian CPU"
    fi
    echo_    "Target:        $IDE_NAME - (The settings below are already set up therefore)"
    echo_    "Defines:       $USE_SYSTEM_DEFINE PRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h $PRJ_DEFINES"
    echo_n "Include Path:  "
    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        echo_n "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH ";
    done
    echo_
    echo_
    echo_ "Generation successful."
}

make_doxygen_ready_comment_blocks()
{
    if [ "A$DOXYGEN_EXPORT_DIR" != "A" ] ; then
        # doxygen export is specified -> copy config file there with suitable doc block
        CONF_BASE=$(basename $CONF_FILE)
        CONFIG_SPEC_DOXYGEN_READY="$DOXYGEN_EXPORT_DIR/spec"'__'"$CONF_BASE"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER-doc.txt"
        {
            #echo_ "/**" >&3
            #echo_ "* \section PrjSpec$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of configuration settings for $PROJECT ." >&3
            #echo_ "* This is only a copy with doxygen ready comment blocks from the original file in IsoAgLib/compiler_projeckdevelop_make/ " >&3
            #echo_ "* Use the file $CONF_FILE in this directory as input file for $0 to create the project generation files." >&3
            #echo_ "*/" >&3
            #echo_ "/*@{*/" >&3
        #   cat $CONF_FILE >&3
            #sed -e "s/USE_TARGET_SYSTEM=\".*/USE_TARGET_SYSTEM=\"$USE_TARGET_SYSTEM\"/g" -e "s/USE_CAN_DRIVER=\".*/USE_CAN_DRIVER=\"$USE_CAN_DRIVER\"/g" -e "s/USE_RS232_DRIVER=\".*/USE_RS232_DRIVER=\"$USE_RS232_DRIVER\"/g" $CONF_FILE > /tmp/$CONF_BASE
            #cat /tmp/$CONF_BASE >&3
            #rm -f /tmp/$CONF_BASE
            #echo_ "/*@}*/" >&3
        
            echo_e "$ENDLINE$ENDLINE @section PrjSpec$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of configuration settings for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" > $CONFIG_SPEC_DOXYGEN_READY
            echo_ " This is only a copy with doxygen ready comment blocks from the original file in IsoAgLib/compiler_projeckdevelop_make/ " >&3
            echo_ " Use the file $CONF_FILE in this directory as input file for $0 to create the project generation files." >&3
            echo_ "\code" >&3
            sed -e "s/USE_TARGET_SYSTEM=\".*/USE_TARGET_SYSTEM=\"$USE_TARGET_SYSTEM\"/g" -e "s/USE_CAN_DRIVER=\".*/USE_CAN_DRIVER=\"$USE_CAN_DRIVER\"/g" -e "s/USE_RS232_DRIVER=\".*/USE_RS232_DRIVER=\"$USE_RS232_DRIVER\"/g" $CONF_DIR/$CONF_FILE > /tmp/$CONF_BASE
            cat /tmp/$CONF_BASE >&3
            echo_ "\endcode" >&3
        } 3>"$CONFIG_SPEC_DOXYGEN_READY"
        rm -f /tmp/$CONF_BASE
    
    fi
}

main "$@"