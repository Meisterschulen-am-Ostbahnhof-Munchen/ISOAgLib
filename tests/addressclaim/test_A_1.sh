#!/bin/sh

# Trying to start two clients with the same IsoName at a first time, meaning the second one should increase its instance when claiming!

rm logTestA-1_0
./can_server --log logTestA-1 &

### Prepare directories
rm -rf client1
rm -rf client2
mkdir client1 
mkdir client2
cp 3_0_VirtualTerminalIso_static client1/client1
cp 3_0_VirtualTerminalIso_static client2/client2
echo "*** FINISHED PREPARATION ***"

sleep 3

### Start client1
cd client1
./client1 &
echo "*** STARTED CLIENT 1 ***"
cd ..

sleep 3

### Start client2
cd client2
./client2 &
echo "*** STARTED CLIENT 2 ***"
cd ..

sleep 10

killall client1
killall client2
killall can_server
