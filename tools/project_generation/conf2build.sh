#!/bin/sh

# conf2build.sh: Script to create file list, configuration
#   headers and project files for applications based on a feature set
#   specifying conf(ig) file
#
# (C) Copyright 2009 - 2010 by OSB AG and developing partners
#
# See the repository-log for details on the authors and file-history.
# (Repository information can be found at <http://isoaglib.com/download>)
#
# Use, modification and distribution are subject to the GNU General
# Public License. (See accompanying file LICENSE.txt)

# Prologue
POSIXLY_CORRECT=y # for the case that bash is running
unset -f unalias
\unalias -a
unset -f command
SYSPATH="$(command -p getconf PATH 2>/dev/null)"
: ${SYSPATH:=/usr/bin:/bin}
PATH="$SYSPATH:$PATH"
umask 022
set -o nounset
set -o errexit

# global constants:
TAB="$(printf '\t')"
LF="
"
PATH_SEPARATOR1='=_=_'
PATH_SEPARATOR2='_=_='
TEMPFILE_PREFIX="/tmp/conf2build$$"

main()
{
    set_default_values
    check_before_user_configuration "$@"
    
    # now source the config setting file
    . ./$CONF_FILE

    check_after_user_configuration
    
    # Create files to be used to build according to the users's
    # configuration settings
    create_buildfiles "$CONF_DIR" "$SCRIPT_DIR"
    
    report_summary
}

# Prefer these functions to echo due to incompatible versions (Bourne
# shell builtin, Bash builtin, command):
echo_() { printf '%s\n' "$*"; }
echo_e() { printf '%b\n' "$*"; }
echo_e_n() { printf '%b' "$*"; }
echo_n() { printf '%s' "$*"; }
# Compatible but slow variant as fallback:
eval 'echo() { '$(which echo)' "$@"; }'

# wrap words into lines
wrap()
{
    printf '%s\n' "$@"
}

# Escapes % and \ to get a literal printf-format:
literal_format()
{
    printf '%s' "$1" | sed -e 's|[%\\]|&&|g'
}

# Joins to one string.
# E.g. the following command prints "a,bc,def":
#   join , a bc def
join()
{
    local IFS="$1"
    shift
    printf '%s' "$*"
}

join_comma() { join ',' "$@"; }
join_semicolon() { join ';' "$@"; }
join_space() { join ' ' "$@"; }
prefix_library_path() { printf -- '-L%s' "$@"; }

# map CONTINUATION FUNCTION ARG ...
# apply FUNCTION to each ARG and continue with CONTINUATION
map()
{
    local CONTINUE="$1"
    local F="$2"
    shift 2
    for A in "$@"; do
        shift
        set -- "$@" "$("$F" "$A")"
    done
    "$CONTINUE" "$@"
}

status_le1() { [ $? -le 1 ]; }

# this function is used to verify and
# correct the several project configuration
# variables
# if one of the following variables isn't set
# the corresponding default values are used
set_default_values()
{
    PARAMETER_TARGET_SYSTEM="UseConfigFile"
    PARAMETER_CAN_DRIVER="UseConfigFile"
    PARAMETER_RS232_DRIVER="UseConfigFile"
    # may be overridden by configuration file or check_after_user_configuration:
    USE_EMBED_LIB_DIRECTORY=''
    # may be overridden by configuration file or check_after_user_configuration:
    USE_EMBED_HEADER_DIRECTORY=''
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
    GENERATE_FILES_ROOT_DIR="$PWD"

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
    # no reasonable default for all targets (will be set later
    # conditionally):
    unset PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL || :
    CAN_INSTANCE_CNT=1
    PRT_INSTANCE_CNT=1
    RS232_CHANNEL_CNT=1
    RS232_INSTANCE_CNT=1
    APP_NAME=''
    PRJ_PROPRIETARY_PGN_INTERFACE=0
    PRJ_ISO11783=0
    PRJ_ISO_TERMINAL=0
    PRJ_ISO_FILESERVER_CLIENT=0
    PRJ_ISO_TERMINAL=0
    PRJ_ISO_TERMINAL_LAYOUT_MANAGER=0
    PRJ_ISO_TASKCONTROLLER_CLIENT=0
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
    PRJ_EEPROM=0
    PRJ_DATASTREAMS=0
    PRJ_OUTPUTS=0
    PRJ_INPUTS_DIGITAL=0
    PRJ_INPUTS_ANALOG=0
    PRJ_INPUTS_COUNTER=0
    PRJ_RS232=0
    # has to be overridden by configuration:
    USE_TARGET_SYSTEM='void'
    USE_HAL_PATH=''
    USE_ISO_TERMINAL_GRAPHICCONTEXT=0
    USE_ISO_TERMINAL_GETATTRIBUTES=0
    USE_ISO_TERMINAL_ATTRIBUTES=0
    PRJ_ISO_TERMINAL_OBJECT_SELECTION1=''
    PRJ_ISO_TERMINAL_OBJECT_SELECTION2=''
    PRJ_ISO_TERMINAL_OBJECT_SELECTION3=''
    PRJ_ISO_TERMINAL_OBJECT_SELECTION4=''
    PRJ_DEFINES=''
    PRJ_INCLUDE_PATH=''
    USE_LINUX_EXTERNAL_INCLUDE_PATH=''
    USE_LINUX_EXTERNAL_LIBRARIES=''
    USE_LINUX_EXTERNAL_LIBRARY_PATH=''
    PRJ_COMPILER_BINARY_PRE=''
    USE_WIN32_EXTERNAL_INCLUDE_PATH=''
    USE_WIN32_EXTERNAL_LIBRARY_PATH=''
    USE_WIN32_EXTERNAL_INCLUDE_PATH=''
    USE_WIN32_EXTERNAL_LIBRARY_PATH=''
    USE_MSVC_EXTERNAL_LIBRARIES=''
    APP_SRC_FILE=''
    USE_RS232_DRIVER='none'
	USE_INPUTS_DRIVER='none'
	USE_OUTPUTS_DRIVER='none'
    DEBUG_DEFINES=''
    NDEBUG_DEFINE="$(wrap NDEBUG)"
    COMBINED_DEFINES="$NDEBUG_DEFINE $DEBUG_DEFINES"
}

# update PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL unless contradiction with
# user configuration
update_prj_system_with_enhanced_can_hal()
{
    local SETPOINT="$1"
    local MESSAGE="$2"
    if [ -n "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL_ORIG" ]; then
        # user has configured PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL
        if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL_ORIG" -ne "$SETPOINT" ]; then
            # contradiction
            printf 'ERROR: try PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=%s\n' "$SETPOINT" >&2
            exit 2
        fi
    elif [ "${PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL:-}" = "$SETPOINT" ]; then
        : # omit redundant update
    else
        # actually update
        printf '%b' "$MESSAGE" >&2
        PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL="$SETPOINT"
    fi
}

check_set_correct_variables()
{
    local CONF_DIR="$1"

    : ${ISO_AG_LIB_PATH:?"ERROR! Please specify the path to the root directory of IsoAgLib - i.e. where xgpl_src is located"}
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

    if [ "$PRJ_ISO11783" -lt 1 -a "$PRJ_ISO_TERMINAL" -gt 0 ]; then
        echo_ "ERROR! Can't utilize PRJ_ISO_TERMINAL as ISO11783 is not activated"
        echo_ "Set PRJ_ISO11783 to 1 if you want ISO 11783 virtual terminal-client support."
        exit 2
    fi

    if [ "$PRJ_ISO11783" -lt 1 -a "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]; then
        echo_ "ERROR! Can't utilize PRJ_ISO_FILESERVER_CLIENT as ISO11783 is not activated"
        echo_ "Set PRJ_ISO11783 to 1 if you want ISO 11783 fileserver-client support."
        exit 2
    fi

    if [ "$PRJ_RS232" -lt 1 -a "$PRJ_RS232_OVER_CAN" -gt 0 ]; then
        echo_ "ERROR! Can't utilize PRJ_RS232_OVER_CAN as PRJ_RS232 isn't set"
        echo_ "Set PRJ_RS232 to 1 to enable RS232 support."
        exit 2
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
    if [ $PARAMETER_RS232_DRIVER != "UseConfigFile" ] ; then
        USE_RS232_DRIVER=$PARAMETER_RS232_DRIVER
        #echo_ "Use Parameter value for rs232 driver: $PARAMETER_RS232_DRIVER"
    fi

    # workaround as long as we don't have separate HALs
    case "$USE_TARGET_SYSTEM" in
      (pc_win32|pc_linux)
        HAL_FIND_PATH="$ISO_AG_LIB_PATH/library/xgpl_src"
        HAL_PREFIX_ISOAGLIB="IsoAgLib/hal/pc"
        HAL_PREFIX_SUPPLEMENTARY="supplementary_driver/hal/pc"
        ;;
       (c2c|Dj1|esx|esxu|p1mc|src9)
        HAL_FIND_PATH="$ISO_AG_LIB_PATH/library/xgpl_src"
        HAL_PREFIX_ISOAGLIB="IsoAgLib/hal/$USE_TARGET_SYSTEM"
        HAL_PREFIX_SUPPLEMENTARY="supplementary_driver/hal/$USE_TARGET_SYSTEM"
        ;;
      (*) #All other one are proprietary and need the USE_HAL_PATH set!
        HAL_FIND_PATH="$USE_HAL_PATH"
        HAL_PREFIX_ISOAGLIB="$USE_TARGET_SYSTEM"
        HAL_PREFIX_SUPPLEMENTARY="$USE_TARGET_SYSTEM"
        ;;
    esac

    # set default values for the default target platforms (HAL)
    # the N/A ones will be re-set in the case afterwards
    HAL_PATH_ISOAGLIB="$HAL_PREFIX_ISOAGLIB"
    HAL_PATH_ISOAGLIB_SYSTEM="$HAL_PREFIX_ISOAGLIB/system"
    HAL_PATH_ISOAGLIB_CAN="$HAL_PREFIX_ISOAGLIB/can"
    HAL_PATH_SUPPLEMENTARY_EEPROM="$HAL_PREFIX_SUPPLEMENTARY/eeprom"
    HAL_PATH_SUPPLEMENTARY_RS232="$HAL_PREFIX_SUPPLEMENTARY/rs232"
    HAL_PATH_SUPPLEMENTARY_OUTPUTS="$HAL_PREFIX_SUPPLEMENTARY/outputs"
    HAL_PATH_SUPPLEMENTARY_INPUTS="$HAL_PREFIX_SUPPLEMENTARY/inputs"
    HAL_PATH_SUPPLEMENTARY_DATASTREAMS="$HAL_PREFIX_SUPPLEMENTARY/datastreams"

    case "$USE_TARGET_SYSTEM" in
        (pc_win32)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/cmake"
            IDE_NAME="CMake"
            ;;
        (pc_linux)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/cmake_make"
            IDE_NAME="CMake, make"
            ;;
        (esx|esxu|c2c)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE"
            IDE_NAME="Tasking EDE"
            ;;
        (Dj1)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/EDE"
            IDE_NAME="Tasking EDE"
            ;;
        (p1mc)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/CCS"
            IDE_NAME="Code Composer Studio"
            ;;
        (ees)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/cmake_make/"
            IDE_NAME="CMake, make"
            ;;
        (src9)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/kdevelop_make"
            IDE_NAME="KDevelop, make"
            ;;
# AMS5 generation needs to be taken out into the proprietary HAL's project generation, which is TBD
#        (ams5)
#            GENERATE_FILES_ROOT_DIR="$CONF_DIR/ams5"
#            IDE_NAME="IAR Embedded Workbench IDE"
#            ;;
        (*)
            GENERATE_FILES_ROOT_DIR="$CONF_DIR/proprietary_hal"
            IDE_NAME="Proprietary - Filelist only"
            ;;
    esac

    # Force mandatory modules
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_ISOAGLIB" ]; then echo_ "Invalid proprietary HAL path: $HAL_FIND_PATH/$HAL_PATH_ISOAGLIB"; exit 2; fi
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_ISOAGLIB_SYSTEM" ]; then echo_ "Proprietary HAL is missing the 'system' directory: $HAL_FIND_PATH/$HAL_PATH_ISOAGLIB_SYSTEM"; exit 2; fi
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_ISOAGLIB_CAN" ]; then echo_ "Proprietary HAL is missing the 'can' directory: $HAL_FIND_PATH/$HAL_PATH_ISOAGLIB_CAB"; exit 2; fi
    # Disable N/A supplementary modules
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_SUPPLEMENTARY_EEPROM" ]; then HAL_PATH_SUPPLEMENTARY_EEPROM=""; fi
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_SUPPLEMENTARY_RS232" ]; then HAL_PATH_SUPPLEMENTARY_RS232=""; fi
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_SUPPLEMENTARY_OUTPUTS" ]; then HAL_PATH_SUPPLEMENTARY_OUTPUTS=""; fi
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_SUPPLEMENTARY_INPUTS" ]; then HAL_PATH_SUPPLEMENTARY_INPUTS=""; fi
    if [ ! -d "$HAL_FIND_PATH/$HAL_PATH_SUPPLEMENTARY_DATASTREAMS" ]; then HAL_PATH_SUPPLEMENTARY_DATASTREAMS=""; fi

    # handle virtual drivers
    if [ "$PRJ_RS232_OVER_CAN" -gt 0 ]; then
        HAL_PATH_SUPPLEMENTARY_RS232="supplementary_driver/hal/virtualDrivers/rs232_over_can"
    fi

    case "$USE_CAN_DRIVER" in
        (simulating)
            update_prj_system_with_enhanced_can_hal 0 'The selected CAN driver "simulating" does NOT provide the enhanced CAN processing.\n'
            ;;
        (msq_server|socket_server|socket_server_hal_simulator)
            ;;
        (sys)
            # Allow any setting of Enhanced CAN HAL, because we do not want to limit Proprietary HALs by that.
            #update_prj_system_with_enhanced_can_hal 0 'The selected CAN driver "sys" on embedded targets does NOT provide the enhanced CAN processing.\n\n'
            ;;
        (*)
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
        eval "$FUNCTION() { $VARIABLE=\"\${$VARIABLE:-}\$*\"; }"
    shift
    "$FUNCTION" "$@"
}

# Write to FD3 what's needed for finding COMM features.
comm_features()
{
    printf '%s' " -name 'isoaglib_config.h'" >&3
    if [ "$PRJ_BASE" -gt 0 ]; then
        printf '%s' " -o -path '*/Part7_ApplicationLayer/*'" >&3
    else
        if expr "$PRJ_TRACTOR_GENERAL" \| "$PRJ_TRACTOR_MOVE" \| "$PRJ_TRACTOR_FACILITIES" \| "$PRJ_TRACTOR_PTO" \| "$PRJ_TRACTOR_LIGHT" \| "$PRJ_TRACTOR_AUX" \| "$PRJ_TIME_GPS" \| "$PRJ_TRACTOR_GUIDANCE" \| "$PRJ_TRACTOR_CERTIFICATION" >/dev/null; then
            printf '%s' " -o -name 'ibasetypes.h' -o -name 'basecommon_c*'" >&3
        fi
        if [ "$PRJ_TRACTOR_GENERAL" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracgeneral_c*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_MOVE" -gt 0 -a "$PRJ_TRACTOR_MOVE_SETPOINT" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracmove*' \)" >&3
        elif [ "$PRJ_TRACTOR_MOVE" -gt 0 -a "$PRJ_TRACTOR_MOVE_SETPOINT" -lt 1 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracmove_c.*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_PTO" -gt 0 -a "$PRJ_TRACTOR_PTO_SETPOINT" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracpto*' \)" >&3
        elif [ "$PRJ_TRACTOR_PTO" -gt 0 -a "$PRJ_TRACTOR_PTO_SETPOINT" -lt 1 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracpto_c.*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_LIGHT" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*traclight*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_FACILITIES" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracfacilities_c*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_AUX" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracaux*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_GUIDANCE" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracguidance*' \)" >&3
        fi
        if [ "$PRJ_TRACTOR_CERTIFICATION" -gt 0 -a "$PRJ_ISO11783" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*traccert*' \)" >&3
        fi
        if [ "$PRJ_TIME_GPS" -gt 0 ]; then
            printf '%s' " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*timeposgps*' \)" >&3
        fi
    fi

    if [ "$PRJ_PROPRIETARY_PGN_INTERFACE" -gt 0 ]; then
        printf '%s' " -o -path '*/Part3_ProprietaryMessages/*'" >&3
    fi

    if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]  ; then
        printf '%s' " -o -path '*/Part13_FileServer_Client/*'" >&3
    fi

    if [ "$PRJ_ISO_TERMINAL" -gt 0 ]; then
        if [ "$USE_ISO_TERMINAL_GRAPHICCONTEXT" -eq 0 ]; then
            # exclude graphicscontext_c
            printf '%s' " -o \( -path '*/Part6_VirtualTerminal_Client/i*' -a -not -name '*graphicscontext_c*' \)" >&3
        else
            printf '%s' " -o -path '*/Part6_VirtualTerminal_Client/i*'" >&3
        fi
    fi
    if [ "$PRJ_DATASTREAMS" -lt 1 ]; then
        if [ "$PRJ_ISO_TERMINAL" -gt 0 -o "$PRJ_TIME_GPS" -gt 0 ]; then
            printf '%s' " -o -path '*/driver/datastreams/volatilememory_c.*'" >&3
        fi
    fi
    if [ "$PRJ_ISO11783" -gt 0 ]; then
        printf '%s' " -o -path '*/i*isobus_c.*' -o -path '*i*proprietarybus_c.*' -o -path '*/Part3_DataLink/i*multi*' -o -path '*/Part3_DataLink/impl/sendstream_c.*' -o -path '*/Part3_DataLink/impl/stream_c.*' -o -path '*/Part3_DataLink/istream_c.*' -o -path '*/supplementary_driver/driver/datastreams/streaminput_c.h'  -o -path '*/IsoAgLib/convert.h'" >&3
        if [ "$PRJ_MULTIPACKET_STREAM_CHUNK" -gt 0 ]; then
            printf '%s' " -o -path '*/Part3_DataLink/impl/streamchunk_c.*' -o -path '*/Part3_DataLink/impl/chunk_c.*'" >&3
        else
            printf '%s' " -o -path '*/Part3_DataLink/impl/streamlinear_c.*'" >&3
        fi
    fi
}

# Write to FD3 what's needed for finding DRIVER features.
driver_and_hal_features()
{
    printf '%s' \
      " -path '*/hal/hal_system.h' -o " \
      " -path '*/hal/hal_can.h' -o " \
      " -path '*/hal/hal_config.h' -o " \
      " -path '*/hal/hal_typedef.h' -o " \
      " -path '*/hal/generic_utils/system*' -o " \
      " -path '*/driver/system*' " >&3
    # For now, simply copying all generic_utils/system files
    # as it doesn't matter as it's just about headers...

    printf '%s' \
      " -path '*${HAL_PATH_ISOAGLIB_CAN}/can*.h'  -o " \
      " -path '*${HAL_PATH_ISOAGLIB_CAN}/hal_can*' -o " \
      " -path '*${HAL_PATH_ISOAGLIB}/system*' -o " \
      " -path '*${HAL_PATH_ISOAGLIB}/errcodes.h' -o " \
      " -path '*${HAL_PATH_ISOAGLIB}/config.h' -o " \
      " -path '*${HAL_PATH_ISOAGLIB}/typedef.h' " >&4

    echo_ "CAN driver:    $USE_CAN_DRIVER"
    case "$USE_CAN_DRIVER" in
        (simulating)
            printf '%s' " -o -path '*${HAL_PATH_ISOAGLIB_CAN}/target_extension_can_simulating*'" >&4
            ;;
        (msq_server)
            printf '%s' " -o -path '*${HAL_PATH_ISOAGLIB_CAN}/target_extension_can_client_msq.*' -o -path '*${HAL_PATH_ISOAGLIB}/can/msq_helper*'" >&4
            ;;
        (socket_server)
            printf '%s' " -o -path '*${HAL_PATH_ISOAGLIB_CAN}/target_extension_can_client_sock.*'" >&4
            ;;
        (socket_server_hal_simulator)
            printf '%s' " -o -path '*${HAL_PATH_ISOAGLIB_CAN}/target_extension_can_client_sock_hal_simulator.*'" >&4
            ;;
        (sys)
            printf '%s' " -o -path '*${HAL_PATH_ISOAGLIB_CAN}/target_extension_can_socketcan.*'" >&4
            ;;
    esac

    # add the standard driver directory sources for CAN
    if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -gt 0 ]; then
        # no msgobjs in enh. can hal
        printf '%s' " -o \( -path '*/driver/can/*' -a -not -name 'msgobj*' \)" >&3
    else
        printf '%s' " -o -path '*/driver/can/*'" >&3
        # we need to integrate the sources for the central CAN FIFO
        printf '%s' " -o -path '*/hal/generic_utils/can/*'" >&3
    fi

    if [ "$PRJ_EEPROM" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/eeprom/*' -o -path '*/hal/hal_eeprom.h'" >&3
        printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_EEPROM}/*'" >&4
    fi
    if [ "$PRJ_DATASTREAMS" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/datastreams/*' -o -path '*/hal/hal_datastreams.h'" >&3
        printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_DATASTREAMS}/*'" >&4
    fi
    if [ "$PRJ_OUTPUTS" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/outputs*' -o -path '*/hal/hal_outputs.h'" >&3
        printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_OUTPUTS}/outputs.h' -o -path '*${HAL_PATH_SUPPLEMENTARY_OUTPUTS}/outputs_target_extensions.h'" >&4
		echo_ "OUTPUTS driver:$USE_OUTPUTS_DRIVER"
        case "$USE_OUTPUTS_DRIVER" in
			(simulating)
				printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_OUTPUTS}/target_extension_outputs_simulating*'" >&4
				;;
			(hal_simulator)
				printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_OUTPUTS}/target_extension_outputs_hal_simulator*'" >&4
				;;
			(sys)
				printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_OUTPUTS}/target_extension_outputs_sys*'" >&4
				;;
			(*)
				echo_ 'ERROR! Please set the config variable "USE_OUTPUTS_DRIVER" to one of "simulating"|"sys"|"hal_simulator"'
				echo_ 'Current Setting is $USE_OUTPUTS_DRIVER'
				exit 3
		esac
    fi
    if [ "$PRJ_INPUTS_DIGITAL" -gt 0 ]; then
        printf '%s' " -o -name '*digitali_c.*'" >&3
    fi
    if [ "$PRJ_INPUTS_ANALOG" -gt 0 ]; then
        printf '%s' " -o -name '*analogi*'" >&3
    fi
    if [ "$PRJ_INPUTS_COUNTER" -gt 0 ]; then
        printf '%s' " -o -name '*counteri*'" >&3
    fi
    if [ "$PRJ_INPUTS_DIGITAL" -gt 0 -o "$PRJ_INPUTS_ANALOG" -gt 0 -o "$PRJ_INPUTS_COUNTER" -gt 0 ]; then
        printf '%s' " -o -name '*inputbase_c.*' -o -name '*inputs_c.*' -o -path '*/hal/hal_inputs.h'" >&3
        printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_INPUTS}/inputs.h' -o -path '*${HAL_PATH_SUPPLEMENTARY_INPUTS}/inputs_target_extensions.h'" >&4
		echo_ "INPUTS driver: $USE_INPUTS_DRIVER"
        case "$USE_INPUTS_DRIVER" in
			(simulating)
				printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_INPUTS}/target_extension_inputs_simulating*'" >&4
				;;
			(hal_simulator)
				printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_INPUTS}/target_extension_inputs_hal_simulator*'" >&4
				;;
			(sys)
				printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_INPUTS}/target_extension_inputs_sys*'" >&4
				;;
			(*)
				echo_ 'ERROR! Please set the config variable "USE_INPUTS_DRIVER" to one of "simulating"|"sys"|"hal_simulator"'
				echo_ 'Current Setting is $USE_INPUTS_DRIVER'
				exit 3
		esac
    fi
    if [ "$PRJ_RS232" -gt 0 ]; then
        printf '%s' " -o -path '*/driver/rs232/*' -o -path '*/hal/hal_rs232.h'" >&3
        printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_RS232}/rs232.h'" >&4
        if [ "$PRJ_RS232_OVER_CAN" -gt 0 ]; then
            echo_ "RS232 driver:  rs232_over_can"
            printf '%s' " -o -path '*/hal/virtualDrivers/rs232_over_can/*'" >&3
        else
            echo_ "RS232 driver:  $USE_RS232_DRIVER"
            case "$USE_RS232_DRIVER" in
                (simulating)
                    printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_RS232}/target_extension_rs232_simulating*'" >&4
                    ;;
                (hal_simulator)
                    printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_RS232}/target_extension_rs232_hal_simulator*'" >&4
                    ;;
                (rte)
                    printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_RS232}/target_extension_rs232_rte*'" >&4
                    ;;
                (sys)
                    if [ "$USE_TARGET_SYSTEM" = "pc_win32" ]; then
                        printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_RS232}/target_extension_rs232_w32_sys*'" >&4
                    else
                        printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_RS232}/target_extension_rs232_linux_sys*'" >&4
                    fi
                    ;;
                (*)
                    echo_ 'ERROR! Please set the config variable "USE_RS232_DRIVER" to one of "simulating"|"sys"|"rte"|"hal_simulator"'
                    echo_ 'Current Setting is $USE_RS232_DRIVER'
                    exit 3
            esac
            printf '%s' " -o -path '*${HAL_PATH_SUPPLEMENTARY_RS232}/rs232_target_extensions.h'" >&4
        fi
    fi
}


#Write to FD3 what's needed for finding COMM PROC features.
comm_proc_features()
{
    if [ "$PRJ_ISO_TASKCONTROLLER_CLIENT" -gt 0 ]; then
        if [ -n "${COMM_PROC_FEATURES:-}" ]; then
            printf '%s' " -o " >&3
        fi
        printf '%s' " -name 'processdatachangehandler_c.*' -o -name 'iprocess_c.*' -o -name 'elementddi_s.h' -o -name 'proc_c.h' -o -path '*/Part10_TaskController_Client/impl/proc*' -o -path '*/Part10_TaskController_Client/iprocesscmd*' -o -path '*/Part10_TaskController_Client/impl/processcmd*' -o -path '*/Part10_TaskController_Client/*procdata*base_c.h'" >&3
        printf '%s' " -o -path '*/Part10_TaskController_Client/i*devproperty*'" >&3
        printf '%s' " -o -path '*/Part10_TaskController_Client/StdMeasureElements/*'" >&3
        printf '%s' " -o -path '*/Part10_TaskController_Client/StdSetpointElements/*'" >&3
    fi
}

# Split given string by delimiting whitespace and print parts with
# given format.
# Parameters: input string, first format, continuation format
split_and_printf()
{
    local INPUT="$1"
    local FORMAT="$2"
    local NEXT_FORMAT="$3"

    while [ -n "$INPUT" ]; do
        local N_MAXIMAL=$(expr length "$INPUT")
        local N_WHITESPACE=$(expr "$INPUT" : ' *')
        if [ 0 -lt "$N_WHITESPACE" ]; then
            INPUT="$(expr substr "$INPUT" $(($N_WHITESPACE + 1)) $N_MAXIMAL)"
        fi
        local N_NONWHITE=$(expr "$INPUT" : '[^ ]*')
        local PART=$(expr substr "$INPUT" 1 $N_NONWHITE)
        printf -- "$FORMAT" "$PART"
        FORMAT="$NEXT_FORMAT"
        if [ 0 -lt "$N_NONWHITE" ]; then
            INPUT="$(expr substr "$INPUT" $(($N_NONWHITE + 1)) $N_MAXIMAL || printf '')"
        fi
    done
}

# Calculate part of find command and write to FD3
find_part()
{
    local OUTER_OP="$1"
    local INNER_OP="$2"
    local INPUT="$3"
    if [ -n "$INPUT" ]; then
        printf '%s \( ' "$OUTER_OP" >&3
        split_and_printf "$INPUT" "$INNER_OP" " -or $INNER_OP" >&3
        printf ' \)' >&3
    fi
}

add_feature_partition_rule()
{
    local FEATURE_NAME="$1"
    local FEATURE_PATTERN="$2"
    append FEATURE_PARTITION_RULES "$FEATURE_NAME$LF$FEATURE_PATTERN$LF"
}

# Partition paths read from STDIN by the given rules and set variables
# PARTITIONED_ accordingly.
partition_features()
{
    local FEATURE_NAME

    if [ -n "${PARTITIONED_DIRECTORY:-}" ]; then
        # delete old partitions:
        while read -r FEATURE_NAME && [ -n "$FEATURE_NAME" ]; do
            unset "PARTITIONED_${FEATURE_NAME}"
        done <<END_OF_PARTITIONED_FEATURES
$PARTITIONED_DIRECTORY
END_OF_PARTITIONED_FEATURES
        unset "$PARTITIONED_DIRECTORY"
    fi

    local CANDIDATES1="$(cat)"
    while read -r FEATURE_NAME && [ -n "$FEATURE_NAME" ]; do
        read -r FEATURE_PATTERN
        local CANDIDATES2="$CANDIDATES1"
        CANDIDATES1=''
        local FILE
        while read -r FILE && [ -n "$FILE" ]; do
            if expr "$FILE" : "$FEATURE_PATTERN" >/dev/null; then
                if ! grep -q "^${FEATURE_NAME}$" <<END_OF_FEATURES >/dev/null
${PARTITIONED_DIRECTORY:-}
END_OF_FEATURES
                then
                    append PARTITIONED_DIRECTORY "$FEATURE_NAME$LF"
                fi
                append "PARTITIONED_${FEATURE_NAME}" "$FILE$LF"
            else
                append CANDIDATES1 "$FILE$LF"
            fi
        done <<END_OF_FILELIST
${CANDIDATES2:-}
END_OF_FILELIST
    done <<END_OF_FEATURE_PARTITION_RULES
${FEATURE_PARTITION_RULES:-}
END_OF_FEATURE_PARTITION_RULES
}

report_feature_partitions()
{
    local FEATURE_NAME
    while read -r FEATURE_NAME && [ -n "$FEATURE_NAME" ]; do
        printf '\n    (%s' "$FEATURE_NAME"
        eval "local FILESUBLIST=\"\$PARTITIONED_${FEATURE_NAME}\""
        local FILE
        while read -r FILE && [ -n "$FILE" ]; do
            printf '\n      %s' "$FILE"
        done <<END_OF_FILELIST
$FILESUBLIST
END_OF_FILELIST
        printf ')'
    done <<END_OF_PARTITIONED_FEATURES
$PARTITIONED_DIRECTORY
END_OF_PARTITIONED_FEATURES
}

prepare_feature_partitions()
{
    FEATURE_PARTITION_RULES=''
    # COMM PROC features:
    add_feature_partition_rule PRJ_ISO_TASKCONTROLLER_CLIENT '.*/processdatachangehandler_c\.[^/]*$\|.*/iprocess_c\.[^/]*$\|.*/elementddi_s\.h$\|.*/proc_c\.h$\|.*/Part10_TaskController_Client/impl/proc\|.*/Part10_TaskController_Client/iprocesscmd\|.*/Part10_TaskController_Client/impl/processcmd\|.*/Part10_TaskController_Client/[^/]*procdata[^/]*base_c\.h$\|.*/Part10_TaskController_Client/.*devproperty\|.*/Part10_TaskController_Client/.*/StdMeasureElements/\|.*/Part10_TaskController_Client/.*/StdSetpointElements'

    # COMM features:
    add_feature_partition_rule PRJ_BASE '.*ibasetypes\.h\.[^/]*$\|.*basecommon_c*\.[^/]*$'
    add_feature_partition_rule PRJ_TRACTOR_GENERAL '.*/Part7_ApplicationLayer/.*tracgeneral_c\.[^/]*$'
    add_feature_partition_rule PRJ_TRACTOR_MOVE '.*/Part7_ApplicationLayer/.*tracmove'
    add_feature_partition_rule PRJ_TRACTOR_PTO '.*/Part7_ApplicationLayer/.*tracpto'
    add_feature_partition_rule PRJ_TRACTOR_LIGHT '.*/Part7_ApplicationLayer/.*traclight'
    add_feature_partition_rule PRJ_TRACTOR_FACILITIES '.*/Part7_ApplicationLayer/.*tracfacilities_c*'
    add_feature_partition_rule PRJ_TRACTOR_AUX '.*/Part7_ApplicationLayer/.*tracaux'
    add_feature_partition_rule PRJ_TRACTOR_GUIDANCE -path '.*/Part7_ApplicationLayer/.*tracguidance'
    add_feature_partition_rule PRJ_TRACTOR_CERTIFICATION '.*/Part7_ApplicationLayer/.*traccert'
    add_feature_partition_rule PRJ_TIME_GPS '.*/Part7_ApplicationLayer/.*timeposgps.*'
    add_feature_partition_rule PRJ_PROPRIETARY_PGN_INTERFACE '.*/Part3_ProprietaryMessages/'
    add_feature_partition_rule PRJ_ISO_FILESERVER_CLIENT '.*/Part13_FileServer_Client/'
    add_feature_partition_rule PRJ_ISO_TERMINAL '.*/Part6_VirtualTerminal_Client/'
    add_feature_partition_rule PRJ_DATASTREAMS '.*/driver/datastreams/volatilememory_c\.[^/]*$'
    add_feature_partition_rule PRJ_MULTIPACKET_STREAM_CHUNK '.*/Part3_DataLink/impl/streamchunk_c\.[^/]*$'
    add_feature_partition_rule PRJ_ISO11783 '.*/Part3_DataLink/i.*multi.*\|.*/Part3_DataLink/impl/sendstream_c\.[^/]*$\|.*/Part3_DataLink/impl/stream_c\.[^/]*$\|.*/Part3_DataLink/istream_c\.[^/]*$\|.*/supplementary_driver/driver/datastreams/streaminput_c\.h\|.*/IsoAgLib/convert\.h'

    # DRIVER features:
    add_feature_partition_rule PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL '.*/hal/generic_utils/can/\|.*/driver/can/'
    add_feature_partition_rule PRJ_EEPROM '.*/driver/eeprom/\|.*/hal/.*/eeprom/\|.*/hal/hal_eeprom\.h$'
    add_feature_partition_rule PRJ_DATASTREAMS '.*/driver/datastreams/\|.*/hal/.*/datastreams/\|.*/hal/hal_datastreams\.h$'
    add_feature_partition_rule PRJ_OUTPUTS '.*/driver/outputs\|.*/hal/.*/outputs/outputs\.h$\|.*/hal/.*/outputs/outputs_target_extensions\.\|*/hal/hal_outputs\.h$'
    add_feature_partition_rule PRJ_INPUTS_DIGITAL '.*digitali_c\.[^/]*$'
    add_feature_partition_rule PRJ_INPUTS_ANALOG '.*analogi\.[^/]*$'
    add_feature_partition_rule PRJ_INPUTS_COUNTER '.*counteri\.[^/]*$'
    add_feature_partition_rule PRJ_INPUTS____ '.*/hal/.*/inputs/inputs\.h$\|.*/hal/.*/inputs/inputs_target_extensions\.\|.*inputbase_c\.[^/]*$\|.*inputs_c\.[^/]*$\|.*/hal/hal_inputs\.h$'
    add_feature_partition_rule PRJ_RS232_OVER_CAN '.*/hal/virtualDrivers/rs232_over_can/'
    add_feature_partition_rule PRJ_RS232 '.*/hal/.*/rs232/\|.*/driver/rs232/'
    add_feature_partition_rule 'MISCELLANEOUS' '.'
}

# this function uses the "find" cmd
# to derive based on the selected features the
# corresponding file list into filelist_$PROJECT_PURE.txt
create_filelist( )
{
    local GENERATE_FILES_ROOT_DIR="$1"
    local SCRIPT_DIR="$2"

    # Create directory to place generated files into...
    mkdir -p "$GENERATE_FILES_ROOT_DIR"
    cd "$GENERATE_FILES_ROOT_DIR"
    mkdir -p $PROJECT
    cd $PROJECT

    {
        local COMM_PROC_FEATURES="$(comm_proc_features 3>&1 1>&9)"
        local COMM_FEATURES="$(comm_features 3>&1 1>&9)"
        TMP_HAL_FEATURES="${TEMPFILE_PREFIX}hal_features"
        local DRIVER_FEATURES="$(driver_and_hal_features 3>&1 4>"$TMP_HAL_FEATURES" 1>&9)"
        local HAL_FEATURES="$(cat "$TMP_HAL_FEATURES")"
    } 9>&1

    local SRC_EXT="\( -name '*.c' -o -name '*.cc' -o -name '*.cpp' \)"
    local HDR_EXT="-name '*.h'"

    FILELIST_LIBRARY_HDR="filelist"'__'"${PROJECT}_hdr.library.txt"
    FILELIST_LIBRARY_PURE="filelist"'__'"${PROJECT}.library.txt"

    FILELIST_APP_HDR="filelist"'__'"${PROJECT}_hdr.app.txt"
    FILELIST_APP_PURE="filelist"'__'"${PROJECT}.app.txt"

    local FILELIST_COMBINED_HDR="filelist"'__'"${PROJECT}_hdr.txt"
    FILELIST_COMBINED_PURE="filelist"'__'"${PROJECT}.txt"

    local FIND_TEMP_PATH="$(printf '%s' \
            "-path '*/scheduler/*'" \
        " -o -path '*/util/*'" \
        " -o -path '*/Part5_NetworkManagement/*'" \
        " -o -path '*/Part12_DiagnosticsServices/*' " \
        " -o -path '*/Part3_DataLink/i*can*' " \
        "${COMM_PROC_FEATURES:+ -o ${COMM_PROC_FEATURES}}" \
        "${COMM_FEATURES:+ -o ${COMM_FEATURES}}" \
        "${DRIVER_FEATURES:+ -o ${DRIVER_FEATURES}}")"
    { local EXCLUDE_PATH_PART1="$(find_part '-and -not' "-path '%s'" "$APP_PATH_EXCLUDE" 3>&1 1>&9)"; } 9>&1
    : ${EXCLUDE_PATH_PART1:="-a -not -path '*/xgpl_src/build/*'"}
    eval "find ../../$ISO_AG_LIB_PATH/library/xgpl_src -follow $SRC_EXT -a \( $FIND_TEMP_PATH \) $EXCLUDE_PATH_PART1 -printf '%h/%f\n' > $FILELIST_LIBRARY_PURE"
    eval "find ../../$ISO_AG_LIB_PATH/library/xgpl_src -follow $HDR_EXT -a \( $FIND_TEMP_PATH \) $EXCLUDE_PATH_PART1 -printf '%h/%f\n' > $FILELIST_LIBRARY_HDR"

    eval "find ../../$HAL_FIND_PATH/$HAL_PREFIX_ISOAGLIB -follow $SRC_EXT -a \( $HAL_FEATURES \) $EXCLUDE_PATH_PART1 -printf '%h/%f\n' >> $FILELIST_LIBRARY_PURE"
    eval "find ../../$HAL_FIND_PATH/$HAL_PREFIX_ISOAGLIB -follow $HDR_EXT -a \( $HAL_FEATURES \) $EXCLUDE_PATH_PART1 -printf '%h/%f\n' >> $FILELIST_LIBRARY_HDR"

    if [ "$HAL_PREFIX_ISOAGLIB" != "$HAL_PREFIX_SUPPLEMENTARY" ]; then
      eval "find ../../$HAL_FIND_PATH/$HAL_PREFIX_SUPPLEMENTARY -follow $SRC_EXT -a \( $HAL_FEATURES \) $EXCLUDE_PATH_PART1 -printf '%h/%f\n' >> $FILELIST_LIBRARY_PURE"
      eval "find ../../$HAL_FIND_PATH/$HAL_PREFIX_SUPPLEMENTARY -follow $HDR_EXT -a \( $HAL_FEATURES \) $EXCLUDE_PATH_PART1 -printf '%h/%f\n' >> $FILELIST_LIBRARY_HDR"
    fi
    
    ##############################
    # find application files
    ##############################
    {
        local APP_SRC_PART="$(find_part -and "-name '%s'" "$APP_SRC_FILE" 3>&1 1>&9)"
        local EXCLUDE_PATH_PART2="$(find_part '-and -not' "-path '%s'" "$APP_PATH_EXCLUDE" 3>&1 1>&9)"
        local EXCLUDE_SRC_PART="$(find_part '-and -not' "-name '%s'" "$APP_SRC_EXCLUDE" 3>&1 1>&9)"
        # remove the joker '*' from the file type spec, as this causes only trouble
        local APP_SEARCH_SRC_CONDITION1="$(printf '%s' "$APP_SEARCH_SRC_CONDITION" | tr -d '*')"
        local APP_SEARCH_SRC_TYPE_PART="$(find_part '' "-name '*%s'" "$APP_SEARCH_SRC_CONDITION1" 3>&1 1>&9)"
        local APP_SEARCH_HDR_TYPE_PART="$(find_part '' "-name '%s'" "$APP_SEARCH_HDR_CONDITION" 3>&1 1>&9)"
    } 9>&1
    
    for EACH_REL_APP_PATH in ${REL_APP_PATH:-}; do
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ -follow $APP_SEARCH_SRC_TYPE_PART $APP_SRC_PART $EXCLUDE_PATH_PART2 $EXCLUDE_SRC_PART -printf '%h/%f\n' >&3"
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ -follow $APP_SEARCH_HDR_TYPE_PART $APP_SRC_PART $EXCLUDE_PATH_PART2 $EXCLUDE_SRC_PART -printf '%h/%f\n' >&4"
    done 3>"$FILELIST_APP_PURE" 4>"$FILELIST_APP_HDR"


    cat $FILELIST_LIBRARY_PURE $FILELIST_APP_PURE > $FILELIST_COMBINED_PURE
    cat $FILELIST_LIBRARY_HDR $FILELIST_APP_HDR > $FILELIST_COMBINED_HDR

    cat $FILELIST_COMBINED_HDR >> $FILELIST_COMBINED_PURE
    rm -f $FILELIST_COMBINED_HDR

    prepare_feature_partitions

    # go back to directory where config file resides
    cd "$GENERATE_FILES_ROOT_DIR"

    return
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

    local FIRST_REL_APP_PATH
    read -r FIRST_REL_APP_PATH DUMMY <<END_OF_PATH
${REL_APP_PATH:-}
END_OF_PATH

    local APP_VERSION_DIR="${FIRST_REL_APP_PATH:+$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH}"
    local VERSION_DIR="${APP_VERSION_DIR:-$GENERATE_FILES_ROOT_DIR/$PROJECT}"
    CONFIG_NAME="$VERSION_DIR/config_${PROJECT}.h"

    # first backup individual settings after line
    # START_INDIVIDUAL_PROJECT_CONFIG
    if [ -f $CONFIG_NAME ] ; then
        grep -A1000 "// START_INDIVIDUAL_PROJECT_CONFIG" < $CONFIG_NAME > $CONFIG_NAME.bak || status_le1
    else
        touch $CONFIG_NAME.bak
    fi
    {
        # write INST counts
        CONFIG_HEADER_FILENAME=$(basename $CONFIG_NAME)
        echo_ "// File: $CONFIG_HEADER_FILENAME" >&3
        echo_ "// IMPORTANT: Never change the first block of this header manually!!!" >&3
        echo_ "//            All manual changes are overwritten by the next call of \"conf2build.sh $CONF_FILE\" " >&3
        echo_ "//            Perform changes direct in the feature and project setup file $CONF_FILE"  >&3
        echo_ "//  ALLOWED ADAPTATION: Move the to be adapted defines from the middle block to the end after" >&3
        echo_ "//                      the line START_INDIVIDUAL_PROJECT_CONFIG and remove the comment indication there."  >&3
        echo_ "//                      All commented out defines in the middle block will be upated on next \"conf2build.sh $CONF_FILE\" call,"  >&3
        echo_ "//                      if the corresponding value in isoaglib_config.h changed" >&3
        echo_e "$ENDLINE" >&3

        echo_ "// These defines are needed for inclusion of the correct HAL modules:" >&3
        echo_ "#define HAL_PATH_ISOAGLIB $HAL_PATH_ISOAGLIB">&3
        echo_ "#define HAL_PATH_ISOAGLIB_SYSTEM $HAL_PATH_ISOAGLIB_SYSTEM">&3
        echo_ "#define HAL_PATH_ISOAGLIB_CAN $HAL_PATH_ISOAGLIB_CAN">&3
        if [ -n "$HAL_PATH_SUPPLEMENTARY_EEPROM" ]; then 
          echo_ "#define HAL_PATH_SUPPLEMENTARY_EEPROM $HAL_PATH_SUPPLEMENTARY_EEPROM">&3
        else
          echo_ "// keep HAL_PATH_SUPPLEMENTARY_EEPROM undefined as this module is not available for the given configuration/target" >&3
        fi
        if [ -n "$HAL_PATH_SUPPLEMENTARY_RS232" ]; then 
          echo_ "#define HAL_PATH_SUPPLEMENTARY_RS232 $HAL_PATH_SUPPLEMENTARY_RS232">&3
        else
          echo_ "// keep HAL_PATH_SUPPLEMENTARY_RS232 undefined as this module is not available for the given configuration/target" >&3
        fi
        if [ -n "$HAL_PATH_SUPPLEMENTARY_OUTPUTS" ]; then
          echo_ "#define HAL_PATH_SUPPLEMENTARY_OUTPUTS $HAL_PATH_SUPPLEMENTARY_OUTPUTS">&3
        else
          echo_ "// keep HAL_PATH_SUPPLEMENTARY_OUTPUTS undefined as this module is not available for the given configuration/target" >&3
        fi
        if [ -n "$HAL_PATH_SUPPLEMENTARY_INPUTS" ]; then
          echo_ "#define HAL_PATH_SUPPLEMENTARY_INPUTS $HAL_PATH_SUPPLEMENTARY_INPUTS">&3
        else
          echo_ "// keep HAL_PATH_SUPPLEMENTARY_INPUTS undefined as this module is not available for the given configuration/target" >&3
        fi
        if [ -n "$HAL_PATH_SUPPLEMENTARY_DATASTREAMS" ]; then
          echo_ "#define HAL_PATH_SUPPLEMENTARY_DATASTREAMS $HAL_PATH_SUPPLEMENTARY_DATASTREAMS">&3
        else
          echo_ "// keep HAL_PATH_SUPPLEMENTARY_DATASTREAMS undefined as this module is not available for the given configuration/target" >&3
        fi
        echo_e "$ENDLINE" >&3

        echo_ "// These defines are set in the $CONF_FILE:" >&3
        for SinglePrjDefine in $PRJ_DEFINES ; do
            SingleDefName="${SinglePrjDefine%%=*}"
            SingleDefRemainder="${SinglePrjDefine#$SingleDefName}"
            SingleDefValue="${SingleDefRemainder#=}"
            echo_ "#ifndef $SingleDefName" >&3
            echo_ "#define $SingleDefName $SingleDefValue" >&3
            echo_ "#endif // $SingleDefName" >&3
        done
        echo_e "$ENDLINE" >&3
    
    
        echo_e "#define CAN_INSTANCE_CNT $CAN_INSTANCE_CNT $ENDLINE" >&3
        echo_e "#define PRT_INSTANCE_CNT $PRT_INSTANCE_CNT $ENDLINE" >&3
        echo_e "#define RS232_CHANNEL_CNT $RS232_CHANNEL_CNT $ENDLINE" >&3
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
    
        if [ "$USE_VT_UNICODE_SUPPORT" -gt 0 ] ; then
            echo_e "#define USE_VT_UNICODE_SUPPORT$ENDLINE" >&3
        fi
    
    
        if [ "$PRJ_BASE" -gt 0 ] ; then
            echo_e "#ifndef USE_BASE $ENDLINE\t#define USE_BASE $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_GENERAL" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_GENERAL $ENDLINE\t#define USE_TRACTOR_GENERAL $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_MOVE" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_MOVE $ENDLINE\t#define USE_TRACTOR_MOVE $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_PTO" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_PTO $ENDLINE\t#define USE_TRACTOR_PTO $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_LIGHT" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_LIGHT $ENDLINE\t#define USE_TRACTOR_LIGHT $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_FACILITIES" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_FACILITIES $ENDLINE\t#define USE_TRACTOR_FACILITIES $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_AUX" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_AUX $ENDLINE\t#define USE_TRACTOR_AUX $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_GUIDANCE" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_GUIDANCE $ENDLINE\t#define USE_TRACTOR_GUIDANCE $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TRACTOR_CERTIFICATION" -gt 0 ]; then
            echo_e "#ifndef USE_TRACTOR_CERTIFICATION $ENDLINE\t#define USE_TRACTOR_CERTIFICATION $ENDLINE#endif" >&3
        fi
        if [ "$PRJ_TIME_GPS" -gt 0 ]; then
            echo_e "#ifndef USE_TIME_GPS $ENDLINE\t#define USE_TIME_GPS $ENDLINE#endif" >&3
        fi
        
        if [ "$PRJ_PROPRIETARY_PGN_INTERFACE" -gt 0 ]; then
            echo_e "#ifndef USE_ISO_PROPRIETARY_PGN $ENDLINE\t#define USE_ISO_PROPRIETARY_PGN $ENDLINE#endif" >&3
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
    
    
		if [ "$PRJ_EEPROM" -gt 0 ] ; then
            echo_e "#define USE_EEPROM_IO" >&3
        fi
    
        if [ "$PRJ_DATASTREAMS" -gt 0 -o $PRJ_ISO_TERMINAL -gt 0 -o $PRJ_TIME_GPS -gt 0 ]; then
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
			if [ "$PRJ_ISO_TASKCONTROLLER_CLIENT" -gt 0 ] ; then
				echo_e "#ifndef USE_ISO_TASKCONTROLLER_CLIENT $ENDLINE  #define USE_ISO_TASKCONTROLLER_CLIENT $ENDLINE#endif" >&3
			fi
            if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ] ; then
                echo_e "#ifndef USE_ISO_FILESERVER_CLIENT $ENDLINE\t#define USE_ISO_FILESERVER_CLIENT $ENDLINE#endif" >&3
            fi
            if [ "$PRJ_MULTIPACKET_STREAM_CHUNK" -gt 0 ] ; then
                echo_e "#ifndef DEF_Stream_IMPL   $ENDLINE\t#define DEF_Stream_IMPL   StreamChunk   $ENDLINE#endif" >&3
                echo_e "#ifndef DEF_Stream_c_IMPL $ENDLINE\t#define DEF_Stream_c_IMPL StreamChunk_c $ENDLINE#endif" >&3
            else
                echo_e "#ifndef DEF_Stream_IMPL   $ENDLINE\t#define DEF_Stream_IMPL   StreamLinear   $ENDLINE#endif" >&3
                echo_e "#ifndef DEF_Stream_c_IMPL $ENDLINE\t#define DEF_Stream_c_IMPL StreamLinear_c $ENDLINE#endif" >&3
            fi
        fi

        if [ "$PRJ_RS232" -gt 0 ]; then
            echo_e "#ifndef USE_RS232$ENDLINE\t#define USE_RS232 1$ENDLINE#endif" >&3
        fi
 
        if [ "$PRJ_OUTPUTS" -gt 0 ]; then
            echo_e "#ifndef USE_OUTPUTS$ENDLINE\t#define USE_OUTPUTS$ENDLINE#endif" >&3
        fi
 
        if [ "$PRJ_INPUTS_DIGITAL" -gt 0 -o "$PRJ_INPUTS_ANALOG" -gt 0 -o "$PRJ_INPUTS_COUNTER" -gt 0 ]; then
            echo_e "#ifndef USE_INPUTS$ENDLINE\t#define USE_INPUTS$ENDLINE#endif" >&3
        fi

        # write overwriteable parts of isoaglib_config.h
        echo_e "$ENDLINE// The following configuration values can be overwritten." >&3
        echo_ "// These settings are initially defined in isoaglib_config.h ." >&3
        echo_ "// These settings are in commented-out, so that you can activate and adapt them by" >&3
        echo_e "// moving them below the line with START_INDIVIDUAL_PROJECT_CONFIG$ENDLINE"  >&3
    } 3>"$CONFIG_NAME"
    TMP_CONFIG1="${TEMPFILE_PREFIX}config1"
    while read -r conf_line; do
        conf_name=$(echo_ $conf_line | sed 's/# *define \(CONFIG_[a-zA-Z0-9_]*\).*/\1/g')
        INDIV_CNT=$(grep -c $conf_name $CONFIG_NAME.bak || status_le1)
        if [ "$INDIV_CNT" -lt 1 ] ; then
            grep -B1 "# *define $conf_line" $ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/isoaglib_config.h >> $CONFIG_NAME
            sed "s|# *define $conf_name|// #define $conf_name|g" $CONFIG_NAME > $TMP_CONFIG1
            #     CMDLINE=$(echo_ "sed -e 's|# *define $conf_name|// #define $conf_name|g' $CONFIG_NAME > $TMP_CONFIG1")
            #     echo_ $CMDLINE | sh
            cp $TMP_CONFIG1 $CONFIG_NAME
            echo_e_n "$ENDLINE" >> $CONFIG_NAME
        fi
    done <<END_OF_CONFIG_SET
$(grep "# *define CONFIG_" <$ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/isoaglib_config.h |
  sed 's/# *define \(CONFIG_[a-zA-Z0-9_]*\).*/\1/g')
END_OF_CONFIG_SET
    {
        echo_e "$ENDLINE// DONT REMOVE THIS AND THE FOLLOWING LINE AS THEY ARE NEEDED TO DETECT YOUR PERSONAL PROJECT ADAPTATIONS!!!" >&3
        FRESH=$(grep -c "// START_INDIVIDUAL_PROJECT_CONFIG" < $CONFIG_NAME.bak || status_le1)
        if [ "$FRESH" -lt 1 ] ; then
            echo_ "// START_INDIVIDUAL_PROJECT_CONFIG" >&3
        fi
        cat $CONFIG_NAME.bak >&3
        rm -f $CONFIG_NAME.bak
    } 3>>"$CONFIG_NAME"
    sed -e 's|^[ \t]*//|//|g' $CONFIG_NAME > $TMP_CONFIG1
    sed -e 's|^[ \t]*/\*|/\*|g' $TMP_CONFIG1 > $CONFIG_NAME

    # go back to directory where config file resides
    cd $1
}

expand_definition()
{
    # Outputting the here document (cat <<) is superior to other
    # means especially when RULE contains quotes:
    eval "cat <<END_OF_STRING
$RULE
END_OF_STRING
" || {
        printf 'ERROR when evaluating rule (%s)\n' "$RULE" >&2
        exit 1
    }
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
    eval "INSERT_${NAME}=\"\$(expand_definition)\"" || {
        printf 'ERROR when trying to set variable %s\n' "INSERT_${NAME}" >&2
        exit 1
    }
    F=expand_report # then expand for report
    eval "REPORT_${NAME}=\"\$(expand_definition)\"" || {
        printf 'ERROR when trying to set variable %s\n' "REPORT_${NAME}" >&2
        exit 1
    }
}

list_source_files()
{
    local FIRST_FORMAT="$1"
    local NEXT_FORMAT="$2" 
    local SOURCE_PATTERN="$3"
    shift 3
    local FORMAT="$FIRST_FORMAT"
    cat "$@" | grep -E "$SOURCE_PATTERN" | 
    while read -r CC_FILE; do
        printf "$FORMAT" "$CC_FILE"
        FORMAT="$NEXT_FORMAT"
    done
}

generate_interface_filelist()
{
    TMP_INTERFACE_FILELIST="${TEMPFILE_PREFIX}interface_filelist"
    TMP_INTERNAL_FILELIST="${TEMPFILE_PREFIX}internal_filelist"
    grep    "/impl/" <"$MakefileFilelistLibraryHdr" > "$TMP_INTERNAL_FILELIST"
    grep    "/hal/"  <"$MakefileFilelistLibraryHdr" >> "$TMP_INTERNAL_FILELIST"

    grep -v "/impl/" <"$MakefileFilelistLibraryHdr" | grep -v ".cpp" > "$TMP_INTERFACE_FILELIST"
    grep -E "driver/*/i*.h" < "$TMP_INTERNAL_FILELIST" >> "$TMP_INTERFACE_FILELIST" || status_le1
    
    # special exception to enable ISO-Request-PGN implementation in app scope
    grep -E "isorequestpgn_c.h" "$TMP_INTERNAL_FILELIST" >> "$TMP_INTERFACE_FILELIST"
    # special exception for singletons.
    # obsolete!
    #grep -E "singleton_container_decl.h" "$TMP_INTERNAL_FILELIST" >> "$TMP_INTERFACE_FILELIST"

    local INTERFACE_FILE
    while read -r INTERFACE_FILE; do
        local BASE_NAME
        sed -e '/#include/!d' \
            -e 's/.*#include[ \t\<\"]*\([^\>\"\t ]*\).*/\1/g' \
            -e 's|.*xgpl_src/IsoAgLib/\(.*\)|\1|g' \
            -e 's|\.\./||g' < "$INTERFACE_FILE" |
        while read -r BASE_NAME; do
          ## Filter out non-ISOAgLib includes (like pthread, ...)
          if [ "$BASE_NAME" != "pthread.h" ]; then
            expr \( "$BASE_NAME" : '.*\.h' \) >/dev/null &&
                ! grep -q -F "/$BASE_NAME" <"$TMP_INTERFACE_FILELIST" &&
                grep -F "$BASE_NAME" <"$TMP_INTERNAL_FILELIST" >>"$TMP_INTERFACE_FILELIST" ||
                status_le1
          fi
        done
    done <"$TMP_INTERFACE_FILELIST"
}


# Create a makefile including another one. Typically the including
# makefile serves as shortcut, because it has a shorter name than the
# included makefile.
shortcut_makefile()
{
    local TARGET="$1"
    local SHORTCUT="$2"

#### By default, the long version is moved to the shortcut version now.
    rm -f "$SHORTCUT"
    mv "$TARGET" "$SHORTCUT"

#### This is the shortcut version (currently with "include Makefile__x__y")
#### which is currently not used anymore. It could be reactivated, but then
#### optional with some cmdline-parameter to enable it.
###########################################################################
#    cat <<END_OF_MAKEFILE >"$SHORTCUT"
#include $TARGET
#END_OF_MAKEFILE
}

omit_or_printf()
{
    [ 1 -lt $# ] || return
    FORMAT="$1"
    shift
    printf -- "$FORMAT" "$@"
}

print_cmake_definitions()
{
    omit_or_printf '\n  -D%s' $(
        echo_ PRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h $PRJ_DEFINES
        if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ]; then
             echo_ SYSTEM_WITH_ENHANCED_CAN_HAL
        fi
        case "$USE_CAN_DRIVER" in
            (msq_server)
                echo_ CAN_DRIVER_MESSAGE_QUEUE
                ;;
            (socket_server|socket_server_hal_simulator)
                echo_ CAN_DRIVER_SOCKET
                ;;
        esac

        # NDEBUG and/or DEBUG will be defined later according to CMake
        # build type, so omit them here:
        sed 's| \?\<N\?DEBUG\>||g' <<EOF
$COMBINED_DEFINES
EOF
    )
}


create_cmake_winlin()
{
    CompletePrjFilelist="$1/$PROJECT/$FILELIST_COMBINED_PURE"

    IsoAglibPrjHeaderFilelist="$1/$PROJECT/$FILELIST_LIBRARY_HDR"
    IsoAglibPrjSourceFilelist="$1/$PROJECT/$FILELIST_LIBRARY_PURE"
    AppPrjHeaderFilelist="$1/$PROJECT/$FILELIST_APP_HDR"
    AppPrjSourceFilelist="$1/$PROJECT/$FILELIST_APP_PURE"    
        
    local RELATIVE_INC_PATHS="$(echo_ ${REL_APP_PATH:-} $PRJ_INCLUDE_PATH)"
    local ALL_INC_PATHS="$(echo_ ${RELATIVE_INC_PATHS:+$(printf -- "$(literal_format "$ISO_AG_LIB_INSIDE")/%s\n" $RELATIVE_INC_PATHS)})"

    local INSERT_CMAKE_PROJECT="$PROJECT"
    local INSERT_CMAKE_DEFINITIONS="$(print_cmake_definitions)"
    local INSERT_CMAKE_INCLUDE_DIRECTORIES_LINUX="$(omit_or_printf '\n  %s' . $ISO_AG_LIB_INSIDE/library $ISO_AG_LIB_INSIDE/library/xgpl_src ${ALL_INC_PATHS:-} ${BIOS_INC:-})"
    local INSERT_CMAKE_INCLUDE_DIRECTORIES_WIN32="$(omit_or_printf '\n  %s' . $ISO_AG_LIB_INSIDE/library $ISO_AG_LIB_INSIDE/library/xgpl_src ${ALL_INC_PATHS:-} ${USE_WIN32_EXTERNAL_INCLUDE_PATH:-} ${BIOS_INC:-})"
    local INSERT_CMAKE_LINK_DIRECTORIES_LINUX="${USE_LINUX_EXTERNAL_LIBRARY_PATH:-}"
    local INSERT_CMAKE_LINK_DIRECTORIES_WIN32="${USE_WIN32_EXTERNAL_LIBRARY_PATH:-}"
    local INSERT_CMAKE_TARGET_LINK_LIBRARIES_LINUX="$(omit_or_printf '\n  %s' "$PROJECT" rt $USE_LINUX_EXTERNAL_LIBRARIES)"
    local INSERT_CMAKE_TARGET_LINK_LIBRARIES_WIN32="$(omit_or_printf '\n  %s' "$PROJECT" odbc32 odbccp32 winmm ws2_32)"
    local INSERT_CMAKE_SOURCE_FILES="$(omit_or_printf '\n  %s' "$(cat filelist__$PROJECT.txt)" )"
    local INSERT_CMAKE_ADD_EXECUTABLE="$(
        omit_or_printf '\n  %s' "$PROJECT" $(
            grep -E '\.cc|\.cpp|\.c|\.h' <"$CompletePrjFilelist" || status_le1))"
    local INSERT_CMAKE_APP_SOURCE_GROUP="$(
        omit_or_printf '\n  %s' $(
            grep -E '\.cc|\.cpp|\.c|\.h' <"$AppPrjSourceFilelist" || status_le1))"
    local INSERT_CMAKE_APP_HEADER_GROUP="$(
        omit_or_printf '\n  %s' $(
            grep -E '\.cc|\.cpp|\.c|\.h' <"$AppPrjHeaderFilelist" || status_le1))"
    local INSERT_CMAKE_ISOAGLIB_SOURCE_GROUP="$(
        omit_or_printf '\n  %s' $(
            grep -E '\.cc|\.cpp|\.c|\.h' <"$IsoAglibPrjSourceFilelist" || status_le1))"
    local INSERT_CMAKE_ISOAGLIB_HEADER_GROUP="$(
        omit_or_printf '\n  %s' $(
            grep -E '\.cc|\.cpp|\.c|\.h' <"$IsoAglibPrjHeaderFilelist" || status_le1))"
    
    : ${CMAKE_SKELETON_FILE:=$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/conf2build_CMakeLists.txt}
    expand_template "$CMAKE_SKELETON_FILE" >"CMakeLists.txt"
}

# for information! (from the windows version)
#    cat $FILELIST_LIBRARY_PURE $FILELIST_APP_PURE > $FILELIST_COMBINED_PURE
#    cat $FILELIST_LIBRARY_HDR $FILELIST_APP_HDR > $FILELIST_COMBINED_HDR
#    cat $FILELIST_COMBINED_HDR >> $FILELIST_COMBINED_PURE
#
#old linux version which did not include the headers:
#LIN local INSERT_CMAKE_ADD_EXECUTABLE="$(
#LIN     omit_or_printf '\n  %s' "$PROJECT" $(
#LIN         cat "$MakefileFilelistLibrary" "$MakefileFilelistApp" | grep -E '\.cc|\.cpp|\.c|\.h' || status_le1))"


create_standard_makefile()
{
    MakefileName="Makefile"
    MakefileNameLong="Makefile__${USE_CAN_DRIVER}__${USE_RS232_DRIVER}"

    : ${MAKEFILE_SKELETON_FILE:=$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/conf2build_MakefileSkeleton.txt}
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

        local RELATIVE_INC_PATHS="$(echo_ ${REL_APP_PATH:-} $PRJ_INCLUDE_PATH)"
        local ALL_INC_PATHS="$(echo_ ${RELATIVE_INC_PATHS:+$(printf -- "$(literal_format "$ISO_AG_LIB_INSIDE")/%s\n" $RELATIVE_INC_PATHS)} $USE_LINUX_EXTERNAL_INCLUDE_PATH)"

        local REPORT_APP_INC="$(echo_ ${ALL_INC_PATHS:+$(printf -- '-I%s\n' $ALL_INC_PATHS)})"
        printf "APP_INC = %s\n" "$REPORT_APP_INC" >&3
        KDEVELOP_INCLUDE_PATH="$ISO_AG_LIB_INSIDE/library;$ISO_AG_LIB_INSIDE/library/xgpl_src;${ALL_INC_PATHS:+$(printf '%s;' $ALL_INC_PATHS)}"
        
        local RULE_LIBPATH="$(map join_space prefix_library_path ${USE_LINUX_EXTERNAL_LIBRARY_PATH:-})"
        define_insert_and_report LIBPATH "$RULE_LIBPATH"
        printf 'LIBPATH = %s\n' "$REPORT_LIBPATH" >&3
        local REPORT_EXTERNAL_LIBS="$USE_LINUX_EXTERNAL_LIBRARIES"
        printf 'EXTERNAL_LIBS = %s\n' "$REPORT_EXTERNAL_LIBS" >&3
        
        echo_e "\n####### Include a version definition file into the Makefile context - when this file exists"  >&3
        printf -- '-include versions.mk\n\n\n' >&3
        
        define_insert_and_report BIOS_INC ''
        define_insert_and_report BIOS_LIB ''
        if [ "$USE_RS232_DRIVER" = "rte" ] ; then
            define_insert_and_report BIOS_LIB '/usr/local/lib/librte_client.a /usr/local/lib/libfevent.a'
            echo_ "BIOS_LIB = $INSERT_BIOS_LIB" >&3
            # the new RTE library places the headers in /usr/local/include --> no special include paths are needed any more
            echo_n "BIOS_INC =" >&3
        fi
        local REPORT_VERSION_DEFINES=''
        local RULE_PROJ_DEFINES="\$(\$F VERSION_DEFINES) -DPRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h${PRJ_DEFINES:+$(printf -- " -D'%s'" $PRJ_DEFINES)}${COMBINED_DEFINES:+$(printf -- " -D'%s'" $COMBINED_DEFINES)}"

        if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ]; then
            append RULE_PROJ_DEFINES ' -DSYSTEM_WITH_ENHANCED_CAN_HAL'
        fi
        case "$USE_CAN_DRIVER" in
            (msq_server)
                append RULE_PROJ_DEFINES ' -DCAN_DRIVER_MESSAGE_QUEUE'
                ;;
            (socket_server|socket_server_hal_simulator)
                append RULE_PROJ_DEFINES ' -DCAN_DRIVER_SOCKET'
                ;;
        esac

        define_insert_and_report PROJ_DEFINES "$RULE_PROJ_DEFINES"
        printf 'PROJ_DEFINES = %s\n' "$INSERT_PROJ_DEFINES" >&3

        local RULE_COMPILER_BINARY_PRE=$(
                [ -n "$PRJ_COMPILER_BINARY_PRE" ] && printf '%s' "$PRJ_COMPILER_BINARY_PRE" && return
                case "$PRJ_DEFINES" in
                    (*SYSTEM_A1*)
                        printf '%s' '/opt/hardhat/devkit/arm/xscale_le/bin/xscale_le-'
                        ;;
                    (*SYSTEM_MCC*)
                        printf '%s' '/opt/eldk/usr/bin/ppc_6xx-'
                        ;;
                    (*)
                        ;;
                esac;)
        define_insert_and_report COMPILER_BINARY_PRE "$RULE_COMPILER_BINARY_PRE"
        printf "\n####### Definition of compiler binary prefix corresponding to selected target\n" >&3
        printf 'COMPILER_BINARY_PRE = %s\n' "$INSERT_COMPILER_BINARY_PRE" >&3
        
        echo_e "\n\nfirst: all\n" >&3
        echo_ "####### Files" >&3
        printf 'SOURCES_LIBRARY =' >&3
        list_source_files ' %s' ' \\\n\t\t%s' '\.cc|\.cpp|\.c' "$MakefileFilelistLibrary" >&3
        printf '\n\nSOURCES_APP =' >&3
        list_source_files ' %s' ' \\\n\t\t%s' '\.cc|\.cpp' "$MakefileFilelistApp" >&3
        printf '\n\nSOURCES_APP_C =' >&3
        list_source_files ' %s' ' \\\n\t\t%s' '\.c |\.c$' "$MakefileFilelistApp" >&3
        printf '\n\nHEADERS_APP =' >&3
        list_source_files ' %s' ' \\\n\t\t%s' '\.h|\.hpp|\.hh' "$MakefileFilelistAppHdr" >&3
        printf '\n\n' >&3

        local REPORT_ISOAGLIB_PATH="$ISO_AG_LIB_INSIDE"
        define_insert_and_report EXTRA_CFLAGS '-Wextra -Winit-self -Wmissing-include-dirs'
        define_insert_and_report CFLAGS "${RULE_CFLAGS:--pipe -O -Wall -g \$(\$F EXTRA_CFLAGS) -fno-builtin -fno-exceptions -Wshadow -Wcast-qual -Wcast-align -Wpointer-arith \$(\$F PROJ_DEFINES)}"
        define_insert_and_report CXXFLAGS "${RULE_CXXFLAGS:--pipe -O -Wall -g \$(\$F EXTRA_CFLAGS) -fno-builtin -fno-exceptions -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith \$(\$F PROJ_DEFINES)}"
        define_insert_and_report INCPATH '-I. -I$($F ISOAGLIB_PATH)/library -I$($F ISOAGLIB_PATH)/library/xgpl_src $($F APP_INC) $($F BIOS_INC)'
        define_insert_and_report CPP_PARAMETERS '$($F CXXFLAGS) $($F INCPATH)'
        define_insert_and_report C_PARAMETERS '$($F CFLAGS) $($F INCPATH)'
        local RULE_LFLAGS=$(
            case "$USE_CAN_DRIVER" in
                (msq_server|socket_server|socket_server_hal_simulator)
                    printf -- '-pthread'
                    ;;
            esac;)' $($F LIBPATH)'
        define_insert_and_report LFLAGS "$RULE_LFLAGS"
        define_insert_and_report SUBLIBS '-lrt'
        define_insert_and_report LIBS '$($F BIOS_LIB) $($F SUBLIBS) $($F EXTERNAL_LIBS)'
        define_insert_and_report LINKER_PARAMETERS_1 '$($F LFLAGS)'
        define_insert_and_report LINKER_PARAMETERS_2 '$($F LIBS)'

        printf '(Linux_specific_settings\n' >&5
        printf '  (Miscellaneous_parameters\n' >&5
        printf '    (Compiler_binary_prefix %s))\n' "$REPORT_COMPILER_BINARY_PRE" >&5
        printf '  (Compiler_parameters %s)\n' "$REPORT_CPP_PARAMETERS" >&5
        printf '  (Linker_parameters %s %s)\n' "$REPORT_LINKER_PARAMETERS_1" "$REPORT_LINKER_PARAMETERS_2" >&5

        partition_features <<EOF
$(cat "$MakefileFilelistLibrary" "$MakefileFilelistApp" |
  grep -E '\.cc|\.cpp|\.c' || status_le1)
EOF
        printf '  (Modules' >&5
        report_feature_partitions >&5
        printf '))\n' >&5

        ##### Library install header file gathering BEGIN
        
        generate_interface_filelist
        printf 'INSTALL_FILES_LIBRARY =' >&3
        list_source_files ' %s' ' \\\n\t\t%s' '.' "$TMP_INTERFACE_FILELIST" >&3
        printf '\n\n' >&3
        
        ##### Library install header file gathering END
        expand_template "$MAKEFILE_SKELETON_FILE" >&3
        echo_ >&3
    } 3>"$MakefileNameLong"

    TMP_MAKEFILE="${TEMPFILE_PREFIX}makefile"

    rm -f $TMP_MAKEFILE

    # replace the install rules for the app config file
    sed -e "s#_PROJECT_CONFIG_REPLACE_#$CONFIG_NAME#g"  $MakefileNameLong > $TMP_MAKEFILE
    cp "$TMP_MAKEFILE" "$MakefileNameLong"

    # replace any path items like "Bla/foo/../Blu" --> "Bla/Blu"
    while [ $(grep -c -e '/[0-9a-zA-Z_+\-]\+/\.\./' $MakefileNameLong) -gt 0 ] ; do
        sed -e 's|/[0-9a-zA-Z_+\-]\+/\.\./|/|g' $MakefileNameLong > $TMP_MAKEFILE
        cp $TMP_MAKEFILE $MakefileNameLong
    done

    shortcut_makefile "$MakefileNameLong" "Makefile"

    # In addition generate a CMakeLists.txt file:
	create_cmake_winlin "$1"
}

create_CMake()
{
    DEV_PRJ_DIR="$1/$PROJECT"
    mkdir -p $DEV_PRJ_DIR
    cd "$DEV_PRJ_DIR"

	create_cmake_winlin "$1"
}


create_pure_application_makefile()
{
    # now create pure application makefile which is based upon an installed library
    MakefileName="MakefileApp"
    MakefileNameLong="MakefileApp__${USE_CAN_DRIVER}__${USE_RS232_DRIVER}"

    : ${MAKEFILE_APP_SKELETON_FILE:=$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/conf2build_MakefileAppSkeleton.txt}

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
        for EACH_REL_APP_PATH in ${REL_APP_PATH:-}; do
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
        printf -- '-include versions.mk\n\n\n' >&3
        
        echo_e_n "PROJ_DEFINES = \$(VERSION_DEFINES) -DPRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h" >&3
        for SinglePrjDefine in $PRJ_DEFINES ; do
            echo_n " -D$SinglePrjDefine" >&3
        done
        if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
            echo_n " -DSYSTEM_WITH_ENHANCED_CAN_HAL" >&3
        fi
        
        echo_e "\n\n####### Definition of compiler binary prefix corresponding to selected target" >&3
        if [ -n "$PRJ_COMPILER_BINARY_PRE" ] ; then
            echo_ "COMPILER_BINARY_PRE = \"$PRJ_COMPILER_BINARY_PRE\"" >&3
        else
            case $PRJ_DEFINES in
                (*SYSTEM_A1*)
                    echo_ "COMPILER_BINARY_PRE = /opt/hardhat/devkit/arm/xscale_le/bin/xscale_le-" >&3
                    echo_ "SYSTEM_A1"
                    ;;
                (*SYSTEM_MCC*)
                    echo_ "COMPILER_BINARY_PRE = /opt/eldk/usr/bin/ppc_6xx-" >&3
                    echo_ "SYSTEM_MCC"
                    ;;
                (*)
                    echo_ "COMPILER_BINARY_PRE = " >&3
                    ;;
            esac
        fi
        
        echo_e "\n\nfirst: all\n" >&3
        echo_ "####### Files" >&3
        echo_n "SOURCES_APP = " >&3
        FIRST_LOOP="YES"
        while read -r CcFile; do
            if [ $FIRST_LOOP != "YES" ] ; then
                echo_e_n '\\' >&3
                echo_e_n "\n\t\t" >&3
            else
                FIRST_LOOP="NO"
            fi
            echo_e_n "$CcFile  " >&3
        done <<END_OF_MODULE_LINES
$(grep -E "\.cc|\.cpp|\.c" < "$MakefileFilelistApp")
END_OF_MODULE_LINES
        echo_e "\n" >&3
        
        cat $MAKEFILE_APP_SKELETON_FILE >&3
    } 3>"$MakefileNameLong"

    # add can_server creation to target "all"
    if [ $USE_CAN_DRIVER = "msq_server" ] ; then
        cp $MakefileNameLong $TMP_MAKEFILE
        sed -e 's#LFLAGS   =#LFLAGS   = -pthread#g' $TMP_MAKEFILE > $MakefileNameLong
    fi

    # replace any path items like "Bla/foo/../Blu" --> "Bla/Blu"
    while [ $(grep -c -e '/[0-9a-zA-Z_+\-]\+/\.\./' $MakefileNameLong) -gt 0 ] ; do
        sed -e 's|/[0-9a-zA-Z_+\-]\+/\.\./|/|g' $MakefileNameLong > $TMP_MAKEFILE
        cp $TMP_MAKEFILE $MakefileNameLong
    done

    shortcut_makefile "$MakefileNameLong" "MakefileApp"
}

create_makefile()
{
    # go to project dir - below config dir
    DEV_PRJ_DIR="$1/$PROJECT"
    cd $DEV_PRJ_DIR
    MakefileFilelistLibrary="$1/$PROJECT/$FILELIST_LIBRARY_PURE"
    MakefileFilelistLibraryHdr="$1/$PROJECT/$FILELIST_LIBRARY_HDR"

    MakefileFilelistApp="$1/$PROJECT/$FILELIST_APP_PURE"
    MakefileFilelistAppHdr="$1/$PROJECT/$FILELIST_APP_HDR"

    create_standard_makefile $1
    create_pure_application_makefile

    cd $1

    return
}

mangle_path1()
{
    if [ 0 -lt $# ]; then
        printf '%s' "$*" | mangle_path1
    else
        sed -e "s|\\\\|${PATH_SEPARATOR1}|g;s|/|${PATH_SEPARATOR1}|g"
    fi
}

demangle_path1()
{
    if [ 0 -lt $# ]; then
        printf '%s' "$*" | demangle_path1
    else
        sed -e "s|${PATH_SEPARATOR1}|\\\\|g"
    fi
}

win_paths_from_unix_paths()
{
    if [ 0 -lt $# ]; then
        printf '%s' "$*" | win_paths_from_unix_paths
    else
        sed -e 's|/|\\|g'
    fi
}

unix_lines_to_windows_lines()
{
    # New versions of SED understand \r directly, but older ones (e.g.
    # the current one of MSYS) don't. With printf it's OK for both:
    sed -e "$(printf 's|\r||g;s|$|\r|')"
}

# Expand the template file. Its path has to be passed as the single
# argument.
# Attention to the special characters \, $ and ` in the template file.
# Preceed them with \ to let them appear literally, i.e. write \\, \$
# and \`.
expand_template()
{
    TEMPLATE_FILE="$1"
    eval "cat <<END_OF_TEMPLATE
$(cat "$TEMPLATE_FILE")
END_OF_TEMPLATE
" || {
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
            (esx)
                ;;
            (pc*)
                ;;
            (*)
                USE_EMBED_LIB_DIRECTORY="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
                USE_EMBED_HEADER_DIRECTORY="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
                ;;
        esac
    fi

    # if no setting in the config file -> derive based on target
    : ${USE_EMBED_LIBS:=$(
            case "$USE_TARGET_SYSTEM" in
                (c2c) printf 'c2c10l.lib' ;;
                (Dj1) printf 'djbiosmvt.lib' ;;
                (esx) printf 'C756/Xos20l.lib Module/Xma20l.lib' ;;
                (esxu) printf 'Mos10l.lib' ;;
            esac;)}
    # if no setting in the config file -> derive based on target
    : ${USE_EMBED_BIOS_SRC:=$(
            case "$USE_TARGET_SYSTEM" in
                (c2c) printf 'c2c10cs.asm c2c10err.c  c2c10err.h c2c10osy.h' ;;
                (Dj1) printf 'DjBiosMVT.h' ;;
                (esx) printf 'Xos20go.asm Xos20err.c xos20esx.h XOS20EEC.H XOS20EEC.OBJ' ;;
                (esxu) printf 'MOS10ERR.C  MOS10ERR.H  MOS10GO.ASM MOS10OSY.H' ;;
            esac;)}

    # if no setting in the config file -> derive based on target
    : ${USE_EMBED_ILO:=$(
            case "$USE_TARGET_SYSTEM" in
                (c2c) printf 'c2c10osy.ilo' ;;
                (Dj1) printf 'MiniVT.ilo' ;;
                (esx) printf 'Xos20lcs.ilo' ;;
                (esxu) printf 'MOS10L.ILO' ;;
            esac;)}

    path_for_ede()
    {
        echo_ "$1/$2" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g'
    }

    isoaglib_path_for_ede()
    {
        path_for_ede "$ISO_AG_LIB_INSIDE" "$1"
    }

    embedlib_path_for_ede()
    {
        path_for_ede "$USE_EMBED_LIB_DIRECTORY" "$1"
    }

    USE_APP_PATH="$(map join_semicolon isoaglib_path_for_ede ${REL_APP_PATH:-})"
    ##### debug: echo_ "USE_APP_PATH: $USE_APP_PATH"

    USE_EMBED_HEADER_DIRECTORY="$(isoaglib_path_for_ede "$USE_EMBED_HEADER_DIRECTORY")"
    USE_EMBED_LIB_DIRECTORY="$(isoaglib_path_for_ede "$USE_EMBED_LIB_DIRECTORY")"
    USE_EMBED_ILO="$(embedlib_path_for_ede "$USE_EMBED_ILO")"
    USE_DEFINES="$(join_comma __TSW_CPP_756__ "PRJ_USE_AUTOGEN_CONFIG=$CONFIG_HDR_NAME" $PRJ_DEFINES $COMBINED_DEFINES)"

    libline_for_ede() { printf -- ' -Wo %s' "$@"; }
    USE_TARGET_LIB_LINE="$(map libline_for_ede embedlib_path_for_ede $USE_EMBED_LIBS)"

    # avoid UNIX style directory seperator "/" as it can disturb Tasking during the link process ( during compile, everything runs fine with UNIX style - WMK seems to have problems with it durign link and hex gen )
    ISO_AG_LIB_PATH_WIN=$(win_paths_from_unix_paths "$ISO_AG_LIB_INSIDE")
    USE_EMBED_LIB_DIRECTORY=$(win_paths_from_unix_paths "$USE_EMBED_LIB_DIRECTORY")
    USE_EMBED_HEADER_DIRECTORY=$(win_paths_from_unix_paths "$USE_EMBED_HEADER_DIRECTORY")
    USE_TARGET_LIB_LINE=$(win_paths_from_unix_paths "$USE_TARGET_LIB_LINE")
    USE_EMBED_ILO=$(win_paths_from_unix_paths "$USE_EMBED_ILO")
    USE_APP_PATH=$(win_paths_from_unix_paths "$USE_APP_PATH")
    DEV_PRJ_DIR_WIN=$(win_paths_from_unix_paths "$DEV_PRJ_DIR")
    USE_DEFINES=$(win_paths_from_unix_paths "$USE_DEFINES")
    USE_EMBED_COMPILER_DIR=$(win_paths_from_unix_paths "$USE_EMBED_COMPILER_DIR")
 
    # remove some file lists, which are not used
    rm -f "$1/$PROJECT/$FILELIST_LIBRARY_PURE" "$1/$PROJECT/$FILELIST_APP_PURE"

    local INSERT_PROJECT="$PROJECT"
    local INSERT_TARGET_LIB_DIRECTORY="$USE_EMBED_LIB_DIRECTORY"
    local INSERT_ISO_AG_LIB_PATH="$ISO_AG_LIB_PATH_WIN"
    local INSERT_TARGET_HEADER_DIRECTORY="$USE_EMBED_HEADER_DIRECTORY"
    local INSERT_TARGET_LIB="$USE_TARGET_LIB_LINE"
    local INSERT_TARGET_ILO="$USE_EMBED_ILO"
    local INSERT_APP_PATH="$USE_APP_PATH"
    local INSERT_PRJ_PATH="$DEV_PRJ_DIR_WIN"
    local INSERT_DEFINES="$USE_DEFINES"
    local INSERT_EMBED_COMPILER_DIR="$USE_EMBED_COMPILER_DIR"
    # now set the target CPU
    local INSERT_TARGET_CPU=$(
        case "$USE_TARGET_SYSTEM" in
            (Dj1) printf 'CpuC167CR' ;;
            (esxu) printf 'CpuF269' ;;
            (*) printf 'CpuC167CS' ;;
        esac;)

    with_ede_includes()
    {
        "$1" 'C:\C166\include.cpp' "${INSERT_EMBED_COMPILER_DIR}\\include.cpp" "${INSERT_APP_PATH}" "${INSERT_ISO_AG_LIB_PATH}/library" "${INSERT_ISO_AG_LIB_PATH}/library/xgpl_src" "${INSERT_TARGET_HEADER_DIRECTORY}"
    }

    with_ede_libraries()
    {
        "$1" 'C:\C166\lib' "${INSERT_EMBED_COMPILER_DIR}\\lib" "${INSERT_TARGET_LIB_DIRECTORY}"
    }

    local INSERT_INCLUDES="$(with_ede_includes join_semicolon)"
    local INSERT_LIBRARIES="$(with_ede_libraries join_semicolon)"
    {
        # Build Tasking Project File by: a) first stub part; b) file list c) second stub part
        expand_template $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/conf2build_EDE.part1.pjt >&3

        TMP_EDE="${TEMPFILE_PREFIX}ede"
        cp $EdePrjFilelist $TMP_EDE
        sed -e 's|/|\\|g' $TMP_EDE |
        tee $EdePrjFilelist >&3

        # insert specific BIOS/OS sources
        local FORMAT="$(literal_format "$USE_EMBED_LIB_DIRECTORY")/%s\n"
        printf -- "$FORMAT" $USE_EMBED_BIOS_SRC >&3
        expand_template $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/conf2build_EDE.part2.pjt >&3
    } 3>"$DEV_PRJ_DIR/$PROJECT_FILE_NAME"
    cd $DEV_PRJ_DIR

    printf '(Tasking_specific_settings\n' >&5
    printf '  (Miscellaneous_parameters\n' >&5
    printf '    (Target_cpu %s)\n' "$INSERT_TARGET_CPU" >&5
    printf '    (Compiler_directory %s)\n' "$INSERT_EMBED_COMPILER_DIR" >&5
    printf '    (Project_path %s)\n' "$INSERT_PRJ_PATH" >&5
    printf '    (Target_ILO %s))\n' "$INSERT_TARGET_ILO" >&5
    printf '  (Compiler_parameters\n' >&5
    printf '    (Includes' >&5
    report_ede_fileparts()
    {
        printf '\n      %s' "$@"
    }
    with_ede_includes report_ede_fileparts >&5
    printf ')\n    (Defines %s))\n' "$INSERT_DEFINES" >&5
    printf '  (Linker_parameters\n' >&5
    printf '    (Libraries ' >&5
    with_ede_libraries report_ede_fileparts >&5
    printf ')\n    (Target_libraries ' >&5
    map report_ede_fileparts embedlib_path_for_ede $USE_EMBED_LIBS >&5
    printf '))\n  (Sources' >&5
    while read -r FILE; do
        printf '\n    %s' "$FILE"
    done <$EdePrjFilelist >&5
    FORMAT="\n    $(literal_format "$USE_EMBED_LIB_DIRECTORY")/%s"
    printf -- "$FORMAT" $USE_EMBED_BIOS_SRC >&5
    printf '))\n' >&5

    #echo_ "Converted UNIX to Windows Linebreak in $PROJECT_FILE_NAME"
    unix_lines_to_windows_lines <<EOF >"$PROJECT_FILE_NAME"
$(cat "$PROJECT_FILE_NAME")
EOF
}

create_CcsPrj()
{
    cd "$1/$PROJECT"
    CCS_PROJECT_NAME="$PROJECT"
    CCS_PROJECT_DIR="$1/$PROJECT"
    CCS_PROJECT_FILE_NAME="$PROJECT"'_'"$USE_TARGET_SYSTEM.pjt"
    CCS_PROJECT_FILE_LIST="$1/$PROJECT/$FILELIST_COMBINED_PURE"
    CCS_CONFIG_HDR_NAME="config_""$PROJECT.h"
    CCS_PROJECT_TEMPLATE=$CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/conf2build_CCSSkeleton.pjt
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
    if [ $PRJ_OUTPUTS -gt 0 -o $PRJ_INPUTS_DIGITAL -gt 0 -o $PRJ_INPUTS_ANALOG -gt 0 -o $PRJ_INPUTS_COUNTER -gt 0 ]; then
        echo_
        echo_ "Misconfigured config file: P1MC has no inputs/outputs capabilities!"
        exit 0
    fi

    if [ $RS232_INSTANCE_CNT -gt 0 -a $PRJ_RS232_OVER_CAN -eq 0 ]; then
        echo_
        echo_ "Misconfigured config file: P1MC has no native rs232. Please enable PRJ_RS232_OVER_CAN or disable RS232_INSTANCE_CNT"
        exit 0
    fi

    for EACH_REL_APP_PATH in ${REL_APP_PATH:-}; do
        local PART="$(echo_ "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|/[0-9a-zA-Z_+\-]+\\\.\.||g')"
        if [ -z "${USE_APP_PATH:-}" ]; then
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
    CCS_SOURCE_FILE_LIST="$(
        while read -r EACH_SOURCE_FILE; do
            printf 'Source="%s"\n' "$EACH_SOURCE_FILE"
        done <<END_OF_FILELIST
$(grep '.*cpp$' <$1/$PROJECT/$FILELIST_COMBINED_PURE)
END_OF_FILELIST
        )"
    local INSERT_PROJECT_DIR="$CCS_PROJECT_DIR"
    local INSERT_ISOAGLIB_DIR="$ISO_AG_LIB_INSIDE"
    local INSERT_SOURCE_FILE_LIST="$CCS_SOURCE_FILE_LIST"
    local INSERT_HEADER_SEARCH_PATH="$CCS_INCLUDE_PATH"
    local INSERT_CONFIG_HEADER_NAME="$CCS_CONFIG_HDR_NAME"
    local INSERT_PROJECT_DEFINES="$CCS_PROJECT_DEFINES"
    local INSERT_PROJECT_NAME="$CCS_PROJECT_NAME"
    local INSERT_LIB_INSTALL_DIR="$CCS_LIB_INSTALL_LIB_DIR"
    expand_template "$CCS_PROJECT_TEMPLATE" > "$CCS_PROJECT_FILE_NAME_PATH"

    # create output
    mkdir -p $CCS_LIB_INSTALL_DIR
    mkdir -p $CCS_LIB_INSTALL_HEADER_DIR
    mkdir -p $CCS_LIB_INSTALL_LIB_DIR

    for FIRST_REL_APP_PATH in ${REL_APP_PATH:-}; do
        mv $CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/config_$PROJECT.h $CCS_LIB_INSTALL_HEADER_DIR
        break;
    done

    while read -r EACH_INSTALL_HEADER; do
        install -D $EACH_INSTALL_HEADER $CCS_LIB_INSTALL_HEADER_DIR/$(echo_ $EACH_INSTALL_HEADER | sed -e 's|.*xgpl_src/||')
    done <<END_OF_HEADER_LIST
$(grep -v ".cpp" < "$1/$PROJECT/$FILELIST_LIBRARY_HDR")
END_OF_HEADER_LIST
}

# print source paths in DSP format to FD3 and user information to FD1.
format_sources_for_dsp()
{
    for i in "$@"; do
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

create_library_makefile()
{
    local DEV_PRJ_DIR="$1/$PROJECT"
    cd "$DEV_PRJ_DIR"
    MakefileFilelistLibrary="$1/$PROJECT/$FILELIST_LIBRARY_PURE"
    MakefileFilelistLibraryHdr="$1/$PROJECT/$FILELIST_LIBRARY_HDR"
    local MAKEFILE_NAME="Makefile"
    local MAKEFILE_LONG_NAME="Makefile__${USE_CAN_DRIVER}__${USE_RS232_DRIVER}"

    : ${MAKEFILE_SKELETON_FILE:=$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/conf2build_MakefileLibSkeleton.txt}

    local INSERT_APPLICATION_NAME="$PROJECT"
    local REPORT_ISOAGLIB_PATH='dummy'
    local INSERT_ISOAGLIB_PATH="$ISO_AG_LIB_INSIDE"
    local INSERT_ISOAGLIB_INSTALL_PATH="$ISOAGLIB_INSTALL_PATH"
    local RELATIVE_INC_PATHS="$(echo_ ${REL_APP_PATH:-} $PRJ_INCLUDE_PATH)"
    local ALL_INC_PATHS="$(echo_ ${RELATIVE_INC_PATHS:+$(printf -- "$(literal_format "$ISO_AG_LIB_INSIDE")/%s\n" $RELATIVE_INC_PATHS)} $USE_LINUX_EXTERNAL_INCLUDE_PATH)"
    local INSERT_APP_INCS="$(echo_ ${ALL_INC_PATHS:+$(printf -- '-I%s\n' $ALL_INC_PATHS)})"
    local REPORT_LIBPATH='dummy'
    local INSERT_LIBPATH="${USE_LINUX_EXTERNAL_LIBRARY_PATH:+-L $(printf '%s -L' $USE_LINUX_EXTERNAL_LIBRARY_PATH)}"
    local INSERT_EXTERNAL_LIBS="$USE_LINUX_EXTERNAL_LIBRARIES"
    local REPORT_PROJ_DEFINES='dummy'
    local INSERT_PROJ_DEFINES="$(
        printf -- '$(VERSION_DEFINES)'
        printf -- ' -DPRJ_USE_AUTOGEN_CONFIG=config_%s.h' "$PROJECT"
        if [ -n "${PRJ_DEFINES:-}" ]; then
            printf -- ' -D%s' "$PRJ_DEFINES"
        fi

        if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ]; then
            printf -- ' -DSYSTEM_WITH_ENHANCED_CAN_HAL'
        fi
        
        case "$USE_CAN_DRIVER" in
            (msq_server)
                printf -- ' -DCAN_DRIVER_MESSAGE_QUEUE'
                ;;
            (socket_server|socket_server_hal_simulator)
                printf -- ' -DCAN_DRIVER_SOCKET'
                ;;
        esac;)"
    local INSERT_COMPILER_BINARY_PRE=$(
            [ -n "$PRJ_COMPILER_BINARY_PRE" ] && printf '%s' "$PRJ_COMPILER_BINARY_PRE" && return
            case "$PRJ_DEFINES" in
                (*SYSTEM_A1*)
                    printf '%s' '/opt/hardhat/devkit/arm/xscale_le/bin/xscale_le-'
                    ;;
                (*SYSTEM_MCC*)
                    printf '%s' '/opt/eldk/usr/bin/ppc_6xx-'
                    ;;
                (*)
                    ;;
            esac;)
    local INSERT_SOURCES_LIBRARY="$(
        list_source_files '%s' ' \\\n\t%s' '\.cc|\.cpp|\.c' "$MakefileFilelistLibrary")"
    generate_interface_filelist
    local INSERT_INSTALL_FILES_LIBRARY="$(
        list_source_files '%s' ' \\\n\t%s' '.' "$TMP_INTERFACE_FILELIST")"
    define_insert_and_report SUBLIBS '-lrt'
    define_insert_and_report EXTRA_CFLAGS '-Wextra -Winit-self -Wmissing-include-dirs'
    define_insert_and_report CXXFLAGS "${RULE_CXXFLAGS:--pipe -O -Wall -g \$(\$F EXTRA_CFLAGS) -fno-builtin -fno-exceptions -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith \$(\$F PROJ_DEFINES)}"
    define_insert_and_report BIOS_INC ''
    local REPORT_APP_INC='dummy'
    define_insert_and_report INCPATH '-I. -I$($F ISOAGLIB_PATH)/library -I$($F ISOAGLIB_PATH)/library/xgpl_src $($F APP_INC) $($F BIOS_INC)'
    local RULE_LFLAGS=$(
        case "$USE_CAN_DRIVER" in
            (msq_server|socket_server|socket_server_hal_simulator)
                printf -- '-pthread'
                ;;
        esac;)' $($F LIBPATH)'
    define_insert_and_report LFLAGS "$RULE_LFLAGS"
    define_insert_and_report BIOS_LIB ''
    if [ "$USE_RS232_DRIVER" = "rte" ] ; then
        define_insert_and_report BIOS_LIB '/usr/local/lib/librte_client.a /usr/local/lib/libfevent.a'
    fi
    local REPORT_EXTERNAL_LIBS='dummy'
    local INSERT_EXTERNAL_LIBS="$USE_LINUX_EXTERNAL_LIBRARIES"
    define_insert_and_report LIBS '$($F BIOS_LIB) $($F SUBLIBS) $($F EXTERNAL_LIBS)'
    local INSERT_PROJECT_CONFIG="$CONFIG_NAME"

    expand_template "$MAKEFILE_SKELETON_FILE" |
    sed -e 's|/[0-9a-zA-Z_+\-]\+/\.\./|/|g' >"$MAKEFILE_LONG_NAME"
    shortcut_makefile "$MAKEFILE_LONG_NAME" "$MAKEFILE_NAME"
    cd "$1"
}


create_buildfiles()
{
    local CONF_DIR="$1"
    local SCRIPT_DIR="$2"
    # verify and correct the variables
    check_set_correct_variables "$CONF_DIR"
    GENERATE_FILES_ROOT_DIR=$(echo_ "$GENERATE_FILES_ROOT_DIR" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's/\\[0-9a-zA-Z_+\-]+\\\.\.//g')
    # echo_ "Create project for $USE_TARGET_SYSTEM in $GENERATE_FILES_ROOT_DIR"


    echo_ "ISOAgLib's Project-Generator (conf2build) running..."
    echo_


    # now call the function create_filelist() which build
    # the file list based on the varibles defined above
    create_filelist "$GENERATE_FILES_ROOT_DIR" "$SCRIPT_DIR"


    # call function to create project specific config file
    create_autogen_project_config $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
    case "$USE_TARGET_SYSTEM" in
        (pc_linux)
           # call function to create the Makefile in the project dir
            create_makefile $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        (ees)
            create_makefile $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        # check if a win32 project file whould be created
        (pc_win32)
            create_CMake $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        (p1mc)
            create_CcsPrj $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        (src9)
            create_library_makefile $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
# AMS5 generation needs to be taken out into the proprietary HAL's project generation, which is TBD
#        (ams5)
#            create_ams5_workspace $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
#            ;;
        (esx | esxu | c2c | Dj1)
            create_EdePrj $GENERATE_FILES_ROOT_DIR "$SCRIPT_DIR"
            ;;
        (*)
            ### PROPRIETARY HAL - to come...
            ;;
    esac

    # cleanup temporary files
    rm -f $GENERATE_FILES_ROOT_DIR/$PROJECT/directories__*.txt
    find $GENERATE_FILES_ROOT_DIR/$PROJECT -name "filelist__*.[alu]*.txt" -exec rm {} \;
    find $GENERATE_FILES_ROOT_DIR/$PROJECT -name "filelist__*-doc.txt" -exec rm {} \;
    #$GENERATE_FILES_ROOT_DIR/$PROJECT/filelist__*.txt

}





# Usage: usage
# Print the usage.
usage ()
{
    cat <<EOF
Usage: conf2build.sh [OPTIONS] project_config_file
Creates Filelist, Projectfile/Makefile and Configuration Settings for an IsoAgLib project.

-h, --help                        print this message and exit.
--report                          Print generation report.
--report=REPORT_FILE              print generation report to REPORT_FILE.
--IsoAgLib-root=DIR               use the given root directory instead of the entry in the selected configuration file.
--target-system=TARGET            produce the project definition files for the selected TARGET instead of the
                                  target which is specified in the configuration file
                                  --> ("pc_linux"|"pc_win32"|"esx"|"esxu"|"c2c"|"Dj1")
--pc-can-driver=CAN_DRIVER        produce the project definition files for the selected CAN_DRIVER if the project shall run on PC
                                  --> ("simulating"|"sys"|"msq_server"|"socket_server"|"socket_server_hal_simulator")
--pc-rs232-driver=RS232_DRIVER    produce the project definition files for the selected RS232_DRIVER if the project shall run on PC
                                  --> ("simulating"|"sys"|"rte"|"hal_simulator").
--little-endian-cpu               select configuration for LITTLE ENDIAN CPU type
--big-endian-cpu                  select configuration for BIG ENDIAN CPU type
--with-makefile-skeleton=filename define project specific MakefileSkeleton text file which is used as base for
                                  Makefiles (default: MakefileSkeleton.txt in the same directory as this script)
--debugdefgroup=GROUPNUMBER       Use group of debug defines, only use for autobuilds.
                                  GROUPNUMBER can be 0, 1 or 2;
                                  when 0: only NDEBUG is set (release build, default);
                                  when 1: NDEBUG and all DEBUG_... variants are set;
                                  when 2: only all DEBUG_... variants are set;
"conf2build.sh" parses the selected project configuration file and overwrites the default values for all contained settings.
It then collects the corresponding files which can then be imported to an individual IDE.
Additionally a project specific configuration header is created in the application source directory with the
name scheme "config_<project_name>.h". If the #define PRJ_USE_AUTOGEN_CONFIG is set to "config_<project_name>.h",
the central configuration header library/xgpl_src/IsoAgLib/isoaglib_config.h will include this header.
Additionally set the SYSTEM_FOO for the wanted platform (this is already done in generated projectfiles).
"conf2build.sh" will output the correct define at the end of the run.
Thus with these two DEFINE settings, the compiler can generate a clean running executable / HEX.

Report bugs to <m.wodok@osb-ag.de>.
EOF
}


abs_dir_of(){ cd "$(dirname "$1")" && pwd; }

assignments()
{
    local FORMAT="%s=$(literal_format $1)\n"
    shift
    printf -- "$FORMAT" "$@"
}

DEBUG_DEF_NAMES='DEBUG_ADDRESS_CLAIM
DEBUG_CAN
DEBUG_CANSERVER
DEBUG_CAN_BUFFER_FILLING
DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
DEBUG_DEVPROPERTYHANDLER
DEBUG_DIAGNOSTICPGN
DEBUG_EEPROM
DEBUG_ELEMENTBASE
DEBUG_FIFO_CAN
DEBUG_FIFO_WRITE
DEBUG_FILESERVER
DEBUG_FILESTREAMINPUT
DEBUG_FILESTREAMOUTPUT
DEBUG_FILTERBOX
DEBUG_HAL
DEBUG_HEAP_USEAGE
DEBUG_ISOMONITOR
DEBUG_LANGUAGE_AUTO_DETECT
DEBUG_MSGOBJ
DEBUG_MULTIRECEIVE
DEBUG_MULTISEND
DEBUG_MUTEX
DEBUG_NETWORK_MANAGEMENT
DEBUG_NMEA
DEBUG_OUTPUTS
DEBUG_RECEIVE
DEBUG_REGISTERERROR
DEBUG_SCHEDULER
DEBUG_SCHEDULER_EXTREME
DEBUG_SENDING
DEBUG_INPUTS
DEBUG_SYSTEM
DEBUG_TASKS_QUEUE
DEBUG_TIME_EVENTS
DEBUG_VTCOMM'

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
            (-h | --help)
                usage
                exit 0 ;;
            ('--IsoAgLib-root='*)
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
            ('--target-system='*)
                PARAMETER_TARGET_SYSTEM=$(echo_ "$option" | sed 's/--target-system=//')
                ;;
            ('--pc-can-driver='*)
                PARAMETER_CAN_DRIVER=$(echo_ "$option" | sed 's/--pc-can-driver=//')
                ;;
            ('--pc-rs232-driver='*)
                PARAMETER_RS232_DRIVER=$(echo_ "$option" | sed 's/--pc-rs232-driver=//')
                ;;
            (--little-endian-cpu)
                PARAMETER_LITTLE_ENDIAN_CPU=1
                ;;
            (--big-endian-cpu)
                PARAMETER_LITTLE_ENDIAN_CPU=0
                USE_BIG_ENDIAN_CPU=1
                ;;
            ('--with-makefile-skeleton='*)
                RootDir=$PWD
                MAKEFILE_SKELETON_FILE=$RootDir/$(echo_ "$option" | sed 's/--with-makefile-skeleton=//')
                ;;
            ('--with-makefile-app-skeleton='*)
                RootDir=$PWD
                MAKEFILE_APP_SKELETON_FILE=$RootDir/$(echo_ "$option" | sed 's/--with-makefile-app-skeleton=//')
                ;;
            ('--report'*) # Set file descriptor for report:
                local REPORTFILE="$(printf '%s' "$option" | sed -e 's|^--report\(=-\?\)\?||')"
                if [ -n "$REPORTFILE" ]; then
                    exec 5>"$REPORTFILE"
                else
                    TMP_REPORTFILE="${TEMPFILE_PREFIX}report"
                    exec 5>"$TMP_REPORTFILE"
                fi
                ;;
            ('--debugdefgroup=0')
                # Keep default for release build.
                ;;
            ('--debugdefgroup=1')
                DEBUG_DEFINES="$(assignments 1 $DEBUG_DEF_NAMES)"
                NDEBUG_DEFINE="$(wrap NDEBUG)"
                COMBINED_DEFINES="$NDEBUG_DEFINE $DEBUG_DEFINES"
                ;;
            ('--debugdefgroup=2')
                DEBUG_DEFINES="$(assignments 1 $DEBUG_DEF_NAMES)"
                NDEBUG_DEFINE=''
                COMBINED_DEFINES="$NDEBUG_DEFINE $DEBUG_DEFINES"
                ;;
            (-*)
                echo_ "Unrecognized option $option'" 1>&2
                usage
                exit 1
                ;;
            (*)
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
    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL_ORIG="${PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL:-}"
    if [ $PARAMETER_TARGET_SYSTEM != "UseConfigFile" ] ; then
        USE_TARGET_SYSTEM=$PARAMETER_TARGET_SYSTEM
    fi
    case "$USE_TARGET_SYSTEM" in
        (pc_linux | pc_win32 | esx | esxu | c2c | p1mc | Dj1 | src9)
            ;;
        (*)
            if [ -z $USE_HAL_PATH ] ; then 
                echo_ "No HAL Path (USE_HAL_PATH) specified for proprietary HAL $USE_TARGET_SYSTEM" 1>&2
                usage
                exit 1
            fi
            ;;
    esac
    
    # check for corrext CAN driver - and automatically adapt to embedded targets
    if [ $PARAMETER_CAN_DRIVER != "UseConfigFile" ] ; then
        USE_CAN_DRIVER=$PARAMETER_CAN_DRIVER
    fi
    
    case "$USE_CAN_DRIVER" in
        (simulating)
            case "$USE_TARGET_SYSTEM" in
                (pc_linux | pc_win32)
                    # enhanced CAN HAL is not yet supported for simulating CAN HAL
                    update_prj_system_with_enhanced_can_hal 0 ''
                    ;;
                (*)
                    printf 'ERROR: USE_CAN_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". Try USE_CAN_DRIVER=sys instead.\n' "$USE_CAN_DRIVER" "$USE_TARGET_SYSTEM" >&2
                    exit 2
                    ;;
            esac
            ;;
        (sys)
            case "$USE_TARGET_SYSTEM" in
                (pc_win32 )
                    echo_ "The sys CAN_DRIVER is not available for the Windows target." 1>&2
                    usage
                    exit 1
                    ;;
                (*)
                    ;;
            esac
            # enhanced CAN HAL is not yet supported for the known embedded targets
            # but it may be supported on a proprietary HAL
            #update_prj_system_with_enhanced_can_hal 0 ''
            ;;
        (msq_server)
            case "$USE_TARGET_SYSTEM" in
                (pc_linux)
                    # enhanced CAN HAL IS supported for the Linux can_server
                    update_prj_system_with_enhanced_can_hal 1 ''
                    ;;
                (pc_win32)
                    printf 'ERROR: USE_CAN_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". Try USE_CAN_DRIVER=socket_server instead.\n' "$USE_CAN_DRIVER" "$USE_TARGET_SYSTEM" >&2
                    exit 2
                    ;;
                (*)
                    printf 'ERROR: USE_CAN_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". Try USE_CAN_DRIVER=sys instead.\n' "$USE_CAN_DRIVER" "$USE_TARGET_SYSTEM" >&2
                    exit 2
                    ;;
            esac
            ;;
        (socket_server | socket_server_hal_simulator)
            # enhanced CAN HAL IS supported for the socket based can_server
            update_prj_system_with_enhanced_can_hal 1 ''
            ;;
        (*)
            echo_ "Unknown CAN driver $USE_CAN_DRIVER" 1>&2
            usage
            exit 1
            ;;
    esac
    
    if [ $PARAMETER_RS232_DRIVER != "UseConfigFile" ] ; then
        USE_RS232_DRIVER=$PARAMETER_RS232_DRIVER
    fi
    if [ $PRJ_RS232 -gt 0 ]; then
      case "$USE_RS232_DRIVER" in
          (simulating | hal_simulator)
              case "$USE_TARGET_SYSTEM" in
                  (pc_linux | pc_win32)
                      ;;
                  (*)
                      printf 'ERROR: USE_RS232_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". Try USE_RS232_DRIVER=sys instead.\n' "$USE_RS232_DRIVER" "$USE_TARGET_SYSTEM" >&2
                      exit 2
                      ;;
              esac
              ;;
          (sys)
              ;;
          (rte)
              case "$USE_TARGET_SYSTEM" in
                  (pc_linux)
                      ;;
                  (pc_win32 | *)
                      printf 'ERROR: USE_RS232_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". Try USE_RS232_DRIVER=sys instead.\n' "$USE_RS232_DRIVER" "$USE_TARGET_SYSTEM" >&2
                      exit 2
                      ;;
              esac
              ;;
          (*)
              echo_ "Unknown RS232 driver $USE_RS232_DRIVER" 1>&2
              usage
              exit 1
              ;;
      esac
    fi

    if [ $PRJ_OUTPUTS -gt 0 ]; then
      case "$USE_OUTPUTS_DRIVER" in
          (simulating | hal_simulator)
              case "$USE_TARGET_SYSTEM" in
                  (pc_linux | pc_win32)
                      ;;
                  (*)
                      printf 'ERROR: USE_OUTPUTS_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". ' "$USE_OUTPUTS_DRIVER" "$USE_TARGET_SYSTEM" >&2
                      exit 2
                      ;;
              esac
              ;;
          (sys)
              case "$USE_TARGET_SYSTEM" in
                  (pc_linux | pc_win32)
                      printf 'ERROR: USE_OUTPUTS_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". ' "$USE_OUTPUTS_DRIVER" "$USE_TARGET_SYSTEM" >&2
                      exit 2
                      ;;
                  (*)
                      ;;
              esac
              ;;
          (*)
              echo_ "Unknown OUTPUTS driver $USE_OUTPUTS_DRIVER" 1>&2
              usage
              exit 1
              ;;
      esac
    fi

    if [ "$PRJ_INPUTS_DIGITAL" -gt 0 -o "$PRJ_INPUTS_ANALOG" -gt 0 -o "$PRJ_INPUTS_COUNTER" -gt 0 ]; then
      case "$USE_INPUTS_DRIVER" in
          (simulating | hal_simulator)
              case "$USE_TARGET_SYSTEM" in
                  (pc_linux | pc_win32)
                      ;;
                  (*)
                      printf 'ERROR: USE_INPUTS_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". ' "$USE_INPUTS_DRIVER" "$USE_TARGET_SYSTEM" >&2
                      exit 2
                      ;;
              esac
              ;;
          (sys)
              case "$USE_TARGET_SYSTEM" in
                  (pc_linux | pc_win32)
                      printf 'ERROR: USE_INPUTS_DRIVER="%s" does not fit to USE_TARGET_SYSTEM="%s". ' "$USE_INPUTS_DRIVER" "$USE_TARGET_SYSTEM" >&2
                      exit 2
                      ;;
                  (*)
                      ;;
              esac
              ;;
          (*)
              echo_ "Unknown INPUTS driver $USE_INPUTS_DRIVER" 1>&2
              usage
              exit 1
              ;;
      esac
    fi

    : ${USE_EMBED_LIB_DIRECTORY:="library/commercial_BIOS/bios_${USE_TARGET_SYSTEM}"}
    : ${USE_EMBED_HEADER_DIRECTORY:="library/commercial_BIOS/bios_${USE_TARGET_SYSTEM}"}

    # check for little/big endian setting
    if [ -n "${PARAMETER_LITTLE_ENDIAN_CPU:-}" ] ; then
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
    echo_    "Defines:       PRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h $PRJ_DEFINES"
    echo_n "Include Path:  "
    for EACH_REL_APP_PATH in ${REL_APP_PATH:-} ; do
        echo_n "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH ";
    done
    echo_
    echo_
    echo_ "Generation successful."
    if [ -n "${TMP_REPORTFILE:-}" ]; then
        cat "$TMP_REPORTFILE"
    fi
}

on_exit()
{
    local STATUS="$?"
    set +o errexit # otherwise Bash may override the exit status due to another error
    set -- "${TMP_REPORTFILE:-}" "${TMP_CONF:-}" "${TMP_CONFIG1:-}" "${TMP_MAKEFILE:-}" "${TMP_EDE:-}" "${TMP_INTERNAL_FILELIST:-}" "${TMP_INTERFACE_FILELIST:-}" "${TMP_HAL_FEATURES:-}"
    # omit empty filenames:
    for FILE in "$@"; do
        shift
        [ -n "$FILE" ] && set -- "$@" "$FILE"
    done
    # delete temporary files:
    [ $# -gt 0 ] && rm -f -- "$@"
    [ "$STATUS" -eq 0 ] || printf 'ERROR (exit status %s).\n' "$STATUS" >&2
}

trap on_exit 0
trap 'exit 255' HUP INT QUIT PIPE TERM

main "$@"
