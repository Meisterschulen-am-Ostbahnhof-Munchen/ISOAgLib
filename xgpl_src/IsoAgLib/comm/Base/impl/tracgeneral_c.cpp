/***************************************************************************
              tracgeneral_c.cpp - working on Base Data Msg Type 1 and 2;
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
#include "tracgeneral_c.h"

using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracGeneral_c& getTracGeneralInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracGeneral_c::instance( rui8_instance );
  };
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracGeneral_c& getTracGeneralInstance( void )
  {
    static TracGeneral_c& c_lbsTracGeneral = TracGeneral_c::instance();
    return c_lbsTracGeneral;
  };
  #endif

  void TracGeneral_c::singletonInit()
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
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
  void TracGeneral_c::init(const DevKey_c* rpc_devKey, IsoAgLib::BaseDataGroup_t rt_mySendSelection)
  {
    // first register in Scheduler_c
    getSchedulerInstance4Comm().registerClient( this );
    c_data.setSingletonKey( getSingletonVecKey() );

    // set the member base msg value vars to NO_VAL codes

    setHitchRear(NO_VAL_8);
    setHitchFront(NO_VAL_8);
    i16_engine = NO_VAL_16S;

    // set the timestamps to 0
    i32_lastBase1 = i32_lastBase2 = i32_lastFuel = 0;
    #ifdef USE_DIN_9684
    i16_rearLeftDraft = i16_rearRightDraft = i16_rearDraftNewton = NO_VAL_16S;
    ui8_rearDraftNominal = NO_VAL_8;
    i16_fuelRate = NO_VAL_16S;
    ui8_fuelTemperature = NO_VAL_8;
    if ( checkAlreadyClosed() )
    {
      b_dinFilterCreated = false;
    }
    #endif
    #ifdef USE_ISO_11783
    i32_lastIsoPositionSimple = 0;

    t_keySwitch = IsoAgLib::IsoNotAvailable; // mark as not available
    ui8_maxPowerTime = ui8_frontLinkForce = ui8_rearLinkForce = NO_VAL_8;
    i16_frontDraft = i16_rearDraft = NO_VAL_16S;
    ui32_lastMaintainPowerRequest = 0;
    b_maintainEcuPower = b_maintainActuatorPower = b_maintainPowerForImplInTransport
      = b_maintainPowerForImplInPark = b_maintainPowerForImplInWork = false;

    if ( checkAlreadyClosed() )
    {
      b_isoFilterCreated = false;

      // *************************************************************************************************
      // Added by Martin Wodok to accomodate LANGUAGE_PGN Messages:
      b_languageVtReceived = false;
      b_languageTecuReceived = false;
      // *************************************************************************************************
    }
    #endif

    // set configure values with call for config
    config(rpc_devKey, rt_mySendSelection);

    // clear state of b_alreadyClosed, so that close() is called one time
    clearAlreadyClosed();
  };

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void TracGeneral_c::close( void )
  {
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
  __IsoAgLib::CANPkgExt_c& TracGeneral_c::dataBase()
  {
    return c_data;
  }

  /** config the TracGeneral_c object after init -> set pointer to devKey and
      config send/receive of different base msg types
      @param rpc_devKey pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
  void TracGeneral_c::config(const DevKey_c* rpc_devKey, IsoAgLib::BaseDataGroup_t rt_mySendSelection)
  {
    // set configure values
    pc_devKey = rpc_devKey;
    t_mySendSelection = rt_mySendSelection;

    // set ui8_sendDevKey to the pointed value, if pointer is valid
    if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup1   ) != 0 ) ) c_sendBase1DevKey = *rpc_devKey;
    else c_sendBase1DevKey.setUnspecified();
    if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2   ) != 0 ) ) c_sendBase2DevKey = *rpc_devKey;
    else c_sendBase2DevKey.setUnspecified();
    if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataFuel     ) != 0 ) ) c_sendFuelDevKey = *rpc_devKey;
    else c_sendFuelDevKey.setUnspecified();
  };

  /** Retrieve the last update time of the specified information type
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
  int32_t TracGeneral_c::lastUpdate( IsoAgLib::BaseDataGroup_t rt_mySendSelection ) const
  {
    const int32_t ci32_now = System_c::getTime();
    switch ( rt_mySendSelection )
    {
      case IsoAgLib::BaseDataGroup2:   return ( ci32_now - i32_lastBase2);
      case IsoAgLib::BaseDataFuel:     return ( ci32_now - i32_lastFuel);
      default: return 0x7FFFFFFF;
    }
  }

  /** process received base msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a msg
      possible errors:
        * LibErr_c::LbsBaseSenderConflict base msg recevied from different member than before
      @see FilterBox_c::processMsg
      @see CANIO_c::processMsg
      @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
    */
  bool TracGeneral_c::processMsg()
  {
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
  bool TracGeneral_c::timeEvent( void )
  {
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    checkCreateReceiveFilter();
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

    // check for different base data types whether the previously
    // sending node stopped sending -> other nodes can now step in
    if ( ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2 ) == 0 )
      && ( ( ci32_now - i32_lastBase2 ) >= 3000                  )
      && ( c_sendBase2DevKey.isSpecified()                       ) )
    { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
      c_sendBase2DevKey.setUnspecified();
    }
    if ( ( ( t_mySendSelection & IsoAgLib::BaseDataFuel ) == 0  )
      && ( ( ci32_now - i32_lastFuel ) >= 3000                  )
      && ( c_sendFuelDevKey.isSpecified()                       ) )
    { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
      c_sendFuelDevKey.setUnspecified();
    }

    if ( ( pc_devKey != NULL ) && (t_mySendSelection != IsoAgLib::BaseDataNothing))
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
  void TracGeneral_c::checkCreateReceiveFilter( void )
  {
    SystemMgmt_c& c_systemMgmt = getSystemMgmtInstance4Comm();
    CANIO_c &c_can = getCanInstance4Comm();
    #ifdef USE_DIN_9684
    if ( ( !b_dinFilterCreated ) && (c_systemMgmt.existActiveLocalDinMember() ) )
    { // check if needed receive filters for DIN are active
      b_dinFilterCreated = true;
      // filter for base data 2
      c_can.insertFilter(*this, (0x7F << 4),(0x15 << 4), false);
      // filter for lower priority base data fuel consumption
      c_can.insertFilter(*this, (0x7C << 4),(0x1C << 4), true);
    }
    #endif
    #ifdef USE_ISO_11783

    if ( ( ! b_isoFilterCreated ) && ( c_systemMgmt.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      b_isoFilterCreated = true;
      // create FilterBox_c for PGN FRONT_HITCH_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(FRONT_HITCH_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN BACK_HITCH_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(BACK_HITCH_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);

      // *************************************************************************************************
      // Added by Martin Wodok to accomodate LANGUAGE_PGN Messages:
      // create FilterBox_c for LANGUAGE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (LANGUAGE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8), false, Ident_c::ExtendedIdent);

      // *************************************************************************************************
    }
    #endif
  }

  #ifdef USE_DIN_9684
  /** send a DIN9684 base information PGN
  * this is only called when sending ident is configured and it has already claimed an address
  */
  bool TracGeneral_c::dinTimeEvent(void)
  {
    CANIO_c& c_can = getCanInstance4Comm();
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_send = getDinMonitorInstance4Comm().dinMemberDevKey(*pc_devKey, true).nr();

    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    if ( ( ( ci32_now - i32_lastBase2 ) >= 100                   )
      && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2 ) != 0 ) )
    { // send actual base2 data
      c_sendBase2DevKey = *pc_devKey;
      data().setBabo(5);
      data().setSend(b_send);
      data().setVal56(i16_engine);
      data().setVal7(hitchRear());
      data().setVal8(hitchFront());

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();

      // update time
      i32_lastBase2 = ci32_now;
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    if ( ( ( ci32_now - i32_lastBase3 ) >= 100                   )
      && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup3 ) != 0 ) )
    { // send actual base3 data
      c_sendBase3DevKey = *pc_devKey;
      data().setBabo(6);
      data().setSend(b_send);
      data().setVal12(i16_rearLeftDraft);
      data().setVal34(i16_rearRightDraft);
      data().setVal56(i16_rearDraftNewton);
      data().setVal7(ui8_rearDraftNominal);

      // CAN_IO::operator<< retreives the information with the help of CAN_Pkg::get_data
      // then it sends the data
      c_can << data();

      // update time
      i32_lastBase3 = ci32_now;
    }

    if ( ( ( ci32_now - i32_lastFuel ) >= 100                  )
      && ( ( t_mySendSelection & IsoAgLib::BaseDataFuel ) != 0 ) )
    { // send actual base3 data
      c_sendFuelDevKey = *pc_devKey;
      data().setBabo(0xC);
      data().setSend(b_send);
      data().setVal12(i16_fuelRate);
      data().setVal3(ui8_fuelTemperature);

      // CAN_IO::operator<< retreives the information with the help of CAN_Pkg::get_data
      // then it sends the data
      c_can << data();

      // update time
      i32_lastFuel = ci32_now;
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    return true;
  }

  /** process a DIN9684 base information PGN */
  bool TracGeneral_c::dinProcessMsg()
  { // a DIN9684 base information msg received
    DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
    bool b_result = false;
    // store the devKey of the sender of base data
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    if (getDinMonitorInstance4Comm().existDinMemberNr(data().send()))
    { // the corresponding sender entry exist in the monitor list
      c_tempDevKey = getDinMonitorInstance4Comm().dinMemberNr(data().send()).devKey();
    }

    // interprete data accordingto BABO
    switch (data().babo())
    {
      case 5: // base data 2: pto, hitch
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( c_tempDevKey, c_sendBase2DevKey, IsoAgLib::BaseDataGroup2 ) )
        { // sender is allowed to send
          // engine speed
          setEngine(data().val56());
          // rear hitch
          setHitchRear(data().val7());
          // front hitch
          setHitchFront(data().val8());

          // set last time
          i32_lastBase2 = ci32_now;
          c_sendBase2DevKey = c_tempDevKey;
        }
        else
        { // there is a sender conflict
          getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
        }
        b_result = true;
        break;
      case 6: // NEW!! base data 3: rear draft
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( c_tempDevKey, c_sendBase3DevKey, IsoAgLib::BaseDataGroup3 ) )
        { // sender is allowed to send
          // reaer left draft
          i16_rearLeftDraft = data().val12();
          // reaer right draft
          i16_rearRightDraft = data().val34();
          // reaer total draft Newton
          i16_rearDraftNewton = data().val56();
          // reaer total draft Nominal
          ui8_rearDraftNominal = data().val7();

          // set last time
          i32_lastBase3 = ci32_now;
          c_sendBase3DevKey = c_tempDevKey;
        }
        else
        { // there is a sender conflict
          getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
        }
        b_result = true;
        break;
      case 0xC: // NEW!! fuel consumption
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( c_tempDevKey, c_sendFuelDevKey, IsoAgLib::BaseDataFuel ) )
        { // sender is allowed to send
          // fuel rate
          i16_fuelRate =  data().val12();
          // fuel temperature
          ui8_fuelTemperature =  data().val3();

          // set last time
          i32_lastFuel = ci32_now;
          c_sendFuelDevKey = c_tempDevKey;
        }
        else
        { // there is a sender conflict
          getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
        }
        b_result = true;
        break;
    } // end switch
    return b_result;
  }

  #endif

  #ifdef USE_ISO_11783
  /**
    process a ISO11783 base information PGN
  */
  bool TracGeneral_c::isoProcessMsg()
  {
    bool b_result = false;
    DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // store the devKey of the sender of base data
    if (getIsoMonitorInstance4Comm().existIsoMemberNr(data().isoSa()))
    { // the corresponding sender entry exist in the monitor list
      c_tempDevKey = getIsoMonitorInstance4Comm().isoMemberNr(data().isoSa()).devKey();
    }

    switch (data().isoPgn() & 0x1FFFF)
    {
      case FRONT_HITCH_STATE_PGN:
      case BACK_HITCH_STATE_PGN:
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( c_tempDevKey, c_sendBase2DevKey, IsoAgLib::BaseDataGroup2 ) )
        { // sender is allowed to send
          uint8_t ui8_tempHitch = ((data().val1() * 4) / 10);
          if ( (ui8_tempHitch != ERROR_VAL_8)
              && (ui8_tempHitch != NO_VAL_8) ) {
            switch ( (data().val2() >> 6) & 3 ) {
            case 0: // not in work
              break;
            case 1: // in work
              ui8_tempHitch |= 0x80;
              break;
            case 2: // Error
              ui8_tempHitch = ERROR_VAL_8S;
              break;
            case 3: // Not available
              ui8_tempHitch = NO_VAL_8;
              break;
            }
          }
          if (data().isoPgn() == FRONT_HITCH_STATE_PGN)
          { // front hitch
            setHitchFront(ui8_tempHitch);
            ui8_frontLinkForce = data().val3();
            i16_frontDraft = static_cast<int16_t>(data().val4()) + (static_cast<int16_t>(data().val5()) << 8);
          }
          else
          { // back hitch
            setHitchRear(ui8_tempHitch);
            ui8_rearLinkForce = data().val3();
            i16_rearDraft = static_cast<int16_t>(data().val4()) + (static_cast<int16_t>(data().val5()) << 8);
          }
          // set last time
          i32_lastBase2 = ci32_now;
          c_sendBase2DevKey = c_tempDevKey;
        }
        else
        { // there is a sender conflict
          getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
        }
        b_result = true;
        break;
      case MAINTAIN_POWER_REQUEST_PGN: // maintain power request
        if ( ( (data().val1() >> 2) & 3) == 1)
          b_maintainEcuPower = true;
        if ( ( (data().val1() >> 4) & 3) == 1)
          b_maintainActuatorPower = true;


        if ( ( (data().val2() >> 6) & 3) == 1)
          b_maintainPowerForImplInTransport = true;
        if ( ( (data().val2() >> 4) & 3) == 1)
          b_maintainPowerForImplInPark = true;
        if ( ( (data().val2() >> 2) & 3) == 1)
          b_maintainPowerForImplInWork = true;

        ui32_lastMaintainPowerRequest = data().time();
        b_result = true;
        break;
      // **********************************************************
      // Added by Martin Wodok for LANGUAGE_PGN:
      case LANGUAGE_PGN:
        switch (data().isoSa())
        {
          case 0x26:
            b_languageVtReceived = true;
            for (int i=0; i<8; i++)
            {
              p8ui8_languageVt[i] = data().getInt32Data(i);
            }
            break;
          case 0xF0:
            b_languageTecuReceived = true;
            for (int i=0; i<8; i++)
            {
              p8ui8_languageTecu[i] = data().getInt32Data(i);
            }
            break;
          default: // don't care for other language pgns...
            break;
        }
        b_result = false; // so that IsoTerminal_c can also receive the LANGUAGE_PGN
        break;
      // **********************************************************
    }
    return b_result;
  }

  /** send a ISO11783 base information PGN.
  * this is only called when sending ident is configured and it has already claimed an address
  */
  bool TracGeneral_c::isoTimeEvent( void )
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberDevKey(*pc_devKey, true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setIsoSa(b_sa);

    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
    if ( ( ( ci32_now - i32_lastBase2 ) >= 100                   )
      && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2 ) != 0 ) )
    { // it's time to send Base2
      isoSendBase2Update();
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    return true;
  }

  /** send Base2 Group data with hitch state */
  void TracGeneral_c::isoSendBase2Update(void)
  { // send actual base2 data
    c_sendBase2DevKey = *pc_devKey;
    CANIO_c& c_can = getCanInstance4Comm();
    data().setIsoPgn(FRONT_HITCH_STATE_PGN);
    switch (hitchFront()) {
      case ERROR_VAL_16S:
        data().setVal1(hitchFront());
        data().setVal2(IsoAgLib::IsoError);
        break;
      case NO_VAL_16S:
        data().setVal1(hitchFront());
        data().setVal2(IsoAgLib::IsoNotAvailable);
        break;
      default:
        data().setVal1(((hitchFront() & 0x7F) * 10) / 4);
        if ((hitchFront() & 0x80) != 0) data().setVal2(1 << 6); // work
        else data().setVal2(1);
        break;
    }
    data().setVal3(ui8_frontLinkForce);
    data().setVal4(i16_frontDraft& 0xFF);
    data().setVal5(i16_frontDraft >> 8);
    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    data().setIsoPgn(BACK_HITCH_STATE_PGN);
    switch (hitchRear()) {
      case ERROR_VAL_16S:
        data().setVal1(hitchRear());
        data().setVal2(IsoAgLib::IsoError);
        break;
      case NO_VAL_16S:
        data().setVal1(hitchRear());
        data().setVal2(IsoAgLib::IsoNotAvailable);
        break;
      default:
        data().setVal1(((hitchRear() & 0x7F) * 10) / 4);
        if ((hitchRear() & 0x80) != 0) data().setVal2(1 << 6); // work
        else data().setVal2(1);
        break;
    }
    data().setVal3(ui8_rearLinkForce);
    data().setVal4(i16_rearDraft& 0xFF);
    data().setVal5(i16_rearDraft >> 8);

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    i32_lastBase2 = Scheduler_c::getLastTimeEventTrigger();
  }

  /** force maintain power from tractor
    * @param rb_ecuPower true -> maintain ECU power
    * @param rb_actuatorPower -> maintain actuator power
    * @param rt_implTransport true -> implement is in transport state
    * @param rt_implPark true -> implement is in park state
    * @param rt_implWork true -> implement is in work state
    */
  void TracGeneral_c::forceMaintainPower( bool rb_ecuPower, bool rb_actuatorPower, IsoAgLib::IsoActiveFlag_t rt_implTransport,
  IsoAgLib::IsoActiveFlag_t rt_implPark, IsoAgLib::IsoActiveFlag_t rt_implWork)
  {
    uint8_t val1 = IsoAgLib::IsoInactive,
            val2 = IsoAgLib::IsoInactive;
    if (rb_ecuPower)      val1 |= ( IsoAgLib::IsoActive << 6);
    if (rb_actuatorPower) val1 |= ( IsoAgLib::IsoActive << 4);

    val2 |= ( rt_implTransport << 6);
    val2 |= ( rt_implPark << 4);
    val2 |= ( rt_implWork << 2);

    data().setIsoPgn(MAINTAIN_POWER_REQUEST_PGN);
    data().setVal1(val1);
    data().setVal2(val2);
    data().setVal34(0);
    data().setVal56(0);
    data().setVal78(0);
    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();
  }
  #endif

  /** deliver the devKey of the sender of the base data
      possible errors:
        * Err_c::range rui8_typeNr doesn't match valid base msg type number
      @param rt_typeGrp base msg type no of interest: BaseDataGroup1 | BaseDataGroup2 | BaseDataCalendar
      @return DEV_KEY code of member who is sending the intereested base msg type
    */
  const DevKey_c& TracGeneral_c::senderDevKey(IsoAgLib::BaseDataGroup_t rt_typeGrp)
  {
    // simply answer first matching result if more than one type is selected
    if ( ( rt_typeGrp & IsoAgLib::BaseDataGroup2   ) != 0 ) return c_sendBase2DevKey;
    if ( ( rt_typeGrp & IsoAgLib::BaseDataFuel     ) != 0 ) return c_sendFuelDevKey;
    else return DevKey_c::DevKeyUnspecified;
  };
} // End Namespace __IsoAgLib
