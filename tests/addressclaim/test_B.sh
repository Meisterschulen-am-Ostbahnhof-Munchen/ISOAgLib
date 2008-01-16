#!/bin/sh

# Trying to start a client and simulate an address claim of an ECU that want to steal the SA of the client

rm logTestB_0
./can_server --log logTestB &

### Prepare directories
rm -rf client1
mkdir client1 
cp 3_0_VirtualTerminalIso_static client1/client1
echo "*** FINISHED PREPARATION ***"

sleep 3


sleep 10
### Start client1
cd client1
./client1 &
echo "*** STARTED CLIENT 1 ***"
cd ..

sleep 3

# Try to steal the SA of the client with a higher prior IsoName
./CanServerMessenger_static -i 18EEFF80 -x -s 1A00E0FF00190EA0 &

sleep 10

killall client1
killall can_server
killall CanServerMessenger_static
