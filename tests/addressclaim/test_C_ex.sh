#!/bin/sh

# Trying to start a client and simulate an address claim of an ECU that want to steal the SA of the client

if [ -z "$1" ]; then
	echo "Please set parameter to virtual terminal startup directory!"
	exit
fi

rm logTestC_ex_0
./can_server --log logTestC_ex &

pushd $1
./startVT.sh
popd

### Prepare directories
rm -rf client1
mkdir client1 
cp 3_0_VirtualTerminalIso_static client1/client1
echo "*** FINISHED PREPARATION ***"

sleep 5

### Start client1
cd client1
./client1 &
echo "*** STARTED CLIENT 1 ***"
cd ..

sleep 5

# Try to steal the SA of the client with a lower prior IsoName
./CanServerMessenger_static -i 18EEFF81 -x -s 1C00E0FF00190EA0 &

sleep 10

killall client1
killall can_server
killall CanServerMessenger_static

pushd $1
./stopVT.sh
popd
