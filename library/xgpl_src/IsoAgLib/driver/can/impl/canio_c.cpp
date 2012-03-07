/*
  canio_c.cpp: master object for CAN communication; owns array of
    FilterBox_c and MsgObj_c instances  

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "canio_c.h"
#include <functional>
#include <algorithm>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <IsoAgLib/util/iassert.h>
#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION || DEBUG_HEAP_USEAGE || DEBUG_CAN_BUFFER_FILLING
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#if DEBUG_HEAP_USEAGE
#  ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
static uint16_t sui16_msgObjTotal = 0;
static uint16_t sui16_deconstructMsgObjCnt = 0;
#  endif
static uint16_t sui16_filterBoxTotal = 0;

#  ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#  endif
#endif


namespace __IsoAgLib {
  
CanIo_c &getCanInstance( uint8_t aui8_instance )
{
  MACRO_MULTITON_GET_INSTANCE_BODY(CanIo_c, CAN_INSTANCE_CNT, aui8_instance);
}


bool
CanIo_c::init(
  uint8_t aui8_busNumber,
  uint16_t aui16_bitrate,
  uint8_t aui8_minObjNr,
  uint8_t aui8_maxObjNr)
{
  if (initialized())
  { // don't allow a re-init of the bus, app has to close bus first.
    return false;
  }

  /* ****************************** */
  /* *****check input parameter**** */
  /* ****************************** */
  // check aui8_minObjNr, aui8_maxObjNr and aui8_busNumber
  if (
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      (aui8_minObjNr > HAL_CAN_MAX_SEND_OBJ)
      ||(aui8_maxObjNr > HAL_CAN_MAX_SEND_OBJ)
#if HAL_CAN_MIN_SEND_OBJ > 0
      ||(aui8_minObjNr < HAL_CAN_MIN_SEND_OBJ) // use this comparison only for (HAL_CAN_MIN_SEND_OBJ > 0), else it gives a warning!
      ||(aui8_maxObjNr < HAL_CAN_MIN_SEND_OBJ) // use this comparison only for (HAL_CAN_MIN_SEND_OBJ > 0), else it gives a warning!
#endif
      ||
#else
#if HAL_CAN_MIN_SEND_OBJ > 0
      (aui8_minObjNr < HAL_CAN_MIN_SEND_OBJ)
      ||
#endif
#endif
      (aui8_busNumber > HAL_CAN_MAX_BUS_NR)
     )
  { // one of the range tests not passed
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  (void) aui8_maxObjNr;
#endif

  mb_runningCanProcess = false;

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  initMinChangedFilterBox();
#endif

  mi32_endLastReconfigTime = 0;
  mi32_lastProcessedCanPkgTime = 0;
  mui8_processedMsgCnt = 0;
  mc_maskStd.set(0x7FF, Ident_c::StandardIdent);
  mc_maskExt.set(0x1FFFFFFFLU, Ident_c::ExtendedIdent);
  mc_maskLastmsg.set(0, Ident_c::ExtendedIdent);

  /** set min and max allowed obj numbers (important for multithreading
      where each threads manages only a portion of all objects)
  */
  setMinHALMsgObjNr(aui8_minObjNr);
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  setMaxHALMsgObjNr(aui8_maxObjNr);
#endif
  mui8_busNumber = aui8_busNumber;

  const bool baseCanInitSuccess = baseCanInit (aui16_bitrate);
  if (baseCanInitSuccess)
  {
    setInitialized();
    return true;
  }
  else
  { // for backwards compatibility, better reset the bus nummer!
    mui8_busNumber = 0xFF;
    return false;
  }
}


void
CanIo_c::close()
{
  if (!initialized())
    return;

  #if defined SYSTEM_WITH_ENHANCED_CAN_HAL
  for (uint8_t i = 0; i < cntFilter(); i++)
    m_arrFilterBox[i].closeHAL();
  #endif

  // call BIOS CAN close function - set error state if BIOS function cause error
  switch (HAL::can_configGlobalClose(mui8_busNumber) )
  {
    case HAL_NO_ERR:
      break;
    case HAL_CONFIG_ERR:
      // ignore this type also, as this is only the indication of try to close an already closed channel
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(mui8_busNumber) << " was already closed before close call" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      break;
    default:
      // wrong channel
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
  }
#if DEBUG_HEAP_USEAGE
  sui16_filterBoxTotal -= cntFilter();
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  sui16_msgObjTotal -= cntMsgObj();
#endif
#endif

  // Clear Filters
  m_arrFilterBox.clear();
  setCntFilter( 0 );

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  // Clear MsgObjs
  marr_msgObj.clear();
  setCntMsgObj( 0 );
#endif

  // indicated closed instance
  setClosed();
  mui8_busNumber = 0xFF; // just for backward compatibility
}


bool
CanIo_c::timeEvent( void )
{
  // if not ready, CanIo_c is not yet initialised complete -> do nothing
  if ( (mui16_bitrate == 0) || ( mui8_busNumber > HAL_CAN_MAX_BUS_NR ) ) return false;

  // start process of all received msg
  return (processMsg() >= 0);
}


#ifdef USE_CAN_MEASURE_BUSLOAD
/** deliver actual BUS load in baud
  @return baudrate in [kbaud] on used CAN BUS
*/
uint16_t CanIo_c::getBusLoad() const
{
  return HAL::can_stateGlobalBusload(mui8_busNumber);
}
#endif


uint8_t
CanIo_c::sendCanFreecnt()
{
  const uint8_t sendObjNr = minHALMsgObjNr();

  const int16_t i16_result =
    HAL::can_stateMsgobjFreecnt( mui8_busNumber, sendObjNr );

  if ( i16_result >= 0 )
    return uint8_t(i16_result);
  else
    return 0;
}


void
CanIo_c::sendCanClearbuf()
{
  const uint8_t sendObjNr = minHALMsgObjNr();

  #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE
   << "CanIo_c::sendCanClearbuf for MsgObjNr: " << uint16_t(sendObjNr) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  HAL::can_useMsgobjClear(mui8_busNumber, sendObjNr);
}


bool
CanIo_c::existFilter(
  const __IsoAgLib::CanCustomer_c& ar_customer,
  const IsoAgLib::iMaskFilterType_c& arc_filterpair,
  ArrFilterBox::iterator* apc_iter)
{
  // check if filter/t_mask are already inserted
  // return false if this setting isn't unique
  bool b_identDefFound = false;

  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter = m_arrFilterBox.begin();

  // check if given FilterBox_c definition is not yet in array
  for (; pc_iter != m_arrFilterBox.end(); pc_iter++)
  {
    if ( ( pc_iter->equalFilterMask( arc_filterpair ) )
      && ( pc_iter->equalCustomer( ar_customer )               ) )
    { // FilterBox_c with equal def found
      //-> don't insert complete ident def two times
      b_identDefFound = true;
      // don't search rest of array
      break;
    }
  }
  if (b_identDefFound && (apc_iter != NULL)) *apc_iter = pc_iter;
  return b_identDefFound;
}


FilterBox_c *
CanIo_c::insertFilter(
  __IsoAgLib::CanCustomer_c & ar_customer,
  const IsoAgLib::iMaskFilterType_c& arc_filterpair,
  int ai_dlcForce,
  bool ab_reconfigImmediate)
{
#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
INTERNAL_DEBUG_DEVICE << "-----------------------------------start CanIo_c::insertFilter " << INTERNAL_DEBUG_DEVICE_ENDL;
      printMyFilterBox();
#endif

  // do not allow filterbox insertion before CanIo_c initialization 
  isoaglib_assert(initialized());

  if (existFilter(ar_customer, arc_filterpair, NULL))
  { // filter,mask,cancustomer definition already inserted
    return NULL;
  }

  //look for existing mask/filter definition
  FilterBox_c* tempFilterBox_c = getFilterBox( arc_filterpair );

  if (tempFilterBox_c != NULL)
  { // filterbox with mask/filter found
    #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "filterbox mask/filter already exist -> insert cancustomer" << INTERNAL_DEBUG_DEVICE_ENDL;
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE << "mask: "<< STL_NAMESPACE::hex << arc_filterpair.getMask() << " filter: " << arc_filterpair.getFilter() << STL_NAMESPACE::dec << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
    INTERNAL_DEBUG_DEVICE << "mask: " << arc_filterpair.getMask() << " filter: " << arc_filterpair.getFilter() << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    #endif
    tempFilterBox_c->insertCustomer (&ar_customer, ai_dlcForce);
    //do not insert new filterbox because it already exists
    return tempFilterBox_c;
  }
  // now the filterBox def is new -> insert new FilterBox_c instance in array

  // use for insert a member var FilterBox_c to avoid creation of temp obj during insert
  // clear temp object
  mc_tempFilterBox.clearData();

  // define temp FilterBox_c with new values
  mc_tempFilterBox.set( arc_filterpair, &ar_customer, ai_dlcForce);

  // insert new FilterBox_c and exit function if no dyn array growth is reported
  const uint8_t b_oldSize = cntFilter();

  // #############################################
  // from now on, the CAN BUS is already in valid useable state, so that the FilterBox_c instance
  // can immediately connect to HAL
  // #############################################

  //check if an idle filterBox exist and if not insert new filterBox at the end of array
  bool filterBoxOverwrite = false;
  //search array for a filterbox that is currently not in use and therefore idle
  uint8_t ui8_overwritenFilterBoxIndex = 0;
  for(; ui8_overwritenFilterBoxIndex < cntFilter(); ui8_overwritenFilterBoxIndex++)
  {
    if ( m_arrFilterBox[ui8_overwritenFilterBoxIndex].isIdle() )
    { //if idle filterbox found overwrite it with the new filterbox
      m_arrFilterBox[ui8_overwritenFilterBoxIndex].set(arc_filterpair, &ar_customer, ai_dlcForce);

      m_arrFilterBox[ui8_overwritenFilterBoxIndex].setFbVecIdx(ui8_overwritenFilterBoxIndex);

      #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      /** when a idle FB is reused then the Full reconfiguration is necessary**/
      /** set Full reconfiguration */
      setFullReconfigNecessary();
      #endif

      #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
        m_arrFilterBox[ui8_overwritenFilterBoxIndex].setBusNumber(mui8_busNumber);
      #else
        m_arrFilterBox[ui8_overwritenFilterBoxIndex].configCan(mui8_busNumber, ui8_overwritenFilterBoxIndex + minReceiveObjNr());
      #endif

      filterBoxOverwrite = true;
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE << "overwrite FilterBox[" << int(ui8_overwritenFilterBoxIndex) << "] with busnumber: " << static_cast<int>(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "                    mask: "
      #ifdef SYSTEM_PC
      << STL_NAMESPACE::hex
      #endif
      << arc_filterpair.getMask() << " filter: " << arc_filterpair.getFilter()
      #ifdef SYSTEM_PC
      << STL_NAMESPACE::dec
      #endif
      << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      break;
    }
  }

  if (!filterBoxOverwrite)
  { //only insert filter box if bus number and msgobj number have the right range
    // no filterBox in the vector is idle -> push back
    // insert temp object in vector m_arrFilterBox -> can cause badAlloc exception
    #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "insertFilterBox with bus number: " << static_cast<int>(mui8_busNumber)  << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "                     mask: "
    #ifdef SYSTEM_PC
    << STL_NAMESPACE::hex
    #endif
    << arc_filterpair.getMask() << " filter: " << arc_filterpair.getFilter()
    #ifdef SYSTEM_PC
    << STL_NAMESPACE::dec
    #endif
    << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    m_arrFilterBox.push_back(mc_tempFilterBox);
    setCntFilter( m_arrFilterBox.size() );


    m_arrFilterBox.back().setFbVecIdx(m_arrFilterBox.size() - 1);

    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    /** set the MinChangedFilterBox */
    if(isFirstAddFilterBox())
        setMinChangedFilterBox(m_arrFilterBox.size() - 1);
    #endif

    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      m_arrFilterBox.back().setBusNumber(mui8_busNumber);
    #else
      uint8_t tempMsgObjNr = minReceiveObjNr() + b_oldSize;
      m_arrFilterBox.back().configCan(mui8_busNumber, tempMsgObjNr);
    #endif

    #if DEBUG_HEAP_USEAGE
    sui16_filterBoxTotal++;
    #endif
  }

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  // if wanted, configure everything, to set CAN hardware MsgObj_c as wanted
  // for sequence of m_arrFilterBox insertions only configure after the last insert
  if ( mui8_busNumber != 0xFF )/* when the add is made before the init CAN don't do the config, config is done at the init CAN */
  {
    if ( ab_reconfigImmediate )
    {
      reconfigureMsgObj();
    } // if (ab_configImmediate)
  }
  #else
  // don't care what the parameter was, we don't have to reconfig here!
  (void) ab_reconfigImmediate;
  #endif

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
INTERNAL_DEBUG_DEVICE << "end CanIo_c::insertFilter " << INTERNAL_DEBUG_DEVICE_ENDL;
      printMyFilterBox();
#endif

  // insertion of FilterBox_c was successfull
  if (filterBoxOverwrite == true)
    return &m_arrFilterBox[ui8_overwritenFilterBoxIndex];
  else //new filterBox inserted at the end
    return &m_arrFilterBox.back();

}


bool
CanIo_c::deleteFilter(
  const __IsoAgLib::CanCustomer_c& ar_customer,
  const IsoAgLib::iMaskFilterType_c& arc_filterpair)
{
  bool b_result = false;

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** at the deleteFilterBox the full reconfiguration is always necessary */
  setFullReconfigNecessary();
  #endif

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "------------------------CanIo_c::delete filter = 0x"
    #ifdef SYSTEM_PC
      << STL_NAMESPACE::hex
    #endif
      << arc_filterpair.getFilter()
      << " on Can bus = "
    #ifdef SYSTEM_PC
      << STL_NAMESPACE::dec
    #endif
      << int(getBusNumber())
      << ", can instance = "
      << getMultitonInst()
      << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter;

  if (existFilter(ar_customer, arc_filterpair, &pc_iter))
  { // filter found -> delete element where pc_iter points to
    if ( pc_iter->deleteFilter(ar_customer) )
    { //no more cancustomer exist for the filterbox -> delete

      // with SYSTEM_WITH_ENHANCED_CAN_HAL the FilterBox_c items are organized
      // in STL_NAMESPACE::vector<T>, so that removal of items in the middle should be avoided
      // ==> a FilterBox_c where the last Customer has been deleted switches to IDLE
      //     mode, so that it does nothing and can stay, until it is at the end of the
      //     list
      // ==>> ONLY POP FilterBox_c from BACK of STL_NAMESPACE::vector<T> to AVOID chain of FilterBox_c movements in STL_NAMESPACE::vector<T>
      //to be deleted filterbox is set to idle
      while( !m_arrFilterBox.empty() && m_arrFilterBox.back().isIdle() )
      {
        m_arrFilterBox.pop_back();
      }

      setCntFilter( m_arrFilterBox.size() );
    }

    #if DEBUG_HEAP_USEAGE
    sui16_filterBoxTotal--;

    INTERNAL_DEBUG_DEVICE
      << sui16_filterBoxTotal << " x FilterBox_c: Mal-Alloc: "
      <<  sizeSlistTWithMalloc( sizeof(FilterBox_c), sui16_filterBoxTotal )
      << "/" << sizeSlistTWithMalloc( sizeof(FilterBox_c), 1 )
      << ", Chunk-Alloc: "
      << sizeSlistTWithChunk( sizeof(FilterBox_c), sui16_filterBoxTotal )
    #ifdef MASSERT
      << "\r\n__mall tot:" << AllocateHeapMalloc
      << ", _mall deal tot: " << DeallocateHeapMalloc
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    #endif

    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    reconfigureMsgObj();
    #endif
    b_result = true;
  }
  return b_result;
}


bool
CanIo_c::deleteAllFiltersForCustomer(
  const __IsoAgLib::CanCustomer_c & ar_customer)
{
  bool b_result = false;
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  setFullReconfigNecessary();
  #endif

  for (ArrFilterBox::iterator pc_iter = m_arrFilterBox.begin(); pc_iter != m_arrFilterBox.end();)
  {
      if(pc_iter->deleteFilter(ar_customer))
      { //no more cancustomer exist for the filterbox -> delete
        b_result = true;
      }

      pc_iter++;
  }

  // the FilterBox_c items are organized
  // in STL_NAMESPACE::vector<T>, so that removal of items in the middle should be avoided
  // ==> a FilterBox_c where the last Customer has been deleted switches to IDLE
  //     mode, so that it does nothing and can stay, until it is at the end of the
  //     list
  // ==>> ONLY POP FilterBox_c from BACK of STL_NAMESPACE::vector<T> to AVOID chain of FilterBox_c movements in STL_NAMESPACE::vector<T>
  //to be deleted filterbox is set to idle
  while( !m_arrFilterBox.empty() && m_arrFilterBox.back().isIdle() )
  {
    m_arrFilterBox.pop_back();
  }

  // update the cached FilterBox_c cnt
  setCntFilter( m_arrFilterBox.size() );

  if (b_result)
  { // at least one filter/customer was removed
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    reconfigureMsgObj();
    #endif
  }

  return b_result;
}


int16_t
CanIo_c::processMsg()
{
  if (mui8_busNumber == 0xFF)
    return -1;
  // --> detect and avoid CAN message processing loops
  if ( mb_runningCanProcess )
    return -1;
  mb_runningCanProcess = true;
  mui8_processedMsgCnt = 0;

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
#  if DEBUG_CAN_BUFFER_FILLING
  bool b_detectedOverflow = false;
#  endif
#endif

  int32_t i32_fbIdx = 0;
  uint32_t i32_ident = 0;
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
uint32_t ui32_msgNbr = 0;
#endif

  Ident_c::identType_t identType(Ident_c::StandardIdent);
  int32_t i32_retVal = HAL_NO_ERR;
  bool b_processed = false;
  bool to_be_processed = false;
  bool b_forceProcessAll= false;


  while(i32_retVal != HAL_UNKNOWN_ERR ) // something has been received from CAN
  {
      System_c::triggerWd();

    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      // read the FbIdx and the mi32_lastProcessedCanPkgTime (used for the reconfigure)
      i32_retVal = HAL::iFifoReadFbIdx(mui8_busNumber, i32_fbIdx, mi32_lastProcessedCanPkgTime, i32_ident,identType);

    #else
      uint8_t ui8_identType = 0;
      i32_retVal = HAL::can_useNextMsgobjNumber(mui8_busNumber, ui32_msgNbr, i32_ident, ui8_identType, mi32_lastProcessedCanPkgTime);
      identType = static_cast<Ident_c::identType_t>(ui8_identType);
      i32_fbIdx = ui32_msgNbr-minReceiveObjNr();
    #endif

      b_processed = false;
      to_be_processed = false;

      switch (i32_retVal)
      {
        case HAL_NO_ERR:
          to_be_processed = true;
          break;
        case HAL_RANGE_ERR:
          getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
          #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
          INTERNAL_DEBUG_DEVICE << "CAN-Receive Range Err" << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          break;
        case HAL_CONFIG_ERR:
          #if DEBUG_CAN_BUFFER_FILLING || DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
          INTERNAL_DEBUG_DEVICE << "\r\nBUS not initialized or wrong BUS nr: " << uint16_t(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
          break;
        case HAL_NOACT_ERR:
          #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
          INTERNAL_DEBUG_DEVICE << "CAN-Receive NoAct Err" << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          // wrong use of MsgObj (not likely) or CAN BUS OFF
          getILibErrInstance().registerError( iLibErr_c::CanOff, iLibErr_c::Can );
          break;
        case HAL_WARN_ERR:
          to_be_processed = true;
          getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
          break;
        case HAL_OVERFLOW_ERR:
          to_be_processed = true;
          getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );

          #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
          HAL::can_stateMsgobjOverflow(mui8_busNumber, i32_ident );

            #if DEBUG_CAN_BUFFER_FILLING
            if ( ! b_detectedOverflow )
            {
              INTERNAL_DEBUG_DEVICE << "\r\nALARM!!!!!! CAN Buffer Overflow at MsgObj: "
                << uint16_t(ui32_msgNbr) << " at BUS: " << uint16_t(mui8_busNumber)
                << " with Ident: " << i32_ident
                << INTERNAL_DEBUG_DEVICE_ENDL;
            }
            b_detectedOverflow = true;
            #endif
          #endif
          b_forceProcessAll = true;
          break;
        case HAL_UNKNOWN_ERR:
        // no message to read
        // don't process message below, keep "to_be_processed == false"
        break;
        default:
         to_be_processed = true;
         getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
        break;
      }

      if (to_be_processed)
      { // found a message to be processed
        /**
        * Possible cases :
        * - i32_fbIdx = -1 --> a reconfiguration was ongoing at the reception of the message, so the suitable FB should be found here
        *                     among all created FB
        * - i32_fbIdx > = m_arrFilterBox.size() --> the FilterBox that wanted the message has been canceled, so discard the message
        * - i32_fbIdx has a valid range, but all the data of the FilterBox are cleared, because it has been canceled.
        *     so the FilterBox is Idle
        */

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
        if(mi32_lastProcessedCanPkgTime < mi32_endLastReconfigTime )
        {
          INTERNAL_DEBUG_DEVICE << " During reconfiguration, received msg : "
                  #ifdef SYSTEM_PC
                  << STL_NAMESPACE::hex
                  #endif
            << i32_ident << INTERNAL_DEBUG_DEVICE_ENDL;
        }
  #endif
        if (mi32_lastProcessedCanPkgTime > mi32_endLastReconfigTime )
        { // the message has not arrived before the last reconfiguration,check on all the FB
#endif
          if(i32_fbIdx >= 0 && i32_fbIdx < static_cast<int32_t>(m_arrFilterBox.size()))
          {
            if(!(m_arrFilterBox[i32_fbIdx].isIdle())  &&  m_arrFilterBox[i32_fbIdx].maskFilterPair().matchMsgId( Ident_c( i32_ident, identType ) ))
            {
              // !! the interested FilterBox processes the message
              #if 0 //#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
                INTERNAL_DEBUG_DEVICE << " CONSUMING FB IDX :"
                #ifdef SYSTEM_PC
                << STL_NAMESPACE::dec
                #endif
                << m_arrFilterBox[i32_fbIdx].getFbVecIdx();
                INTERNAL_DEBUG_DEVICE << ", FB IDX READ from CAN : "
                #ifdef SYSTEM_PC
                << STL_NAMESPACE::dec
                #endif
                << i32_fbIdx;
                INTERNAL_DEBUG_DEVICE << " interested in the MSG Id : 0x"
                #ifdef SYSTEM_PC
                << STL_NAMESPACE::hex
                #endif
                << i32_ident << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
              b_processed = m_arrFilterBox[i32_fbIdx].processMsg();
              if(b_processed)
              {
                mui8_processedMsgCnt++;
              }
            }
          }
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
        }
#endif

// try to search all FilterBox_c instances
        if ( ! b_processed)
        {
#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
          INTERNAL_DEBUG_DEVICE << " CanIo_c::processMsg --- search among all the FilterBox " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
          CanIo_c::ArrFilterBox::iterator pc_iFilterBox;
          bool b_foundOne=false, b_initial=true;

              do {
                b_foundOne = canMsg2FilterBox( i32_ident, identType, pc_iFilterBox, b_initial );
                b_initial = false; // change, so that next call continues search in canMsg2FilterBox

                if ( b_foundOne )
                { // matching instance found
                  #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
                  INTERNAL_DEBUG_DEVICE<< " Found Fb with index :"
                  #ifdef SYSTEM_PC
                  << STL_NAMESPACE::dec
                  #endif
                  << pc_iFilterBox->getFbVecIdx();
                  INTERNAL_DEBUG_DEVICE << " interested in the MSG Id : 0x"
                  #ifdef SYSTEM_PC
                  << STL_NAMESPACE::hex
                  #endif
                  << i32_ident << INTERNAL_DEBUG_DEVICE_ENDL;
                  #endif

                  HAL::wdTriggern();
                  b_processed = pc_iFilterBox->processMsg();
                  if(b_processed)
                  {
                    mui8_processedMsgCnt++;
                  }
                }
              } while (b_foundOne && !b_processed); // if one found, try if another one can be found!
#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      if(!b_processed)
        INTERNAL_DEBUG_DEVICE << " NO Customer interested in the MSG Id : 0x"
		#ifdef SYSTEM_PC
        << STL_NAMESPACE::hex
		#endif
        << i32_ident << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

          }
         /** consume the message in the FIFO : increment the index Ac +=2 only whether a message has been read (ret==true)*/
        #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
        HAL::iFifoPopMessage(mui8_busNumber);
        #endif
       }// ret== true

    #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    HAL::can_useMsgobjPopFront(mui8_busNumber, ui32_msgNbr);
    #endif

    if((i32_retVal == HAL_RANGE_ERR) || (i32_retVal == HAL_CONFIG_ERR) || (i32_retVal == HAL_NOACT_ERR))
    {
      mb_runningCanProcess = false;
      return mui8_processedMsgCnt;
    }

    if (( Scheduler_Task_c::getAvailableExecTime() == 0 ) && (!b_forceProcessAll))
    { // switch the flag back, so that further processings are enabled
      mb_runningCanProcess = false;
      return -1;
    }
  }//while

  // switch the flag back, so that further processings are enabled
  mb_runningCanProcess = false;
  // return the number of received telegrams
  return mui8_processedMsgCnt;
}


CanIo_c&
CanIo_c::operator<<( CanPkg_c& acrc_src )
{ // immediately return if CAN is not yet configured for send
  // this is not a valid case! --> assert!!
  isoaglib_assert (isReady2Send());
  if ( ! isReady2Send() )
    return *this;

  // set send MsgObj ID
  uint8_t ui8_sendObjNr = minHALMsgObjNr();
  uint32_t i32_now = HAL::getTime();

  // wait till Msg can be placed in send buffer
  while ( HAL::can_stateMsgobjFreecnt( mui8_busNumber, ui8_sendObjNr ) < 1 )
  { // perform wait loop
    HAL::wdTriggern();

    // don't do it with busy waiting, yield 1ms, less or not at all!
    HAL::sleep_max_ms( 1 );

    /**
      * redmine ticket 69.
      * we wait for CONFIG_CAN_BLOCK_TIME ms if the send-queue is opening for this one package.
      * if we cannot send out the message within CONFIG_CAN_BLOCK_TIME ms we assume that the
      * bus is in an error situation (bus-off or bus-passive) and no messages can be sent out.
      * In that case we clear the send buffer and try to insert the most recent message.
      **/
    if ( ( HAL::getTime() - i32_now ) > CONFIG_CAN_BLOCK_TIME )
    {
      // clear MsgObj CAN queue
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE
       << "CanIo_c::operator<< can package could not be passed to can-hardware for 10 seconds. Emptying queue and inserting new package. Error-Bus: " << uint16_t(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      // typically we're in BUS-WARN here, so the messages won't get sent out,
      // that's why we overflow the send-buffer...
      getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
      HAL::can_useMsgobjClear(mui8_busNumber,ui8_sendObjNr);
      break;
    }
  }
  HAL::wdTriggern();

  #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  doDebug(mui8_busNumber, ui8_sendObjNr);
  #endif

  int16_t i16_sendFuncState = HAL::can_useMsgobjSend(mui8_busNumber, ui8_sendObjNr, &acrc_src);

  #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  doDebug(mui8_busNumber, ui8_sendObjNr);
  #endif

  // it's time to trigger the watchdog
  HAL::wdTriggern();
  // BIOS CAN send function with lowest configured msg-obj, which was
  // configured for send during init
  switch (i16_sendFuncState)
  {
    case HAL_NO_ERR:
      // everything o.k.
      break;
    case HAL_CONFIG_ERR:
      // no send obj or BUS not initialized -> should not happen, cause this is done
      // on init of this object
      // (and is done if timeEvent notices that CAN wasn't configured by init)
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
        INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(mui8_busNumber) << " not initialized or MsgObj: " << uint16_t(ui8_sendObjNr) << " no send obj" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
      break;
    case HAL_NOACT_ERR:
      // BUS off
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(mui8_busNumber) << " in BUS OFF STATE" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::CanOff, iLibErr_c::Can );
      break;
    case HAL_OVERFLOW_ERR:
      // overflow of send buffer
      // With the current max-10ms-block strategy this case shouldn't be reached anymore.
      getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
      break;
    case HAL_RANGE_ERR:
      // BUS nr or obj nr outer allowed limits (shouldn't be the case after successful init call)
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
    case HAL_WARN_ERR:
      // signal for BUS-WARN problem
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(mui8_busNumber) << " in WARN STATE" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
      break;

   #ifdef USE_CAN_SEND_DELAY_MEASUREMENT
    case HAL_NEW_SEND_DELAY:
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(mui8_busNumber) << " encountered new Maximum Send Delay.. \n";
      #endif

      if (mi32_maxSendDelay >= 0)
      { // we're in mode: Test-MaxSendDelay
        if (HAL::can_getMaxSendDelay(mui8_busNumber) > mi32_maxSendDelay)
        { // this is way too max, register an error!
          getILibErrInstance().registerError( iLibErr_c::CanTooSlowSend, iLibErr_c::Can );
      #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
          INTERNAL_DEBUG_DEVICE << ".. which has now registered an ERROR as it was higher (it was "<< HAL::can_getMaxSendDelay(mui8_busNumber) <<") than our trigger!" << INTERNAL_DEBUG_DEVICE_ENDL;
        }
        else INTERNAL_DEBUG_DEVICE << ".. but this " << HAL::can_getMaxSendDelay(mui8_busNumber) << " was not yet higher than our trigger!" << INTERNAL_DEBUG_DEVICE_ENDL;
      }
      else INTERNAL_DEBUG_DEVICE << ".. but ignored, as app didn't define a maxDelay-Trigger! " << INTERNAL_DEBUG_DEVICE_ENDL;
      #else
        }
      }
      #endif
      break;
	#endif

  } //switch

  // return pointer to self for sequence of send operations
  return *this;
}


#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
void
CanIo_c::getCommonFilterMask()
{
  // preset all bits with "1"
  mc_maskStd.set(~0, Ident_c::StandardIdent);
  mc_maskExt.set(~0, Ident_c::ExtendedIdent);

  // combine masks of all m_arrFilterBox with AND
  for (ArrFilterBox::iterator pc_iter = m_arrFilterBox.begin();
       pc_iter != m_arrFilterBox.end();
       ++pc_iter)
  {
    if (!pc_iter->isIdle())
    {
      if (pc_iter->maskFilterPair().getType() == Ident_c::StandardIdent) {
        mc_maskStd.ident_bitAnd(Ident_c( pc_iter->maskFilterPair().getMask(), pc_iter->maskFilterPair().getType() ));
      }
      else {
        mc_maskExt.ident_bitAnd(Ident_c( pc_iter->maskFilterPair().getMask(), pc_iter->maskFilterPair().getType() ));
      }
    }
  }
}


void
CanIo_c::getCommonFilterMaskAfterMerge()
{
  // combine masks of all m_arrFilterBox with AND
  for (ArrMsgObj::iterator pc_iter = marr_msgObj.begin();
       pc_iter != marr_msgObj.end();
       pc_iter++
      )
  {
    if (pc_iter->filter().identType() == Ident_c::StandardIdent)
      pc_iter->commonFilterAfterMerge( mc_maskStd );
    else pc_iter->commonFilterAfterMerge( mc_maskExt );
  }
}


int16_t
CanIo_c::FilterBox2MsgObj()
{
  int16_t i16_result = 0;
  Ident_c c_tempIdent( 0, IsoAgLib::iIdent_c::ExtendedIdent );
  ArrMsgObj::iterator pc_iterMsgObj, pc_search4MsgObjReuse = marr_msgObj.begin();

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE << " CanIo_c::FilterBox2MsgObj "<< INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  ArrFilterBox::iterator pc_iterFilterBox = m_arrFilterBox.begin();

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  const uint16_t ui16_debugOldSize = cntMsgObj();
#endif

  if(isFullReconfigNecessary())
  {
  /** clear  the arrPfilterBox for each MsgObj */
    for (pc_iterMsgObj = marr_msgObj.begin(); pc_iterMsgObj != marr_msgObj.end(); pc_iterMsgObj++)
    {
        HAL::wdTriggern();
        pc_iterMsgObj->clearArrFbIdx();
    }
#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE << " CanIo_c::FilterBox2MsgObj  - FULL RECONFIGURATION on BUS number = "<< int(getBusNumber())
    << ", can instance = " << getMultitonInst() << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  }
  else //if the full reconfiguration is not necessary, the pc_iterFilterBox start from the last FilterBox added
  {
#ifndef WIN32
   pc_iterFilterBox = static_cast <ArrFilterBox::iterator> (&m_arrFilterBox[getMinChangedFilterBox()]);
#else
	  pc_iterFilterBox = m_arrFilterBox.begin();
	  for (uint32_t ui32_loopFilerBox = 0; ui32_loopFilerBox < getMinChangedFilterBox(); ui32_loopFilerBox++) pc_iterFilterBox++;
#endif
   pc_search4MsgObjReuse = marr_msgObj.end(); // indicate that NO existing MsgObj_c is available for reuse

  }

  HAL::wdTriggern();

  /** rebuild the arrPfilterBox for each MsgObj */
  // gather globalMask dependent filter bits of each m_arrFilterBox
  // and create suitable marr_msgObj if not already created
  for (; pc_iterFilterBox != m_arrFilterBox.end();
       ++pc_iterFilterBox)
  {
    if (!pc_iterFilterBox->isIdle())
    {
        // get filter Ident_c setting from actual FilterBox
        c_tempIdent.set( pc_iterFilterBox->maskFilterPair().getFilter(), pc_iterFilterBox->maskFilterPair().getType() );
        // mask the actual filter Ident_c with the bits of the global mask
        if (c_tempIdent.identType() == Ident_c::StandardIdent)
          c_tempIdent.ident_bitAnd(mc_maskStd);
        else c_tempIdent.ident_bitAnd(mc_maskExt);

        for (pc_iterMsgObj = marr_msgObj.begin(); pc_iterMsgObj != pc_search4MsgObjReuse; pc_iterMsgObj++)
        {
          if (pc_iterMsgObj->equalFilter(c_tempIdent)) break;
        }
        // if no item with this filter was found add new MsgObj
        if (pc_iterMsgObj == pc_search4MsgObjReuse)
        {
          // no equal item found in marr_msgObj -> create new MsgObj_c in marr_msgObj
          if (pc_search4MsgObjReuse != marr_msgObj.end())
          {
            // the marr_msgObj for this filter setting was
            // already previous created, can now be reused

            // set filter in marr_msgObj
            // new last element to be configured after actual last element
            pc_search4MsgObjReuse->setFilter(c_tempIdent);
#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
            pc_search4MsgObjReuse->setCanSingletonKey(getMultitonInst());
#endif
            pc_search4MsgObjReuse->insertFilterBox(pc_iterFilterBox);
            pc_search4MsgObjReuse++;
          }
          else
          {
            // previous less marr_msgObj was created -> create new marr_msgObj
            // clear temp obj (it is reused for each function call)
            mc_tempObj.close();
            // set filter in new pc_lt_element
            mc_tempObj.setFilter(c_tempIdent);
            mc_tempObj.insertFilterBox(pc_iterFilterBox);

#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
            mc_tempObj.setCanSingletonKey(getMultitonInst());
#endif
            mc_tempObj.setBusNumber(mui8_busNumber);

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
            INTERNAL_DEBUG_DEVICE << "create a new object with bus Number " << int(getBusNumber())
            << " , Filter = 0x"
#ifdef SYSTEM_PC
            << STL_NAMESPACE::hex
#endif
            << c_tempIdent.ident() << ", can Instance = "<< getMultitonInst()
            << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

            // insert obj in vector
            marr_msgObj.push_front(mc_tempObj);
            setCntMsgObj( marr_msgObj.size() );
#if DEBUG_HEAP_USEAGE
            sui16_msgObjTotal++;
#endif
            pc_search4MsgObjReuse = marr_msgObj.end();

          }
        }
        else
        {
          // same filter definition at position q already inserted in msgOb vector
          // insert new FilterBox_c reference
          pc_iterMsgObj->insertFilterBox(pc_iterFilterBox);
        }


    }
  }
  // if less objects are created than already in vector, erase the unused
  // -> behind the last instance is NOT the list end
  if (pc_search4MsgObjReuse!= marr_msgObj.end())
  {
#if DEBUG_HEAP_USEAGE
    const uint16_t cui16_oldSize = cntMsgObj();
#endif
    HAL::wdTriggern();
    marr_msgObj.erase(pc_search4MsgObjReuse, marr_msgObj.end());
    setCntMsgObj( marr_msgObj.size() );
#if DEBUG_HEAP_USEAGE
    sui16_msgObjTotal -= ( cui16_oldSize - cntMsgObj() );
    sui16_deconstructMsgObjCnt += (cui16_oldSize - cntMsgObj() );
#endif
  }

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE << "Old number MsgObj = "
#ifdef SYSTEM_PC
  << STL_NAMESPACE::dec
#endif
  << ui16_debugOldSize << ", New size = " << cntMsgObj() << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  if ( i16_result >= 0) i16_result = cntMsgObj();
#if DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << "MsgObj: " << sui16_msgObjTotal << " -> ";
#endif
  HAL::wdTriggern();

  return i16_result;

}


void
CanIo_c::CheckSetCntMsgObj()
{
  // set i16_minDistance to the max possible bit difference
  // => amount of bits in data type for ident
#ifdef NO_8BIT_CHAR_TYPE
  int16_t i16_minDistance = sizeof(MASK_TYPE)*16;
#else
  int16_t i16_minDistance = sizeof(MASK_TYPE)*8;
#endif
  int16_t i16_tempDist;
  // maxHALMsgObjNr() delivers also the special lastMsgObj, which cannot be used for normal receiveMsgObj
  // ==> subtract from (max - min + 1 ) again one item
  uint8_t ui8_allowedSize = maxHALMsgObjNr() - minReceiveObjNr() + 1 - 1;

  unsigned int ui_lsbDiffMin = 0;
  unsigned int ui_lsbDiffTemp = 0;

  // before any preparation of further merge work - check whether we have to merge anyway
  if (cntMsgObj() <= ui8_allowedSize) return;

  // NOW WE HAVE TO MERGE AT LEAST TWO MsgObj_c together
#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
INTERNAL_DEBUG_DEVICE << " CanIo_c::--------------------Before Merge " << INTERNAL_DEBUG_DEVICE_ENDL;
  printMsgObjInfo();

#endif

  ArrMsgObj::iterator pc_minLeft = marr_msgObj.begin(),
          pc_minRight = marr_msgObj.begin();

  Ident_c* pc_useGlobalMask = NULL;

  // [min;max] allowed, but first one or two reserved for send
  bool b_continueMerging = true;
  while (b_continueMerging)
  { // more objects than allowed are in marr_msgObj, because intervall between
    // min+1 and max is smaller than size -> shrink array
    if (cntMsgObj() <= ui8_allowedSize) b_continueMerging = false;

		//process the message arrived during the reconfiguration
    if(HAL::isFifoCriticalFilled(mui8_busNumber))
  	processMsg();

    // start a comparison(left_ind, right_ind) loop for all elements
    for (ArrMsgObj::iterator pc_leftInd = marr_msgObj.begin(); pc_leftInd != marr_msgObj.end(); pc_leftInd++)
    {
      ArrMsgObj::iterator pc_rightInd = pc_leftInd;
      for (pc_rightInd++; pc_rightInd != marr_msgObj.end(); pc_rightInd++)
      {
        // retreive bit distance between instances left_ind and right_ind -> use Bit-XOR
        i16_tempDist = pc_leftInd->filter().bitDiff(pc_rightInd->filter(), ui_lsbDiffTemp);

        // a) store new min only if capacity of left is enough for objects of right
        // b) if bitDiff equal => prefere filters with differ in the most significant bits
        // c) prefere diffs in the two upper bytes even when bittDiff is not optimal
        if (((i16_tempDist < i16_minDistance) ||
             ((i16_tempDist == i16_minDistance) && (ui_lsbDiffTemp > ui_lsbDiffMin)) ||
             ((i16_tempDist-1 == i16_minDistance) && (ui_lsbDiffTemp > ui_lsbDiffMin) && (ui_lsbDiffMin<16) && (ui_lsbDiffTemp>=16)))
          && (pc_leftInd->filter().identType() == pc_rightInd->filter().identType())
           )
        {
          // new max similar filter found
          pc_minLeft = pc_leftInd;
          pc_minRight = pc_rightInd;
          i16_minDistance = i16_tempDist;
          ui_lsbDiffMin = ui_lsbDiffTemp;
          if ( i16_tempDist == 0 )
          {
            b_continueMerging = true; // merge the compared MsgObj_c IN ANY CASE, as they are equal
            break; // stop searching for "smaller" difference if 0 is found
          }
        }
      }
    }
    if ( b_continueMerging )
    { // we have to merge the found min bit difference because:
      // a) amount of MsgObj_c still too large
      // b) a pair of MsgObj_c with EQUAL (BitDiff of zero) has been found

      // now min_dist is minimal bit distance of most similar filters at i16_minLeft and
      // i16_minRight -> merge them
      // merge  right filter into the left
      pc_useGlobalMask = &mc_maskExt;
      mc_maskExt.set( mc_maskExt.ident() & (~( pc_minLeft->filter().ident() ^ pc_minRight->filter().ident() ) ), Ident_c::ExtendedIdent );

      HAL::wdTriggern();

      pc_minLeft->merge(*pc_minRight);
      marr_msgObj.erase(pc_minRight);
      setCntMsgObj( marr_msgObj.size() );
#if DEBUG_HEAP_USEAGE
      sui16_msgObjTotal--;
      sui16_deconstructMsgObjCnt++;
#endif
      // update the filters in the filters in the existing MsgObj_c to the changed mask
      for (ArrMsgObj::iterator c_iter = marr_msgObj.begin(); c_iter != marr_msgObj.end(); c_iter++)
      {
        (*c_iter).updateFilterWithMask( *pc_useGlobalMask );
      }


      // reset search arguments for posible next search
      pc_minRight = pc_minLeft = marr_msgObj.begin();
#ifdef NO_8BIT_CHAR_TYPE
      i16_minDistance = sizeof(MASK_TYPE)*16;
#else
      i16_minDistance = sizeof(MASK_TYPE)*8;
#endif
    }
  }

  // now the amount of marr_msgObj is allowed
#if DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << sui16_msgObjTotal << " x MsgObj_c: Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(MsgObj_c), sui16_msgObjTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(MsgObj_c), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(MsgObj_c), sui16_msgObjTotal )
#ifdef MASSERT
    << "\r\n__mall tot:" << AllocateHeapMalloc
    << ", _mall deal tot: " << DeallocateHeapMalloc
    << INTERNAL_DEBUG_DEVICE_ENDL;
#else
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
#endif

   HAL::wdTriggern();
}


bool
CanIo_c::registerChangedGlobalMasks(void)
{
  int16_t i16_retvalInit = HAL::can_configGlobalMask(
    mui8_busNumber,
    mc_maskStd.ident(),
    mc_maskExt.ident(),
    mc_maskLastmsg.ident());

  // check for error state
  if (i16_retvalInit == HAL_RANGE_ERR)
  { // BIOS complains about limits of BUSnr or msgObj
    // seldom, because before checked with defined LIMITS
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  else
  {
    return true;
  }
}
#endif //end not defined SYSTEM_WITH_ENHANCED_CAN_HAL


bool
CanIo_c::canMsg2FilterBox(
  uint32_t aui32_ident,
  Ident_c::identType_t at_type,
  ArrFilterBox::iterator& ar_arrFilterBoxIter,
  bool ab_start)
{
  if (ab_start)
  { // init
    ar_arrFilterBoxIter = m_arrFilterBox.begin();
  }
  else
  {
    ar_arrFilterBoxIter++;
  }

  while (ar_arrFilterBoxIter != m_arrFilterBox.end())
  {
    if (!ar_arrFilterBoxIter->isIdle())
    {
      if ( ar_arrFilterBoxIter->maskFilterPair().matchMsgId( Ident_c( aui32_ident, at_type ) ) )
      { // matching FilterBox_c found
        return true;   
      }
    }
    ar_arrFilterBoxIter++;
  }
  // if execution reaches this point, no matching FilterBox_c found
  return false;
}


FilterBox_c*
CanIo_c::getFilterBox( const IsoAgLib::iMaskFilterType_c& arc_maskFilter ) const
{
  for(uint8_t i = 0; i < cntFilter(); i++)
  {
    if( m_arrFilterBox[i].equalFilterMask( arc_maskFilter ) )
      return const_cast<FilterBox_c*>(&m_arrFilterBox[i]);
  }

  //filterbox with mask, filter not found
  return NULL;
}


bool
CanIo_c::verifyBusMsgobjNr(
  uint8_t aui8_busNr, 
  uint8_t aui8_objNr)
{
  //check if there is an error
  if (
     (aui8_busNr > HAL_CAN_MAX_BUS_NR)
  || (aui8_objNr < HAL_CAN_MIN_REC_OBJ)
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  || (aui8_objNr > HAL_CAN_MAX_REC_OBJ)
  #endif
     )
  {
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  return true;
}


bool
CanIo_c::reconfigureMsgObj()
{ // verify that this instance is yet initialised complete (CAN BUS is dedicated)
  #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  // no msg objects exist only filterboxes and no register restrictions therefore no reconfigure must be done
  return true;
  #else
  if ( mui8_busNumber > HAL_CAN_MAX_BUS_NR )
  {
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }

  #if DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << sui16_filterBoxTotal << " x FilterBox_c: Mal-Alloc: "
    <<  sizeSlistTWithMalloc( sizeof(FilterBox_c), sui16_filterBoxTotal )
    << "/" << sizeSlistTWithMalloc( sizeof(FilterBox_c), 1 )
    << ", Chunk-Alloc: "
    << sizeSlistTWithChunk( sizeof(FilterBox_c), sui16_filterBoxTotal )
  #ifdef MASSERT
    << "\r\n__mall tot:" << AllocateHeapMalloc
    << ", _mall deal tot: " << DeallocateHeapMalloc
    << INTERNAL_DEBUG_DEVICE_ENDL;
  #else
    << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  #endif

  //process the message arrived during the reconfiguration
  if(HAL::isFifoCriticalFilled(mui8_busNumber))
    processMsg();

  bool b_result = true;
  // store old mask to check if global CAN BIOS mask must be changed
  Ident_c oldMaskStd = mc_maskStd,
        oldMaskExt = mc_maskExt;

  // build suitable global t_mask
  getCommonFilterMask();


  if ((oldMaskStd != mc_maskStd) || (oldMaskExt != mc_maskExt))
  {
      setFullReconfigNecessary();
  }

  uint16_t ui32_oldMsgBoxArrsize = marr_msgObj.size();


/** 1. extend / add MsgObj */

  // create according to new global t_mask all MsgObj_c with
  // unique filter settings -> merge all filter settings where
  // the the global t_mask delets all different bits

  if ( FilterBox2MsgObj() < 0) b_result = false;



/** 2.eventually merge MsgObj */
 // shrink number of MsgObj to fit the intervall of
  // [min_msgObjNr; maxMsgObjNr]

    CheckSetCntMsgObj();

    //process the message arrived during the reconfiguration
    if(HAL::isFifoCriticalFilled(mui8_busNumber))
      processMsg();

  // check and correct global masks after merge in CheckSetCntMsgObj()
  getCommonFilterMaskAfterMerge();

  if ((oldMaskStd != mc_maskStd) || (oldMaskExt != mc_maskExt))
  {
#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE << "CAN BUS number ="
    #ifdef SYSTEM_PC
      << STL_NAMESPACE::dec
    #endif
      << int(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "New Global  mask =0x"
    #ifdef SYSTEM_PC
      << STL_NAMESPACE::hex
    #endif
      << mc_maskExt.ident() << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "OLD Global  mask =0x"
    #ifdef SYSTEM_PC
      << STL_NAMESPACE::hex
    #endif
      << oldMaskExt.ident() << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    // global t_mask setting has changed -> reinit CAN with new global masks
    registerChangedGlobalMasks();
  }

 /** set the min num Received obj for a can instance */
  HAL::setIrqMinReceiveObjNr(mui8_busNumber,minReceiveObjNr());

/** 3. Free the IRQ table,  all the received CAN messages are written in the FIFO */
  HAL::freeIrqTable(mui8_busNumber);


/** 4. Configure the last CAN MsgOBj if not already open*/
  if ( !mc_lastMsgObj.isOpen() )
  {
    Ident_c tmp ( 0, Ident_c::ExtendedIdent );
    // configure the last MsgObj_c
    mc_lastMsgObj.setFilter( tmp );
    mc_lastMsgObj.configCan(mui8_busNumber, HAL_CAN_LAST_MSG_OBJ_NR );
  }

/** 5. update IRQ table for each CAN MsgObj */

  uint16_t i = 0;
  bool b_retUpdTbl = false;

  // config the hardware -> config all MsgObj_c in CAN hardware
  for (ArrMsgObj::iterator pc_iterMsgObj = marr_msgObj.begin();
       pc_iterMsgObj != marr_msgObj.end();
       pc_iterMsgObj++
      )
  {

   /** reinitialize the each AN MSGOBJ : CLOSE and OPEN with the new mask*/
    pc_iterMsgObj->setIsOpen(false);

    if (HAL::can_configMsgobjClose(mui8_busNumber, i+minReceiveObjNr() ) == HAL_RANGE_ERR)
    { // given BUS or MsgObj number is wrong
        getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    }

    // call the configCan function for each MsgObj
    pc_iterMsgObj->configCan(mui8_busNumber, i+minReceiveObjNr());

    b_retUpdTbl = pc_iterMsgObj->msgObjUpdateTable(mui8_busNumber,i);

    if(false == b_retUpdTbl)
    {
      IsoAgLib::getILibErrInstance().registerError( IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::Process );
    }

    i++;
  }

/** 6. close the CAN obj not used anymore */
/** the new marr_msgObj has less element then the old one, close the CAN MsgObj not anymore necessary */

   for(;i < ui32_oldMsgBoxArrsize; i++)
   {
    if (HAL::can_configMsgobjClose(mui8_busNumber, i+minReceiveObjNr() ) == HAL_RANGE_ERR)
    { // given BUS or MsgObj number is wrong
        getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    }
    #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "Close can object num = "<< (i+minReceiveObjNr() )<< " on can bus number =" << int(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
   }

/** 7. close the Last Msg Obj */
    mc_lastMsgObj.close();
    mc_lastMsgObj.closeCan();

/** reinit the variable for the Full reconfiguration */
    initMinChangedFilterBox();

    setEndLastReconfigTime();

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
INTERNAL_DEBUG_DEVICE << " CanIo_c::reconfigureMsgObj" << INTERNAL_DEBUG_DEVICE_ENDL;
  printMsgObjInfo();

INTERNAL_DEBUG_DEVICE << "-------------------------------------IRQ TABLE " << INTERNAL_DEBUG_DEVICE_ENDL;

  HAL::printIrqTable(mui8_busNumber);

#endif

  // clear any CAN BUFFER OVERFLOW error that might occure
  // for last message object
  getILibErrInstance().clear( iLibErr_c::CanOverflow, iLibErr_c::Can );

  HAL::wdTriggern();

  return b_result;
  #endif //SYSTEM_WITH_ENHANCED_CAN_HAL
}


uint8_t
CanIo_c::updateMinReceiveObjNr()
{
  mui8_minReceiveObjNr = 1 + minHALMsgObjNr();
  return mui8_minReceiveObjNr;
}


bool
CanIo_c::setBitrate(
  uint16_t aui16_newSpeed, 
  bool ab_force)
{
  // don't reconfigure if same bitrate
  if ((aui16_newSpeed == mui16_bitrate)&&(ab_force == false)) return true;

  bool b_success = true;

  // init CAN with new speed (it checks if bitrate is allowed and closes can first
  if (baseCanInit(aui16_newSpeed) == false) return false;

  // config the MsgObj with same filter but with new speed
  int16_t i = 0;

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  // config the hardware -> config all MsgObj_c in CAN hardware
  for (ArrMsgObj::iterator pc_iterMsgObj = marr_msgObj.begin();
       pc_iterMsgObj != marr_msgObj.end();
       pc_iterMsgObj++
      )
  { // call the configCan function for each MsgObj
    pc_iterMsgObj->configCan(mui8_busNumber, i+minReceiveObjNr() );
    i++;
  }
  #else
  // config the hardware -> config all FilterBox_c in CAN hardware
  for (ArrFilterBox::iterator pc_iterFilterBox = m_arrFilterBox.begin();
       pc_iterFilterBox != m_arrFilterBox.end();
       pc_iterFilterBox++
      )
  { // call the configCan function for each FilterBox
    pc_iterFilterBox->configCan(mui8_busNumber, i+minReceiveObjNr() );
    i++;
  }
  #endif
  return b_success;
}


CanIo_c::CanIo_c( void )
  : Subsystem_c(),
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    marr_msgObj(),
    mc_tempObj(),
    mc_lastMsgObj(),
    mt_msgObjCnt(0),
    mi32_minChangedFilterBox(0),
  #endif
    m_arrFilterBox(),
    mt_filterBoxCnt(0),
    mc_tempFilterBox(),
    mi32_maxSendDelay(-1),
    mi32_lastCanCheck(0),
    mi32_lastProcessedCanPkgTime(0),
    mui16_bitrate(0),
    mi32_endLastReconfigTime(0),
    mc_maskStd( 0, Ident_c::StandardIdent ),
    mc_maskExt( 0, Ident_c::ExtendedIdent ),
    mc_maskLastmsg( 0, Ident_c::ExtendedIdent ),
    mui8_busNumber(0xFF),
    mui8_minmsgObjLimit(0),
    mui8_minReceiveObjNr(0),
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    mui8_maxMsgObjLimit(0),
  #endif
    mui8_processedMsgCnt(0),
    mb_runningCanProcess(false)
{}


bool
CanIo_c::baseCanInit (uint16_t aui16_bitrate)
{
  HAL::wdTriggern();
  // check if aui16_bitrate is one of the allowed settings
  int16_t pi16_allowed[] = HAL_CAN_BITRATE_LIST;
  bool b_allowed = false;

  for (uint8_t ui8_index = 0; ui8_index < HAL_CAN_BITRATE_CNT; ui8_index++)
  {
    if (pi16_allowed[ui8_index] == aui16_bitrate)
    { // given bitrate is in allowed list at position ui8_index
      b_allowed = true; // store success state
      break; // exit search loop
    }
  }
  // if given bitrate fits to no allowed value set a range error state
  if (!b_allowed)
  {
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false; // exit function with error value
  }

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  bool b_lastMsgObjWasOpen = false;
  if ( mc_lastMsgObj.isOpen() )
  {
    mc_lastMsgObj.close();
    b_lastMsgObjWasOpen = true;
  }
  #else
  for (uint8_t i = 0; i < cntFilter(); i++)
    m_arrFilterBox[i].closeHAL();
  #endif

  // store wanted bitrate for CAN communication
  mui16_bitrate = aui16_bitrate;

  // update the receive Filter offset
  updateMinReceiveObjNr();

  // to prevent double re-init of can close it first
  HAL::can_configGlobalClose(mui8_busNumber);
  #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE
      << "CanIo_c::baseCanInit( " << aui16_bitrate << " ) vor HAL::can_configGlobalInit" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // init CAN BUS (BIOS function)
  mc_maskLastmsg.set(0, Ident_c::ExtendedIdent);

  int16_t i16_retvalInit = HAL::can_configGlobalInit(mui8_busNumber, mui16_bitrate, mc_maskStd.ident(), mc_maskExt.ident(),
                      mc_maskLastmsg.ident());

  // check for error state
  if (i16_retvalInit == HAL_RANGE_ERR)
  { // BIOS complains about limits of BUSnr or msgObj
    // seldom, because before checked with defined LIMITS
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( b_lastMsgObjWasOpen )
  { // open last message buffer during reconfig of other MSgObj as CAN is already activated and at least one MsgObj is already in use
    // --> avoid loss of CAN messages which would normally be stored in the already active MsgObj_c instances
    Ident_c tmp ( 0, Ident_c::ExtendedIdent );
    // configure the last MsgObj_c
    mc_lastMsgObj.setFilter( tmp );
    mc_lastMsgObj.configCan(mui8_busNumber, HAL_CAN_LAST_MSG_OBJ_NR );
  }
  #endif
  // init of BUS without error -> continue with send obj configuration

  // init BIOS send Msg Object (BIOS dependent)

  // return value which infomrs about success on BIOS configuration
  int16_t i16_retvalConfig = 0;
  // config the send CAN object with BIOS function
  bool b_configSuccess = true;

  const uint8_t cui8_firstSendMsgObj = minHALMsgObjNr();

  Ident_c c_extIdent( 0, Ident_c::ExtendedIdent );

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  i16_retvalConfig = HAL::can_configMsgobjInit(mui8_busNumber,cui8_firstSendMsgObj, c_extIdent, true);
  #else
  i16_retvalConfig = HAL::can_configMsgobjInit(mui8_busNumber,cui8_firstSendMsgObj, c_extIdent, c_extIdent, true);
  #endif

  // check for error state
  switch (i16_retvalConfig)
  {
    case HAL_NO_ERR:
      break;
    case HAL_BUSY_ERR:
      // send obj already in use
      getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::Can );
      break;
    case HAL_CONFIG_ERR:
      // because CAN was initialized direct above without error, msgObj limits was
      // allowed and the SEND type is hardcoded to an allowed number,
      // HAL_CONFIG_ERR error can only be caused by allocating problem
      getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Can );
      b_configSuccess = false;
      break;
  }

  // return true -> must be set according success
  return b_configSuccess;
}


bool
CanIo_c::stopSendRetryOnErr()
{
  bool b_result = false;

  bool b_bit1err = HAL::can_stateGlobalBit1err(mui8_busNumber);
  uint8_t minObjNr = minHALMsgObjNr();

  if (b_bit1err )
  { // only send 1 has bit1 err -> clear msg obj
    #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE
     << "CanIo_c::stopSendRetryOnErr() Blocked BUS Nr: " << uint16_t(mui8_busNumber)
     << ", ObjNr: " << minObjNr
     << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    HAL::can_useMsgobjClear(mui8_busNumber, minObjNr);
    b_result = true;
  }
  return b_result;
}


void
CanIo_c::setMaxSendDelay( int32_t ai32_maxSendDelay )
{
  mi32_maxSendDelay = ai32_maxSendDelay;
}


#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION && !defined(SYSTEM_WITH_ENHANCED_CAN_HAL)
void
CanIo_c::printMsgObjInfo()
{
  INTERNAL_DEBUG_DEVICE << " CanIo_c::CAN Number " << int(getBusNumber()) << INTERNAL_DEBUG_DEVICE_ENDL;

  for (ArrMsgObj::iterator pc_iterMsgObj = marr_msgObj.begin();
       pc_iterMsgObj != marr_msgObj.end(); ++pc_iterMsgObj)
  {
    pc_iterMsgObj->printMyFilterBox();
  }
};
#endif


#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
void
CanIo_c::doDebug( uint8_t ui8_busNr, uint8_t ui8_sendObjNr )
{
  bool b_bit1err = HAL::can_stateGlobalBit1err(ui8_busNr);
  bool b_warnState = HAL::can_stateGlobalWarn(ui8_busNr);

  if ( b_bit1err )
  {
    INTERNAL_DEBUG_DEVICE << "BITERR VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNr) << ", sendObjNr: "
      << uint16_t(ui8_sendObjNr)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  if ( b_warnState )
  {
    INTERNAL_DEBUG_DEVICE << "CAN_WARN VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNr) << ", sendObjNr: "
      << uint16_t(ui8_sendObjNr)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
}
#endif


#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
void
CanIo_c::printMyFilterBox(){

  for ( uint32_t i = 0; i < m_arrFilterBox.size(); i++ )
  {
    INTERNAL_DEBUG_DEVICE << "CANIO::VECTOR FilterBox :Filter: 0x"
#ifdef SYSTEM_PC
        << std::hex
#endif
        << m_arrFilterBox[i].maskFilterPair().getFilter()
        << ", Mask: 0x"
#ifdef SYSTEM_PC
        << std::hex
#endif
        << m_arrFilterBox[i].maskFilterPair().getMask()
        << ", IdentType: "
#ifdef SYSTEM_PC
        << std::dec
#endif
        << m_arrFilterBox[i].maskFilterPair().getType()
        << ", FbVecId : "
#ifdef SYSTEM_PC
        << std::dec
#endif
        << m_arrFilterBox[i].getFbVecIdx();
    INTERNAL_DEBUG_DEVICE  << INTERNAL_DEBUG_DEVICE_ENDL;
  }
}
#endif


} // __IsoAgLib
