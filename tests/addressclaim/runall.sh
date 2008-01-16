#!/bin/sh

ISOAGLIB_ROOT=../../
VT_STARTUP_DIR=<SET ME!>

report=report

runtest()
{
    typeset test=$1
    typeset trace=$2
    typeset log=$3

    echo "Running test script" $test

    sleep 2

    ./$test $VT_STARTUP_DIR >> $log 2>&1

    echo >> $report
    echo Test $test >> $report

    echo >> $report
    echo "diff result:" >> $report
    cat $(basename $trace) | grep 18e[ea]ff | perl -p -e 's!\d*!!' > .tmp
    diff .tmp templates/$(basename $trace ex_0)0 >> $report
    rm .tmp

    echo >> $report
    echo "sdiff result:" >> $report
    cat $(basename $trace) | grep 18e[ea]ff | perl -p -e 's!\d*!!' | sdiff -b - templates/$(basename $trace ex_0)0 >> $report
}

# check symbolic link to startVT.sh
if [ ! -w $VT_STARTUP_DIR/startVT.sh ]; then
    echo "No startVT.sh found at VT startup directory!"
    exit 1
fi

# check symbolic link to stopVT.sh
if [ ! -w $VT_STARTUP_DIR/stopVT.sh ]; then
    echo "No stopVT.sh found at VT startup directory!"
    exit 1
fi

# copy current binaries to current folder
if [ ! -w $ISOAGLIB_ROOT/examples/compiler_projects/kdevelop_make/3_0_VirtualTerminalIso/3_0_VirtualTerminalIso_static ]; then
    echo "File 3_0_VirtualTerminalIso_static not exists! Please compile tutorial 3_0!"
    exit 1
fi

cp $ISOAGLIB_ROOT/examples/compiler_projects/kdevelop_make/3_0_VirtualTerminalIso/3_0_VirtualTerminalIso_static .

if [ ! -w $ISOAGLIB_ROOT/examples/compiler_projects/kdevelop_make/3_0_VirtualTerminalIso/can_server ]; then
    echo "File can_server not exists! Please compile tutorial 3_0!"
    exit 1
fi

cp $ISOAGLIB_ROOT/examples/compiler_projects/kdevelop_make/3_0_VirtualTerminalIso/can_server .

if [ ! -w $ISOAGLIB_ROOT/examples/compiler_projects/kdevelop_make/CanServerMessenger/CanServerMessenger_static ]; then
    echo "File CanServerMessenger_static not exists! Please compile CanServerMessenger!"
    exit 1
fi

cp $ISOAGLIB_ROOT/examples/compiler_projects/kdevelop_make/CanServerMessenger/CanServerMessenger_static .

rm $report

echo "Test start" >> $report
date >> $report

# remove former created log files
rm test*.log

runtest test_A_1_ex.sh logTestA-1_ex_0 testA-1.log

runtest test_A_2_ex.sh logTestA-2_ex_0 testA-2.log

runtest test_B_ex.sh logTestB_ex_0 testB.log

runtest test_C_ex.sh logTestC_ex_0 testC.log

runtest test_D_ex.sh logTestD_ex_0 testD.log

runtest test_E_ex.sh logTestE_ex_0 testE.log

# for testcase F there must be an virtual terminal that changes it's SA after some seconds !!!
# runtest test_F_ex.sh logTestF_ex_0 testF.log

runtest test_G_1_ex.sh logTestG-1_ex_0 testG-1.log

runtest test_G_2_ex.sh logTestG-2_ex_0 testG-2.log

echo >> $report
echo "Test end" >> $report
date >> $report
