/***************************************************************************
                          can.h - include dependent on used target (defined
													           in Application_Config/isoaglib_config.h)
																		 the suitable HAL specific header for
																		 CAN communication
                             -------------------
    begin                : Sun Mar 09 2003
    copyright            : (C) 2003 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

/* ************************************************************ */
/** \file IsoAgLib/hal/can.h
  * include dependent on used target (defined in
	  Application_Config/isoaglib_config.h) the suitable HAL
		specific header for CAN communication.
*/
/* ************************************************************ */
#ifndef _HAL_INDEPENDEND_CAN_H_
#define _HAL_INDEPENDEND_CAN_H_

// include interface aplication relevant configuration settings
// #include <Application_Config/isoaglib_config.h>
#include "config.h"

/** CAN state labels for C167
		( and comparable ) CAN controllers
	*/
typedef enum
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
#if defined(SYSTEM_PC)
	#include "pc/can/can.h"
#elif defined(SYSTEM_ESX)
	#include "esx/can/can.h"
#elif defined(SYSTEM_IMI)
	#include "imi/can/can.h"
#elif defined(SYSTEM_PM167)
	#include "pm167/can/can.h"
#elif defined(SYSTEM_MITRON167)
	#include "mitron167/can/can.h"
#endif


#endif
