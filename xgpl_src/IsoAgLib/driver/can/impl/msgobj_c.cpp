/***************************************************************************
                          msgobj_c.cpp  -  MsgObj_c encapsulate a hardware CAN Msg
                                         Object, to present a standard interaction
                                         with CAN for all platforms
                             -------------------
    begin                : Fri Jul 30 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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

#include "msgobj_c.h"
#include "canio_c.h"
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/hal/system.h>

#if defined(DEBUG) || defined(DEBUG_CAN_BUFFER_FILLING)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
namespace __IsoAgLib {

/** default constructor for MsgObj_c which only init all member values defined start state */
MsgObj_c::MsgObj_c()
  : arrPfilterBox()
{
  // set all member variables to initial values
  c_filter.setEmpty(true);
  setBusNumber(0);
  setMsgObjNr(0);
  setIsOpen(false);
}

/** copy constructor for this class, which gets data from another MsgObj_c instance
  @param arc_src source MsgObj_c instance which should be cloned by this instance
*/
MsgObj_c::MsgObj_c(const MsgObj_c& src)
{ // set all member variables to the corresponding value from the source instance
  c_filter = src.c_filter;
  setMsgObjNr(src.msgObjNr());
  // if constructor for src is later closed, the correlated CAN msg object would
  // be closed too -> the state of this instance would be undefined / wrong
  // -> set state of this object to Not-Open
  setIsOpen(false);

  // copy the registered pointers to FilterBox_c instances
  #if 0
  setCntFilterBox(src.cnt_filterBox());
  for (int16_t i = 0; i < cnt_filterBox(); i++) arrPfilterBox[i] = src.arrPfilterBox[i];
  #else
  arrPfilterBox.clear();
  arrPfilterBox = src.arrPfilterBox;
  #endif
}

/** destructor of MsgObj_c instance, which closes the hardware MsgObj_c */
MsgObj_c::~MsgObj_c()
{
  // check if this instances manges really a running BIOS Msg object
  if ((cnt_filterBox() != 0) || (c_filter.ident() != 0)||(busNumber() != 0)||(msgObjNr() != 0))
  { // one of the values is different from the default setting
    if ((busNumber() < 2) && (msgObjNr() > 0) && (msgObjNr() < 16))
    { // CAN BUS number and the Msg Obj number are allowed
      // use member function close to clear registered pointers and to close hardware
      // CAN Msg Obj if open
      close();
    }
  }
}

/** merge two msgObj instances arc_left and arc_right and tell with ab_extendedType
  if 11bit or 29 bit identifiers are used
  (uses BIOS function)

  possible errors:
  * range BUS or MsgObj numbers out of allowed limits
  * hwConfig BUS not initialized or ID can't be changed
  @param arc_right reference to MsgObj_c which should be merged into this instance
  @return true -> successful merged; false -> too many FilterBorefs for one MsgObj
*/
bool MsgObj_c::merge(MsgObj_c& right)
{
  // use only common bits for t_filter
  c_filter.ident_bitAnd(right.filter());

  // check settings of isOpen()
  // if one is open -> store it in b_result
  // if both are open -> close one
  if ((isOpen()) && (verifyBusMsgobjNr()))
  { // this MsgObj_c instance is open and has correct BUS and MsgObj numbers

    // close the right object if it´s open -> the member function closeCan
    // does the job
    right.closeCan();
    if (HAL::can_configMsgobjChgid(busNumber(), msgObjNr(), c_filter)
        == HAL_CONFIG_ERR)
    { // BUS not initialized or ID can'tbe changed
      getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
    }
  }
  else if ((right.isOpen()) && (right.verifyBusMsgobjNr()))
  { // right one is open and has correct BUS and MsgObj numbers
    setBusNumber(right.busNumber());
    setMsgObjNr(right.msgObjNr());
    setIsOpen(right.isOpen());

    if (HAL::can_configMsgobjChgid(busNumber(), msgObjNr(), c_filter)
      == HAL_CONFIG_ERR)
    { // BUS not initialized or ID can'tbe chenged
      #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "\r\nBUS not initialized or ID can't be changed" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
    }
    right.setIsOpen(false); // now left is correlated to the open obj
  }

  #if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
  #if 0
  for ( int i = 0; i < cnt_filterBox(); i++ )
  #else
  for ( int i = 0; i < arrPfilterBox.size(); i++ )
  #endif
  {
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE.setf( STL_NAMESPACE::ios_base::hex, STL_NAMESPACE::ios_base::basefield );
    #endif
    INTERNAL_DEBUG_DEVICE << "existing FilterBox in merge nr: " << i
      << "FilterBox: 0x"
      << arrPfilterBox[i]->filter().ident()
      << ", Mask: 0x" << arrPfilterBox[i]->mask().ident()
      << ", IdentType: " << arrPfilterBox[i]->identType()
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE.setf( STL_NAMESPACE::ios_base::dec, STL_NAMESPACE::ios_base::basefield );
    #endif
  }
  #endif


  // collect FilterBox_c references
  while ( !right.arrPfilterBox.empty() )
  {
    arrPfilterBox.push_back( right.arrPfilterBox.back() );
    right.arrPfilterBox.pop_back();
  }
  #if 0
  #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG)
    #if CAN_INSTANCE_CNT == 1
    static uint16_t sui16_maxFilterPerMsgObjCnt = 0;
    uint16_t &r_maxCnt = sui16_maxFilterPerMsgObjCnt;
    #elif CAN_INSTANCE_CNT == 2
    static uint16_t sui16_maxFilterPerMsgObjCnt[2] = {0, 0};
    uint16_t &r_maxCnt = sui16_maxFilterPerMsgObjCnt[busNumber()];
    #else
    static uint16_t sui16_maxFilterPerMsgObjCnt[CAN_INSTANCE_CNT];
    uint16_t &r_maxCnt = sui16_maxFilterPerMsgObjCnt[busNumber()];
    #endif

    if ( (cnt_filterBox() + right.cnt_filterBox()) > r_maxCnt )
    { // new max filter cnt per MsgObj found
      r_maxCnt = (cnt_filterBox() + right.cnt_filterBox());
      INTERNAL_DEBUG_DEVICE << "New Max Amount " << r_maxCnt
        << " of FilterBox_c instances per MsgObj_c with ident "
        << int16_t( filter().ident() )
        << INTERNAL_DEBUG_DEVICE_ENDL;
    }
  #endif
  setCntFilterBox(cnt_filterBox() + right.cnt_filterBox());
  #endif
  //exit function with success indication
  return true;
}

/** close the BIOS msg object of this instance and close hardware CAN Msg Obj if it's open
  possible errors:
      * range wrong BUS or MsgObj number stored in this instance
*/
void MsgObj_c::close()
{
  c_filter.setEmpty(true);

  // the member function closeCan checks isOpen() and closes the BIOS object
  // if needed
  closeCan();

  arrPfilterBox.clear();
}

/** insert pointer to FilterBox_c which receive
  CAN messages by this msgObj and reports success with true

  possible errors:
      * can_overflow to many references to FilterBox_c instances for this MsgObj
  @param arc_box reference to FilterBox_c which should be inserted as possible processing instance of msg received by this instance
  @return true -> this reference could be stored in this MsgObj_c (limited amount)
*/
bool MsgObj_c::insertFilterBox(FilterRef arc_box)
{
  for ( int i = 0; i < cnt_filterBox(); i++ )
  {
    #if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE.setf( STL_NAMESPACE::ios_base::hex, STL_NAMESPACE::ios_base::basefield );
    #endif
    INTERNAL_DEBUG_DEVICE << "existing FilterBox in insertFilterBox nr: " << i
      << "FilterBox: 0x"
      << arrPfilterBox[i]->filter().ident()
      << ", Mask: 0x" << arrPfilterBox[i]->mask().ident()
      << ", IdentType: " << arrPfilterBox[i]->identType()
      << ", FilterBox instance: " << &(*arrPfilterBox[i])
      << INTERNAL_DEBUG_DEVICE_ENDL;
    #ifdef SYSTEM_PC
    INTERNAL_DEBUG_DEVICE.setf( STL_NAMESPACE::ios_base::dec, STL_NAMESPACE::ios_base::basefield );
    #endif
    #endif

    // check if given FilterRef is already registered
    if ( &(*arrPfilterBox[i]) == &(*arc_box) )
    {
      #if defined( DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION )
      INTERNAL_DEBUG_DEVICE << "Reject try to insert the same FilterBox a second time"
        << &(*arrPfilterBox[i]) << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return true;
    }
  }
  const unsigned int cui_oldSize = arrPfilterBox.size();
  arrPfilterBox.push_back( arc_box );
  if ( arrPfilterBox.size() > cui_oldSize )
  { // fine
    return true;
  }
  else
  { // vector didn't grow --> out of memory
    getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
    return false;
  }
}

/** delete pointer to a Filter Box and move following pointers one position forward
  possible errors:
      * elNonexistent to be deleted FilterBox_c reference not registered for this MsgObj
  @param arc_box reference to FilterBox_c which should be deleted from reference array
  @return true -> given FilterBox_c was deleted from local reference array
*/
bool MsgObj_c::deleteFilterBox(FilterRef arc_box)
{
  bool b_result = false;
  #if 0
  int16_t i = 0;
  // search the a pointer to the given filter box
  for (; i < cnt_filterBox(); i++)
  {
    if (arrPfilterBox[i] == arc_box)
    { // the pointer at position i points to the given FilterBox
      setCntFilterBox(cnt_filterBox() - 1); // decrement the FilterBox_c counter
      break;
    }
  }

  // i is now at to be deleted  pointer
  // or i == cnt_filterBox(), when not found -> nothing changed in following for
  // move following pointers one position to left
  for (; i < cnt_filterBox(); i++)
  { // insert pointer from position (i+1) to position i
    arrPfilterBox[i] = arrPfilterBox[i+1];
    b_result = true; // if control is here one entry is deleted
  }
  if (!b_result)
  { // to be deleted reference not found
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Can );
  }
  #else
  for ( STL_NAMESPACE::vector<FilterRef>::iterator iter = arrPfilterBox.begin() ; iter != arrPfilterBox.end(); iter++ )
  {
    if ( *iter == arc_box )
    { // to be deleted iterator is found
      arrPfilterBox.erase( iter );
      b_result = true;
      break;
    }
  }
  if ( !b_result )
  { // nothing has been erased
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Can );
  }
  #endif
  return b_result;
}

/** start processing a received CAN msg
  (called by interrupt function) (uses BIOS function)

  possible errors:
      * hwConfig MsgObj wans't configured properly
      * can_warn CAN is in WARN state
      * can_off CAN is in OFF state
      * range since config of this MsgObj some action changed the
            BUS or MsgObj number to incorrect values
      * can_overflow receive puffer overflow during receive
          (not likely because processing is called by receive IRQ)
      * dependant error if insertion in registered FilterBox_c causes error
  @param aui8_busNumber BUS number to check for received msg
  @param ab_forceProcessAll true -> process all CAN messages in HAL buffer, even if timeEvent would force stop
                            ( this important to process all buffered messages before FilterBoxes are reconfigured )
  @return number of received messages
  */
uint8_t MsgObj_c::processMsg(uint8_t aui8_busNumber, bool ab_forceProcessAll)
{
  uint8_t b_count = 0;
  int32_t i32_ident;
  bool b_processed = false,
    b_toProcess = true;
  bool b_forceProcessAll = ab_forceProcessAll;

  #ifdef DEBUG_CAN_BUFFER_FILLING
  bool b_detectedOverflow = false;
    #if CAN_INSTANCE_CNT == 1
    static uint16_t sui16_maxBufferUseage = 0;
    uint16_t &r_maxCnt = sui16_maxBufferUseage;

    static uint16_t sui16_minBufferFree = 0xFFFF;
    uint16_t &r_minFree = sui16_minBufferFree;
    #elif CAN_INSTANCE_CNT == 2
    static uint16_t sui16_maxBufferUseage[2] = {0, 0};
    uint16_t &r_maxCnt = sui16_maxBufferUseage[aui8_busNumber];

    static uint16_t sui16_minBufferFree[2] = {0xFFFF,0xFFFF};
    uint16_t &r_minFree = sui16_minBufferFree[aui8_busNumber];
    #else
    static uint16_t sui16_maxBufferUseage[CAN_INSTANCE_CNT];
    uint16_t &r_maxCnt = sui16_maxBufferUseage[aui8_busNumber];

    static uint16_t sui16_minBufferFree[CAN_INSTANCE_CNT];
    uint16_t &r_minFree = sui16_minBufferFree[aui8_busNumber];
    if ( ( sui16_maxBufferUseage[0] == 0 ) && ( sui16_maxBufferUseage[1] == 0 ) )
    {
      for ( uint16_t ind = 0; ind < CAN_INSTANCE_CNT; ind++) sui16_minBufferFree[ind] = 0xFFFF;
    }
    #endif
    if ( HAL::can_stateMsgobjBuffercnt(aui8_busNumber, msgObjNr()) > r_maxCnt )
    { // new MAX detected -> update and print
      r_maxCnt = HAL::can_stateMsgobjBuffercnt(aui8_busNumber, msgObjNr());
      INTERNAL_DEBUG_DEVICE << "\r\nNew Max buffer filling: " << r_maxCnt
        << " at MsgObj: " << uint16_t(msgObjNr())
        << " with Filter: " << c_filter.ident()
        << " at BUS: " << uint16_t(aui8_busNumber)
        << INTERNAL_DEBUG_DEVICE_ENDL;
    }
    if ( HAL::can_stateMsgobjFreecnt(aui8_busNumber, msgObjNr()) < r_minFree )
    { // new MIN detected -> update and print
      r_minFree = HAL::can_stateMsgobjFreecnt(aui8_busNumber, msgObjNr());
      INTERNAL_DEBUG_DEVICE << "\r\nNew Min buffer free: " << r_minFree
        << " at MsgObj: " << uint16_t(msgObjNr())
        << " with Filter: " << c_filter.ident()
        << " at BUS: " << uint16_t(aui8_busNumber)
        << INTERNAL_DEBUG_DEVICE_ENDL;
    }
  #endif

  // process all CAN msgs of this MsgObj_c as long as some time available for processing
  // ( the time check is needed to avoid blocking of other important tasks if
  //   to much CAN msgs are received )
  while ( ( HAL::can_stateMsgobjBuffercnt(aui8_busNumber, msgObjNr()) > 0          )
       && ( ( Scheduler_c::getCentralSchedulerAvailableExecTime() != 0 ) || ( b_forceProcessAll ) || (b_count == 0) ) ) ///< force at least ONE processing
  { // increment counter
    b_count++;
    b_processed = false;
    b_toProcess = true;
    // get received data by BIOS call
    switch (HAL::can_useMsgobjReceivedIdent(aui8_busNumber, msgObjNr(), i32_ident))
    {
      case HAL_NO_ERR:
        break;
      case HAL_RANGE_ERR:
        getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "CAN-Receive Range Err" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        HAL::can_useMsgobjPopFront(aui8_busNumber, msgObjNr());
        return (b_count-1);
      case HAL_CONFIG_ERR:
        #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG)
        INTERNAL_DEBUG_DEVICE << "\r\nBUS not initialized or wrong BUS nr: " << uint16_t(aui8_busNumber) << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
        HAL::can_useMsgobjPopFront(aui8_busNumber, msgObjNr());
        return (b_count-1);
      case HAL_NOACT_ERR:
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "CAN-Receive NoAct == BUS OFF Err" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        // wrong use of MsgObj (not likely) or CAN BUS OFF
        getILibErrInstance().registerError( iLibErr_c::CanOff, iLibErr_c::Can );
        HAL::can_useMsgobjPopFront(aui8_busNumber, msgObjNr());
        return (b_count-1);
      case HAL_WARN_ERR:
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "CAN WARN Err" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        getILibErrInstance().registerError( iLibErr_c::CanWarn, iLibErr_c::Can );
        break;
      case HAL_OVERFLOW_ERR:
        // CAN BUFFER Overflow can most probably happen on process of
        // messages in last msg obj, where ALL CAN messages are placed
        // during reconfiguration
        getILibErrInstance().registerError( iLibErr_c::CanOverflow, iLibErr_c::Can );
        HAL::can_stateMsgobjOverflow(aui8_busNumber, msgObjNr() );
        #ifdef DEBUG_CAN_BUFFER_FILLING
        if ( ! b_detectedOverflow )
        {
          char temp[200];
          STL_NAMESPACE::sprintf( temp, "\r\nALARM!!!!!! CAN Buffer Overflow at MsgObj: %d at BUS: %d with Ident: 0x%lx, \r\nBufer-Filling: %d, Free: %d",
            uint16_t(msgObjNr()), uint16_t(aui8_busNumber), c_filter.ident(), HAL::can_stateMsgobjBuffercnt(aui8_busNumber, msgObjNr() ),
            HAL::can_stateMsgobjFreecnt(aui8_busNumber, msgObjNr() ) );
          INTERNAL_DEBUG_DEVICE << temp << INTERNAL_DEBUG_DEVICE_ENDL;
        }
        b_detectedOverflow = true;
        #endif
        b_forceProcessAll = true;
        break;
    }

    if (b_toProcess)
    {
      // search for the suiting FilterBox
      for (int16_t i=0; i < cnt_filterBox(); i++)
      {
        if (arrPfilterBox[i]->matchMsgId(i32_ident, c_filter.identType()))
        { // ident of received data matches the filter of the i'th registered FilterBox
          if ( arrPfilterBox[i]->processMsg() )
          { // customer indicated, that he processed the received data
            b_processed = true;
            break;
          }
        }
      }
      if ( ! b_processed )
      { // try to search all FilterBox_c instances
        /** @todo check why ISO 11783 message which matches filter 0xCB0000 needs always global search */
        CanIo_c::ArrFilterBox::iterator pc_iFilterBox;
        bool b_foundOne=false;
        #if CAN_INSTANCE_CNT > 1
        for ( uint16_t ind = 0; ind < CAN_INSTANCE_CNT; ind++)
        {
          if ( getCanInstance( ind ).getBusNumber() == aui8_busNumber )
          { // corresponding CanIo_c instance with same channel found
        #endif
            do {
              #if CAN_INSTANCE_CNT == 1
              b_foundOne =    getCanInstance().canMsg2FilterBox( i32_ident, c_filter.identType(), pc_iFilterBox, !b_foundOne );
              #else
              b_foundOne = getCanInstance(ind).canMsg2FilterBox( i32_ident, c_filter.identType(), pc_iFilterBox, !b_foundOne );
              #endif
              if ( b_foundOne )
              { // matching instance found
                HAL::wdTriggern();
                b_processed = pc_iFilterBox->processMsg();
              }
            } while (b_foundOne && !b_processed); // if one found, try if another one can be found!
        #if CAN_INSTANCE_CNT > 1
            break;
          }
        }
        #endif
      }
    }
    // now received message is either processed, or none of the registered CANCustomers is interested
    HAL::can_useMsgobjPopFront(aui8_busNumber, msgObjNr());
  }   // end while
  return b_count;
}

/** configures the CAN hardware of given Msg Object (uses BIOS function)
  possible errors:
      * hwConfig used CAN BUS wans't configured properly
      * range given BUS or MsgObj number not in allowed area
      * hwBusy wanted MsgObj already in use
      * unspecified some other error
  @param aui8_busNumber BUS number, where this instance should act
  @param aui8_msgNr CAN hardware msg number for BIOS interaction
  @return true -> BIOS CAN object without errors configured
*/
bool MsgObj_c::configCan(uint8_t aui8_busNumber, uint8_t aui8_msgNr)
{
  bool b_result = false;
  if (!verifyBusMsgobjNr(aui8_busNumber, aui8_msgNr))
  { // the given values are not within allowed limits (defined in isoaglib_config.h)
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  else
  { // values are o.k.
    // store ui8_busNumber for later close of can
    setBusNumber(aui8_busNumber);
    setMsgObjNr(aui8_msgNr);

    //store busNumber and msgNr for each filterBox
    for(uint8_t i = 0; i < arrPfilterBox.size(); i++)
    {
      arrPfilterBox[i]->configCan(aui8_busNumber, aui8_msgNr);
    }
  }
  if (c_filter.identType() == Ident_c::BothIdent) c_filter.setIdentType(DEFAULT_IDENT_TYPE);
  switch (HAL::can_configMsgobjInit(aui8_busNumber, aui8_msgNr, c_filter, 0))
  {
    case HAL_NO_ERR:
      /* no error -> clear error state */
      setIsOpen(true);
      // remove any previously active lock
      lock( false );

      b_result = true;
      break;
    case HAL_BUSY_ERR:
      /* this BIOS-Obj is already in use */
      getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::Can );
      break;
    case HAL_CONFIG_ERR:
      /* BUS not initialized, undefined msg type, CAN-BIOS memory error */
      #if defined(DEBUG_CAN_BUFFER_FILLING) || defined(DEBUG)
      INTERNAL_DEBUG_DEVICE << "\r\nALARM Not enough memory for CAN buffer" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      getILibErrInstance().registerError( iLibErr_c::HwConfig, iLibErr_c::Can );
      break;
    case HAL_RANGE_ERR:
      /* undefined BUS number, undefined BIOS-Obj number, wrong puffer size */
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
      break;
    default:
      /* unspecified error */
      getILibErrInstance().registerError( iLibErr_c::Unspecified, iLibErr_c::Can );
      break;
  }
  return b_result;
}

/** verify given BUS number and MsgObj number, if they are within allowed
  limits (defined in isoaglib_config.h)
  if called withoutparameter values (default -1) the actual configured are
  checked -> if these are incorrect range is set
  (mostly used by MsgObj_c to verify itself)
  @param ac_busNr number of the BUS (default the actual stored number is checked)
  @param ac_msgobjNr number of the MsgObj (default the actual stored number is checked)
  @return true -> values are correct
*/
bool MsgObj_c::verifyBusMsgobjNr(int8_t ac_busNr, int8_t ac_msgobjNr)
{
  //store parameter values to temp val, to check actual configured
  // values on default parameter value
  uint8_t b_testBus = (ac_busNr >= 0)?ac_busNr:busNumber();
  uint8_t b_testMsgobj = (ac_msgobjNr >= 0)?ac_msgobjNr:msgObjNr();

  //check if there is an error
  if (
     (b_testBus > HAL_CAN_MAX_BUS_NR)
  || (b_testMsgobj < HAL_CAN_MIN_REC_OBJ)
  || (b_testMsgobj > HAL_CAN_MAX_REC_OBJ)
     )
  { // set range-error status if called with default values for both parameters
    if ((ac_busNr < 0) && (ac_msgobjNr < 0)) getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    return false;
  }
  else
  { // set no-range-error status if called with default values for both parameters
    return true;
  }
}

/** get the common filter part of all merged
  FilterBox instances
  @return common filter of all FilterBoxes in this MsgObj_c instance
*/
void MsgObj_c::commonFilterAfterMerge( Ident_c& arc_globalMask )
{ // clear any bits which are no more used in the current global mask
  c_filter.ident_bitAnd( arc_globalMask );
}


/** close the correlated CAN object
  (uses BIOS function)
  possible errors:
      * range wrong BUS or MsgObj number stored in this instance
*/
void MsgObj_c::closeCan()
{
  // check isOpen() and close CAN
  if (isOpen())
  {
    if (HAL::can_configMsgobjClose(busNumber(), msgObjNr() ) == HAL_RANGE_ERR)
    { // given BUS or MsgObj number is wrong
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Can );
    }
    setIsOpen(false);
  }
}


/** lock the corresponding hardware MsgObj to avoid receiving further CAN messages.
  This important for handling of the LastMsgObj, as it should only receive messages
  during process of CanIo_c::reconfigureMsgObj() - but as the messages shall be processed
  within normal CanIo_c::processMsg(), nu furhter messages shall be placed in the receive queue
  of the BIOS/OS. A immediate process wihtin execution of CanIo_c::reconfigureMsgObj() can cause
  deadlocks when the reconfig is initiated as a result of:
  -# Singleton_c::instance() -> Singleton_c::init()
  -# -> partialClass_c::init()
  -# -> CanIo_c::reconfigureMsgObj()
  -# -> Msg_Obj_c::processMsg()
  -# -> partialClass_c::processMsg()
  -# -> trigger update/reaction by Singleton_c::update() !!! undefined result, as Singleton_c::instance()
        has not yet finished, so that this type of circular access on the same Singleton_c::instance()
        is blocked by returning the signal value 0x1

  Thus CanIo_c::reconfigureMsgObj() locks the lastMessageObject at the end, so that the buffer content is
  simply conserved until normal CanIo_c::processMsg() is called.
*/
void MsgObj_c::lock( bool ab_lock )
{
  if ( ! ab_lock )
  {
    if ( ! isLocked() ) return; ///< object is not locked
    // activate normal configured ident
    //HAL::can_configMsgobjChgid(bit_data.busNumber, bit_data.ui8_msgObjNr, c_filter);
    HAL::can_configMsgobjLock( bit_data.busNumber, bit_data.ui8_msgObjNr, false );
    // clear lock bit
    bit_data.isLocked = 0;
  }
  else
  {
    if ( isLocked() ) return; ///< is already locked
    //HAL::can_configMsgobjChgid(bit_data.busNumber, bit_data.ui8_msgObjNr, c_tempIdent);
    HAL::can_configMsgobjLock( bit_data.busNumber, bit_data.ui8_msgObjNr, true );
    // set lock bit
    bit_data.isLocked = 1;
  }
}

} // end namespace __IsoAgLib
