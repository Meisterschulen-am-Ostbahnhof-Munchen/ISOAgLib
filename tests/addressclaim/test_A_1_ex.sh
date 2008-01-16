#!/bin/sh

# Trying to start two clients with the same IsoName at a first time, meaning the second one should increase its instance when claiming!

if [ -z "$1" ]; then
	echo "Please set parameter to virtual terminal startup directory!"
	exit
fi

rm logTestA-1_ex_0
./can_server --log logTestA-1_ex &

pushd $1
./startVT.sh
popd

### Prepare directories
rm -rf client1
rm -rf client2
mkdir client1 
mkdir client2
cp 3_0_VirtualTerminalIso_static client1/client1
cp 3_0_VirtualTerminalIso_static client2/client2
echo "*** FINISHED PREPARATION ***"

sleep 5

### Start client1
cd client1
./client1 &
echo "*** STARTED CLIENT 1 ***"
cd ..

sleep 5

### Start client2
cd client2
./client2 &
echo "*** STARTED CLIENT 2 ***"
cd ..

sleep 30

killall client1
killall client2
killall can_server

pushd $1
./stopVT.sh
popd
