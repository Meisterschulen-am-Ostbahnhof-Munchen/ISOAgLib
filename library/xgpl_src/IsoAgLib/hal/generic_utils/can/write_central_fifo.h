/*
  write_central_fifo.h:
    definition of Hardware Abstraction
    Layer for central system functions.

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef WRITE_CENTRALFIFO_H
#define WRITE_CENTRALFIFO_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/driver/can/impl/filterbox_c.h>

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
    void findFilterBox( uint8_t aui8_busNum, uint8_t aui8_objNr,uint32_t rui32_msgId,int32_t* pui32_filterBoxIdx );

//bool addElement(uint8_t objNr, MASK_TYPE r_mask, MASK_TYPE r_filter,bool extId, int32_t r_fbIdx );
//bool addElement(uint8_t objNr,const addElem_s* sc_elem, uint32_t rui32_numEl );
//void freeIrqTable(uint8_t rui8_busNum);
//bool updateTable(uint8_t rui8_busNum,uint8_t rui8_objNr,uint32_t rui32_numEl, comparableTable_s* p_newTable );
    bool isIrqTable( uint8_t aui8_busNum, uint8_t aui8_objNum );




  }

}//__HAL


namespace __HAL {

  extern "C" {
#ifdef SYSTEM_PC
    __HAL::tCanMsgReg* IwriteCentralCanfifo( uint8_t bBus,uint8_t bOjekt,__HAL::tCanMsgReg* tCanregister );

#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
    __HAL::tCanMsgReg* IRQ_TriggerSend( uint8_t bBus,uint8_t bOjekt,__HAL::tCanMsgReg* tCanregister );
#endif

#endif


    void getIrqData( void* inputData, NEAR_MEM HAL::fifoData_s* destination, uint8_t aui8_bXtd=1 );



  }
}


#endif
#endif
