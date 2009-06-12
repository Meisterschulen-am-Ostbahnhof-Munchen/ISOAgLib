#!/bin/bash

# Change this to the path to your binary
CAN_MESSENGER='../bin/x86linux/can_messenger -x -n 1 -c 0'
#CAN_MESSENGER="rte_can -h rte4 -x -n 1 -c 3"

REQ_PGN="$CAN_MESSENGER -i 0x18eaffea -s"

CMD_ECU_ID="$REQ_PGN c5fd00"
CMD_ECU_SW="$REQ_PGN dafe00"
CMD_ECU_DIAG="$REQ_PGN 32fd00"
CMD_ISO_CERT="$REQ_PGN 42fd00"
CMD_ACTIVE_TRBL_CODE="$REQ_PGN cafe00"
CMD_PREV_ACTIVE_TRBL_CODE="$REQ_PGN cbfe00"



sendDiagPGN()
{
	case $1 in
		ECU_ID) CMD=$CMD_ECU_ID;;
		ECU_SW) CMD=$CMD_ECU_SW;;
		ECU_DIAG) CMD=$CMD_ECU_DIAG;;
		ISO_CERT) CMD=$CMD_ISO_CERT;;
    ACTIVE_TROUBLE_CODE) CMD=$CMD_ACTIVE_TRBL_CODE;;
    PREVIOUSLY_ACTIVE_TROUBLE_CODE) CMD=$CMD_PREV_ACTIVE_TRBL_CODE;;
		*) CMD=( "$CMD_ECU_ID" "$CMD_ECU_SW" "$CMD_ECU_DIAG" "$CMD_ISO_CERT" "$CMD_ACTIVE_TRBL_CODE" "$CMD_PREV_ACTIVE_TRBL_CODE" );;
	esac
  # get number of elements in the array
  ELEMENTS=${#CMD[@]}

  # echo each element in array 
  # for loop
  for (( i=0;i<$ELEMENTS;i++)); do
    $CAN_MESSENGER -i 0x18eeffea -s 1b0e000000000000;
    ${CMD[${i}]};
    sleep 2;
  done


}

# Send an address claim to renew our connection

# Now send the command
sendDiagPGN $1

