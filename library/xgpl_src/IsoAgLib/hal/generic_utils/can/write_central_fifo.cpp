/*
  writeCentralFifo.cpp:
    definition of Hardware Abstraction Layer for central system
    functions.

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/hal/generic_utils/can/write_central_fifo.h>


#ifdef SYSTEM_PC
#include <IsoAgLib/hal/pc/errcodes.h>
#include <IsoAgLib/hal/pc/can/can_target_extensions.h>
#include <IsoAgLib/hal/pc/can/can_server.h>
#endif

#include <IsoAgLib/util/iliberr_c.h>

#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/hal/hal_system.h>

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL

namespace HAL {

extern "C" {


//static void initMoIdentArr(uint8_t rui8_busNum);

/**Support table for quick search of the FilterBox interested to the message.
*/
NEAR_MEM static mo_compare_s  moIdentArr[cui32_maxNbrCan][cui8_numMsbObj]; /** the index is the MsbObjNum of the ArrMsgObj */
NEAR_MEM static bool sb_initialized[cui32_maxNbrCan];
static uint8_t minReceiveObjNr[cui32_maxNbrCan];


bool isIrqTable(uint8_t aui8_busNum, uint8_t aui8_objNum)
{
#if DEBUG_FIFO_CAN
  if(aui8_busNum > HAL_CAN_MAX_BUS_NR || aui8_objNum - minReceiveObjNr[aui8_busNum] >= cui8_numMsbObj)
  {
    // range error
    IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can );
    return false;
  }
#endif

  if(moIdentArr[aui8_busNum][aui8_objNum - minReceiveObjNr[aui8_busNum]].sizeTable == 0)
    return false;
  return true;
}

//static bool b_irqWritingFlag[cui32_maxCanBusCnt];

//inline bool getIrqWritingFlag(uint8_t rui8_busNum){return b_irqWritingFlag[rui8_busNum];}
//inline void setIrqWritingFlag(uint8_t rui8_busNum, bool b_flag){b_irqWritingFlag[rui8_busNum] = b_flag;}

/** register the min obj num receive for a can instance */
void setIrqMinReceiveObjNr(uint8_t aui8_busNum,uint8_t aui8_minObjNr)
{
	minReceiveObjNr[aui8_busNum] = 	aui8_minObjNr;
}

/** free the table related a busNum */
void freeIrqTable(uint8_t aui8_busNum)
{
#if DEBUG_FIFO_CAN
  if(aui8_busNum > HAL_CAN_MAX_BUS_NR )
  {
    // range error
    IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Can );
  }
#endif

  for(uint8_t i = 0 ; i < cui8_numMsbObj ; i++ )
  {
    moIdentArr[aui8_busNum][i].sizeTable = 0;
    if(moIdentArr[aui8_busNum][i].table != NULL)
    {
      STL_NAMESPACE::free(moIdentArr[aui8_busNum][i].table);
    }
    moIdentArr[aui8_busNum][i].table = NULL;
  }

}
/** Utility routine for initializing the moIdentArr
*/

static void initMoIdentArr(uint8_t aui8_busNum)
{

  for(uint8_t i = 0 ; i < cui8_numMsbObj ; i++ )
  {
    moIdentArr[aui8_busNum][i].sizeTable = 0;
    moIdentArr[aui8_busNum][i].table = NULL;
  }
  sb_initialized[aui8_busNum] = true;
}

/** Utility routine for adding an element to the comparableTable_s for a specified moIdentArr element.
*/
//bool irqTableData(FilterBox_c& cp_elem,uint32_t rui32_numEl)


bool updateTable(uint8_t aui8_busNum,uint8_t aui8_objNr,uint32_t aui32_numEl, comparableTable_s* p_newTable )
{
  isoaglib_assert( aui8_busNum <= HAL_CAN_MAX_BUS_NR );
  isoaglib_assert( aui8_objNr < cui8_numMsbObj );

  /** initializing */
  if(sb_initialized[aui8_busNum] != true)
    initMoIdentArr(aui8_busNum);

  comparableTable_s* p_oldTable = moIdentArr[aui8_busNum][aui8_objNr].table;

//assign before the pointer and then modify the size
  moIdentArr[aui8_busNum][aui8_objNr].table = p_newTable;
  moIdentArr[aui8_busNum][aui8_objNr].sizeTable = aui32_numEl;


  if(p_oldTable != NULL)
    STL_NAMESPACE::free(p_oldTable);

  return true;
}


/** Find the FilterBoxNr which is interested in the CAN message
*@param aui8_busNum : Bus number
*@param aui8_objNr : MsgObjNr in the MsgObj_c class
*@param rui32_msgId : type of CAN frame : false = standard (11 bits) or true = extended (29 bits)
*@param pi32_filterBoxIdx
*@return true if a filterBox is found and rui8_filterBoxNr is the FilterBoxNr.
*/
void findFilterBox(uint8_t aui8_busNum, uint8_t aui8_objNr,uint32_t rui32_msgId,int32_t* pi32_filterBoxIdx )
{

//  *pi32_filterBoxIdx = -1; //initialized
  uint8_t ui8_objNr = aui8_objNr - minReceiveObjNr[aui8_busNum];
  for(int32_t i32_idx = moIdentArr[aui8_busNum][ui8_objNr].sizeTable - 1; i32_idx >= 0 ; i32_idx --)
  {

/** the check (Mask & r_msgId & 0x7FF == Mask & Filter & 0x7FF)11bit or (Mask & r_msgId == Mask & Filter)ext */
     if((moIdentArr[aui8_busNum][ui8_objNr].table[i32_idx].fbMask & rui32_msgId) == moIdentArr[aui8_busNum][ui8_objNr].table[i32_idx].fbFilter)
     {
        *pi32_filterBoxIdx = moIdentArr[aui8_busNum][ui8_objNr].table[i32_idx].i32_filterBoxVecIdx;
        break;
     }
  }

}


#if DEBUG_FIFO_CAN
void printIrqTable(uint8_t rui8_busNum)
{

INTERNAL_DEBUG_DEVICE << "CAN BUS = "
#ifdef SYSTEM_PC
<< std::dec
#endif
<< int(rui8_busNum) << INTERNAL_DEBUG_DEVICE_ENDL;

  for(uint32_t i = 0 ; i < cui8_numMsbObj ; i++ )
  {
#ifdef SYSTEM_PC
    std::cout.setf( std::ios_base::dec, std::ios_base::basefield );
#endif

    INTERNAL_DEBUG_DEVICE << "Index = " << i
    <<", Num Elem = " << moIdentArr[rui8_busNum][i].sizeTable << INTERNAL_DEBUG_DEVICE_ENDL;

    for(uint32_t j = 0; j< moIdentArr[rui8_busNum][i].sizeTable; j++)
    {
      #ifdef SYSTEM_PC
      std::cout.setf( std::ios_base::hex, std::ios_base::basefield );
      #endif
      INTERNAL_DEBUG_DEVICE << "    Mask = 0x " << moIdentArr[rui8_busNum][i].table[j].fbMask
      << ", Filter = 0x" << moIdentArr[rui8_busNum][i].table[j].fbFilter ;
      #ifdef SYSTEM_PC
      std::cout.setf( std::ios_base::dec, std::ios_base::basefield );
      #endif
      INTERNAL_DEBUG_DEVICE << ", FilterBox index = " << moIdentArr[rui8_busNum][i].table[j].i32_filterBoxVecIdx << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
}
#endif

} // end EXTERN C


} //__HAL

#endif

