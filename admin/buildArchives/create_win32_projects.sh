#!/bin/bash

cd ../../examples/compiler_projects

EXAMPLE_LIST=`ls conf_* | grep -v "~" | sed -e 's/[ \t\n]+/:/g'`
TARGET_LIST="pc_win32"
CAN_LIST="socket_server"
DEVICE_LIST="pc pcan sontheim vector_canlib vector_xl"
RS232_LIST="simulating"

for conf_example in $EXAMPLE_LIST ; do
    EXAMPLE=`echo $conf_example | sed -e 's/conf_//g'`
    EXAMPLE_DOXY=`echo $EXAMPLE | sed -e 's/-/__/g'`
    for target in $TARGET_LIST ; do
        for can_drv in $CAN_LIST ; do
            for device in $DEVICE_LIST ; do
                for rs232_drv in $RS232_LIST ; do
                    if [ $can_drv = "sys" ] ; then
                        if    [ $target = "pc_linux" ] ; then
                            can_drv="msq_server"
                        elif [ $target = "pc_win32" ] ; then
                            continue
                        fi
                    fi
                    
                    if test $can_drv = "msq_server" ; then
                        if    [ $target != "pc_linux" ] ; then
                            continue
                        fi
                    fi
                    
                    
                    if test $rs232_drv = "rte" -a $target != "pc_linux" ; then
                        continue
                    fi
                    
                    case "$target" in
                        pc*)
                        ;;
                        *)
                        if test $rs232_drv != "sys" -o $can_drv != "sys" ; then
                            continue
                        fi
                        ;;
                    esac
                    
                    echo "./update_makefile.sh --target-system=$target --pc-can-driver=$can_drv --pc-rs232-driver=$rs232_drv --pc-can-device-for-server=$device $conf_example"
                    echo "./update_makefile.sh --target-system=$target --pc-can-driver=$can_drv --pc-rs232-driver=$rs232_drv --pc-can-device-for-server=$device $conf_example" | sh
                done
            done
        done
    done
done
