/*
  canfifo_c.h:
    definition of Hardware Abstraction
    Layer for central system functions.

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/*! @file canfifo_c.h
 * Header file containing the non-blocking buffer class interface.
 */

/*!
* compilation define:
* - CAN_FIFO_READ_WAITING : when the consumer in the read operation waits the time necessary to the producer for writing.*.
*/
#ifndef _CANFIFO_H

#define _CANFIFO_H

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL

#ifdef SYSTEM_PC

#include <iostream>

#endif


#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/driver/can/impl/filterbox_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>

#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/hal/hal_can.h>

#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>
#include <IsoAgLib/hal/generic_utils/can/write_central_fifo.h>


namespace HAL{

/**
* Number of canfifo buffer elements.
* The number of buffer elements should be 2^N where N must be less than  TARGET_WORDSIZE -1,
* otherwise the overflow of the UC and AC counter can lead to loss of data between producer and consumer.
*/
#define TARGET_WORDSIZE (HAL_SIZEOF_INT * 8)

#if CAN_FIFO_EXPONENT_BUFFER_SIZE >= (TARGET_WORDSIZE - 1)
#error CAN_FIFO_EXPONENT_BUFFER_SIZE must be less than  TARGET_WORDSIZE -1
#endif

extern "C" {

/**
* Exponent of the 2^N operation,used  to determine the BufferSize
*/
const uint32_t cui32_maxNbrCan = (HAL_CAN_MAX_BUS_NR + 1  );
const unsigned int uic_expBufferSize = CAN_FIFO_EXPONENT_BUFFER_SIZE;

/** When during the reconfiguration only a number of place less than cui_toleranceLevel
* are free in the CAN FIFO , then the FIFO is considered in critical Filled level and the
* Canio_c::processMsg is called to free the CAN FIFO buffer */

const unsigned int cui_toleranceLevel = CAN_FIFO_CRITICAL_FILLING_TOLERANCE_LEVEL;


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

bool b_Reading; /*!< \brief the consumer is reading */

} iFifo_s;



/*!\brief Purpose :Interface for a read access to the non-blocking buffer.
*/
} //end extern C

/**
 The consumer does below actions:
 - check whether there is anything to read (buffer not wide)
 - if something to read, gets the data
 - increment AC position counter. This operation must be atomic.
 .
 @param aui8_busNum
 @param ar_readData after the call,it contains the read data
 @return false if nothing to read.
*/
int32_t iFifoRead(uint8_t aui8_busNum,fifoData_s& ar_readData);

/**
 The productor does the below actions:
 - before writing, checks whether the buffer is full;
 - then gives to UC an odd value,which means "I'm writing".
   This assignement must be an atomic operation .
 - it writes the data to the buffer;
 - then gives to the UC an even value, which means "I have finished to write".
   Again this assignement must be an atomic operation
 .
  @param aui8_busNum
  @param ai32_fbIdx
  @param ai32_msgId
  @param irqData
  @param aui8_bXtd (default:1)
  @return false whether the buffer is full and productor should wait.
*/
bool iFifoWrite(uint8_t aui8_busNum,int32_t ai32_fbIdx,int32_t ai32_msgId, void* irqData,uint8_t aui8_bXtd=1);


void iFifoDiscardOldMessage(uint8_t aui8_busNum);
bool isConsumerReading(uint8_t aui8_busNum);
bool iFifoIsMsgAvailable(uint8_t aui8_busNum);

} // __HAL




#endif
#endif //_CANFIFO_H

