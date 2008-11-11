/***************************************************************************
                          canio_c.cpp - master object for CAN communication;
                                      owns array of FilterBox_c and
                                      MsgObj_c instances
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

#include "canio_c.h"
#include <functional>
#include <algorithm>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/hal/system.h>
#ifdef USE_CAN_EEPROM_EDITOR
  #include <IsoAgLib/hal/eeprom.h>
#endif

#if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION) || defined(DEBUG_HEAP_USEAGE) || defined(DEBUG_CAN_BUFFER_FILLING)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>

#ifdef DEBUG_HEAP_USEAGE
static uint16_t sui16_msgObjTotal = 0;
static uint16_t sui16_filterBoxTotal = 0;
static uint16_t sui16_deconstructMsgObjCnt = 0;

#ifdef MASSERT
extern unsigned int AllocateHeapMalloc;
extern unsigned int DeallocateHeapMalloc;
#endif
#endif


namespace __IsoAgLib {
#if defined( CAN_INSTANCE_CNT ) && ( CAN_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique CanIo_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  CanIo_c& getCanInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return CanIo_c::instance( aui8_instance );
  }
#else
  /** C-style function, to get access to the unique CanIo_c singleton instance */
  CanIo_c& getCanInstance( void )
  {
    static CanIo_c &c_can_io = CanIo_c::instance();
    return c_can_io;
  }
#endif

/*******************************************/
/** definition of public element functions */
/*******************************************/

bool CanIo_c::msb_sendPrioritized=false;

void
CanIo_c::singletonInit()
{
  /// Settings taken form constructor
  mui8_busNumber = 0xFF;

  /// Default to NO maximum send delay detection
  mi32_maxSendDelay = -1;

  mt_filterBoxCnt = 0;
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  mt_msgObjCnt = 0;
  #endif


  /// singletonInit stuff
  init(0xFF, DEFAULT_BITRATE, DEFAULT_CONFIG_IDENT_TYPE, CONFIG_CAN_DEFAULT_MIN_OBJ_NR, CONFIG_CAN_DEFAULT_MAX_OBJ_NR);
}

/** Initialize the CAN hardware, and instantiate one msg object for
  sending of messages. Do configuration for BUS number, sending bitrate,
  CAN ident length, minx/max hardware/BIOS Msg Obj numbers by parameters;
  called by specified constructor or external functions;
  wrong BUS and msg obj numbers are rejected and cause set of Err_c:range

  If even the aui8_busNumber parameter has the default value 0xFF,
  then the configuration settings of a previous init call are not
  changed. In this case, the CAN BUS is only reset with the old settings.
  This is enabled by the default value 0xFF for aui8_busNumber, which is
  changed to CONFIG_CAN_DEFAULT_BUS_NUMBER for the first call of init() after the constructor.
  In all other cases, the special value 0xFF is indicator for empty parameter list.

  possible errors:
      * Err_c::range on undefined BUS,  msgOb_nr or sendBufferSize,
      * Err_c::hwConfig on uninitialized BUS, undef. msgType or CAN-BIOS mem-err,
      * Err_c::busy on already used sending Msg-Obj
  @param aui8_busNumber optional number of the CAN bus
  @param aui16_bitrate optional bitrate (default by define in isoaglib_config.h)
  @param ren_identType optional length of the ident
    (S (11bit), E (29bit), B
    (send both standard and extended ident msg) (default by define in isoaglib_config.h)
  @param aui8_minObjNr optional minimum number for hardware CAN message object
    (important for sharing CAN controller with other tasks) (default by define in isoaglib_config.h)
  @param aui8_maxObjNr optional maximum number for hardware CAN message object
    (default by define in isoaglib_config.h)
  @return true -> correct initialisation without errors
  @see HAL::can_configGlobalInit
  @see HAL::can_configMsgobjInit
  @see Ident_c::t_identType
*/
  bool CanIo_c::init(uint8_t aui8_busNumber, uint16_t aui16_bitrate,
                  Ident_c::identType_t ren_identType,
                  uint8_t aui8_minObjNr,
                  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
                  uint8_t aui8_maxObjNr
                  #else
                  // this parameter is NOT needed with SYSTEM_WITH_ENHANCED_CAN_HAL
                  uint8_t /*aui8_maxObjNr*/
                  #endif
                  )
{ // first make shure that the base system is initialized
  getSystemInstance().init();
  mb_runningCanProcess = false;
  // if FilterBox_c instances were created before the CAN_IO was
  // explicitly initialized, we must call reconfigureMsgObj NOW
  bool b_callReconfigureMsgObj = false;

  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  initMinChangedFilterBox();
  #endif

  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  static bool firstDefaultInitCallStart = true;
  if ( firstDefaultInitCallStart ) {
    firstDefaultInitCallStart = false;
    INTERNAL_DEBUG_DEVICE
        << "Start CanIo_c::init() mit BUS: " << int(aui8_busNumber)
        << ", Bitrate: " << int(aui16_bitrate) << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  #endif

  if ( ( aui8_busNumber != 0xFF ) || ( aui8_busNumber != mui8_busNumber ) )
  { // this is the first call of init after constructor,
    // or at least the BUS Number parameter is different from
    // default value -> interprete as explicit call which can
    // override old setting

    // close CAN Bus, if mui8_busNumber is valid at the
    // moment (this can be a reconfig after a default
    // config)
    if ( mui8_busNumber <= HAL_CAN_MAX_BUS_NR )
    {
      #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
      for (uint8_t i = 0; i < cntFilter(); i++)
        m_arrFilterBox[i].closeHAL();
      #endif
      HAL::can_configGlobalClose(mui8_busNumber);
    }

    /* ************************************* */
    /* *****set initial attribute values**** */
    /* ************************************* */
    // set t_mask to clear values
    men_identType = DEFAULT_CONFIG_IDENT_TYPE;

    // set object vars to 0 detect forgotten init call
    mui8_busNumber = 0;
    setMinHALMsgObjNr(0);
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    setMaxHALMsgObjNr(0);
    #endif
    mui16_bitrate = 0;
    mi32_canErrStart = 0;
    mi32_canErrEnd = 0;

    mi32_endLastReconfigTime = 0;
    mi32_lastProcessedCanPkgTime = 0;
    mui8_processedMsgCnt = 0;
    mc_maskStd.set(0x7FF, Ident_c::StandardIdent);
    mc_maskExt.set(0x1FFFFFFF, Ident_c::ExtendedIdent);
    mc_maskLastmsg.set(0, DEFAULT_IDENT_TYPE);

    // check if some FilterBox_c instances were already created, so that
    // we must call reconfigureMsgObj NOW
    if ( ! m_arrFilterBox.empty() )
    {
      b_callReconfigureMsgObj = true;
      #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
        INTERNAL_DEBUG_DEVICE
        #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
          << "Call reconfigureMsgObj as some FilterBox_c instances are already created"
        #else
          << "there were already some FilterBox_c instances, that should be reactivated with fresh initialized CAN"
        #endif
          << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
    }
  }
  if ( aui8_busNumber == 0xFF )
  { // called with default value -> do nothing till called with normal parameter
    #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    static bool firstDefaultInitCallEnd = true;
    if ( ! firstDefaultInitCallEnd ) {
      firstDefaultInitCallEnd = false;
      INTERNAL_DEBUG_DEVICE
          << "Ende CanIo_c::init() mit Default Werten bei spc_instance == "
          << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    #endif
    return false;
  }
  else
  {
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
      #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE
          << "Ende CanIo_c::init() mit falschen Parametern" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return false; // exit function with error information
    }

    /* ******************************************************* */
    /* ****implementing of function with passed parameters**** */
    /* ******************************************************* */

    /** set min and max allowed obj numbers (important for multithreading
        where each threads manages only a portion of all objects)
    */
    setMinHALMsgObjNr(aui8_minObjNr);
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    setMaxHALMsgObjNr(aui8_maxObjNr);
    #endif
    // store given BUS number: if default value 0xFF is given, use
    // the default defined value
    mui8_busNumber = (aui8_busNumber != 0xFF)?aui8_busNumber:CONFIG_CAN_DEFAULT_BUS_NUMBER;
    // store wanted CAN identifier type
    men_identType = ren_identType;
  }
  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE
      << "Ende CanIo_c::init()" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  // even for call of init without parameter the base can init
  // should be repeated
  // check and store bitrate, config CAN and send object(s)
  const bool b_result = baseCanInit(aui16_bitrate);

  if ( b_callReconfigureMsgObj )
  {
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    reconfigureMsgObj();
    #else
    for (uint8_t i = 0; i < cntFilter(); i++)
      m_arrFilterBox[i].configCan(mui8_busNumber, i + minReceiveObjNr() );
    #endif
  }
  return b_result;
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void CanIo_c::close( void )
{
  if ( mui8_busNumber == 0xFF )
  { // CAN already closed -> don't call HAL close again
    return;
  }

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
      #if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION)
      INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(mui8_busNumber) << " was already closed before close call" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      break;
    default:
      // wrong channel
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
  }
  #ifdef DEBUG_HEAP_USEAGE
  sui16_filterBoxTotal -= cntFilter();
    #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    sui16_msgObjTotal -= cntMsgObj();
    #endif
  #endif
  m_arrFilterBox.clear();
  setCntFilter( 0 );
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  marr_msgObj.clear();
  setCntMsgObj( 0 );
  #endif
  mui8_busNumber = 0xFF;
}


/** periodically called function which does
  periodically needed actions; f.e. trigger watch
  dog and start processing received messages
  AND call init if mui16_bitrate is still set to 0 -> no CAN init performed
  @return true -> time events was performed
*/
bool CanIo_c::timeEvent( void ){
  // check if init was called
  if ( (mui16_bitrate == 0) || ( mui8_busNumber > HAL_CAN_MAX_BUS_NR ) )
  { // this can't be the case after correct call of init
    init(0xFF, DEFAULT_BITRATE, DEFAULT_CONFIG_IDENT_TYPE, CONFIG_CAN_DEFAULT_MIN_OBJ_NR
         , CONFIG_CAN_DEFAULT_MAX_OBJ_NR ); // call with default values
  }
  // if still not ready, CanIo_c is not yet initialised complete -> do nothing
  if ( (mui16_bitrate == 0) || ( mui8_busNumber > HAL_CAN_MAX_BUS_NR ) ) return false;

  // start process of all received msg
  return (processMsg() >= 0)?true:false;
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

/** set the minimum delay in msec. between two sent CAN messages
  @param aui16_minDelay minimum time between two CAN messages [msec.]
*/
void CanIo_c::setSendpause(uint16_t aui16_minDelay) const
{ // set send MsgObj ID
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  HAL::can_configMsgobjSendpause(mui8_busNumber, ui8_sendObjNr, aui16_minDelay);
}

/** deliver the numbers which can be placed at the moment in the send buffer
  @param ren_identType type of searched ident: standard 11bit or extended 29bit
    (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
  @return number of msgs which fit into send buffer
*/
uint8_t CanIo_c::sendCanFreecnt(Ident_c::identType_t /*ren_identType*/)
{ // set send MsgObj ID
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  const int16_t i16_result = HAL::can_stateMsgobjFreecnt(mui8_busNumber, ui8_sendObjNr);

  if ( i16_result >= 0 )
    return uint8_t(i16_result);
  else
    return 0;
}


/** clear the send buffer
  <!--@param ren_identType type of searched ident: standard 11bit or extended 29bit
    (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)-->
*/
void CanIo_c::sendCanClearbuf(Ident_c::identType_t /*ren_identType*/)
{
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE
   << "CanIo_c::sendCanClearbuf for MsgObjNr: " << uint16_t(ui8_sendObjNr) << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif

  HAL::can_useMsgobjClear(mui8_busNumber, ui8_sendObjNr);
}


/** test if a FilterBox_c definition already exist
  (version expecial for standard ident, chosen at compile time)
  @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param aui16_mask individual mask for this filter box
  @param at_filter individual filter
  @param ren_identType type of searched ident: standard 11bit (default) or extended 29bit
  @param apc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CanIo_c::existFilter(const __IsoAgLib::CanCustomer_c& ar_customer,
                          uint16_t aui16_mask, uint16_t at_filter,
                          Ident_c::identType_t ren_identType, ArrFilterBox::iterator* apc_iter)

{
  // check if filter/t_mask are already inserted
  // return false if this setting isnt unique
  Ident_c c_compFilter = Ident_c(at_filter, ren_identType);
  Ident_c c_compMask = Ident_c(aui16_mask, ren_identType);
  c_compFilter.set(at_filter, ren_identType);
  c_compMask.set(aui16_mask, ren_identType);

  return existFilter(ar_customer, c_compMask, c_compFilter, apc_iter);
}
/** test if a FilterBox_c definition already exist
  (version expecial for extended ident, chosen at compile time)
  @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param aui32_mask individual mask for this filter box
  @param aui32_filter individual filter
  @param ren_identType type of searched ident: standard 11bit (default) or extended 29bit
  @param apc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CanIo_c::existFilter(const __IsoAgLib::CanCustomer_c& ar_customer,
                          uint32_t aui32_mask, uint32_t aui32_filter,
                          Ident_c::identType_t ren_identType, ArrFilterBox::iterator* apc_iter)

{
  // check if filter/t_mask are already inserted
  // return false if this setting isnt unique
  Ident_c c_compFilter = Ident_c(aui32_filter, ren_identType);
  Ident_c c_compMask = Ident_c(aui32_mask, ren_identType);
  c_compFilter.set(aui32_filter, ren_identType);
  c_compMask.set(aui32_mask, ren_identType);

  return existFilter(ar_customer, c_compMask, c_compFilter, apc_iter);
}

/** test if a FilterBox_c definition already exist
  (version with comper items as Ident_c class instances, chosen by compiler)
  @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param ac_compMask individual mask for this filter box
  @param ac_compFilter individual filter
  @param apc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CanIo_c::existFilter(const __IsoAgLib::CanCustomer_c& ar_customer,
                          const Ident_c& ac_compMask, const Ident_c& ac_compFilter,
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
    if ( ( pc_iter->equalFilterMask(ac_compMask, ac_compFilter ) )
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

/**
  Create a Standard Iso Filter Box.
  @see __IsoAgLib::CANCustomer
  @see __IsoAgLib::CanIo_c::insertFilter
  @param ar_customer reference to __IsoAgLib::CanCustomer_c which needs filtered
      messages (-> on received msg call ar_customer.processMsg())
  @param aui32_pgn PGN
  @param ab_reconfigImmediate true -> all Filter objects are reconfigured to according
  @return != NULL -> if inserting and wanted reconfiguration are performed without errors, a reference to the created FilterBox is returned
  @exception badAlloc
**/
FilterBox_c*  CanIo_c::insertStandardIsoFilter(__IsoAgLib::CanCustomer_c& ar_customer, uint32_t aui32_pgn,bool ab_reconfigImmediate)
{
  int8_t i8_dataLen = 0;

    switch(aui32_pgn)
    {
      case REQUEST_PGN_MSG_PGN:
      case (REQUEST_PGN_MSG_PGN | 0xFF): //xxeaffxx
        i8_dataLen = 3;
        break;

      case ACKNOWLEDGEMENT_PGN: /**variable data len : see ISO/CD ISO-11783-12 */
      case CLIENT_TO_FS_PGN:    /** variable data len : see ISO-11783 -13 */
      case FS_TO_CLIENT_PGN:    /** variable data len : see ISO-11783 -13 */
      case PROPRIETARY_A_PGN: /** multipacket supported, data len 0..1785 bytes*/
      case PROPRIETARY_A2_PGN: /** multipacket supported, data len 0..1785 bytes*/
      case SOFTWARE_IDENTIFICATION_PGN: /** variable data len: see ISO/CD ISO-11783-12 */
      case ECU_IDENTIFICATION_INFORMATION_PGN: /** variable data len: see ISO/CD ISO-11783-12 */
      case PROPRIETARY_B_PGN: /** variable data len: see SAE J1939 71 */
        i8_dataLen = -1;
        break;

      default:i8_dataLen= 8;
      break;
    }

    return insertFilter(ar_customer,(0x3FFFF00UL),MASK_TYPE(static_cast<MASK_TYPE>(aui32_pgn) << 8),ab_reconfigImmediate,Ident_c::ExtendedIdent,i8_dataLen);
}

/** create a Filter Box with specified at_mask/at_filter
  on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
  ab_reconfigImmediate == true -> useful for
  avoiding unneeded reconfiguration during
  sequence of FilterBox_c insertions;
  by ar_customer CanIo_c (FilterBox_c) can start direct processing
  of received data in dedicated customer object (no search);
  uses BIOS functions

  This specialized function allows the user to use the same message
  object as the filter box that is given "apc_connectedFilterBox"
  Use this feature if messages from two or more filterboxes have to be processed chronologically

  possible errors:
      * Err_c::badAlloc on not enough memory for new FilterBox_c instance or for new configured MsgObj_c's
  @see __IsoAgLib::CANCustomer
  @param ar_customer reference to __IsoAgLib::CanCustomer_c which needs filtered
      messages (-> on received msg call ar_customer.processMsg())
  @param aui32_mask individual mask for this filter box
  @param aui32_filter individual filter
  @param ab_reconfigImmediate true -> all Filter objects are reconfigured to according

      CAN hardware MsgObj after creating this filter
  @param at_identType ident type of the created ident: standard 11bit (default) or extended 29bit
  @return != NULL -> if inserting and wanted reconfiguration are performed without errors, a reference to the created FilterBox is returned
  @exception badAlloc
*/
FilterBox_c* CanIo_c::insertFilter(__IsoAgLib::CanCustomer_c & ar_customer,
                                  uint32_t aui32_mask, uint32_t aui32_filter,
                                  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
                                  bool ab_reconfigImmediate,
                                  #else
                                  // this parameter is NOT needed with SYSTEM_WITH_ENHANCED_CAN_HAL
                                  bool /*ab_reconfigImmediate*/,
                                  #endif
                                  const Ident_c::identType_t at_identType,
                                  int8_t ai8_dlcForce)

{
  Ident_c c_newMask = Ident_c(aui32_mask, at_identType);
  Ident_c c_newFilter = Ident_c(aui32_filter, at_identType);

#if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION)
INTERNAL_DEBUG_DEVICE << "-----------------------------------start CanIo_c::insertFilter " << INTERNAL_DEBUG_DEVICE_ENDL;
      printMyFilterBox();
#endif

  if (existFilter(ar_customer, c_newMask, c_newFilter, NULL))
  { // filter,mask,cancustomer definition already inserted
    return NULL;
  }

  //look for existing mask/filter definition
  FilterBox_c* tempFilterBox_c = getFilterBox(c_newMask, c_newFilter);

  if (tempFilterBox_c != NULL)
  { //cancustomer does not exist -> insert
    #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "filterbox mask/filter already exist -> insert cancustomer" << INTERNAL_DEBUG_DEVICE_ENDL;
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE << "mask: "<< STL_NAMESPACE::hex << c_newMask.ident() << " filter: " << c_newFilter.ident() << STL_NAMESPACE::dec << INTERNAL_DEBUG_DEVICE_ENDL;
    #else
    INTERNAL_DEBUG_DEVICE << "mask: " << c_newMask.ident() << " filter: " << c_newFilter.ident() << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    #endif
    tempFilterBox_c->insertCustomer (&ar_customer, ai8_dlcForce);
    //do not insert new filterbox because it already exists
    return tempFilterBox_c;
  }
  // now the filterBox def is new -> insert new FilterBox_c instance in array

  // use for insert a member var FilterBox_c to avoid creation of temp obj during insert
  // clear temp object
  mc_tempFilterBox.clearData();

  // define temp FilterBox_c with new values
  mc_tempFilterBox.set(c_newMask, c_newFilter, &ar_customer, ai8_dlcForce);

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
      m_arrFilterBox[ui8_overwritenFilterBoxIndex].set(c_newMask, c_newFilter, &ar_customer, ai8_dlcForce);

      m_arrFilterBox[ui8_overwritenFilterBoxIndex].setFbVecIdx(ui8_overwritenFilterBoxIndex);

      #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      /** when a idle FB is reused then the Full reconfiguration is necessary**/
      /** set Full reconfiguration */
      setFullReconfigNecessary();
      #endif

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( mui8_busNumber != 0xFF ) /* when the add is made before the init CAN don't do the config, config is done at the init CAN */
  {
      m_arrFilterBox[ui8_overwritenFilterBoxIndex].configCan(mui8_busNumber, ui8_overwritenFilterBoxIndex + minReceiveObjNr());
  }
#endif

      filterBoxOverwrite = true;
      #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE << "overwrite FilterBox[" << int(ui8_overwritenFilterBoxIndex) << "] with busnumber: " << static_cast<int>(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      INTERNAL_DEBUG_DEVICE << "                    mask: "
      #ifdef SYSTEM_PC
      << STL_NAMESPACE::hex
      #endif
      << c_newMask.ident() << " filter: " << c_newFilter.ident()
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
    #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "insertFilterBox with bus number: " << static_cast<int>(mui8_busNumber)  << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_DEVICE << "                     mask: "
    #ifdef SYSTEM_PC
    << STL_NAMESPACE::hex
    #endif
    << c_newMask.ident() << " filter: " << c_newFilter.ident()
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

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  if ( mui8_busNumber != 0xFF ) /* when the add is made before the init CAN don't do the config, config is done at the init CAN */
  {
    uint8_t tempMsgObjNr = minReceiveObjNr() + b_oldSize;
    m_arrFilterBox.back().configCan(mui8_busNumber, tempMsgObjNr);
  }
#endif


    if (b_oldSize >= cntFilter())
    { // dynamic array didn't grow -> alloc error
      getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Can );
      return NULL; // exit the function
    }
    #ifdef DEBUG_HEAP_USEAGE
    else
    {
      sui16_filterBoxTotal++;
    }
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

 #endif

#if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
INTERNAL_DEBUG_DEVICE << "end CanIo_c::insertFilter " << INTERNAL_DEBUG_DEVICE_ENDL;
      printMyFilterBox();
#endif

  // insertion of FilterBox_c was successfull
  if (filterBoxOverwrite == true)
    return &m_arrFilterBox[ui8_overwritenFilterBoxIndex];
  else //new filterBox inserted at the end
    return &m_arrFilterBox.back();

}


/** delete a FilterBox definition
  @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param at_mask individual mask for this filter box
  @param at_filter individual filter
  @param at_identType ident type of the deleted ident: standard 11bit or extended 29bit
        (defualt DEFAULT_IDENT_TYPE defined in isoaglib_config.h)
  @return true -> FilterBox_c found and deleted
  @todo SOON: incorporate additional parameter "bool ab_reconfigImmediate = true" from insertFilter before at_identType - AND update all callers
*/
bool CanIo_c::deleteFilter(const __IsoAgLib::CanCustomer_c& ar_customer,
                           MASK_TYPE at_mask, MASK_TYPE at_filter,
                           const Ident_c::identType_t at_identType)

{
  bool b_result = false;


  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** at the deleteFilterBox the full reconfiguration is always necessary */
  setFullReconfigNecessary();
  #endif

#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "------------------------CanIo_c::delete filter = 0x"
    #ifdef SYSTEM_PC
      << STL_NAMESPACE::hex
    #endif
      << at_filter
      << " on Can bus = "
    #ifdef SYSTEM_PC
      << STL_NAMESPACE::dec
    #endif
      << int(getBusNumber())
      << ", can instance = "
      << getSingletonVecKey()
      << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  Ident_c c_deleteMask = Ident_c(at_mask, at_identType),
        c_deleteFilter = Ident_c(at_filter, at_identType);

  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter;

  if (existFilter(ar_customer, c_deleteMask, c_deleteFilter, &pc_iter))
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
      while( m_arrFilterBox.back().isIdle() )
      { //remove idle filterBox if at the end of vector
        m_arrFilterBox.pop_back();
      }


      setCntFilter( m_arrFilterBox.size() );
    }

    #ifdef DEBUG_HEAP_USEAGE
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




bool CanIo_c::deleteAllFiltersForCustomer (const __IsoAgLib::CanCustomer_c & ar_customer)
{
  bool b_result = false;
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  setFullReconfigNecessary();
  #endif

  for (ArrFilterBox::iterator pc_iter = m_arrFilterBox.begin(); pc_iter != m_arrFilterBox.end();
  )
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
  while( m_arrFilterBox.back().isIdle() )
  { //remove idle filterBox if at the end of vector
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


/**
  initiate processing of all received msg
  check all active MsgObj_c for received CAN msg and
  initiate their processing
  @return <0 --> not enough time to process all messages.
         ==0 --> no messages were received.
         >0  --> all messages are processed, number of messages
*/

int16_t CanIo_c::processMsg(){
  // immediately leave this function, if this CAN_IO instance is marked as currently processing
  // --> detect and avoid CAN message processing loops
  if ( mb_runningCanProcess ) return -1;
  mb_runningCanProcess = true;
  mui8_processedMsgCnt = 0;

#ifdef DEBUG_CAN_BUFFER_FILLING
  bool b_detectedOverflow = false;
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
          #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
          INTERNAL_DEBUG_DEVICE << "CAN-Receive Range Err" << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          break;
        case HAL_CONFIG_ERR:
          #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION)
          INTERNAL_DEBUG_DEVICE << "\r\nBUS not initialized or wrong BUS nr: " << uint16_t(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
          break;
        case HAL_NOACT_ERR:
          #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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

          #ifdef DEBUG_CAN_BUFFER_FILLING
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
        //no message to read
        break;
        default:
         to_be_processed = true;
         getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
        break;
      }


      if(to_be_processed) //found a message to be processed
      {
        /**
        * Possible cases :
        * - i32_retVal = HAL_UNKNOWN_ERR -> nothing to read
        * - i32_fbIdx = -1 --> a reconfiguration was ongoing at the reception of the message, so the suitable FB should be found here
        *                     among all created FB
        * - i32_fbIdx > = m_arrFilterBox.size() --> the FilterBox that wanted the message has been canceled, so discard the message
        * - i32_fbIdx has a valid range, but all the data of the FilterBox are cleared, because it has been canceled.
        *     so the FilterBox is Idle
        */

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
        if(mi32_lastProcessedCanPkgTime < mi32_endLastReconfigTime )
        {
          INTERNAL_DEBUG_DEVICE << " During reconfiguration, received msg : "
                  #ifdef SYSTEM_PC
                  << STL_NAMESPACE::hex
                  #endif
            << i32_ident << INTERNAL_DEBUG_DEVICE_ENDL;
        }
  #endif
        if(mi32_lastProcessedCanPkgTime > mi32_endLastReconfigTime ) // the message has not arrived before the last reconfiguration,check on all the FB
        {
#endif
          if(i32_fbIdx >= 0 && i32_fbIdx < static_cast<int32_t>(m_arrFilterBox.size()))
          {
            if(!(m_arrFilterBox[i32_fbIdx].isIdle())  &&  m_arrFilterBox[i32_fbIdx].matchMsgId(i32_ident,identType))
            {
              // !! the interested FilterBox processes the message
              #if 0 //#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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
#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
          INTERNAL_DEBUG_DEVICE << " CanIo_c::processMsg --- search among all the FilterBox " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
          CanIo_c::ArrFilterBox::iterator pc_iFilterBox;
          bool b_foundOne=false;

              do {
                b_foundOne = canMsg2FilterBox( i32_ident, identType, pc_iFilterBox, !b_foundOne );

                if ( b_foundOne )
                { // matching instance found
                  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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
#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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

/** function for sending data out of CanPkgExt_c (uses BIOS function)
  if send buffer is full a local loop waits till buffer has enough space
  (every 100ms the watchdog is triggered, to avoid watchdog reset)

  possible errors:
      * Err_c::hwConfig on wrong configured CAN obj, not init BUS or no configured send obj
      * Err_c::range on undef BUS or BIOS send obj nr
      * Err_c::can_warn on physical CAN-BUS problems
      * Err_c::can_off on physical CAN-BUS off state
  @param acrc_src CanPkgExt_c which holds the to be sent data
  @return reference to this CANIOExt_c instance ==> needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
*/
CanIo_c& CanIo_c::operator<<(CanPkgExt_c& acrc_src)
{
  if ( ! isReady2Send() ) return *this;
  //check if source and destination address are valid
#if ((defined(USE_ISO_11783)) && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
  // when ISO is compiled, we must make sure, that the ISO specific
  // resolving is only used for extended ident messages
  #ifdef ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
  if ( ( mb_canChannelCouldSendIso                           )
    && ( acrc_src.identType() == Ident_c::ExtendedIdent     )
    && ( ! acrc_src.isProprietaryMessageOnStandardizedCan() ) )
  #else
  if ( ( mb_canChannelCouldSendIso ) && ( acrc_src.identType() == Ident_c::ExtendedIdent ) )
  #endif // end of ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL
  #endif
  {
    if ( ! acrc_src.resolveSendingInformation() )
    { // preconditions for correct sending are not fullfilled -> set error state
      getILibErrInstance().registerError(IsoAgLib::iLibErr_c::CanBus, IsoAgLib::iLibErr_c::Can);
      return *this;
    }
  }
  return CanIo_c::operator<<( static_cast<CanPkg_c&>(acrc_src) );
}

/** function for sending data out of CanPkg_c (uses BIOS function)
  if send buffer is full a local loop waits till buffer has enough space
  (every 100ms the watchdog is triggered, to avoid watchdog reset)

  possible errors:
      * Err_c::hwConfig on wrong configured CAN obj, not init BUS or no configured send obj
      * Err_c::range on undef BUS or BIOS send obj nr
      * Err_c::can_warn on physical CAN-BUS problems
      * Err_c::can_off on physical CAN-BUS off state
  @param acrc_src CanPkg_c which holds the to be sent data
  @return reference to this CanIo_c instance ==> needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
*/
CanIo_c& CanIo_c::operator<<(CanPkg_c& acrc_src)
{ // immediately return if CAN is not yet configured for send
  if ( ! isReady2Send() ) return *this;

  // set send MsgObj ID
  uint8_t ui8_sendObjNr = minHALMsgObjNr();

  #if 0
  if ( mui8_busNumber == 1 ) {
    INTERNAL_DEBUG_DEVICE
      << "CanIo_c::operator<< mit MIN MsgObj Nr: " << uint16_t( ui8_sendObjNr )
      << "Xtd: " << acrc_src.identType()
      << "\n\r";
  }
  #endif

  // wait till Msg can be placed in send buffer
  while ( HAL::can_stateMsgobjFreecnt( mui8_busNumber, ui8_sendObjNr ) < 1 )
  {  // perform wait loop
    // trigger the watchdog
    HAL::wdTriggern();
    // exit loop, if CAN BUS is OFF and exit function
    if (HAL::can_stateGlobalOff(mui8_busNumber))
    { /** @todo SOON: check whether HAL::can_stateGlobalOff() detects all possible reasons that can block the send queue from emptying by sending of queued messages on CAN BUS */
      // clear MsgObj CAN queue
      #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
      INTERNAL_DEBUG_DEVICE
       << "CanIo_c::operator<< BUS OFF BUS Nr: " << uint16_t(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif

      HAL::can_useMsgobjClear(mui8_busNumber,ui8_sendObjNr);
      return *this;
    }
  }
  // it's time to trigger the watchdog
  HAL::wdTriggern();

  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  doDebug(mui8_busNumber, ui8_sendObjNr);
  #endif

  int16_t i16_sendFuncState = HAL::can_useMsgobjSend(mui8_busNumber, ui8_sendObjNr, &acrc_src);

  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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
      #if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION)
        INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(mui8_busNumber) << " not initialized or MsgObj: " << uint16_t(ui8_sendObjNr) << " no send obj" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
      break;
    case HAL_NOACT_ERR:
      // BUS off
      #if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION)
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(mui8_busNumber) << " in BUS OFF STATE" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::CanOff, iLibErr_c::Can );
      break;
    case HAL_OVERFLOW_ERR:
      // overflow of send buffer
      getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
      break;
    case HAL_RANGE_ERR:
      // BUS nr or obj nr outer allowed limits (shouldn't be the case after successful init call)
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
    case HAL_WARN_ERR:
      // signal for BUS-WARN problem
      #if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION)
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(mui8_busNumber) << " in WARN STATE" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
      break;

   #ifdef USE_CAN_SEND_DELAY_MEASUREMENT
    case HAL_NEW_SEND_DELAY:
      #if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION)
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(mui8_busNumber) << " encountered new Maximum Send Delay.. \n";
      #endif

      if (mi32_maxSendDelay >= 0)
      { // we're in mode: Test-MaxSendDelay
        if (HAL::can_getMaxSendDelay(mui8_busNumber) > mi32_maxSendDelay)
        { // this is way too max, register an error!
          getILibErrInstance().registerError( iLibErr_c::CanTooSlowSend, iLibErr_c::Can );
      #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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

/**********************************************/
/** definition of protected element functions */
/**********************************************/
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
/** evaluate common bits of all defined filterBox
    instances and set it in mask -> build up global mask */
void CanIo_c::getCommonFilterMask()
{
  // preset all bits with "1"
  mc_maskStd.set(~0, Ident_c::StandardIdent);
  mc_maskExt.set(~0, Ident_c::ExtendedIdent);

  // combine masks of all m_arrFilterBox with AND
  for (ArrFilterBox::iterator pc_iter = m_arrFilterBox.begin();
       pc_iter != m_arrFilterBox.end();
       pc_iter++
      )
      {
        if(!pc_iter->isIdle())
        {
          if (pc_iter->identType() == Ident_c::StandardIdent) {
            mc_maskStd.ident_bitAnd(pc_iter->mask());
          }
          else {
            mc_maskExt.ident_bitAnd(pc_iter->mask());
          }
        }
      }
}

/** correct global masks after merge based on merged IDs of all
    resulting MsgObj_c instances
 */
void CanIo_c::getCommonFilterMaskAfterMerge()
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

/** build unique MsgObj_c's based on given
  global mask (different FilterBox_c def's can result in one
  MsgObj_c if differences are hidden by global mask), return count of obj

  possible errors:
      * Err_c::badAlloc on not enough memory for created MsgObj_c instances
  @see MsgObj
  @see FilterBox
*/
int16_t CanIo_c::FilterBox2MsgObj()
{
  int16_t i16_result = 0;
  Ident_c c_tempIdent;
  ArrMsgObj::iterator pc_iterMsgObj, pc_search4MsgObjReuse = marr_msgObj.begin();

#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE << " CanIo_c::FilterBox2MsgObj "<< INTERNAL_DEBUG_DEVICE_ENDL;
#endif


ArrFilterBox::iterator pc_iterFilterBox = m_arrFilterBox.begin();

#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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
#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE << " CanIo_c::FilterBox2MsgObj  - FULL RECONFIGURATION on BUS number = "<< int(getBusNumber())
    << ", can instance = " << getSingletonVecKey() << INTERNAL_DEBUG_DEVICE_ENDL;
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
  for (;
       pc_iterFilterBox != m_arrFilterBox.end();
       pc_iterFilterBox++
      )
  {

    if(!pc_iterFilterBox->isIdle())
    {
        // get filter Ident_c setting from actual FilterBox
        c_tempIdent = pc_iterFilterBox->filter();
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
            pc_search4MsgObjReuse->setCanSingletonKey(getSingletonVecKey());
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
            mc_tempObj.setCanSingletonKey(getSingletonVecKey());
            #endif
            mc_tempObj.setBusNumber(mui8_busNumber);

            #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
            INTERNAL_DEBUG_DEVICE << "create a new object with bus Number " << int(getBusNumber())
            << " , Filter = 0x"
            #ifdef SYSTEM_PC
            << STL_NAMESPACE::hex
            #endif
            << c_tempIdent.ident() << ", can Instance = "<< getSingletonVecKey()
            << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif

            // check if insertion try result in growed dyn array
            const uint16_t ui16_oldSize = cntMsgObj();
            // insert obj in vector
            marr_msgObj.push_front(mc_tempObj);
            setCntMsgObj( marr_msgObj.size() );
            if (ui16_oldSize >= cntMsgObj())
            { // dyn array didn't grow -> set badAlloc error state

            #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
            INTERNAL_DEBUG_DEVICE << "CanIo_c::FilterBox2MsgObj : the vector doesn't grow " << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
              getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Can );
            }
            #ifdef DEBUG_HEAP_USEAGE
            else
            {
              sui16_msgObjTotal++;
            }
            #endif
            pc_search4MsgObjReuse = marr_msgObj.end();

          }
        }
        else
        {
          // same filter definition at position q already inserted in msgOb vector
          // insert new FilterBox_c reference
          if ( pc_iterMsgObj->insertFilterBox(pc_iterFilterBox) == false)
            i16_result = -1;
        }


    }
  }
  // if less objects are created than already in vector, erase the unused
  // -> behind the last instance is NOT the list end
  if (pc_search4MsgObjReuse!= marr_msgObj.end())
  {
    #ifdef DEBUG_HEAP_USEAGE
    const uint16_t cui16_oldSize = cntMsgObj();
    #endif
    HAL::wdTriggern();
    marr_msgObj.erase(pc_search4MsgObjReuse, marr_msgObj.end());
    setCntMsgObj( marr_msgObj.size() );
    #ifdef DEBUG_HEAP_USEAGE
    sui16_msgObjTotal -= ( cui16_oldSize - cntMsgObj() );
    sui16_deconstructMsgObjCnt += (cui16_oldSize - cntMsgObj() );
    #endif
  }

  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE << "Old number MsgObj = "
  #ifdef SYSTEM_PC
  << STL_NAMESPACE::dec
  #endif
  << ui16_debugOldSize << ", New size = " << cntMsgObj() << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  if ( i16_result >= 0) i16_result = cntMsgObj();
  #ifdef DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << "MsgObj: " << sui16_msgObjTotal << " -> ";
  #endif
  HAL::wdTriggern();

  return i16_result;

}

/** checks if count of preconfigured msgObj
  is allowed; shrink by merging the objects
  with smallest Bit distance
  -> all existing MsgObj_c instances can be
     configured in hardware after execution
  @see MsgObj
*/
void CanIo_c::CheckSetCntMsgObj()
{
  // set i16_minDistance to the max possible bit difference
  // => amount of bits in data type for ident
  int16_t i16_minDistance = sizeof(MASK_TYPE)*8,
      i16_tempDist;
  // maxHALMsgObjNr() delivers also the special lastMsgObj, which cannot be used for normal receiveMsgObj
  // ==> subtract from (max - min + 1 ) again one item
  uint8_t ui8_allowedSize = maxHALMsgObjNr() - minReceiveObjNr() + 1 - 1;

  unsigned int ui_lsbDiffMin = 0;
  unsigned int ui_lsbDiffTemp = 0;

  // before any preparation of further merge work - check whether we have to merge anyway
  if (cntMsgObj() <= ui8_allowedSize) return;

  // NOW WE HAVE TO MERGE AT LEAST TWO MsgObj_c together
#if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
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
      #ifdef DEBUG_HEAP_USEAGE
      sui16_msgObjTotal--;
      sui16_deconstructMsgObjCnt++;
      #endif
      // update the filters in the filters in the existing MsgObj_c to the changed mask
      for (ArrMsgObj::iterator c_iter = marr_msgObj.begin(); c_iter != marr_msgObj.end(); c_iter++)
      {
        c_iter->updateFilterWithMask( *pc_useGlobalMask );
      }


      // reset search arguments for posible next search
      pc_minRight = pc_minLeft = marr_msgObj.begin();
      i16_minDistance = sizeof(MASK_TYPE)*8;
    }
  }

  // now the amount of marr_msgObj is allowed
  #ifdef DEBUG_HEAP_USEAGE
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

/* *************************************** */
/* definition of private element functions */
/* *************************************** */

/** call the needed HAL function for setting the new global masks,
  without invalidating already open send and last msg obj.
  */
bool CanIo_c::registerChangedGlobalMasks(void)
{
  int16_t i16_retvalInit = HAL::can_configGlobalMask(mui8_busNumber, mc_maskStd.ident(), mc_maskExt.ident(),
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

/** helper function to search all FilterBoxes for matching
  instance which maps to received CAN messages
  ( needed if the coordinated FilterBoxes don't match,
    especially important to process messages from global
    CAN MsgObj which doesn't have explicit connected FilterBox
    instances )
  @param aui32_ident Ident of received CAN message
  @return is another filterbox found?
*/
bool CanIo_c::canMsg2FilterBox( uint32_t aui32_ident, Ident_c::identType_t at_type, ArrFilterBox::iterator& ar_arrFilterBoxIter, bool ab_start )
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
    if ( ar_arrFilterBoxIter->matchMsgId( aui32_ident, at_type ) )
    { // matching FilterBox_c found
      return true;
    }
    ar_arrFilterBoxIter++;
  }
  // if execution reaches this point, no matching FilterBox_c found
  return false;
}


/** search for a FilterBox with given mask and filter
    @param  at_mask    filterBox mask
    @param  at_filter  filterBox filter
    @return            located FilterBox or NULL if not exist
  */
FilterBox_c* CanIo_c::getFilterBox(Ident_c& at_mask, Ident_c& at_filter) const
{

  for(uint8_t i = 0; i < cntFilter(); i++)
  {
    if( m_arrFilterBox[i].equalFilterMask(at_mask, at_filter) )
      return const_cast<FilterBox_c*>(&m_arrFilterBox[i]);
  }

  //filterbox with mask, filter not found
  return NULL;
}

/** verify given BUS number and MsgObj number, if they are correct
  (mostly used by MsgObj_c to verify itself)
  @param aui8_busNr number of the BUS
  @param aui8_objNr number of the MsgObj
  @return true -> values are correct
*/
bool CanIo_c::verifyBusMsgobjNr(uint8_t aui8_busNr, uint8_t aui8_objNr)
{ //check if there is an error
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


/** reconfigure the MsgObj after insert/delete of FilterBox */
bool CanIo_c::reconfigureMsgObj()
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


  #ifdef DEBUG_HEAP_USEAGE
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
#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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
    #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
    INTERNAL_DEBUG_DEVICE << "Close can object num = "<< (i+minReceiveObjNr() )<< " on can bus number =" << int(mui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
   }

/** 7. close the Last Msg Obj */
    mc_lastMsgObj.close();
    mc_lastMsgObj.closeCan();

/** reinit the variable for the Full reconfiguration */
    initMinChangedFilterBox();

    setEndLastReconfigTime();

#if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
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

/** get offset for received messages
    @return offset for received messages
  */
uint8_t CanIo_c::updateMinReceiveObjNr()
{
  mui8_minReceiveObjNr = 1 + minHALMsgObjNr();
  #ifdef USE_CAN_EEPROM_EDITOR
  // the MsgObj next to the sending MsgObj are used for the EEPROM Editor
  // --> increase the offset for the first receive MsgObj by furhter two items
  mui8_minReceiveObjNr += 2;
  #endif
  return mui8_minReceiveObjNr;
}

/** switch CAN bitrate (possible during runtime
  with automatic reconfiguring of CAN MsgObj)
  @param aui16_newSpeed
*/
bool CanIo_c::setBitrate(uint16_t aui16_newSpeed, bool ab_force)
{ // don't reconfigure if same bitrate
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


/** private constructor which prevents direct instantiation in user application
  * NEVER define instance of CanIo_c within application
  * (set mui8_busNumber to 0xFF so that init() detects first call after constructor)
  */
CanIo_c::CanIo_c( void ) :
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    marr_msgObj(),
    mc_tempObj(),
    mc_lastMsgObj(),
    mt_msgObjCnt(),
    mi32_minChangedFilterBox(0),
  #endif
    m_arrFilterBox(),
    mt_filterBoxCnt(0),
    mc_tempFilterBox(),
    mi32_maxSendDelay(0),
    mi32_canErrStart(0),
    mi32_canErrEnd(0),
    mi32_lastCanCheck(0),
    mi32_lastProcessedCanPkgTime(0),
    mui16_bitrate(0),
    mi32_endLastReconfigTime(0),
    mc_maskStd(),
    mc_maskExt(),
    mc_maskLastmsg(),
    men_identType(Ident_c::StandardIdent),
    mui8_busNumber(0),
    mui8_minmsgObjLimit(0),
    mui8_minReceiveObjNr(0),
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    mui8_maxMsgObjLimit(0),
  #endif
    mui8_processedMsgCnt(0),
    mb_runningCanProcess(false)
  #if ((defined( USE_ISO_11783)) && ((CAN_INSTANCE_CNT > PRT_INSTANCE_CNT) || defined (ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)))
    ,mb_canChannelCouldSendIso(false)
  #endif
{}


/** perform bas init for CAN with set of speed and init of send object(s)
  @param aui16_bitrate wanted CAN bitrate
*/
bool CanIo_c::baseCanInit(uint16_t aui16_bitrate)
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
  #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  INTERNAL_DEBUG_DEVICE
      << "CanIo_c::baseCanInit( " << aui16_bitrate << " ) vor HAL::can_configGlobalInit" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // init CAN BUS (BIOS function)
  mc_maskLastmsg.set(0, DEFAULT_IDENT_TYPE);

  int16_t i16_retvalInit = HAL::can_configGlobalInit(mui8_busNumber, mui16_bitrate, mc_maskStd.ident(), mc_maskExt.ident(),
                      mc_maskLastmsg.ident());

  #ifdef USE_CAN_EEPROM_EDITOR
    /* if CAN init with success init CAN EEEditor */
    if ( ( i16_retvalInit == HAL_NO_ERR) && ( mui8_busNumber == CONFIG_EEPROM_USE_CAN_BUS ) )
    {
      uint8_t b_eepromType = Ident_c::ExtendedIdent;
      HAL::InitEEEditor( CONFIG_EEPROM_USE_CAN_BUS, minReceiveObjNr() - 2,
          (minReceiveObjNr() - 1), CONFIG_EEPROM_USE_CAN_REC_IDENT, b_eepromType,
          CONFIG_EEPROM_USE_CAN_BUFFER_SIZE, CONFIG_CAN_SEND_BUFFER_SIZE);
    }
  #endif

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
  Ident_c c_sendIdent;


  const uint8_t cui8_firstSendMsgObj = minHALMsgObjNr();
  switch (men_identType)
  {
    case Ident_c::StandardIdent:
      c_sendIdent.setIdentType(Ident_c::StandardIdent);
      break;
    case Ident_c::ExtendedIdent:
      c_sendIdent.setIdentType(Ident_c::ExtendedIdent);
      break;
  }
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  i16_retvalConfig = HAL::can_configMsgobjInit(mui8_busNumber,cui8_firstSendMsgObj, c_sendIdent, true);
  #else
  i16_retvalConfig = HAL::can_configMsgobjInit(mui8_busNumber,cui8_firstSendMsgObj, c_sendIdent, c_sendIdent, true);
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


  #if ( ( CAN_INSTANCE_CNT > PRT_INSTANCE_CNT ) || defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL) )
    if ( getSingletonVecKey() >= PRT_INSTANCE_CNT ) mb_canChannelCouldSendIso = false;
    else mb_canChannelCouldSendIso = true;
  #endif

  // return true -> must be set according success
  return b_configSuccess;
}

/** check for can send conflict error and stop send retry on error
  (thus avoid BUS OFF)
  @return true -> there was send error and send retry stopped
*/
bool CanIo_c::stopSendRetryOnErr()
{
  bool b_result = false;

  bool b_bit1err = HAL::can_stateGlobalBit1err(mui8_busNumber);
  bool minObjNr = minHALMsgObjNr();

  if (b_bit1err )
  { // only send 1 has bit1 err -> clear msg obj
    #ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
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


/** set the new maximum send delay
    @param ai32_maxSendDelay new maximum send delay. negative values (like -1) for NOT using this feature!
 */
void CanIo_c::setMaxSendDelay (int32_t ai32_maxSendDelay)
{
  mi32_maxSendDelay = ai32_maxSendDelay;
}


/** set this client to have send-priority
  @param ab_sendPrioritized enable (true) or disable (false) sending in Prioritized Mode
 */
void CanIo_c::setSendPriority(bool ab_sendPrioritized)
{ // set send MsgObj ID
  msb_sendPrioritized = ab_sendPrioritized;
}

#if defined(DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION) && !defined(SYSTEM_WITH_ENHANCED_CAN_HAL)
void CanIo_c::printMsgObjInfo()
{
  INTERNAL_DEBUG_DEVICE << " CanIo_c::CAN Number " << int(getBusNumber()) << INTERNAL_DEBUG_DEVICE_ENDL;

  for (ArrMsgObj::iterator pc_iterMsgObj = marr_msgObj.begin();
        pc_iterMsgObj != marr_msgObj.end();
        pc_iterMsgObj++
        )
    {
        pc_iterMsgObj->printMyFilterBox();
    }
};
#endif


#ifdef DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
void CanIo_c::doDebug(uint8_t ui8_busNr, uint8_t ui8_sendObjNr)
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

#if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
void CanIo_c::printMyFilterBox(){

  for ( uint32_t i = 0; i < m_arrFilterBox.size(); i++ )
  {
    INTERNAL_DEBUG_DEVICE << "CANIO::VECTOR FilterBox :Filter: 0x"
    #ifdef SYSTEM_PC
        << std::hex
    #endif
        << m_arrFilterBox[i].filter().ident()
        << ", Mask: 0x"
    #ifdef SYSTEM_PC
        << std::hex
    #endif
        << m_arrFilterBox[i].mask().ident()
        << ", IdentType: "
    #ifdef SYSTEM_PC
      << std::dec
      #endif
      << m_arrFilterBox[i].identType()
        << ", FbVecId : "
    #ifdef SYSTEM_PC
        << std::dec
    #endif
        << m_arrFilterBox[i].getFbVecIdx();
    INTERNAL_DEBUG_DEVICE  << INTERNAL_DEBUG_DEVICE_ENDL;
  }
}
#endif

} // end of namespace __IsoAgLib
