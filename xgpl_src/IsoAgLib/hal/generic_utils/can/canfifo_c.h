/***************************************************************************
                          canfifo_c.h - definition of Hardware Abstraction
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

/** @file canfifo_c.h
 * Header file containing the non-blocking buffer class interface.
 */

/**
* List of all the compilation define:
* - READ_WAITING : when the consumer in the read operation waits the time necessary to the producer for writing.
* - CANFIFO_TARGET : for the target environment
* - DYN_SIZE : if you want that the dimension of the buffer is passed as argument to the constructor of the class canFifo
* - PC_CANFIFO_DEBUG : to activate debug information on PC
* - CANFIFO_TARGET_DEBUG
*.
*/
#ifndef _CANFIFO_H

#define _CANFIFO_H



#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL

#ifdef SYSTEM_PC

#include <iostream>

#endif


#include <IsoAgLib/typedef.h>
#include <IsoAgLib/driver/can/impl/filterbox_c.h>
#include <IsoAgLib/util/impl/canpkg_c.h>

#include <IsoAgLib/hal/system.h>
#include <IsoAgLib/hal/can.h>

#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>
#include <IsoAgLib/hal/generic_utils/can/writeCentralFifo.h>

namespace HAL{

#define TARGET_WORDSIZE (SIZEOF_INT * 8)

extern "C" {

/**
* Exponent of the 2^N operation,used  to determine the BufferSize
*/
const uint32_t cui32_maxNbrCan = (HAL_CAN_MAX_BUS_NR + 1  );
const unsigned int uic_expBufferSize = 4;

/**
* Number of buffer elements.
* The number of buffer elements should be 2^N where N must be less than  TARGET_WORDSIZE -1,
* otherwise the overflow of the UC and AC counter can lead to loss of data between producer and consumer.
* For PC environment define the PC_CANFIFO_DEBUG for activating the check.
*/
/*
#if (uic_expBufferSize > (TARGET_WORDSIZE -1))
Error on Buffer size, the variable uic_expBufferSize is too big.
Please mofify
#endif
*/


const unsigned int uic_bufferSize = 1 << uic_expBufferSize;


#ifdef SYSTEM_PC
using namespace std;
#endif

/**
* Fifo non-blocking buffer
*/

typedef struct {

volatile unsigned int ui_AckCount;   /*!< \brief Last buffer element read by the consumer (AC)*/
volatile unsigned int ui_UpdCount;   /*!< \brief Last buffer element written by the productor (UC)*/

fifoData_s p_fifoBuffer[uic_bufferSize];  /*!< \brief Buffer shared between productor and consumer */

#ifdef READ_WAITING
long i_wt;     /*!< \brief Time occurring to the productor for writing one buffer element */
#endif

bool b_Reading; /*!< \brief the consumer is reading */

} iFifo_s;



/*!\brief Purpose :Interface for a read access to the non-blocking buffer.
*/
} //end extern C

/**
* The consumer does below actions:
* - check whether there is anything to read (buffer not wide)
* - if something to read, gets the data
* - increment AC position counter. This operation must be atomic.
*.
@param aref_readData : after the call,it contains the read data .
@return false if nothing to read.
*/
int32_t iFifoRead(uint8_t aui8_busNum,fifoData_s& ar_readData);

/**
* The productor does the below actions:
* - before writing, checks whether the buffer is full;
* - then gives to UC an odd value,which means "I'm writing".
*   This assignement must be an atomic operation .
* - it writes the data to the buffer;
* - then gives to the UC an even value, which means "I have finished to write".
*   Again this assignement must be an atomic operation
* .
@param acref_writeData : Data to be written.
@return false whether the buffer is full and productor should wait.
*/
bool iFifoWrite(uint8_t aui8_busNum,int32_t ai32_fbIdx,int32_t ai32_msgId, void* irqData);
//bool iFifoWrite(uint8_t ui8_busNum,const fifoData_s& acref_writeData);

//void iFifoPopMessage(uint8_t ui8_busNumber);
void iFifoDiscardOldMessage(uint8_t aui8_busNum);
bool isConsumerReading(uint8_t aui8_busNum);
//int32_t fifo_useMsgObjGet(uint8_t rui8_busNumber,__IsoAgLib::CanPkg_c* rpc_data);

} // __HAL




#endif
#endif //_CANFIFO_H

