#!/bin/sh

# Trying to start two clients seperated with the same IsoName at a first time. After this the clients started one after another, meaning the second one shouldn't be loaded because of the same IsoName!

rm logTestA-2_0
./can_server --log logTestA-2 &

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

killall client1
echo "*** KILLED CLIENT 1 ***"

sleep 3

### Start client2
cd client2
./client2 &
echo "*** STARTED CLIENT 2 ***"
cd ..

sleep 3

killall client2
echo "*** KILLED CLIENT 2 ***"

sleep 3

echo
echo "*** CONTENT EEPROM CLIENT 1 ***"
hexdump -C client1/eeprom.dat
echo

echo "*** CONTENT EEPROM CLIENT 2 ***"
hexdump -C client2/eeprom.dat
echo

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

sleep 3

echo
echo "*** CONTENT EEPROM CLIENT 1 ***"
hexdump -C client1/eeprom.dat
echo

echo "*** CONTENT EEPROM CLIENT 2 ***"
hexdump -C client2/eeprom.dat
echo

sleep 10

killall client1
killall client2
killall can_server
