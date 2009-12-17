/*
  can.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    CAN communication

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/can.h
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for CAN communication.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDEND_CAN_H_
#define _HAL_INDEPENDEND_CAN_H_

// include interface aplication relevant configuration settings
// #include <IsoAgLib/isoaglib_config.h>
#include "config.h"

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


// now include dependent on used target the suitable header
#define _hal_can_header_ <IsoAgLib/hal/HAL_SUBDIR/can/can.h>
#include _hal_can_header_


#endif
