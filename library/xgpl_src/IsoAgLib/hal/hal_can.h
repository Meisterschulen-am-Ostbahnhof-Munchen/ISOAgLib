/*
  hal_can.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    CAN communication

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* ************************************************************ */
/** \file IsoAgLib/hal/hal_can.h
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for CAN communication.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDENT_CAN_H_
#define _HAL_INDEPENDENT_CAN_H_

#include <IsoAgLib/isoaglib_config.h>


/** CAN state labels for C167
		( and comparable ) CAN controllers
	*/
typedef enum C16xCtrlStatusReg_en
{
	CanStateNoErr = 0x0000,
	CanStateWarn = 0x4000,
	CanStateOff = 0x8000,
	CanStateWarnOrOff = 0xC000,
	CanStateTxOk = 0x0800,
	CanStateRxOk = 0x1000,
	CanStateLecErrMask = 0x0700,
	CanStateStuffErr = 0x0100,
	CanStateFormErr = 0x0200,
	CanStateAckErr = 0x0300,
	CanStateBit1Err = 0x0400,
	CanStateBit0Err = 0x0500,
	CanStateCrcErr = 0x0600
} C16xCtrlStatusReg_t;

/**
  change the the send rate for one MsgObj by setting the minimum pause
  time between two messages [msec.]

  Optional, can be implemented if it is supported by the BIOS.
  Currently not used by ISOAgLib.

  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param aui16_minSend minimum send pause between two sent messages [msec.]
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
*/
int16_t can_configMsgobjSendpause(uint8_t aui8_busNr, uint8_t aui8_msgobjNr, uint16_t aui16_minSend);


#ifdef HAL_PATH_ISOAGLIB_CAN
#  define _hal_can_header_ <HAL_PATH_ISOAGLIB_CAN/can.h>
#  include _hal_can_header_
#else
#  error "No HAL_PATH_ISOAGLIB_CAN set. (CAN is a mandatory module)"
#endif

#endif
