/*
  config.h: include dependent on used target (defined in
    IsoAgLib/isoaglib_config.h) the suitable HAL specific header for
    central configuration settings

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
/** \file IsoAgLib/hal/config.h
  * include dependent on used target (defined in
	  IsoAgLib/isoaglib_config.h) the suitable HAL
		specific header for central configuration settings.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDEND_CONFIG_H_
#define _HAL_INDEPENDEND_CONFIG_H_

// include interface aplication relevant configuration settings
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/compiler_adaptation.h>

#define HAL_CAN_BITRATE_LIST {10, 20, 40, 50, 100, 125, 250, 500, 1000}
#define HAL_CAN_BITRATE_CNT 9
/** index for "LastMsgObj" where all messages are stored, which map through the special lastMsgMask */
#define HAL_CAN_LAST_MSG_OBJ_NR 14
#define HAL_CAN_MIN_SEND_OBJ 0
#define HAL_CAN_MAX_SEND_OBJ 13
// the send msg objects take the lowest possible msg ob numbers
// ==> take 2 as min msg obj nr
#define HAL_CAN_MIN_REC_OBJ 1
#define HAL_CAN_MAX_REC_OBJ 14

/// define list of allowed speed settings
#define HAL_RS232_BAUDRATE_LIST {75, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600}
#define HAL_RS232_BITRATE_CNT 9                                          

// now include dependent on used target the suitable header
#define _hal_config_header_ <IsoAgLib/hal/HAL_SUBDIR/config.h>
#include _hal_config_header_


#endif
