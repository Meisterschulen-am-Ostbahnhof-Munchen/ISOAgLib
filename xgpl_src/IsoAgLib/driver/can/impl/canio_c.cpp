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
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/hal/system.h>
#include <IsoAgLib/hal/eeprom.h>

#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

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
  /** C-style function, to get access to the unique CANIO_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  CANIO_c& getCanInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return CANIO_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique DINMonitor_c singleton instance */
  CANIO_c& getCanInstance( void )
  {
    static CANIO_c &c_can_io = CANIO_c::instance();
    return c_can_io;
  };
#endif

/*******************************/
/** important global variables */
/*******************************/
/** global array for pointers to dynamic MsgObj_c array of CANIO_c objects for each BUS
  -> by BUS Nr parameter in interrupt function,
     the appropriate dynamic MsgObj_c array can be found */
#ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
STL_NAMESPACE::slist<MsgObj_c,STL_NAMESPACE::__malloc_alloc_template<0> >* irqMsgObjVec[CAN_BUS_CNT];
#else
STL_NAMESPACE::slist<MsgObj_c>* irqMsgObjVec[CAN_BUS_CNT];
#endif

uint8_t b_irqCanReceiveOffset[CAN_BUS_CNT];


/*******************************************/
/** definition of public element functions */
/*******************************************/

/**
  Initialize the CAN hardware, and instantiate one msg object for
   sending of messages. Do configuration for BUS number, sending bitrate,
  CAN ident length, minx/max hardware/BIOS Msg Obj numbers by parameters;
  called by specified constructor or external functions;
  wrong BUS and msg obj numbers are rejected and cause set of Err_c:range

  If even the rui8_busNumber parameter has the default value 0xFF,
  then the configuration settings of a previous init call are not
  changed. In this case, the CAN BUS is only reset with the old settings.
  This is enabled by the default value 0xFF for rui8_busNumber, which is
  changed to CONFIG_CAN_DEFAULT_BUS_NUMBER for the first call of init() after the constructor.
  In all other cases, the special value 0xFF is indicator for empty parameter list.

  possible errors:
      * Err_c::range on undefined BUS,  msgOb_nr or sendPufferSize,
      * Err_c::hwConfig on uninitialized BUS, undef. msgType or CAN-BIOS mem-err,
      * Err_c::busy on already used sending Msg-Obj
  @param rui8_busNumber optional number of the CAN bus
  @param rui16_bitrate optional bitrate (default by define in isoaglib_config.h)
  @param identType_t optional length of the ident
    (S (11bit), E (29bit), B
    (send both standard and extended ident msg) (default by define in isoaglib_config.h)
  @param rui8_minObjNr optional minimum number for hardware CAN message object
    (important for sharing CAN controller with other tasks) (default by define in isoaglib_config.h)
  @param rui8_maxObjNr optional maximum number for hardware CAN message object
    (default by define in isoaglib_config.h)
  @return true -> correct initialisation without errors
  @see HAL::can_configGlobalInit
  @see HAL::can_configMsgobjInit
  @see Ident_c::t_identType
*/
bool CANIO_c::init(uint8_t rui8_busNumber, uint16_t rui16_bitrate,
          Ident_c::identType_t ren_identType,
          uint8_t rui8_minObjNr,  uint8_t rui8_maxObjNr)
{ // first make shure that the base system is initialized
  getSystemInstance().init();
  // if FilterBox_c instances were created before the CAN_IO was
  // explicitly initialized, we must call reconfigureMsgObj NOW
  bool b_callReconfigureMsgObj = false;

  #ifdef DEBUG
  static bool firstDefaultInitCallStart = true;
  if ( firstDefaultInitCallStart ) {
    firstDefaultInitCallStart = false;
    INTERNAL_DEBUG_DEVICE
        << "Start CANIO_c::init() mit BUS: " << int(rui8_busNumber)
        << ", Bitrate: " << int(rui16_bitrate) << "\n";
  }
  #endif

  if ( ( rui8_busNumber != 0xFF ) || ( rui8_busNumber != ui8_busNumber ) )
  { // this is the first call of init after constructor,
    // or at least the BUS Number parameter is different from
    // default value -> interprete as explicit call which can
    // override old setting

    // close CAN Bus, if ui8_busNumber is valid at the
    // moment (this can be a reconfig after a default
    // config)
    if ( ui8_busNumber <= HAL_CAN_MAX_BUS_NR )
    {
      HAL::can_configGlobalClose(ui8_busNumber);
    }

    /* ************************************* */
    /* *****set initial attribute values**** */
    /* ************************************* */
    // set t_mask to clear values
    en_identType = DEFAULT_CONFIG_IDENT_TYPE;

    // set object vars to 0 detect forgotten init call
    ui8_busNumber = 0;
    setMinMsgObjNr(0);
    setMaxMsgObjNr(0);
    ui16_bitrate = 0;
    i32_canErrStart = 0;
    i32_canErrEnd = 0;
    ui8_processedMsgCnt = 0;
    c_maskStd.set(~0, Ident_c::StandardIdent);
    c_maskExt.set(~0, Ident_c::ExtendedIdent);
    c_maskLastmsg.set(0, DEFAULT_IDENT_TYPE);
    // check if some FilterBox_c instances were already created, so that
    // we must call reconfigureMsgObj NOW
    if ( arrFilterBox.size() > 0 )
    {
      b_callReconfigureMsgObj = true;
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE
         << "Call reconfigureMsgObj as some FilterBox_c instances are already  created"
         << "\n";
      #endif
    }
  }
  if ( rui8_busNumber == 0xFF )
  { // called with default value -> do nothing till called with normal parameter
    #ifdef DEBUG
    static bool firstDefaultInitCallEnd = true;
    if ( ! firstDefaultInitCallEnd ) {
      firstDefaultInitCallEnd = false;
      INTERNAL_DEBUG_DEVICE
          << "Ende CANIO_c::init() mit Default Werten bei spc_instance == "
          << uint32_t( spc_instance )
          << "\n";
    }
    #endif
    return false;
  }
  else
  {
    /* ****************************** */
    /* *****check input parameter**** */
    /* ****************************** */
    // check rui8_minObjNr, rui8_maxObjNr and rui8_busNumber
    if (
        (rui8_minObjNr > HAL_CAN_MAX_SEND_OBJ)
      ||(rui8_maxObjNr > HAL_CAN_MAX_SEND_OBJ)
    #if HAL_CAN_MIN_SEND_OBJ > 0
      ||(rui8_minObjNr < HAL_CAN_MIN_SEND_OBJ) // use this comparison only for (HAL_CAN_MIN_SEND_OBJ > 0), else it gives a warning!
      ||(rui8_maxObjNr < HAL_CAN_MIN_SEND_OBJ) // use this comparison only for (HAL_CAN_MIN_SEND_OBJ > 0), else it gives a warning!
    #endif
      ||(rui8_busNumber > HAL_CAN_MAX_BUS_NR)
        )
    { // one of the range tests not passed
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Can );
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE
          << "Ende CANIO_c::init() mit falschen Parametern\n";
      #endif
      return false; // exit function with error information
    }

    /* ******************************************************* */
    /* ****implementing of function with passed parameters**** */
    /* ******************************************************* */

    /** set min and max allowed obj numbers (important for multithreading
        where each threads manages only a portion of all objects)
    */
    setMinMsgObjNr(rui8_minObjNr);
    setMaxMsgObjNr(rui8_maxObjNr);
    // store given BUS number: if default value 0xFF is given, use
    // the default defined value
    ui8_busNumber = (rui8_busNumber != 0xFF)?rui8_busNumber:CONFIG_CAN_DEFAULT_BUS_NUMBER;
    // store wanted CAN identifier type
    en_identType = ren_identType;
    // set global irqMsgObjVec pointer for irqFun
    irqMsgObjVec[ui8_busNumber] = &arrMsgObj;
  }
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE
      << "Ende CANIO_c::init()\n";
  #endif

  // even for call of init without parameter the base can init
  // should be repeated
  // check and store bitrate, config CAN and send object(s)
  const bool b_result = baseCanInit(rui16_bitrate);
  if ( b_callReconfigureMsgObj ) reconfigureMsgObj();
  return b_result;
}
/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void CANIO_c::close( void )
{
  if ( ui8_busNumber == 0xFF )
  { // CAN already closed -> don't call HAL close again
    return;
  }

  // call BIOS CAN close function - set error state if BIOS function cause error
  switch (HAL::can_configGlobalClose(ui8_busNumber) )
  {
    case HAL_NO_ERR:
      break;
    case HAL_CONFIG_ERR:
      // ignore this type also, as this is only the indication of try to close an already closed channel
      #if defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(ui8_busNumber) << " was already closed before close call\r\n";
      #endif
      break;
    default:
      // wrong channel
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Can );
      break;
  }
  #ifdef DEBUG_HEAP_USEAGE
  sui16_filterBoxTotal -= arrFilterBox.size();
  sui16_msgObjTotal -= arrMsgObj.size();
  #endif
  arrFilterBox.clear();
  arrMsgObj.clear();
  ui8_busNumber = 0xFF;
}


/**
  periodically called function which does
  periodically needed actions; f.e. trigger watch
  dog and start processing received messages
  AND call init if ui16_bitrate is still set to 0 -> no CAN init performed
  @return true -> time events was performed
*/
bool CANIO_c::timeEvent( void ){
  // check if init was called
  if ( (ui16_bitrate == 0) || ( ui8_busNumber > HAL_CAN_MAX_BUS_NR ) )
  { // this can't be the case after correct call of init
    init(); // call with default values
  }
  // if still not ready, CANIO_c is not yet initialised complete -> do nothing
  if ( (ui16_bitrate == 0) || ( ui8_busNumber > HAL_CAN_MAX_BUS_NR ) ) return false;

  #if ( CONFIG_CAN_USE_FALLBACK_ON_CAN_DEADLOCK )
  // check if CAN BUS is o.k.
  int32_t i32_now = HAL::getTime();
  int32_t i32_lastSend;
  bool b_sendProblem = false;
  uint8_t ui8_bufCnt;
  i32_lastSend = HAL::can_stateMsgobjTxok(ui8_busNumber, minMsgObjNr());
  ui8_bufCnt = HAL::can_stateMsgobjBuffercnt(ui8_busNumber, minMsgObjNr());
  if ( ((i32_now - i32_lastSend) > 3000)
     &&(ui8_bufCnt > 0)
     )
  {
    b_sendProblem = true;
  }
  if (en_identType == Ident_c::BothIdent)
  { // check for second obj if DIN and ISO are used
    i32_lastSend = HAL::can_stateMsgobjTxok(ui8_busNumber, minMsgObjNr()+1);
    ui8_bufCnt = HAL::can_stateMsgobjBuffercnt(ui8_busNumber, minMsgObjNr()+1);
    if ( ((i32_now - i32_lastSend) > 3000)
       &&(ui8_bufCnt > 0)
       )
    {
      b_sendProblem = true;
    }
  }
  if (HAL::can_stateGlobalBlocked())b_sendProblem = true;
  if (b_sendProblem)
  { // a send problem occured -> close CAN and open again
    setBitrate(CONFIG_CAN_FALLBACK_BITRATE);
  }
  #endif

  // start process of all received msg
  return (processMsg() >= 0)?true:false;
}

/**
  deliver actual BUS load in baud
  @return baudrate in [kbaud] on used CAN BUS
*/
uint16_t CANIO_c::getBusLoad() const
{
  return HAL::can_stateGlobalBusload(ui8_busNumber);
}

/**
  set the minimum delay in msec. between two sent CAN messages
  @param rui16_minDelay minimum time between two CAN messages [msec.]
*/
void CANIO_c::setSendpause(uint16_t rui16_minDelay)const
{ // set send MsgObj ID
  uint8_t ui8_sendObjNr = minMsgObjNr();
  HAL::can_configMsgobjSendpause(ui8_busNumber, ui8_sendObjNr, rui16_minDelay);
  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if (en_identType == Ident_c::BothIdent)
  {
    HAL::can_configMsgobjSendpause(ui8_busNumber, (ui8_sendObjNr + 1), rui16_minDelay);
  }
}

/**
  deliver the numbers which can be placed at the moment in the send puffer
  @param ren_identType type of searched ident: standard 11bit or extended 29bit
    (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
  @return number of msgs which fit into send buffer
*/
uint8_t CANIO_c::sendCanFreecnt(Ident_c::identType_t ren_identType)
{ // set send MsgObj ID
  uint8_t ui8_sendObjNr = minMsgObjNr();
  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if ((en_identType == Ident_c::BothIdent) && (ren_identType ==Ident_c::ExtendedIdent))
  {
    ui8_sendObjNr += 1;
  }
  const int16_t i16_result = HAL::can_stateMsgobjFreecnt(ui8_busNumber, ui8_sendObjNr);
  if ( i16_result >= 0 ) return uint8_t(i16_result);
  else return 0;
}


/**
  clear the send buffer
  @param ren_identType type of searched ident: standard 11bit or extended 29bit
    (default DEFAULT_IDENT_TYPE set in isoaglib_config.h)
*/
void CANIO_c::sendCanClearbuf(Ident_c::identType_t ren_identType)
{
  uint8_t ui8_sendObjNr = minMsgObjNr();
  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if ((en_identType == Ident_c::BothIdent) && (ren_identType ==Ident_c::ExtendedIdent))
  {
    ui8_sendObjNr += 1;
  }
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE
   << "CANIO_c::sendCanClearbuf for MsgObj: " << uint16_t(ui8_sendObjNr) << "\r\n";
  #endif

  HAL::can_useMsgobjClear(ui8_busNumber, ui8_sendObjNr);
}


/**
  test if a FilterBox_c definition already exist
  (version expecial for standard ident, chosen at compile time)
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param ren_identType type of searched ident: standard 11bit (default) or extended 29bit
  @param rpc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CANIO_c::existFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
    uint16_t rt_mask, uint16_t rt_filter,
    Ident_c::identType_t ren_identType, ArrFilterBox::iterator* rpc_iter)
{
  // check if filter/t_mask are already inserted
  // return false if this setting isn´t unique
  Ident_c c_compFilter = Ident_c(rt_filter, ren_identType);
  Ident_c c_compMask = Ident_c(rt_mask, ren_identType);
  c_compFilter.set(rt_filter, ren_identType);
  c_compMask.set(rt_mask, ren_identType);

  return existFilter(rref_customer, c_compMask, c_compFilter, rpc_iter);
}
/**
  test if a FilterBox_c definition already exist
  (version expecial for extended ident, chosen at compile time)
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param ren_identType type of searched ident: standard 11bit (default) or extended 29bit
  @param rpc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CANIO_c::existFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
    uint32_t rt_mask, uint32_t rt_filter,
    Ident_c::identType_t ren_identType, ArrFilterBox::iterator* rpc_iter)
{
  // check if filter/t_mask are already inserted
  // return false if this setting isn´t unique
  Ident_c c_compFilter = Ident_c(rt_filter, ren_identType);
  Ident_c c_compMask = Ident_c(rt_mask, ren_identType);
  c_compFilter.set(rt_filter, ren_identType);
  c_compMask.set(rt_mask, ren_identType);

  return existFilter(rref_customer, c_compMask, c_compFilter, rpc_iter);
}

/**
  test if a FilterBox_c definition already exist
  (version with comper items as Ident_c class instances, chosen by compiler)
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rc_compMask individual mask for this filter box
  @param rc_compFilter individual filter
  @param rpc_iter optional pointer Iterator to result FilterBox
  @return true -> same FilterBox_c already exist
*/
bool CANIO_c::existFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
    const Ident_c& rc_compMask, const Ident_c& rc_compFilter, ArrFilterBox::iterator* rpc_iter)
{
  // check if filter/t_mask are already inserted
  // return false if this setting isn´t unique
  bool b_identDefFound = false;

  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter = arrFilterBox.begin();

  // check if given FilterBox_c definition is not yet in array
  for (; pc_iter != arrFilterBox.end(); pc_iter++)
  {
    if ( ( pc_iter->equalFilterMask(rc_compMask, rc_compFilter ) )
      && ( pc_iter->equalCustomer( rref_customer )               ) )
    { // FilterBox_c with equal def found
      //-> don't insert complete ident def two times
      b_identDefFound = true;
      // don't search rest of array
      break;
    }
  }
  if (b_identDefFound && (rpc_iter != NULL)) *rpc_iter = pc_iter;
  return b_identDefFound;
}

/**
  create a Filter Box with specified rt_mask/rt_filter
  on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
  rb_reconfigImmediate == true -> useful for
  avoiding unneeded reconfiguration during
  sequence of FilterBox_c insertions;
  by rref_customer CANIO_c (FilterBox_c) can start direct processing
  of received data in dedicated customer object (no search);
  uses BIOS functions

  This specialized function allows the user to use the same message
  object as the filter box that is given "rpc_connectedFilterBox"
  Use this feature if messages from two or more filterboxes have to be processed chronologically

  possible errors:
      * Err_c::badAlloc on not enough memory for new FilterBox_c instance or for new configured MsgObj_c's
  @see __IsoAgLib::CANCustomer
  @param rref_customer reference to __IsoAgLib::CANCustomer_c which needs filtered
      messages (-> on received msg call rref_customer.processMsg())
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param rb_reconfigImmediate true -> all Filter objects are reconfigured to according
      CAN hardware MsgObj after creating this filter
  @param rt_identType ident type of the created ident: standard 11bit (default) or extended 29bit
  @return != NULL -> if inserting and wanted reconfiguration are performed without errors, a reference to the created FilterBox is returned
  @exception badAlloc
*/
FilterBox_c* CANIO_c::insertFilter(__IsoAgLib::CANCustomer_c& rref_customer,
    uint32_t rt_mask, uint32_t rt_filter, bool rb_reconfigImmediate,
    const Ident_c::identType_t rt_identType, FilterBox_c* rpc_connectedFilterBox)
{
  Ident_c c_newMask = Ident_c(rt_mask, rt_identType);
  Ident_c c_newFilter = Ident_c(rt_filter, rt_identType);
  if (existFilter(rref_customer, c_newMask, c_newFilter))
  {
    // filter definition already inserted
    return NULL;
  }
  // now the filterBox def is new -> insert new FilterBox_c instance in array

  // use for insert a member var FilterBox_c to avoid creation of temp obj during insert
  // clear temp object
  c_tempFilterBox.clearData();

  // define temp FilterBox_c with new values
  c_tempFilterBox.set(c_newMask, c_newFilter, &rref_customer, rpc_connectedFilterBox);

  // insert new FilterBox_c and exit function if no dyn array growth is reported
  const uint8_t b_oldSize = arrFilterBox.size();
  // insert temp object in vector arrFilterBox -> can cause badAlloc exception
  arrFilterBox.push_front(c_tempFilterBox);
  if (b_oldSize >= arrFilterBox.size())
  { // dynamic array didn't grow -> alloc error
    getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::Can );
    return NULL; // exit the function
  }
  #ifdef DEBUG_HEAP_USEAGE
  else
  {
    sui16_filterBoxTotal++;
  }
  #endif

  // if wanted, configure everything, to set CAN hardware MsgObj_c as wanted
  // for sequence of arrFilterBox insertions only configure after the last insert
  if ( rb_reconfigImmediate )
  {
    reconfigureMsgObj();
  } // if (rb_configImmediate)

  // insertion of FilterBox_c was successfull
  return &arrFilterBox.front();
}



/**
  create a Filter Box with specified rt_mask/rt_filter
  on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
  rb_reconfigImmediate == true -> useful for
  avoiding unneeded reconfiguration during
  sequence of FilterBox_c insertions;
  by rref_customer CANIO_c (FilterBox_c) can start direct processing
  of received data in dedicated customer object (no search);
  uses BIOS functions

  This specialized function allows the user to use the same message
  object as the filter box that exists with the given extra rt_mask/rt_filter pair!
  Use this feature if messages from two or more filterboxes have to be processed chronologically

  possible errors:
      * Err_c::badAlloc on not enough memory for new FilterBox_c instance or for new configured MsgObj_c's
  @see __IsoAgLib::CANCustomer
  @param rref_customer reference to __IsoAgLib::CANCustomer_c which needs filtered
      messages (-> on received msg call rref_customer.processMsg())
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param rb_reconfigImmediate true -> all Filter objects are reconfigured to according
      CAN hardware MsgObj after creating this filter
  @param rt_identType ident type of the created ident: standard 11bit (default) or extended 29bit
  @return != NULL -> if inserting and wanted reconfiguration are performed without errors, a reference to the created FilterBox is returned
  @exception badAlloc
*/
FilterBox_c* CANIO_c::insertFilter(__IsoAgLib::CANCustomer_c& rref_customer,
    uint32_t rt_mask, uint32_t rt_filter, bool rb_reconfigImmediate,
    const Ident_c::identType_t rt_identType, uint32_t rt_connectedMask, uint32_t rt_connectedFilter, const Ident_c::identType_t rt_connectedIdentType)
{
  Ident_c c_connectedMask = Ident_c(rt_connectedMask, rt_connectedIdentType);
  Ident_c c_connectedFilter = Ident_c(rt_connectedFilter, rt_connectedIdentType);

  bool b_connectedFilterFound = false;

  // check if given FilterBox_c definition is in array
  ArrFilterBox::iterator pc_iter = arrFilterBox.begin();
  for (; pc_iter != arrFilterBox.end(); pc_iter++)
  {
    if ( pc_iter->equalFilterMask(c_connectedMask, c_connectedFilter ) )
    { // FilterBox_c with equal def (mask, filter, NOT customer) found
      b_connectedFilterFound = true;
      break; // don't search rest of array
    }
  }

  if (!b_connectedFilterFound) // "existFilter()" not used as it also compares the customer!
    return NULL;

  return insertFilter(rref_customer, rt_mask, rt_filter, rb_reconfigImmediate, rt_identType, &(*pc_iter));
}


/**
  delete a FilterBox definition
  @param rref_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
  @param rt_mask individual mask for this filter box
  @param rt_filter individual filter
  @param rt_identType ident type of the deleted ident: standard 11bit or extended 29bit
        (defualt DEFAULT_IDENT_TYPE defined in isoaglib_config.h)
  @return true -> FilterBox_c found and deleted
*/
bool CANIO_c::deleteFilter(const __IsoAgLib::CANCustomer_c& rref_customer,
    MASK_TYPE rt_mask, MASK_TYPE rt_filter,
    const Ident_c::identType_t rt_identType)
{
  bool b_result = false;
  Ident_c c_deleteMask = Ident_c(rt_mask, rt_identType),
        c_deleteFilter = Ident_c(rt_filter, rt_identType);
  // iterator for quick access to all array elements
  ArrFilterBox::iterator pc_iter;

  if (existFilter(rref_customer, c_deleteMask, c_deleteFilter, &pc_iter))
  { // filter found -> delete element where pc_iter points to
    arrFilterBox.erase(pc_iter);
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
      << "\r\n";
    #else
      << "\r\n\r\n";
    #endif
    #endif
    reconfigureMsgObj();
    b_result = true;
  }
  return b_result;
}

/**
  verify given BUS number and MsgObj number, if they are correct
  (mostly used by MsgObj_c to verify itself)
  @param rui8_busNr number of the BUS
  @param rui8_msgobjNr number of the MsgObj
  @return true -> values are correct
*/
bool CANIO_c::verifyBusMsgobjNr(uint8_t rui8_busNr, uint8_t rui8_msgobjNr)
{ //cehck if there is an error
  if (
     (rui8_busNr > HAL_CAN_MAX_BUS_NR)
  || (rui8_msgobjNr < (minMsgObjNr() + 1))
  || (rui8_msgobjNr > maxMsgObjNr())
     )
  {
    return false;
  }
  else
  {
    return true;
  }
}


/**
  initiate processing of all received msg
  check all active MsgObj_c for received CAN msg and
  initiate their processing
  @return <0 --> not enough time to process all messages.
         ==0 --> no messages were received.
         >0  --> all messages are processed, number of messages
*/
int16_t CANIO_c::processMsg(){
  ui8_processedMsgCnt = 0;

  // first check if last Message Object is open,
  // and process these messages first
  // - this happens, if this is first processMsg call after
  //   reconfigureMsgObj
  if ( c_lastMsgObj.isOpen() )
  { // process all messages which where placed during the reconfig in the last message object
    ui8_processedMsgCnt += c_lastMsgObj.processMsg( ui8_busNumber, true );
    c_lastMsgObj.close();
  }

  for (ArrMsgObj::iterator pc_iter = arrMsgObj.begin(); pc_iter != arrMsgObj.end(); pc_iter++)
  { // stop processing of message buffers, if not enough time
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return -1;
    System_c::triggerWd();
    ui8_processedMsgCnt += pc_iter->processMsg(ui8_busNumber);
  }

  // return the number of received telegrams
  return ui8_processedMsgCnt;
}


/**
  function for sending data out of CANPkg_c (uses BIOS function)
  if send puffer is full a local loop waits till puffer has enough space
  (every 100ms the watchdog is triggered, to avoid watchdog reset)

  possible errors:
      * Err_c::hwConfig on wrong configured CAN obj, not init BUS or no configured send obj
      * Err_c::range on undef BUS or BIOS send obj nr
      * Err_c::can_warn on physical CAN-BUS problems
      * Err_c::can_off on physical CAN-BUS off state
  @param rrefc_src CANPkg_c which holds the to be sent data
  @return reference to this CANIO_c instance ==> needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
*/
CANIO_c& CANIO_c::operator<<(CANPkg_c& refc_src)
{ // immediately return if CAN is not yet configured for send
  if ( ! isReady2Send() ) return *this;
  // set send MsgObj ID
  uint8_t ui8_sendObjNr = minMsgObjNr();
  #if 0
  if ( ui8_busNumber == 1 ) {
    INTERNAL_DEBUG_DEVICE
      << "CANIO_c::operator<< mit MIN MsgObj Nr: " << uint16_t( ui8_sendObjNr )
      << "Xtd: " << refc_src.identType()
      << "\n\r";
  }
  #endif

  // if both standard and extended ident can be sent -> use second send obj if data uses extended ident
  if (en_identType == Ident_c::BothIdent)
  {
    if (refc_src.identType() == Ident_c::ExtendedIdent)
    {
      ui8_sendObjNr += 1;
    }
  }

  // wait till Msg can be placed in send buffer
  /** @todo check for better handling of full buffer - don't just loop 10 times */
  for ( uint16_t ind = 0;
        ( ( HAL::can_stateMsgobjFreecnt( ui8_busNumber, ui8_sendObjNr ) < 1 ) && ( ind < 10 ) );
        ind++ )
  {  // perform wait loop
    int16_t i16_result = 0;
    for (int16_t i = 0; i < 200; i++) i16_result++;
    // it's time to trigger the watchdog
    HAL::wdTriggern();
    // exit loop, if CAN BUS is blocked and exit function
    if (HAL::can_stateGlobalBlocked(ui8_busNumber))
    {  // clear MsgObj CAN queue
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE
       << "CANIO_c::operator<< Blocked BUS Nr: " << uint16_t(ui8_busNumber) << "\r\n";
      #endif

      HAL::can_useMsgobjClear(ui8_busNumber,ui8_sendObjNr);
      return *this;
    }
  }
  // it's time to trigger the watchdog
  HAL::wdTriggern();

  // Msg can be placed in MsgObj
  #ifdef DEBUG
  bool b_bit1err = HAL::can_stateGlobalBit1err(ui8_busNumber);
  bool b_sendproblem = HAL::can_stateMsgobjSendproblem(ui8_busNumber, ui8_sendObjNr);
  bool b_warnState = HAL::can_stateGlobalWarn(ui8_busNumber);

  if ( b_bit1err )
  {
      INTERNAL_DEBUG_DEVICE
      << "BITERR VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNumber) << ", MsgObj: " << uint16_t(ui8_sendObjNr)
      << "\r\n";
  }
  if ( b_sendproblem )
  {
    INTERNAL_DEBUG_DEVICE << "SENDPROBLEM VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNumber) << ", MsgObj: " << uint16_t(ui8_sendObjNr)
      << "\r\n";
  }
  if ( b_warnState )
  { INTERNAL_DEBUG_DEVICE
      << "CAN_WARN VOR HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNumber) << ", MsgObj: " << uint16_t(ui8_sendObjNr)
      << "\r\n";
  }
  #endif
  int16_t i16_sendFuncState = HAL::can_useMsgobjSend(ui8_busNumber, ui8_sendObjNr, &refc_src);

  #ifdef DEBUG
  b_bit1err = HAL::can_stateGlobalBit1err(ui8_busNumber);
  b_sendproblem = HAL::can_stateMsgobjSendproblem(ui8_busNumber, ui8_sendObjNr);
  b_warnState = HAL::can_stateGlobalWarn(ui8_busNumber);

  if ( b_bit1err )
  { INTERNAL_DEBUG_DEVICE
      << "BITERR NACH HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNumber) << ", MsgObj: " << uint16_t(ui8_sendObjNr)
      << "\r\n";
  }
  if ( b_sendproblem )
  {
    INTERNAL_DEBUG_DEVICE << "SENDPROBLEM NACH HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNumber) << ", MsgObj: " << uint16_t(ui8_sendObjNr)
      << "\r\n";
  }
  if ( b_warnState )
  { INTERNAL_DEBUG_DEVICE
      << "CAN_WARN NACH HAL::can_useMsgobjSend BUS: "
      << uint16_t(ui8_busNumber) << ", MsgObj: " << uint16_t(ui8_sendObjNr)
      << "\r\n";
  }
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
      #if defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "\r\nBUS " << uint16_t(ui8_busNumber) << " not initialized or MsgObj: " << uint16_t(ui8_sendObjNr) << " no send obj\r\n";
      #endif
      getLbsErrInstance().registerError( LibErr_c::HwConfig, LibErr_c::Can );
      break;
    case HAL_NOACT_ERR:
      // BUS off
      getLbsErrInstance().registerError( LibErr_c::CanOff, LibErr_c::Can );
      break;
    case HAL_OVERFLOW_ERR:
      // overflow of send puffer
      getLbsErrInstance().registerError( LibErr_c::CanOverflow, LibErr_c::Can );
      break;
    case HAL_RANGE_ERR:
      // BUS nr or obj nr outer allowed limits (shouldn't be the case after successful init call)
      getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Can );
      break;
    case HAL_WARN_ERR:
      // signal for BUS-WARN problem
      #if defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "BUS " << uint16_t(ui8_busNumber) << " in WARN STATE\r\n";
      #endif
      getLbsErrInstance().registerError( LibErr_c::CanWarn, LibErr_c::Can );
      break;
  } //switch

  // return pointer to self for sequence of send operations
  return *this;
}

/**********************************************/
/** definition of protected element functions */
/**********************************************/


/** evaluate common bits of all defined filterBox
    instances and set it in mask -> build up global mask */
void CANIO_c::getCommonFilterMask(){
  // preset all bits with "1"
  c_maskStd.set(~0, Ident_c::StandardIdent);
  c_maskExt.set(~0, Ident_c::ExtendedIdent);

  // combine masks of all arrFilterBox with AND
  for (ArrFilterBox::iterator pc_iter = arrFilterBox.begin();
       pc_iter != arrFilterBox.end();
       pc_iter++
      )
      {
        if (pc_iter->identType() == Ident_c::StandardIdent) {
          c_maskStd.ident_bitAnd(pc_iter->mask());
          c_maskStd.ident_bitAnd(pc_iter->additionalMask());
        }
        else {
          c_maskExt.ident_bitAnd(pc_iter->mask());
          c_maskExt.ident_bitAnd(pc_iter->additionalMask());
        }
      }
}

/** correct global masks after merge based on merged IDs of all
    resulting MsgObj_c instances
 */
void CANIO_c::getCommonFilterMaskAfterMerge(){

  // combine masks of all arrFilterBox with AND
  for (ArrMsgObj::iterator pc_iter = arrMsgObj.begin();
       pc_iter != arrMsgObj.end();
       pc_iter++
      )
  {
    if (pc_iter->filter().identType() == Ident_c::StandardIdent)
      pc_iter->commonFilterAfterMerge( c_maskStd );
    else pc_iter->commonFilterAfterMerge( c_maskExt );
  }
}

/**
  build unique MsgObj_c's based on given
  global mask (different FilterBox_c def's can result in one
  MsgObj_c if differences are hidden by global mask), return count of obj

  possible errors:
      * Err_c::badAlloc on not enough memory for created MsgObj_c instances
  @see MsgObj
  @see FilterBox
*/
int16_t CANIO_c::FilterBox2MsgObj(){
  int16_t i16_result = 0;
  Ident_c c_tempIdent;
  ArrMsgObj::iterator pc_iterMsgObj, pc_searchEnd = arrMsgObj.begin();

  // clear the settings in all previously created MsgObj_c instances
  for (pc_iterMsgObj = arrMsgObj.begin(); pc_iterMsgObj != arrMsgObj.end(); pc_iterMsgObj++)
  { // first process all messages in the buffer IF OPEN, and then close
    HAL::wdTriggern();
    if ( pc_iterMsgObj->isOpen() )
    { // process received messages
      pc_iterMsgObj->lock( true );
      pc_iterMsgObj->processMsg( ui8_busNumber, true );
    }
    pc_iterMsgObj->close();
    pc_iterMsgObj->lock( false );
  }

  HAL::wdTriggern();
  // gather globalMask dependent filter bits of each arrFilterBox
  // and create suitable arrMsgObj if not already created
  for (ArrFilterBox::iterator pc_iterFilterBox = arrFilterBox.begin();
       pc_iterFilterBox != arrFilterBox.end();
       pc_iterFilterBox++
      )
  { // get filter Ident_c setting from actual FilterBox
    c_tempIdent = pc_iterFilterBox->filter();
    // mask the actual filter Ident_c with the bits of the global mask
    if (c_tempIdent.identType() == Ident_c::StandardIdent)
      c_tempIdent.ident_bitAnd(c_maskStd);
    else c_tempIdent.ident_bitAnd(c_maskExt);

    for (pc_iterMsgObj = arrMsgObj.begin(); pc_iterMsgObj != pc_searchEnd; pc_iterMsgObj++)
    { // compare and break search loop if equal
      if (pc_iterMsgObj->equalFilter(c_tempIdent)) break;
    }

    // if no item with this filter was found add new MsgObj
    if (pc_iterMsgObj == pc_searchEnd)
    {
      // no equal item found in arrMsgObj -> create new MsgObj_c in arrMsgObj
      if (pc_searchEnd != arrMsgObj.end())
      {
        // the arrMsgObj for this filter setting was
        // already previous created, can now be reused

        // set filter in arrMsgObj
        // new last element to be configured after actual last element
        pc_searchEnd->setFilter(c_tempIdent);
        pc_searchEnd->insertFilterBox(pc_iterFilterBox);
        pc_searchEnd++;
      }
      else
      {
        // previous less arrMsgObj was created -> create new arrMsgObj
        // clear temp obj (it is reused for each function call)
        c_tempObj.close();
        // set filter in new pc_lt_element
        c_tempObj.setFilter(c_tempIdent);
        c_tempObj.insertFilterBox(pc_iterFilterBox);
        // check if insertion try result in growed dyn array
        const uint16_t ui16_oldSize = arrMsgObj.size();
        // insert obj in vector
        arrMsgObj.push_front(c_tempObj);
        if (ui16_oldSize >= arrMsgObj.size())
        { // dyn array didn't grow -> set badAlloc error state
          getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::Can );
        }
        #ifdef DEBUG_HEAP_USEAGE
        else
        {
          sui16_msgObjTotal++;
        }
        #endif
        pc_searchEnd = arrMsgObj.end();
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
  // if less objects are created than already in vector, erase the unused
  // -> behind the last instance is NOT the list end
  if (pc_searchEnd!= arrMsgObj.end())
  {
    #ifdef DEBUG_HEAP_USEAGE
    const uint16_t cui16_oldSize = arrMsgObj.size();
    #endif
    HAL::wdTriggern();
    arrMsgObj.erase(pc_searchEnd, arrMsgObj.end());
    #ifdef DEBUG_HEAP_USEAGE
    sui16_msgObjTotal -= ( cui16_oldSize - arrMsgObj.size() );
    sui16_deconstructMsgObjCnt += (cui16_oldSize - arrMsgObj.size() );
    #endif
  }
  if ( i16_result >= 0) i16_result = arrMsgObj.size();
  #ifdef DEBUG_HEAP_USEAGE
  INTERNAL_DEBUG_DEVICE
    << "MsgObj: " << sui16_msgObjTotal << " -> ";
  #endif
  return i16_result;
}



/**
  checks if count of preconfigured msgObj
  is allowed; shrink by merging the objects
  with smallest Bit distance
  -> all existing MsgObj_c instances can be
     configured in hardware after execution
  @see MsgObj
*/
void CANIO_c::CheckSetCntMsgObj(){
  // set i16_minDistance to the max possible bit difference
  // => amount of bits in data type for ident
  int16_t i16_minDistance = sizeof(MASK_TYPE)*8,
      i16_tempDist;
  uint8_t b_allowedSize = maxMsgObjNr() - (minMsgObjNr() + 1);
  // check if result of b_allowedSize is correct
  if ((b_allowedSize == 0) || (b_allowedSize > 14)) b_allowedSize = 12;

  // if both ident types are sent, two send objects are configured -> reserve more space
  if (en_identType == Ident_c::BothIdent) b_allowedSize--;
  ArrMsgObj::iterator pc_minLeft = arrMsgObj.begin(),
          pc_minRight = arrMsgObj.begin();

  // [min;max] allowed, but first one or two reserved for send
  while (arrMsgObj.size() > b_allowedSize)
  { // more objects than allowed are in arrMsgObj, because intervall between
    // min+1 and max is smaller than size -> shrink array

    // start a comparison(left_ind, right_ind) loop for all elements
    for (ArrMsgObj::iterator pc_leftInd = arrMsgObj.begin(); pc_leftInd != arrMsgObj.end(); pc_leftInd++)
    {
      ArrMsgObj::iterator pc_rightInd = pc_leftInd;
      for (pc_rightInd++; pc_rightInd != arrMsgObj.end(); pc_rightInd++)
      {
        // retreive bit distance between instances left_ind and right_ind -> use Bit-XOR
        i16_tempDist = pc_leftInd->filter().bit_diff(pc_rightInd->filter());

        // store new min only if capacity of left is enough for objects of right
        if ((i16_tempDist < i16_minDistance)
          && (pc_leftInd->getFilterBoxCapacity() >= pc_rightInd->cnt_filterBox())
          && (pc_leftInd->filter().identType() == pc_rightInd->filter().identType())
           )
        {
          // new max similar filter found
          pc_minLeft = pc_leftInd;
          pc_minRight = pc_rightInd;
          i16_minDistance = i16_tempDist;
          if ( i16_tempDist == 0 ) break; // stop searching for "smaller" difference if 0 is found
        }
      }
    }
    // now min_dist is minimal bit distance of most similar filters at i16_minLeft and
    // i16_minRight -> merge them
    // merge right filter into the left
    pc_minLeft->merge(*pc_minRight);
    arrMsgObj.erase(pc_minRight);
    #ifdef DEBUG_HEAP_USEAGE
    sui16_msgObjTotal--;
    sui16_deconstructMsgObjCnt++;
    #endif
    // reset search arguments for posible next search
    pc_minRight = pc_minLeft = arrMsgObj.begin();
    i16_minDistance = sizeof(MASK_TYPE)*8;
  }
  // now the amount of arrMsgObj is allowed
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
    << "\r\n";
  #else
    << "\r\n\r\n";
  #endif
  #endif
}

/* *************************************** */
/* definition of private element functions */
/* *************************************** */

/**
  reconfigure the MsgObj after insert/delete of FilterBox
*/
bool CANIO_c::reconfigureMsgObj()
{ // verify that this instance i yet initialised complete (CAN BUS is dedicated)
  if ( ui8_busNumber > HAL_CAN_MAX_BUS_NR ) return false;

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
    << "\r\n";
  #else
    << "\r\n\r\n";
  #endif
  #endif

  bool b_result = true;
  b_irqCanReceiveOffset[ui8_busNumber] = 1;
  //increase receive offset to 2 if both 11bit and 29bit ident CAN are used

  if (en_identType == Ident_c::BothIdent) b_irqCanReceiveOffset[ui8_busNumber] = 2;
  // store old mask to check if global CAN BIOS mask must be changed
  Ident_c oldMaskStd = c_maskStd,
        oldMaskExt = c_maskExt;

  // build suitable global t_mask
  getCommonFilterMask();

  if ( c_lastMsgObj.isOpen() )
  { // process all messages which where placed during the reconfig in the last message object
    c_lastMsgObj.processMsg( ui8_busNumber, true );
    // unlock it in any case
    c_lastMsgObj.lock( false );
  }
  else
  { // not yet open -> open it now
    Ident_c tmp ( 0, Ident_c::ExtendedIdent );
    // configure the last MsgObj_c
    c_lastMsgObj.setFilter( tmp );
    c_lastMsgObj.configCan(ui8_busNumber, HAL_CAN_LAST_MSG_OBJ_NR );
  }

  // create according to new global t_mask all MsgObj_c with
  // unique filter settings -> merge all filter settings where
  // the the global t_mask delets all different bits
  if ( FilterBox2MsgObj() == false) b_result = false;

  // shrink number of MsgObj to fit the intervall of
  // [min_msgObjNr; maxMsgObjNr]
  CheckSetCntMsgObj();

  // check and correct global masks after merge in CheckSetCntMsgObj()
  getCommonFilterMaskAfterMerge();

  if ((oldMaskStd != c_maskStd) || (oldMaskExt != c_maskExt))
  {
    // global t_mask setting has changed -> reinit CAN with new global masks
    baseCanInit(ui16_bitrate);
  }

  int16_t i = 0;
  // config the hardware -> config all MsgObj_c in CAN hardware
  for (ArrMsgObj::iterator pc_iterMsgObj = arrMsgObj.begin();
       pc_iterMsgObj != arrMsgObj.end();
       pc_iterMsgObj++
      )
  { // call the configCan function for each MsgObj
    pc_iterMsgObj->configCan(ui8_busNumber, minMsgObjNr()+i+b_irqCanReceiveOffset[ui8_busNumber]);
    i++;
  }

  // lock the MsgObj_c to avoid receive of further messages
  c_lastMsgObj.lock( true );

  // clear any CAN BUFFER OVERFLOW error that might occure
  // for last message object
  getLbsErrInstance().clear( LibErr_c::CanOverflow, LibErr_c::Can );

  return b_result;
}

/**
  helper function to search all FilterBoxes for matching
  instance which maps to received CAN messages
  ( needed if the coordinated FilterBoxes don't match,
    especially important to process messages from global
    CAN MsgObj which doesn't have explicit connected FilterBox
    instances )
  @param rui32_ident Ident of received CAN message
  @return pointer to matching FilterBox instance or NULL if no matching found
*/
FilterBox_c* CANIO_c::canMsg2FilterBox( uint32_t rui32_ident, Ident_c::identType_t rt_type )
{
  for (ArrFilterBox::iterator pc_iterFilterBox = arrFilterBox.begin();
       pc_iterFilterBox != arrFilterBox.end();
       pc_iterFilterBox++
      )
  {
    if ( pc_iterFilterBox->matchMsgId( rui32_ident, rt_type ) )
    { // matching FilterBox_c found
      return &(*pc_iterFilterBox);
    }
  }
  // if execution reaches this point, no matching FilterBox_c found
  return NULL;
}


/**
  switch CAN bitrate (possible during runtime
  with automatic reconfiguring of CAN MsgObj)
  @param rui16_newSpeed
*/
bool CANIO_c::setBitrate(uint16_t rui16_newSpeed, bool rb_force)
{ // don't reconfigure if same bitrate
  if ((rui16_newSpeed == ui16_bitrate)&&(rb_force == false)) return true;

  bool b_success = true;
  uint8_t b_receiveOffset = 1;
  //increase receive offset to 2 if both 11bit and 29bit ident CAN are used

  if (en_identType == Ident_c::BothIdent) b_receiveOffset = 2;

  // init CAN with new speed (it checks if bitrate is allowed and closes can first
  if (baseCanInit(rui16_newSpeed) == false) return false;

  // config the MsgObj with same filter but with new speed
  int16_t i = 0;
  // config the hardware -> config all MsgObj_c in CAN hardware
  for (ArrMsgObj::iterator pc_iterMsgObj = arrMsgObj.begin();
       pc_iterMsgObj != arrMsgObj.end();
       pc_iterMsgObj++
      )
  { // call the configCan function for each MsgObj
    pc_iterMsgObj->configCan(ui8_busNumber, minMsgObjNr()+i+b_receiveOffset);
    i++;
  }
  return b_success;
}

/**
  perform bas init for CAN with set of speed and init of send object(s)
  @param rui16_bitrate wanted CAN bitrate
*/
bool CANIO_c::baseCanInit(uint16_t rui16_bitrate)
{
  HAL::wdTriggern();
  // check if rui16_bitrate is one of the allowed settings
  int16_t pi16_allowed[] = HAL_CAN_BITRATE_LIST;
  bool b_allowed = false;
  for (uint8_t ui8_index = 0; ui8_index < HAL_CAN_BITRATE_CNT; ui8_index++)
  {
    if (pi16_allowed[ui8_index] == rui16_bitrate)
    { // given bitrate is in allowed list at position ui8_index
      b_allowed = true; // store success state
      break; // exit search loop
    }
  }
  // if given bitrate fits to no allowed value set a range error state
  if (!b_allowed)
  {
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Can );
    return false; // exit function with error value
  }

  bool b_lastMsgObjWasOpen = false;
  if ( c_lastMsgObj.isOpen() )
  { // the last MsgObj_c is open -> so it may contain some unprocessed messages
    // ==>> process all messages which where placed during the reconfig in the last message object
    c_lastMsgObj.processMsg( ui8_busNumber, true );
    c_lastMsgObj.close();
    b_lastMsgObjWasOpen = true;
  }


  // store wanted bitrate for CAN communication
  ui16_bitrate = rui16_bitrate;

  // to prevent double re-init of can close it first
  HAL::can_configGlobalClose(ui8_busNumber);
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE
      << "CANIO_c::baseCanInit( " << rui16_bitrate << " ) vor HAL::can_configGlobalInit\n";
  #endif
  // init CAN BUS (BIOS function)
  int16_t i16_retvalInit = HAL::can_configGlobalInit(ui8_busNumber, ui16_bitrate, c_maskStd.ident(), c_maskExt.ident(),
                      c_maskLastmsg.ident());

  #ifdef USE_CAN_EEPROM_EDITOR
    /* if CAN init with success init CAN EEEditor */
    if ( ( i16_retvalInit == HAL_NO_ERR) && ( ui8_busNumber == CONFIG_EEPROM_USE_CAN_BUS ) )
    {
      uint8_t b_eepromType;
      if ((en_identType == Ident_c::BothIdent) || (en_identType == Ident_c::StandardIdent))
      {
        b_eepromType = Ident_c::StandardIdent;
      }
      else
      {
        b_eepromType = Ident_c::ExtendedIdent;
      }
      HAL::InitEEEditor( CONFIG_EEPROM_USE_CAN_BUS, maxMsgObjNr(),
          (maxMsgObjNr() - 1), CONFIG_EEPROM_USE_CAN_REC_IDENT, b_eepromType,
          CONFIG_EEPROM_USE_CAN_BUFFER_SIZE, CONFIG_CAN_SEND_BUFFER_SIZE);
    }
  #endif

  // check for error state
  if (i16_retvalInit == HAL_RANGE_ERR)
  { // BIOS complains about limits of BUSnr or msgObj
    // seldom, because before checked with defined LIMITS
    getLbsErrInstance().registerError( LibErr_c::Range, LibErr_c::Can );
    return false;
  }


  if ( b_lastMsgObjWasOpen )
  { // open last message buffer during reconfig of other MSgObj as CAN is already activated and at least one MsgObj is already in use
    // --> avoid loss of CAN messages which would normally be stored in the already active MsgObj_c instances
    Ident_c tmp ( 0, Ident_c::ExtendedIdent );
    // configure the last MsgObj_c
    c_lastMsgObj.setFilter( tmp );
    c_lastMsgObj.configCan(ui8_busNumber, HAL_CAN_LAST_MSG_OBJ_NR );
  }

  // init of BUS without error -> continue with send obj configuration

  // init BIOS send Msg Object (BIOS dependent)

  // return value which infomrs about success on BIOS configuration
  int16_t i16_retvalConfig = 0;
  // config the send CAN object with BIOS function
  bool b_configSuccess = true;
  Ident_c c_sendIdent;

  const uint8_t cui8_firstSendMsgObj = minMsgObjNr();
  const uint8_t cui8_potentialXtdSendObj = minMsgObjNr() + 1;

  for (uint8_t ui8_msgObjNr = cui8_firstSendMsgObj; ui8_msgObjNr <= cui8_potentialXtdSendObj; ui8_msgObjNr++)
  {
    switch (en_identType)
    {
      case Ident_c::StandardIdent:
        c_sendIdent.setIdentType(Ident_c::StandardIdent);
        break;
      case Ident_c::ExtendedIdent:
        c_sendIdent.setIdentType(Ident_c::ExtendedIdent);
        break;
      case Ident_c::BothIdent:
        if (ui8_msgObjNr == cui8_firstSendMsgObj) c_sendIdent.setIdentType(Ident_c::StandardIdent);
        else c_sendIdent.setIdentType(Ident_c::ExtendedIdent);
        break;
    }

    i16_retvalConfig = HAL::can_configMsgobjInit(ui8_busNumber,ui8_msgObjNr, c_sendIdent, true);
    // check for error state
    switch (i16_retvalConfig)
    {
      case HAL_NO_ERR:
        break;
      case HAL_BUSY_ERR:
        // send obj already in use
        getLbsErrInstance().registerError( LibErr_c::Busy, LibErr_c::Can );
        break;
      case HAL_CONFIG_ERR:
        // because CAN was initialized direct above without error, msgObj limits was
        // allowed and the SEND type is hardcoded to an allowed number,
        // HAL_CONFIG_ERR error can only be caused by allocating problem
        getLbsErrInstance().registerError( LibErr_c::BadAlloc, LibErr_c::Can );
        b_configSuccess = false;
        break;
    }
    // only run second time if both ident types should be sent
    if (en_identType != Ident_c::BothIdent) break;
  } // end for

  // return true -> must be set according success
  return b_configSuccess;
}

/**
  check for can send conflict error and stop send retry on error
  (thus avoid BUS OFF)
  @return true -> there was send error and send retry stopped
*/
bool CANIO_c::stopSendRetryOnErr()
{
  bool b_result = false;

  bool b_bit1err = HAL::can_stateGlobalBit1err(ui8_busNumber);
  bool b_sendproblem = HAL::can_stateMsgobjSendproblem(ui8_busNumber, minMsgObjNr());

  if (b_bit1err && b_sendproblem)
  { // only send 1 has bit1 err -> clear msg obj
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE
     << "CANIO_c::stopSendRetryOnErr() Blocked BUS Nr: " << uint16_t(ui8_busNumber)
     << ", ObjNr: " << minMsgObjNr()
     << "\r\n";
    #endif
    HAL::can_useMsgobjClear(ui8_busNumber, minMsgObjNr());
    b_result = true;
  }
  else if ( en_identType == Ident_c::BothIdent )
  {
    b_sendproblem = HAL::can_stateMsgobjSendproblem(ui8_busNumber, (minMsgObjNr()+1));
    if (b_bit1err && b_sendproblem)
    { // only send 1 has bit1 err -> clear msg obj
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE
       << "CANIO_c::stopSendRetryOnErr() Blocked BUS Nr: " << uint16_t(ui8_busNumber)
       << ", ObjNr: " << (minMsgObjNr()+1)
       << "\r\n";
      #endif
      HAL::can_useMsgobjClear(ui8_busNumber, (minMsgObjNr()+1));
      b_result = true;
    }
  }
  return b_result;
}

} // end of namespace __IsoAgLib

