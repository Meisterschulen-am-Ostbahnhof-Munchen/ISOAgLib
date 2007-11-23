/***************************************************************************
                          icanfifo.h - definition of Hardware Abstraction
                                     Layer for central system functions.

                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
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


#ifndef ICANFIFO_H
#define ICANFIFO_H

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL


#ifdef SYSTEM_PC
#define NEAR
#else
#define NEAR _near
#endif

namespace HAL {

extern "C" {



const uint8_t cui8_numMsbObj = CONFIG_CAN_DEFAULT_MAX_OBJ_NR + 1;

typedef struct {

  MASK_TYPE fbMask; // ui32 or ui16 result of mask & 0x7FF if necessary
  MASK_TYPE fbFilter; //ui32 or ui16 result of the (Mask & Filter & 0x7FF if necessary )
  int32_t i32_filterBoxVecIdx; // index of the FilterBox in the array arrFilterBox
} comparableTable_s;


typedef struct {
      uint32_t dwId;                          /** Identifier */
      uint8_t bXtd;                           /** Type of Bit Identifier extended or standard*/
      uint8_t bDlc;                           /** Len of the data */
      uint8_t abData[8];                      /** Data received **/
      int32_t i32_time;                       /** time of received message */
      int32_t i32_fbIndex;
}fifoData_s;


bool updateTable(uint8_t aui8_busNum,uint8_t aui8_objNr,uint32_t aui32_numEl, comparableTable_s* p_newTable );


void freeIrqTable(uint8_t aui8_busNum);

void setIrqMinReceiveObjNr(uint8_t aui8_busNum, uint8_t aui8_minRecObjNr);

void iFifoPopMessage(uint8_t aui8_busNumber);


int32_t fifo_useMsgObjGet(uint8_t aui8_busNumber,__IsoAgLib::CanPkg_c* apc_data);

#ifdef DEBUG
void printIrqTable(uint8_t aui8_busNumber);
#endif


}


bool isFifoCriticalFilled(uint8_t aui8_busNum);


int32_t iFifoReadFbIdx(uint8_t aui8_busNumber,int32_t& ri32_fbIdx,int32_t& ri32_rcvTime,uint32_t& ri32_ident,__IsoAgLib::Ident_c::identType_t& r_msgtype);
}

#endif
#endif
