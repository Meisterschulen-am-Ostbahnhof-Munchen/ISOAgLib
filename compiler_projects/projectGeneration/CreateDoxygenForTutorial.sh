#!/bin/sh
DOXYGEN_EXPORT_DIR="../../IsoAgLib_Examples/tutorial"
EXAMPLE_LIST=`ls conf_* | grep -v "~" | grep -e "conf_[0-9]" | sed -e 's/[ \t\n]+/:/g'`
# EXAMPLE_LIST="conf_tractor"
TARGET_LIST="pc_linux:pc_win32:esx:imi:pm167"
CAN_LIST="simulating:sys:vector_canlib:vector_xl:sontheim"
RS232_LIST="simulating:sys:rte"
for conf_example in $EXAMPLE_LIST ; do
  EXAMPLE_DIR=""
  case "$conf_example" in
    conf_0*) EXAMPLE_DIR="0_SystemMgmt" ;;
    conf_1*) EXAMPLE_DIR="1_BaseData" ;;
    conf_2*) EXAMPLE_DIR="2_ProcessData" ;;
    conf_3*) EXAMPLE_DIR="3_Terminal" ;;
		conf_4*) EXAMPLE_DIR="4_SupplementaryDriver" ;;
		conf_5*) EXAMPLE_DIR="5_CanFeatures" ;;
  esac
  EXAMPLE=`echo $conf_example | sed -e 's/conf_//g'`
  EXAMPLE_DOXY=`echo $EXAMPLE | sed -e 's/-/__/g'`
	IFS=:
  for target in $TARGET_LIST ; do
	  for can_drv in $CAN_LIST ; do
  	  for rs232_drv in $RS232_LIST ; do
        if [ $can_drv = "sys" ] ; then
	        if    [ $target = "pc_linux" ] ; then
      		  can_drv="rte"
          elif [ $target = "pc_win32" ] ; then
      		  continue
          fi
        fi
        if [ $can_drv = "vector_canlib" ] ; then
	        if    [ $target != "pc_win32" ] ; then
      		  continue
          fi
        fi
        if [ $can_drv = "vector_xl" ] ; then
	        if    [ $target != "pc_win32" ] ; then
      		  continue
          fi
        fi
        if [ $can_drv = "sontheim" ] ; then
	        if    [ $target != "pc_win32" ] ; then
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
   		  # echo "Target: $target, CAN: $can_drv, RS232: $rs232_drv"
        echo "$EXAMPLE_DIR $EXAMPLE $conf_example $target __ $can_drv __ $rs232_drv"
				echo "./update_makefile.sh --target-system=$target --pc-can-driver=$can_drv --pc-rs232-driver=$rs232_drv --doxygen-export-directory=$DOXYGEN_EXPORT_DIR/$EXAMPLE_DIR/$EXAMPLE $conf_example" | sh
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
  echo -e "\n\n" >> $COLLECT_NAME
  cat $COLLECT_DIR/spec_*.txt >> $COLLECT_NAME
  echo -e "\n\n" >> $COLLECT_NAME
  cat $COLLECT_DIR/config_*.txt >> $COLLECT_NAME
  echo -e "\n\n" >> $COLLECT_NAME
  cat $COLLECT_DIR/file*.txt >> $COLLECT_NAME
  echo " */" >> $COLLECT_NAME
  rm -f $COLLECT_DIR/spec_*.txt $COLLECT_DIR/config_*.txt $COLLECT_DIR/file*.txt
  #exit 0
done
