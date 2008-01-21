#!/bin/sh

# use /bin/echo instead of the shell's echo (could have different parameter options, e.g. dash!)
alias echo=$(which echo)

DOXYGEN_EXPORT_DIR="../../examples/src/Tutorials"

#EXAMPLE_LIST=`ls conf_tractor* | grep -v "~" | sed -e 's/[ \t\n]+/:/g'`
EXAMPLE_LIST=`ls conf_* | grep -v "~" | sed -e 's/[ \t\n]+/:/g'`
TARGET_LIST="pc_win32:pc_linux:esx:c2c:imi:pm167"
CAN_LIST="simulating:sys:socket_server:msq_server"
RS232_LIST="simulating:sys:rte"
DEVICE_LIST="no_card:pcan:A1:rte:sontheim:vector_canlib:vector_xl"
for conf_example in $EXAMPLE_LIST ; do
  echo "Processing... ". $conf_example
  EXAMPLE_DIR=""
  case "$conf_example" in
    conf_0*) EXAMPLE_DIR="0_NetworkManagement" ;;
    conf_1*) EXAMPLE_DIR="1_ApplicationLayer" ;;
    conf_2*) EXAMPLE_DIR="2_ProcessData" ;;
    conf_3*) EXAMPLE_DIR="3_VirtualTerminal_Client" ;;
		conf_4*) EXAMPLE_DIR="4_SupplementaryDriver" ;;
		conf_5*) EXAMPLE_DIR="5_CanFeatures" ;;
		conf_6*) EXAMPLE_DIR="6_ParallelMutex" ;;
		conf_7*) EXAMPLE_DIR="7_SchedulerTask" ;;
		conf_Tractor*) DOXYGEN_EXPORT_DIR="../../examples/src";EXAMPLE_DIR="Tractor" ;;
		conf_CanServerMessenger*) DOXYGEN_EXPORT_DIR="../../examples/src";EXAMPLE_DIR="CanServerMessenger" ;;
		*) EXAMPLE_DIR="." ;;
  esac
  EXAMPLE=`echo $conf_example | sed -e 's/conf_//g'`
  EXAMPLE_DOXY=`echo $EXAMPLE | sed -e 's/-/__/g'`
	IFS=:
  for target in $TARGET_LIST ; do
    for can_drv in $CAN_LIST ; do
      for can_device in $DEVICE_LIST ; do
        for rs232_drv in $RS232_LIST ; do
          if test $target = "pc_win32" ; then
            if test $can_drv != "socket_server" -a $can_drv != "simulating" ; then
              continue
            fi
            if test $can_device != "no_card" ; then
              continue
            fi
          elif test $target = "pc_linux" ; then
            if test $can_drv != "msq_server" -a $can_drv != "simulating" ; then
              continue;
            fi
            if test $can_device = "sontheim" -o $can_device = 'vector_canlib' -o $can_device = 'vector_xl' ; then
              continue
            fi
            if test $can_drv = "simulating" -a $can_device != "no_card" ; then
              continue
            fi
          else
            if test $can_device != "no_card" ; then
              continue;
            fi
          fi

          if [ $can_drv = "sys" ] ; then
            if    [ $target = "pc_linux" ] ; then
              can_drv="msq_server"
            elif [ $target = "pc_win32" ] ; then
              continue
            fi
          fi
  
          if [ $can_drv = "msq_server" ] ; then
            if    [ $target != "pc_linux" ] ; then
              continue
            fi
          fi
          if [ $can_drv = "socket_server" ] ; then
            if    test $target != "pc_linux" -a $target != "pc_win32" ; then
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
           echo "target= $target candriver= $can_drv can_device= $can_device rs232= $rs232_drv $conf_example"
#            echo "./update_makefile.sh --target-system=$target --pc-can-driver=$can_drv --pc-can-device-for-server=$can_device --pc-rs232-driver=$rs232_drv --doxygen-export-directory=$DOXYGEN_EXPORT_DIR/$EXAMPLE_DIR/$EXAMPLE $conf_example"
             echo "./update_makefile.sh --target-system=$target --pc-can-driver=$can_drv --pc-can-device-for-server=$can_device --pc-rs232-driver=$rs232_drv --doxygen-export-directory=$DOXYGEN_EXPORT_DIR/$EXAMPLE_DIR/$EXAMPLE $conf_example" | sh
        done
      done
    done
  done
  echo "Sammel Dateien"
  # collect the exportet examples together
  COLLECT_DIR="$DOXYGEN_EXPORT_DIR/$EXAMPLE_DIR/$EXAMPLE/"
  COLLECT_NAME="$COLLECT_DIR/TutPrjDesc$EXAMPLE_DOXY.txt"
  echo "Collect Phase for $EXAMPLE_DOXY in $COLLECT_NAME"
  echo "/** \page TutPrjDesc$EXAMPLE_DOXY Tutorial project description for build of Tutorial $EXAMPLE_DOXY" > $COLLECT_NAME
  echo "  This page contains the needed project specification and the resulting project file lists" >> $COLLECT_NAME
  echo "  and configuration header contents to build the project." >> $COLLECT_NAME
  echo -e '\n\n' >> $COLLECT_NAME
  cat $COLLECT_DIR/spec_*.txt >> $COLLECT_NAME
  echo -e '\n\n' >> $COLLECT_NAME
  cat $COLLECT_DIR/config_*.txt >> $COLLECT_NAME
  echo -e '\n\n' >> $COLLECT_NAME
  cat $COLLECT_DIR/file*.txt >> $COLLECT_NAME
  echo " */" >> $COLLECT_NAME
  rm -f $COLLECT_DIR/spec_*.txt $COLLECT_DIR/config_*.txt $COLLECT_DIR/file*.txt
  #exit 0
done
