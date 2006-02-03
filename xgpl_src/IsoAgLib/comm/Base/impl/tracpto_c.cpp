/***************************************************************************
                    tracpto_c.cpp - working on Base Data Msg Type 2;
                                    stores, updates  and delivers all base
                                    data informations from CANCustomer_c
                                    derived for CAN sending and receiving
                                    interaction;
                                    from ElementBase_c derived for
                                    interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
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
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include <IsoAgLib/comm/SystemMgmt/impl/istate_c.h>
#ifdef USE_DIN_9684
  #include <IsoAgLib/comm/SystemMgmt/DIN9684/impl/dinmonitor_c.h>
#endif
#ifdef USE_ISO_11783
  #include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#endif
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/iutil_funcs.h>
#include "tracpto_c.h"
#include "tracgeneral_c.h"

using namespace std;

#define TIMEOUT_PTO_DISENGAGED 300

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracPTO_c& getTracPtoInstance(uint8_t rui8_instance)
  { // if > 1 singleton instance is used, no static reference can be used
    return TracPTO_c::instance(rui8_instance);
  };
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracPTO_c& getTracPtoInstance( void )
  {
    static TracPTO_c& c_lbsTracPto = TracPTO_c::instance();
    return c_lbsTracPto;
  };
  #endif

  void TracPTO_c::singletonInit()
  {
    setAlreadyClosed();
    init( NULL, IsoAgLib::BaseDataNothing );
  }

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances, to receive
      the needed CAN msg with base msg type 1,2 and calendar
      possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_devKey optional pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rb_sendState optional setting to express TECU sending state (default: only receive in implement mode)
   */
  void TracPTO_c::init(const DevKey_c* rpc_devKey, bool rb_sendState)
  {
    // first register in Scheduler_c
    getSchedulerInstance4Comm().registerClient( this );
    c_data.setSingletonKey( getSingletonVecKey() );

    // set the member base msg value vars to NO_VAL codes
    i16_ptoFront = i16_ptoRear = NO_VAL_16S;

    // set the timestamps to 0
    i32_lastPtoFront = i32_lastPtoRear = 0;
    #ifdef USE_DIN_9684
    if ( checkAlreadyClosed() )
    {
      b_dinFilterCreated = false;
    }
    #endif
    #ifdef USE_ISO_11783
    t_frontPtoEngaged = t_rearPtoEngaged
    = t_frontPto1000 = t_rearPto1000
    = t_frontPtoEconomy = t_rearPtoEconomy = IsoAgLib::IsoNotAvailable; // mark as not available

    if ( checkAlreadyClosed() )
    {
      b_isoFilterCreated = false;
    }
    #endif

    // set configure values with call for config
    config(rpc_devKey, rb_sendState);

    // clear state of b_alreadyClosed, so that close() is called one time
    clearAlreadyClosed();
  };

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void TracPTO_c::close( void ) {
    if ( ! checkAlreadyClosed() ) {
      // avoid another call
      setAlreadyClosed();
      // unregister from timeEvent() call by Scheduler_c
      getSchedulerInstance4Comm().unregisterClient( this );
    }
  };

  /** deliver reference to data pkg as reference to CANPkgExt_c
      to implement the base virtual function correct
    */
  __IsoAgLib::CANPkgExt_c& TracPTO_c::dataBase()
  {
    return c_data;
  }

  /** config the TracPTO_c object after init -> set pointer to devKey and
      config send/receive of different base msg types
      @param rpc_devKey pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rb_sendState optional setting to express TECU sending state
   */
  void TracPTO_c::config(const DevKey_c* rpc_devKey, bool rb_sendState)
  {
    // set configure values
    pc_devKey = rpc_devKey;
    b_sendState = rb_sendState;

    // set ui8_sendDevKey to the pointed value, if pointer is valid
    if ((rpc_devKey != NULL) && (b_sendState)) c_sendDevKey = *rpc_devKey;
    else c_sendDevKey.setUnspecified();
  };

  /** Retrieve the last update time */
  int32_t TracPTO_c::lastedTimeSinceUpdate() const
  {
    const int32_t ci32_now = System_c::getTime();
    if (i32_lastPtoFront < i32_lastPtoRear ) return (ci32_now - i32_lastPtoRear);
    else return (ci32_now - i32_lastPtoFront);
  }
  /** Retrieve the time of last update */
  int32_t TracPTO_c::lastUpdateTime() const
  {
    if (i32_lastPtoFront < i32_lastPtoRear ) return i32_lastPtoRear;
    else return i32_lastPtoFront;
  }

  /** process received base msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a msg
      possible errors:
        * LibErr_c::LbsBaseSenderConflict base msg recevied from different member than before
      @see FilterBox_c::processMsg
      @see CANIO_c::processMsg
      @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
    */
  bool TracPTO_c::processMsg(){
    #if defined(USE_ISO_11783) && defined(USE_DIN_9684)
    if (c_data.identType() == Ident_c::ExtendedIdent)
    #endif // USE_DIN_9684 && USE_ISO_11783
    #ifdef USE_ISO_11783
    { // an ISO11783 base information msg received
      return isoProcessMsg();
    }
    #endif // USE_ISO_11783
    #if defined(USE_ISO_11783) && defined(USE_DIN_9684)
    else
    #endif
    #ifdef USE_DIN_9684
    return dinProcessMsg();
    #endif
    return false;
  };

  /** functions with actions, which must be performed periodically
      -> called periodically by Scheduler_c
      ==> sends base msg if configured in the needed rates
      possible errors:
        * dependant error in CANIO_c on CAN send problems
      @see CANPkg_c::getData
      @see CANPkgExt_c::getData
      @see CANIO_c::operator<<
      @return true -> all planned activities performed in allowed time
    */
  bool TracPTO_c::timeEvent( void ) {

    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    checkCreateReceiveFilter();
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

    // check for different base data types whether the previously
    // sending node stopped sending -> other nodes can now step in
    if ( ( !b_sendState               )
      && ( c_sendDevKey.isSpecified() ) )
    { // previously a node sent PTO data
      if ( ( ( ci32_now - i32_lastPtoFront ) >= 3000 ) && ( ( ci32_now - i32_lastPtoRear ) >= 3000 ) )
      { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
        c_sendDevKey.setUnspecified();
      }
      #ifdef USE_ISO_11783
      else
      {
        if ( ( ci32_now - i32_lastPtoFront ) >= TIMEOUT_PTO_DISENGAGED )
        { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
          // --> switch values to ZERO
          i16_ptoFront = 0;
          t_frontPtoEngaged = IsoAgLib::IsoInactive;
        }
        if ( ( ci32_now - i32_lastPtoRear ) >= TIMEOUT_PTO_DISENGAGED )
        { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
          // --> switch values to ZERO
          i16_ptoRear = 0;
          t_rearPtoEngaged = IsoAgLib::IsoInactive;
        }
      }
      #endif
    }
    if ( ( pc_devKey != NULL ) && (b_sendState))
    { // there is at least something configured to send
      #ifdef USE_ISO_11783
      if (getIsoMonitorInstance4Comm().existIsoMemberDevKey(*pc_devKey, true))
      { // stored base information sending ISO member has claimed address
        isoTimeEvent();
      }
      #endif
      #if defined(USE_ISO_11783) && defined(USE_DIN_9684)
      if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
      #endif
      #ifdef USE_DIN_9684
      if (getDinMonitorInstance4Comm().existDinMemberDevKey(*pc_devKey, true))
      { // stored base information sending DIN member has claimed address
        dinTimeEvent();
      }
      #endif
    }
    return true;
  }

  /** check if filter boxes shall be created - create only ISO or DIN filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracPTO_c::checkCreateReceiveFilter( void )
  {
    SystemMgmt_c& c_systemMgmt = getSystemMgmtInstance4Comm();
    CANIO_c &c_can = getCanInstance4Comm();
    #ifdef USE_DIN_9684
    if ( ( !b_dinFilterCreated ) && (c_systemMgmt.existActiveLocalDinMember() ) )
    { // check if needed receive filters for DIN are active
      b_dinFilterCreated = true;
      // filter for base data 2
      c_can.insertFilter(*this, (0x7F << 4),(0x15 << 4), true);
    }
    #endif

    #ifdef USE_ISO_11783
    if ( ( ! b_isoFilterCreated ) && ( c_systemMgmt.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      b_isoFilterCreated = true;
      // create FilterBox_c for PGN FRONT_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(FRONT_PTO_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN BACK_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(BACK_PTO_STATE_PGN) << 8), true, Ident_c::ExtendedIdent);
    }
    #endif
  }

  #ifdef USE_DIN_9684
  /** send a DIN9684 base information PGN
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool TracPTO_c::dinTimeEvent( void )
  {
    CANIO_c& c_can = getCanInstance4Comm();
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_send = getDinMonitorInstance4Comm().dinMemberDevKey(*pc_devKey, true).nr();

    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    if ( ( ( ci32_now - i32_lastPtoFront ) >= 100 )
      && ( b_sendState                            ) )
    { // send actual base2 data
      c_sendDevKey = *pc_devKey;
      data().setBabo(5);
      data().setSend(b_send);
      dinSetPtoFlags( data() );
      // set other information
      getTracGeneralInstance4Comm().dinSetHitchEngineFlags( data() );

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    return true;
  }

  /** process a DIN9684 base information PGN */
  bool TracPTO_c::dinProcessMsg()
  { // a DIN9684 base information msg received
    // store the devKey of the sender of base data
    if (! getDinMonitorInstance4Comm().existDinMemberNr(data().send()))
    { // the sender is not known -> ignore and block interpretation by other classes
      return true;
    }
    // the corresponding sender entry exist in the monitor list
    DevKey_c c_tempDevKey = getDinMonitorInstance4Comm().dinMemberNr(data().send()).devKey();

    // interprete data accordingto BABO
    if (data().babo() == 5) {
      // base data 2: pto, hitch
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey ) )
      { // sender is allowed to send
        dinParsePtoFlags( data() );
        // parse other information
        getTracGeneralInstance4Comm().dinParseHitchEngineFlags( data() );
      }
      return true;
    }
    else
    {
      return false;
    }
  }
  /** helper function to do the parsing of the flag data of a
   * received DIN9684 base message with Pto,Hitch,Engine information */
  void TracPTO_c::dinParsePtoFlags(const BasePkg_c& rrefc_pkg)
  { // rear pto
    setPtoRear(rrefc_pkg.val12());
    // front pto
    setPtoFront(rrefc_pkg.val34());

    // set last time
    i32_lastPtoFront = i32_lastPtoRear = Scheduler_c::getLastTimeEventTrigger();
    c_sendDevKey = getDinMonitorInstance4Comm().dinMemberNr(rrefc_pkg.send()).devKey();
  }
  /** helper function to set the Hitch and Engine flags of a DIN base data message */
  void TracPTO_c::dinSetPtoFlags(BasePkg_c& rrefc_pkg)
  {
    rrefc_pkg.setVal12(i16_ptoRear);
    rrefc_pkg.setVal34(i16_ptoFront);

    // update time
    i32_lastPtoFront = i32_lastPtoRear = Scheduler_c::getLastTimeEventTrigger();
  }

  #endif
  /** check if a received message should be parsed */
  bool TracPTO_c::checkParseReceived(const DevKey_c& rrefc_currentSender ) const
  {
    return ( ( !b_sendState   ) // I'm not the sender
          && ( // one of the following conditions must be true
               (c_sendDevKey == rrefc_currentSender) // actual sender equivalent to last
            || (c_sendDevKey.isUnspecified() ) // last sender has not correctly claimed address member
            || ((i16_ptoFront==-32768)&&(i16_ptoRear==-32768)) // current information is not valid
            )
          )?true:false;
  };

  #ifdef USE_ISO_11783
  /**
    process a ISO11783 base information PGN
  */
  bool TracPTO_c::isoProcessMsg()
  {
    bool b_result = false;
    DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // store the devKey of the sender of base data
    if (getIsoMonitorInstance4Comm().existIsoMemberNr(data().isoSa()))
    { // the corresponding sender entry exist in the monitor list
      c_tempDevKey = getIsoMonitorInstance4Comm().isoMemberNr(data().isoSa()).devKey();
    }

    if (((data().isoPgn() & 0x1FFFF) == FRONT_PTO_STATE_PGN) || ((data().isoPgn() & 0x1FFFF) == BACK_PTO_STATE_PGN))
    {
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey ) )
      { // sender is allowed to send
        if (data().isoPgn() == FRONT_PTO_STATE_PGN)
        { // front PTO
          i32_lastPtoFront = ci32_now;
          setPtoFront(data().val12() / 8); // ISO defines a resolution of 0.125 per bit!!!
          t_frontPtoEngaged = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 6) & 3);
          t_frontPto1000 = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 4) & 3);
          t_frontPtoEconomy = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 2) & 3);
        }
        else
        { // back PTO
          i32_lastPtoRear = ci32_now;
          setPtoRear(data().val12() / 8); // ISO defines a resolution of 0.125 per bit!!!
          t_rearPtoEngaged = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 6) & 3);
          t_rearPto1000 = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 4) & 3);
          t_rearPtoEconomy = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 2) & 3);
        }
        // set last time
        c_sendDevKey = c_tempDevKey;
      }
      else
      { // there is a sender conflict
        getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
      }
      b_result = true;
    }
    return b_result;
  }

  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool TracPTO_c::isoTimeEvent( void )
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberDevKey(*pc_devKey, true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setIsoSa(b_sa);

    if (Scheduler_c::getAvailableExecTime() == 0) return false;
    if ( ((ci32_now - i32_lastPtoFront ) >= 100) && (b_sendState) )
    { // it's time to send Base2
      c_sendDevKey = *pc_devKey;
      CANIO_c& c_can = getCanInstance4Comm();

      data().setIsoPgn(FRONT_PTO_STATE_PGN);
      data().setVal12(ptoFront()*8); // ISO defines a resolution of 0.125 per bit!!!
      data().setVal34(NO_VAL_16);

      uint8_t ui8_val5 = (t_frontPtoEngaged << 6);
      ui8_val5 |= (t_frontPto1000 << 4);
      ui8_val5 |= (t_frontPtoEconomy << 2);
      data().setVal5(ui8_val5);
      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();

      data().setIsoPgn(BACK_PTO_STATE_PGN);
      data().setVal12(ptoRear() * 8); // ISO defines a resolution of 0.125 per bit!!!
      data().setVal34(NO_VAL_16);

      ui8_val5 = (t_frontPtoEngaged << 6);
      ui8_val5 |= (t_frontPto1000 << 4);
      ui8_val5 |= (t_frontPtoEconomy << 2);
      data().setVal5(ui8_val5);
      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();

      // update time
      i32_lastPtoFront = Scheduler_c::getLastTimeEventTrigger();
    }
    if (Scheduler_c::getAvailableExecTime() == 0) return false;

    return true;
  }
  #endif

} // End Namespace __IsoAgLib
