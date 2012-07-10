/*
  canfifo_c.cpp:
    definition of Hardware Abstraction Layer for central system
    functions.

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** @file canfifo_c.cpp
 * Source file containing the methods for buffer reading/writing
 */


#include <IsoAgLib/isoaglib_config.h>
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL

#include "canfifo_c.h"
#if DEBUG_FIFO_CAN
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
#endif

namespace HAL {

extern "C" {

NEAR_MEM static iFifo_s s_canFifoInstance[cui32_maxNbrCan];

}

inline void getFifoCanIdenType(bool b_isExt, __IsoAgLib::Ident_c::identType_t& r_ident);


#ifdef CAN_FIFO_READ_WAITING

/**
* Method for retrieving the current Time.
@return : the current time expressed in microseconds for the target and in milliseconds for the PC.
*/
static inline long getCurrentTime()
{

#if defined (SYSTEM_ESX) || defined(SYSTEM_ESXu) || defined(SYSTEM_C2C)
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
  INTERNAL_DEBUG_DEVICE << "timeBeforeReading = " << timeBeforeReading << INTERNAL_DEBUG_DEVICE_ENDL;
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
      #if DEBUG_FIFO_CAN
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
    #if DEBUG_FIFO_CAN
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


USE_NEAR_MEM fifoData_s* ps_readData = &s_canFifoInstance[aui8_busNum].p_fifoBuffer[(s_canFifoInstance[aui8_busNum].ui_AckCount/2)% getBufferSize()];
  ri32_fbIdx = ps_readData->i32_fbIndex;
  rui32_rcvTime = ps_readData->i32_time;
  rui32_msgId = ps_readData->dwId;
  getFifoCanIdenType(ps_readData->bXtd, r_msgtype);

#if DEBUG_FIFO_CAN
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

  USE_NEAR_MEM uint8_t* pui8_source = (uint8_t*)&s_canFifoInstance[aui8_busNum].p_fifoBuffer[(s_canFifoInstance[aui8_busNum].ui_AckCount/2)% getBufferSize()];
  for ( unsigned int cnt = sizeof(fifoData_s); cnt > 0; cnt--)
  {
         *pui8_destination= *pui8_source;
          pui8_destination++;
          pui8_source++;
  }

  //aref_readData = s_canFifoInstance[rui8_busNum].p_fifoBuffer[(s_canFifoInstance[rui8_busNum].ui_AckCount/2)% getBufferSize()];

#if DEBUG_FIFO_CAN
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
  CNAMESPACE::memset(&pt_receive,0,sizeof(fifoData_s));


  int32_t b_retVal = iFifoRead(aui8_busNumber,pt_receive);/** read the message but doesn`t pop it*/

  if (b_retVal == HAL_NO_ERR)
  {

    apc_data->setTime(pt_receive.i32_time);

    __IsoAgLib::Ident_c::identType_t idType;
    getFifoCanIdenType(pt_receive.bXtd, idType);

    apc_data->setIdent(pt_receive.dwId, idType);
    apc_data->setDataFromString(pt_receive.abData, pt_receive.bDlc);
  }
  return b_retVal;
}

/** function for retrieving the identifier type of the can message **/
inline  void getFifoCanIdenType(bool b_isExt,__IsoAgLib::Ident_c::identType_t& r_ident)
{(b_isExt == true )? (r_ident = __IsoAgLib::Ident_c::ExtendedIdent) : (r_ident = __IsoAgLib::Ident_c::StandardIdent);}


/** function used by the productor for writing in the FIFO */
void iFifoWrite(uint8_t aui8_busNum, int32_t ai32_fbIdx, int32_t ai32_msgId, void* irqData, uint8_t aui8_bXtd)
{

  unsigned int ui_tmpAc = s_canFifoInstance[aui8_busNum].ui_AckCount;
  const unsigned int ui_tmpUc = s_canFifoInstance[aui8_busNum].ui_UpdCount;

  /** buffer is full, the productor should wait and try after a while */
  if( ui_tmpUc == ui_tmpAc + ( getBufferSize() * 2 ) )
  {
    IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::HalCanBusWarn, aui8_busNum );

    if( ! isConsumerReading( aui8_busNum ) )
    {
      iFifoDiscardOldMessage( aui8_busNum ); //AC+=2
      //overwrite the old message
      ui_tmpAc = s_canFifoInstance[aui8_busNum].ui_AckCount;
    }
    else // trace, new message discarded
      return;
  }

  /* Before writing the UC should have an odd value;
     With UC = odd value, the consumer knows that the buffer is not accessible */
  s_canFifoInstance[aui8_busNum].ui_UpdCount++;

  /* write in the FIFO */
  USE_NEAR_MEM fifoData_s* ps_writeData = &s_canFifoInstance[aui8_busNum].p_fifoBuffer[(ui_tmpUc/2) % getBufferSize()];
  ps_writeData->dwId = ai32_msgId;
  ps_writeData->i32_time = HAL::getTime();
  ps_writeData->i32_fbIndex = ai32_fbIdx;

  // getIrqData is implemented in the target specific HAL and can directly place the data of the
  // received message into pointed place inside the FIFO
  __HAL::getIrqData( irqData, ps_writeData , aui8_bXtd );

  /* After the writing operation, the productor set the UC to a even value.
     With UC = even value the consumer knows that the buffer is accessible. */

  s_canFifoInstance[aui8_busNum].ui_UpdCount++;

  /* The instruction is traslated to the following asm instructions:
  extp r9,#0x1
  mov r1,[r8]
  add r1,#0x1
  extp r9,#0x1
  mov [r8],r1

  TODO atomic? verify!
  */
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
