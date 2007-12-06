/***************************************************************************
                          canfifo_c.cpp - definition of Hardware Abstraction
                                     Layer for central system functions.

                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
/** @file canFifo.cpp
 * Source file containing the methods for buffer reading/writing
 */


#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
#include "canfifo_c.h"
#ifdef DEBUG
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif

namespace HAL {

extern "C" {

NEAR static iFifo_s s_canFifoInstance[cui32_maxNbrCan];

}

inline void getFifoCanIdenType(bool b_isExt, __IsoAgLib::Ident_c::identType_t& r_ident);


#ifdef CAN_FIFO_READ_WAITING

/**
* Method for retrieving the current Time.
@return : the current time expressed in microseconds for the target and in milliseconds for the PC.
*/
static inline long getCurrentTime()
{

#if defined (SYSTEM_ESX) || defined(SYSTEM_ESXu) || defined(SYSTEM_C2C) || defined(SYSTEM_IMI) || defined(SYSTEM_PM167)
 __HAL::tTime l_retTime;

  __HAL::get_time_ext(&l_retTime);
  long l_timeMicroSec = 1000 * l_retTime.l1ms + l_retTime.w1us;

  return l_timeMicroSec;
#else
long l_currentTime;


time(&l_currentTime);
    return l_currentTime;

#endif


}

/**
* Method to check whether at the current time the Wt is elapsed respect the input time
*/
static inline bool isWtElapsed(uint8_t aui8_busNum,long ai_oldTime)
{
  long l_currentTime = getCurrentTime();

 // if(l_currentTime - ai_oldTime > s_canFifoInstance[aui8_busNum].i_wt)
  if(l_currentTime - ai_oldTime > CAN_FIFO_WRITING_TIME)
    return true;
  else
    return false;
}

#endif

/**
* It returns the size of the FIFO buffer
*/
static inline unsigned int getBufferSize()
{
  return uic_bufferSize;
}

/*!\brief Purpose :Check whether there is anything to read. If nothing to read, returns false
*/

/**
* There is nothing to read when :
* - the buffer is wide : UC = AC or
* - the producer is writing right now in the place where the consumer would like to read : UC-AC = 1;
*.
*/
static inline bool isAnythingToRead(unsigned int aui_tmpUc,unsigned int aui_tmpAc)
{
  //buffer is empty, nothing to read;
  //the consumer should try after a while or
  // the producer is writing right now in the place where the consumer would read

  if((aui_tmpUc == aui_tmpAc) || (aui_tmpUc - aui_tmpAc == 1)) return false;

  return true;
}

#ifdef CAN_FIFO_READ_WAITING
/**
* Check whether there is something to read.
* If nothing to read because the productor is writing, wait until it ends
*/
static bool isAnythingToReadWaiting(uint8_t aui8_busNum,unsigned int aui_tmpUc, unsigned int aui_tmpAc)
{

  if(aui_tmpUc == aui_tmpAc) return false; // buffer wide
 /**************************************************************************************
   the consumer would like to read the element where the productor is writing right now
   the consumer should wait a time which is comparable to the writing extimated time (timerValue)
  ****************************************************************************************/
  long timeBeforeReading = getCurrentTime();//set time Wt = time foreseen for writing

#ifdef SYSTEM_PC
  INTERNAL_DEBUG_DEVICE << "timeBeforeReading = " << timeBeforeReading << endl;
#endif

  while(aui_tmpUc - aui_tmpAc == 1)
  {
    aui_tmpUc = s_canFifoInstance[aui8_busNum].ui_UpdCount;

    HAL::wdTriggern();

    // if after the writing extimated time the "while condition" is still valid, then
    // better to exit and retry later; the productor could have lost its time slice (for ex.)
    if(true == isWtElapsed(aui8_busNum,timeBeforeReading))
    {

#ifdef SYSTEM_PC
      INTERNAL_DEBUG_DEVICE << "Timer for writing is elapsed " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      return false;
    }
  }

  return true;

}

#endif // CAN_FIFO_READ_WAITING

/** Check whether there is a message available : return value = true if a message is available**/

bool iFifoIsMsgAvailable(uint8_t aui8_busNum)
{

  if(aui8_busNum > HAL_CAN_MAX_BUS_NR )
  {
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "iFifoIsMsgAvailable: input parameter error "  << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false;
  }

  const unsigned int ui_tmpUc = s_canFifoInstance[aui8_busNum].ui_UpdCount;

  #ifdef CAN_FIFO_READ_WAITING
  return isAnythingToReadWaiting(aui8_busNum, ui_tmpUc,s_canFifoInstance[aui8_busNum].ui_AckCount);
  #else
  return isAnythingToRead(ui_tmpUc,s_canFifoInstance[aui8_busNum].ui_AckCount);
  #endif

}



/** Check whether the CAN fifo is critical filled
*   @return true if the CAN FIFO is in a critical filled status **/

bool isFifoCriticalFilled(uint8_t aui8_busNum)
{
  const unsigned int ui_tmpAc = s_canFifoInstance[aui8_busNum].ui_AckCount;
  const unsigned int ui_tmpUc = s_canFifoInstance[aui8_busNum].ui_UpdCount;

/**buffer is critically filled */
  if(ui_tmpUc > ui_tmpAc +(getBufferSize() * 2) - cui_toleranceLevel )
  {
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << " Can Fifo is critical filled" << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    return true;
  }
  return false;
}


/** read the FbIndex, rcvTime,refui32_msgId */

int32_t iFifoReadFbIdx(uint8_t aui8_busNum,int32_t& ri32_fbIdx, int32_t& rui32_rcvTime,uint32_t& rui32_msgId,__IsoAgLib::Ident_c::identType_t& r_msgtype)
{

  int32_t i32_retValue = HAL_NO_ERR;

  if(aui8_busNum > HAL_CAN_MAX_BUS_NR )
  {
      i32_retValue = HAL_RANGE_ERR;
      return i32_retValue;
  }

  const unsigned int ui_tmpUc = s_canFifoInstance[aui8_busNum].ui_UpdCount;

#ifdef CAN_FIFO_READ_WAITING
  if(false == isAnythingToReadWaiting(aui8_busNum, ui_tmpUc,s_canFifoInstance[aui8_busNum].ui_AckCount))
  {
    i32_retValue = HAL_UNKNOWN_ERR;
    return i32_retValue;
  }
#else
  if(false == isAnythingToRead(ui_tmpUc,s_canFifoInstance[aui8_busNum].ui_AckCount))
   {
    i32_retValue = HAL_UNKNOWN_ERR;
    return i32_retValue;
   }
#endif

/*********************************************
     READING FROM THE BUFFER
**********************************************/
 // delay_us(100);

  /** the consumer is reading */
  if(!s_canFifoInstance[aui8_busNum].b_Reading )
    s_canFifoInstance[aui8_busNum].b_Reading = true;


NEAR_PTR fifoData_s* ps_readData = &s_canFifoInstance[aui8_busNum].p_fifoBuffer[(s_canFifoInstance[aui8_busNum].ui_AckCount/2)% getBufferSize()];
  ri32_fbIdx = ps_readData->i32_fbIndex;
  rui32_rcvTime = ps_readData->i32_time;
  rui32_msgId = ps_readData->dwId;
  getFifoCanIdenType(ps_readData->bXtd, r_msgtype);

#ifdef DEBUG
INTERNAL_DEBUG_DEVICE
        << " iFifoReadFbIdx:READER in the buffer index =" << ((s_canFifoInstance[aui8_busNum].ui_AckCount/2)% getBufferSize()) << INTERNAL_DEBUG_DEVICE_ENDL;
INTERNAL_DEBUG_DEVICE
        << " READER : fbIdx = " << ri32_fbIdx <<  "rcvTime = " << rui32_rcvTime << INTERNAL_DEBUG_DEVICE_ENDL;
#ifdef SYSTEM_PC
  std::cout.setf( std::ios_base::hex, std::ios_base::basefield );
#endif
INTERNAL_DEBUG_DEVICE
        << " READER : msgId = " << rui32_msgId << INTERNAL_DEBUG_DEVICE_ENDL;

#endif
// operation must be atomic
//  incrementAc();

  return i32_retValue;

}

/** read the data of one element in the FIFO buffer */

int32_t iFifoRead(uint8_t aui8_busNum,fifoData_s& ar_readData)
{
  int32_t i32_retValue = HAL_NO_ERR;

  const unsigned int ui_tmpUc = s_canFifoInstance[aui8_busNum].ui_UpdCount;


#ifdef CAN_FIFO_READ_WAITING
  if(false == isAnythingToReadWaiting(aui8_busNum,ui_tmpUc,s_canFifoInstance[aui8_busNum].ui_AckCount))
  {
    i32_retValue = HAL_UNKNOWN_ERR;
    return i32_retValue;
   }
#else
  if(false == isAnythingToRead(ui_tmpUc,s_canFifoInstance[aui8_busNum].ui_AckCount))
  {
    i32_retValue = HAL_UNKNOWN_ERR;
    return i32_retValue;
   }
#endif

/*********************************************
     READING FROM THE BUFFER
**********************************************/
 // delay_us(100);

    /** the cosumer is reading */
  if(!s_canFifoInstance[aui8_busNum].b_Reading )
    s_canFifoInstance[aui8_busNum].b_Reading = true;


  uint8_t* pui8_destination = (uint8_t*)&ar_readData;

  NEAR_PTR uint8_t* pui8_source = (uint8_t*)&s_canFifoInstance[aui8_busNum].p_fifoBuffer[(s_canFifoInstance[aui8_busNum].ui_AckCount/2)% getBufferSize()];
  for ( unsigned int cnt = sizeof(fifoData_s); cnt > 0; cnt--)
  {
         *pui8_destination= *pui8_source;
          pui8_destination++;
          pui8_source++;
  }

  //aref_readData = s_canFifoInstance[rui8_busNum].p_fifoBuffer[(s_canFifoInstance[rui8_busNum].ui_AckCount/2)% getBufferSize()];

#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "iFifoRead: READER in the buffer index = " << ((s_canFifoInstance[aui8_busNum].ui_AckCount/2)% getBufferSize()) << INTERNAL_DEBUG_DEVICE_ENDL;
#ifdef SYSTEM_PC
  std::cout.setf( std::ios_base::hex, std::ios_base::basefield );
#endif
  INTERNAL_DEBUG_DEVICE << "READER msgId = " << ar_readData.dwId<< INTERNAL_DEBUG_DEVICE_ENDL;
#endif

// operation must be atomic
//the operation is postponed
//  iFifoIncrementAc();
  return i32_retValue;
}


/** used by the productor, discard a message */
void iFifoDiscardOldMessage(uint8_t aui8_busNum)
{
     s_canFifoInstance[aui8_busNum].ui_AckCount += 2;

}

/** check whether in that moment the consumer is reading */
bool isConsumerReading(uint8_t aui8_busNum)
{
    return s_canFifoInstance[aui8_busNum].b_Reading;
}


/** Transfer front element in central Fifo into the pointed CanPkg_c;
  DON'T clear this item from Fifo buffer.
*/
int32_t fifo_useMsgObjGet(uint8_t aui8_busNumber,__IsoAgLib::CanPkg_c* apc_data)
{
  fifoData_s pt_receive;
  STL_NAMESPACE::memset(&pt_receive,0,sizeof(fifoData_s));


  int32_t b_retVal = iFifoRead(aui8_busNumber,pt_receive);/** read the message but doesn`t pop it*/

  if (b_retVal == HAL_NO_ERR)
  {

    __IsoAgLib::CanPkg_c::setTime(pt_receive.i32_time);

    __IsoAgLib::Ident_c::identType_t idType;
    getFifoCanIdenType(pt_receive.bXtd, idType);

    __IsoAgLib::CanPkg_c::setIdent(pt_receive.dwId, idType);
    apc_data->setDataFromString(pt_receive.abData, pt_receive.bDlc);
  }
  return b_retVal;
}

/** function for retrieving the identifier type of the can message **/
inline  void getFifoCanIdenType(bool b_isExt,__IsoAgLib::Ident_c::identType_t& r_ident)
{(b_isExt == true )? (r_ident = __IsoAgLib::Ident_c::ExtendedIdent) : (r_ident = __IsoAgLib::Ident_c::StandardIdent);}


/** function used by the productor for writing in the FIFO */
bool iFifoWrite(uint8_t aui8_busNum,int32_t ai32_fbIdx,int32_t ai32_msgId,void* irqData, uint8_t aui8_bXtd)
{

  unsigned int ui_tmpAc = s_canFifoInstance[aui8_busNum].ui_AckCount;
  const unsigned int ui_tmpUc = s_canFifoInstance[aui8_busNum].ui_UpdCount;

/**buffer is full, the productor should wait and try after a while */
  if(ui_tmpUc == ui_tmpAc +(getBufferSize() * 2) )
  {
      if(!isConsumerReading(aui8_busNum)) // discard an old message
      {
           iFifoDiscardOldMessage(aui8_busNum); //AC+=2

          #ifdef DEBUG_FIFO_WRITE
            INTERNAL_DEBUG_DEVICE << "Discarded an old message " << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
           IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::CanWarn, IsoAgLib::iLibErr_c::Can );

          //overwrite the old message
           ui_tmpAc = s_canFifoInstance[aui8_busNum].ui_AckCount;

			#ifdef DEBUG_FIFO_CAN

			  __HAL::tSend pt_send;
			  pt_send.bXtd = 1;
			  pt_send.dwId = ai32_msgId;
			  pt_send.bDlc = 8;
			  pt_send.abData[0] = 0xff;
			  pt_send.abData[1] = 0xff;
			  pt_send.abData[2] = 0xff;
			  pt_send.abData[3] = 0xff;
			  pt_send.abData[4] = 0xff;
			  pt_send.abData[5] = 0xff;
			  pt_send.abData[6] = 0xff;
			  pt_send.abData[7] = 0xff;


      		  send_can_msg(aui8_busNum, 1, &pt_send);

			#endif
           // b_ret = iFifoWrite(bBus,fifoData);
      }
      else // trace, new message discarded
      {

          IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::CanWarn, IsoAgLib::iLibErr_c::Can );

          return false;
      }

  }

/*************************************************************************
  Before writing the UC should have an odd value;
  With UC = odd value, the consumer knows that the buffer is not accessible
*****************************************************************************/
// atomic operation
/** UC ++ */
  s_canFifoInstance[aui8_busNum].ui_UpdCount++;
//end atomic operation

/** write in the FIFO **/

  NEAR_PTR fifoData_s* ps_writeData = &s_canFifoInstance[aui8_busNum].p_fifoBuffer[(ui_tmpUc/2)% getBufferSize()];
  ps_writeData->dwId = ai32_msgId;
  ps_writeData->i32_time = HAL::getTime();
  ps_writeData->i32_fbIndex = ai32_fbIdx;
  
  

  // getIrqData is implemented in the target specific HAL and can directly place the data of the
  // received message into pointed place inside the FIFO
  __HAL::getIrqData( irqData, ps_writeData , aui8_bXtd);

/******************************
       WRITE INTO THE BUFFER
********************************/
// necessary copy byte to byte in target environment for the _near using
/*
  NEAR uint8_t* pui8_destination = (uint8_t*)&s_canFifoInstance[rui8_busNum].p_fifoBuffer[(ui_tmpUc/2)% getBufferSize()];
  uint8_t* pui8_source = (uint8_t*)&acref_writeData;
  for ( unsigned int cnt = sizeof(fifoData_s); cnt > 0; cnt--)
  {
         *pui8_destination= *pui8_source;
          pui8_destination++;
          pui8_source++;
  }
*/
#ifdef DEBUG_FIFO_WRITE
  INTERNAL_DEBUG_DEVICE << "WRITER in the buffer index = " << ((ui_tmpUc/2)% getBufferSize()) ;
  INTERNAL_DEBUG_DEVICE << ", DATA WRITTEN fbIndex = " << s_canFifoInstance[aui8_busNum].p_fifoBuffer[(ui_tmpUc/2)% getBufferSize()].i32_fbIndex ;

#ifdef SYSTEM_PC
  std::cout.setf( std::ios_base::hex, std::ios_base::basefield );
#endif
  INTERNAL_DEBUG_DEVICE << ", DATA WRITTEN msgId = " << s_canFifoInstance[aui8_busNum].p_fifoBuffer[(ui_tmpUc/2)% getBufferSize()].dwId << INTERNAL_DEBUG_DEVICE_ENDL;
#endif


/***************************************************************************
  After the writing operation, the productor set the UC to a even value.
  With UC = even value the consumer knows that the buffer is accessible.
****************************************************************************/

  //atomic operation
/** UC ++ */
  s_canFifoInstance[aui8_busNum].ui_UpdCount++;
  //end atomic operation
  /*
  The C++ instruction is traslated to the following asm instructions:
  extp r9,#0x1
  mov r1,[r8]
  add r1,#0x1
  extp r9,#0x1
  mov [r8],r1
*/

  return true;
}

/** pop up the message from the Fifo after having read */
void iFifoPopMessage(uint8_t aui8_busNum)
{
    s_canFifoInstance[aui8_busNum].ui_AckCount += 2;

    /** consumer has finished to read the a certain element */
    s_canFifoInstance[aui8_busNum].b_Reading = false;

}


}// HAL

#endif


