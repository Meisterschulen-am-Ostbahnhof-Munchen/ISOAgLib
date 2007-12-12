/***************************************************************************
                          write_central_fifo.h - definition of Hardware Abstraction
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

#ifndef WRITE_CENTRALFIFO_H
#define WRITE_CENTRALFIFO_H

#include <IsoAgLib/typedef.h>
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/driver/can/impl/filterbox_c.h>
#include <IsoAgLib/hal/config.h>

#ifdef SYSTEM_PC
#include <IsoAgLib/hal/pc/can/can_typedef.h>
#endif

#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>
#include <IsoAgLib/hal/generic_utils/can/canfifo_c.h>



#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL

namespace HAL {

extern "C" {


/** dimension of the array moIdent_arr
*/

/** the highest receive MsgObj number (including the special last message object!!) is ONE larger than the highest
    number for a message object for SEND */




typedef struct {
// uint8_t sizeTable;
  uint32_t sizeTable;
  comparableTable_s* table;
}mo_compare_s;

typedef STL_NAMESPACE::vector<int32_t>::iterator FilterBoxIdx;

/** verify whether there are element in the table */
void findFilterBox(uint8_t aui8_busNum, uint8_t aui8_objNr,uint32_t rui32_msgId,int32_t* pui32_filterBoxIdx );

//bool addElement(uint8_t objNr, MASK_TYPE r_mask, MASK_TYPE r_filter,bool extId, int32_t r_fbIdx );
//bool addElement(uint8_t objNr,const addElem_s* sc_elem, uint32_t rui32_numEl );
//void freeIrqTable(uint8_t rui8_busNum);
//bool updateTable(uint8_t rui8_busNum,uint8_t rui8_objNr,uint32_t rui32_numEl, comparableTable_s* p_newTable );
 bool isIrqTable(uint8_t aui8_busNum, uint8_t aui8_objNum);




}

}//__HAL


namespace __HAL {

extern "C" {
#ifdef SYSTEM_PC
__HAL::tCanMsgReg* IwriteCentralCanfifo(uint8_t bBus,uint8_t bOjekt,__HAL::tCanMsgReg* tCanregister);

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
__HAL::tCanMsgReg* IRQ_TriggerSend(uint8_t bBus,uint8_t bOjekt,__HAL::tCanMsgReg* tCanregister);
#endif

#endif


void getIrqData(void* inputData, NEAR_MEM HAL::fifoData_s* destination, uint8_t aui8_bXtd=1);



}
}


#endif
#endif
