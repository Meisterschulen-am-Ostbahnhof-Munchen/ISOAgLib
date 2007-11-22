#!/bin/bash

CMD=$1; shift

PWD=$( pwd )

LOG_DIR=${PWD}/log
if [ ! -d ${LOG_DIR} ]; then
  mkdir ${LOG_DIR}
fi

VT_BASE_DIR=${PWD}/../../../../../../VtServer
VT_UPDATE_DIR=${VT_BASE_DIR}/compiler_projects/project_generation
VT_KDEV_DIR=${VT_BASE_DIR}/compiler_projects/kdevelop_qmake

ISO_BASE_DIR=${VT_BASE_DIR}/IsoAgLib
ISO_UPDATE_DIR=${ISO_BASE_DIR}/compiler_projects/projectGeneration
ISO_KDEV_DIR=${ISO_BASE_DIR}/compiler_projects/kdevelop_qmake
ISO_TUT_SRC_DIR=${ISO_BASE_DIR}/examples/src/Tutorials

VT_SRV_DIR=${VT_KDEV_DIR}/vtserver_can_server_file
VT_CLI_DIR=${VT_KDEV_DIR}/test_client_stdin_can_server
ISO_33TUT_DIR=${ISO_KDEV_DIR}/3_3_VirtualTerminalPhase3
ISO_33TUT_SRC_DIR=${ISO_TUT_SRC_DIR}/3_VirtualTerminal_Client/3_3_VirtualTerminalPhase3
ISO_33TUT_XML_DIR=${ISO_33TUT_SRC_DIR}/MaskDefinition
VT2ISO_DIR=${ISO_BASE_DIR}/build_utils/vt2iso
VT2ISO=${VT2ISO_DIR}/bin/vt2iso

# Make binaries
function run_make() {
  local DIR=$1; shift
  local NAME=$1; shift

  echo "Making ${NAME} ... ($*)"
  if ( cd ${DIR}; $* ); then
    echo "... done ${NAME}"
  else
    echo "FAILED: ${NAME}"
    exit
  fi
}

# Run application in background
function run_app_bg() {
  local APP=$1; shift
  local DIR=$( dirname ${APP} )
  local BASE=$( basename ${APP} )

  echo "Start ${BASE} [${DIR}] ..."
  (
    cd ${DIR}
    ./${BASE} $* >${LOG_DIR}/${BASE}.out 2>&1 &
    # Keep process ID
    echo $! >${LOG_DIR}/${BASE}.pid
  )
}

######################################################################
# CLEAN

# Remove old processes if any
for i in ${LOG_DIR}/*.pid
do
  PID=$( cat $i )
  BASE=$( basename $i .pid )
  echo "Killing ${BASE} pid=${PID}"
  kill ${PID}
  rm $i
done

# Remove allready running CAN server
echo "Stop any CAN-Server"
killall can_server
killall -9 can_server
echo "Stop any 3.0 tutorial CAN-Server"
killall 3_3_VirtualTerminalPhase3
killall -9 3_3_VirtualTerminalPhase3
echo "Stop any test client"
killall test_client_stdin_can_server
killall -9 test_client_stdin_can_server
echo "Stop any VT server"
killall vtserver_can_server_file
killall -9 vtserver_can_server_file

# Cleanup environment
if [ "${CMD}" != "keeppool" ]; then
  echo "Remove old object pool files (*.iop)"
  rm -rf ${VT_SRV_DIR}/agco_pools
  rm -f ${VT_SRV_DIR}/agco_settings.bin
fi

# Just exit all
if [ "${CMD}" = "exit" ]; then
  exit
fi

# Update Makefiles
if [ "${CMD}" = "update" ]; then
  run_make ${ISO_UPDATE_DIR} "Make Makefile VT-Client" ./update_makefile.sh \
  	conf_3_3_VirtualTerminalPhase3
  run_make ${VT_UPDATE_DIR} "Make Makefile VT-Test Client" ./update_makefile.sh \
  	conf_test_client_stdin_can_server
  exit
fi

# Clean objects
if [ "${CMD}" = "clean" ]; then
  run_make ${ISO_33TUT_DIR} "3.3 Tutorial VT-Client" make clean
  run_make ${VT_SRV_DIR} "VT-Server" make clean
  run_make ${VT2ISO_DIR} "VT-Test Client" make clean
  if [ -d ${VT_CLI_DIR} ]; then
    run_make ${VT_CLI_DIR} "VT-Test Client" make clean
  fi
  exit
fi

# Update XML object pools
if [ "${CMD}" = "xml" ]; then
  run_make ${VT2ISO_DIR} "VT-Test Client" make
  run_make ${ISO_33TUT_XML_DIR} "3.3 Tutorial XML" ${VT2ISO} simpleVTIsoPool
  exit
fi

######################################################################
# BUILD

# Make binaries
run_make ${ISO_33TUT_DIR} "3.0 Tutorial VT-Client" make
run_make ${VT_SRV_DIR} "VT-Server" make
#run_make ${VT_CLI_DIR} "VT-Test Client" make

if [ "${CMD}" = "build" ]; then exit; fi

######################################################################
# RUN

# Start CAN-server as background job
run_app_bg ${VT_SRV_DIR}/can_server --log ${LOG_DIR}/can

# Start VT-server
run_app_bg ${VT_SRV_DIR}/vtserver_can_server_file

# Start VT-client
#run_app_bg ${VT_CLI_DIR}/test_client_stdin_can_server
run_app_bg ${ISO_33TUT_DIR}/3_3_VirtualTerminalPhase3

echo "Done"
exit
