#!/bin/sh

# Trying to start a client and simulate an address claim of an ECU with the same IsoName like the test client but another SA

rm logTestD_0
./can_server --log logTestD &

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

sleep 3

# Send address claim with same IsoName and another SA
./CanServerMessenger_static -i 18EEFF81 -x -s 1B00E0FF00190EA0 &

sleep 10

killall client1
killall can_server
killall CanServerMessenger_static
