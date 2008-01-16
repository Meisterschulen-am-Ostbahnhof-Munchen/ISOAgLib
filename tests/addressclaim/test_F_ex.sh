#!/bin/sh

# Trying to start a client and simulate an address claim of an ECU with the same IsoName and SA like the test client

if [ -z "$1" ]; then
	echo "Please set parameter to virtual terminal startup directory!"
	exit
fi

rm logTestF_ex_0
./can_server --log logTestF_ex &

pushd $1
./startVT.sh
popd

### Prepare directories
rm -rf client1
mkdir client1 
cp 3_0_VirtualTerminalIso_static client1/client1
echo "*** FINISHED PREPARATION ***"

sleep 3

### Start client1
cd client1
./client1 &
echo "*** STARTED CLIENT 1 ***"
cd ..

sleep 30

killall client1
killall can_server

pushd $1
./stopVT.sh
popd
