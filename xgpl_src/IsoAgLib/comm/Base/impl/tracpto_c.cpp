/***************************************************************************
                    tracpto_c.cpp - working on PTO data;
                                    stores, updates  and delivers all base
                                    data informations from CANCustomer_c
                                    derived for CAN sending and receiving
                                    interaction;
                                    from BaseCommon_c derived for
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
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include "tracpto_c.h"
#include "tracgeneral_c.h"

using namespace std;

#define TIMEOUT_PTO_DISENGAGED 300

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracPTO_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracPTO_c& getTracPtoInstance(uint8_t rui8_instance)
  { // if > 1 singleton instance is used, no static reference can be used
    return TracPTO_c::instance(rui8_instance);
  };
  #else
  /** C-style function, to get access to the unique TracPTO_c singleton instance */
  TracPTO_c& getTracPtoInstance( void )
  {
    static TracPTO_c& c_lbsTracPto = TracPTO_c::instance();
    return c_lbsTracPto;
  };
  #endif
  /** config the TracPTO_c object after init -> set pointer to devKey and
      config send/receive of different base msg types
      @param rpc_devKey pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
   */
  void TracPTO_c::config(const DevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_IdentMode)
  {
    //call config for handling which is base data independent
    BaseCommon_c::config(rpc_devKey, rt_IdentMode);
    // set the member base msg value vars to NO_VAL codes
    i16_ptoFront = i16_ptoRear = NO_VAL_16S;
    ui16_frontPtoSetPoint = ui16_rearPtoSetPoint = NO_VAL_16;

    // set the timestamps to 0
    i32_lastPtoFront = i32_lastPtoRear = 0;
    #ifdef USE_ISO_11783
    t_frontPtoEngaged = t_rearPtoEngaged
    = t_frontPto1000 = t_rearPto1000
    = t_frontPtoEconomy = t_rearPtoEconomy = IsoAgLib::IsoNotAvailable; // mark as not available
    ui16_frontPtoSetPoint = 0;
    ui16_rearPtoSetPoint = 0;
    t_frontPtoEngagementReqStatus = t_rearPtoEngagementReqStatus
    = t_frontPtoModeReqStatus = t_rearPtoModeReqStatus
    = t_frontPtoEconomyModeReqStatus = t_rearPtoEconomyModeReqStatus = IsoAgLib::IsoNotAvailableReq;
    t_frontPtoShaftSpeedLimitStatus = t_rearPtoShaftSpeedLimitStatus = IsoAgLib::IsoNotAvailableLimit;
    #endif
  };

#ifdef USE_ISO_11783
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
  bool TracPTO_c::timeEvent( )
  {
    BaseCommon_c::timeEvent();
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // check for different pto data types whether the previously
    // sending node stopped sending -> other nodes can now step in
    if ( ( ( ci32_now - i32_lastPtoFront ) >= TIMEOUT_PTO_DISENGAGED ) || (getSelectedDataSourceDevKey().isUnspecified() ) )
    { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
      // --> switch values to ZERO
      i16_ptoFront = 0;
      t_frontPtoEngaged = IsoAgLib::IsoInactive;
    }
    if ( ( ( ci32_now - i32_lastPtoRear ) >= TIMEOUT_PTO_DISENGAGED ) || (getSelectedDataSourceDevKey().isUnspecified() ) )
    { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
      // --> switch values to ZERO
      i16_ptoRear = 0;
      t_rearPtoEngaged = IsoAgLib::IsoInactive;
    }
    return true;
  }
#endif

  /** check if filter boxes shall be created - create only ISO or DIN filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracPTO_c::checkCreateReceiveFilter( )
  {
    SystemMgmt_c& c_systemMgmt = getSystemMgmtInstance4Comm();
    CANIO_c &c_can = getCanInstance4Comm();
    #ifdef USE_DIN_9684
    if ( ( !checkDinFilterCreated() ) && (c_systemMgmt.existActiveLocalDinMember() ) )
    { // check if needed receive filters for DIN are active
      setDinFilterCreated();
      // filter for base data 2
      c_can.insertFilter(*this, (0x7F << 4),(0x15 << 4), true);
    }
    #endif

    #ifdef USE_ISO_11783
    if ( ( !checkIsoFilterCreated() ) && ( c_systemMgmt.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setIsoFilterCreated();
      // create FilterBox_c for PGN FRONT_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(FRONT_PTO_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN REAR_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(REAR_PTO_STATE_PGN) << 8), true, Ident_c::ExtendedIdent);
    }
    #endif
  }

  #ifdef USE_DIN_9684
  /** send a DIN9684 base information PGN
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool TracPTO_c::dinTimeEventTracMode( )
  {
    CANIO_c& c_can = getCanInstance4Comm();
    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_send = getDinMonitorInstance4Comm().dinMemberDevKey(*getDevKey(), true).nr();

    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    if (  ( lastedTimeSinceUpdate()  >= 100 )
      &&  checkMode(IsoAgLib::IdentModeTractor) )
    { // send actual base2 data
      setSelectedDataSourceDevKey( *getDevKey() );
      data().setIdent((0x15<<4 | b_send), __IsoAgLib::Ident_c::StandardIdent );
      dinSetPtoFlags( data() );
      // set other information
      getTracGeneralInstance4Comm().dinSetHitchEngineFlags( data() );
      data().setLen(8);

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();

      // update time
      setUpdateTime(Scheduler_c::getLastTimeEventTrigger());
    }
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    return true;
  }

  /** process a DIN9684 base information PGN */
  bool TracPTO_c::dinProcessMsg()
  { // a DIN9684 base information msg received
    // store the devKey of the sender of base data
    if (! getDinMonitorInstance4Comm().existDinMemberNr(data().dinSa()) )
    { // the sender is not known -> ignore and block interpretation by other classes
      return true;
    }
    // the corresponding sender entry exist in the monitor list
    DevKey_c c_tempDevKey = getDinMonitorInstance4Comm().dinMemberNr(data().dinSa()).devKey();

    // interprete data accordingto BABO
    if (dataBabo() == 5) {
      // base data 2: pto, hitch
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey ) )
      { // sender is allowed to send
        dinParsePtoFlags( data() );
        // parse other information
        getTracGeneralInstance4Comm().dinParseHitchEngineFlags( data() );
        setSelectedDataSourceDevKey( c_tempDevKey );
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
  void TracPTO_c::dinParsePtoFlags(const CANPkgExt_c& rrefc_pkg)
  { // rear pto
    setPtoRear( rrefc_pkg.getUint16Data(0) * 8 );
    // front pto
    setPtoFront(rrefc_pkg.getUint16Data(2) * 8 );
  }
  /** helper function to set the Hitch and Engine flags of a DIN base data message */
  void TracPTO_c::dinSetPtoFlags(CANPkgExt_c& rrefc_pkg)
  {
    rrefc_pkg.setUint16Data(0, ptoRear() / 8 );
    rrefc_pkg.setUint16Data(2, ptoFront() / 8 );
  }
  #endif

  #ifdef USE_ISO_11783
  /**
    process a ISO11783 base information PGN
  */
  bool TracPTO_c::isoProcessMsg()
  {
    DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // store the devKey of the sender of base data
    if (getIsoMonitorInstance4Comm().existIsoMemberNr(data().isoSa()))
    { // the corresponding sender entry exist in the monitor list
      c_tempDevKey = getIsoMonitorInstance4Comm().isoMemberNr(data().isoSa()).devKey();
    }

    if (((data().isoPgn() & 0x1FFFF) == FRONT_PTO_STATE_PGN) || ((data().isoPgn() & 0x1FFFF) == REAR_PTO_STATE_PGN))
    {
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey ) )
      { // sender is allowed to send
        if (data().isoPgn() == FRONT_PTO_STATE_PGN)
        { // front PTO
          i32_lastPtoFront = ci32_now;
          i16_ptoFront = data().getUint16Data(0);
          ui16_frontPtoSetPoint = data().getUint16Data(2);
          t_frontPtoEngaged = IsoAgLib::IsoActiveFlag_t(          ( data().getUint8Data(4) >> 6) & 3 );
          t_frontPto1000 =    IsoAgLib::IsoActiveFlag_t(          ( data().getUint8Data(4) >> 4) & 3 );
          t_frontPtoEconomy = IsoAgLib::IsoActiveFlag_t(          ( data().getUint8Data(4) >> 2) & 3 );
          t_frontPtoEngagementReqStatus =   IsoAgLib::IsoReqFlag_t( data().getUint8Data(4)       & 3 );
          t_frontPtoModeReqStatus =         IsoAgLib::IsoReqFlag_t( (data().getUint8Data(5) >> 6) & 3 );
          t_frontPtoEconomyModeReqStatus =  IsoAgLib::IsoReqFlag_t( (data().getUint8Data(5) >> 4) & 3 );
          t_frontPtoShaftSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t( (data().getUint8Data(5) >> 1) & 0x7 );
        }
        else
        { // back PTO
          i32_lastPtoRear = ci32_now;
          i16_ptoRear = data().getUint16Data(0);
          ui16_rearPtoSetPoint = data().getUint16Data(2);
          t_rearPtoEngaged = IsoAgLib::IsoActiveFlag_t(         ( data().getUint8Data(4) >> 6) & 3 );
          t_rearPto1000 =    IsoAgLib::IsoActiveFlag_t(         ( data().getUint8Data(4) >> 4) & 3 );
          t_rearPtoEconomy = IsoAgLib::IsoActiveFlag_t(         ( data().getUint8Data(4) >> 2) & 3 );
          t_rearPtoEngagementReqStatus =  IsoAgLib::IsoReqFlag_t( data().getUint8Data(4)       & 3 );
          t_rearPtoModeReqStatus =        IsoAgLib::IsoReqFlag_t( (data().getUint8Data(5) >> 6) & 3 );
          t_rearPtoEconomyModeReqStatus = IsoAgLib::IsoReqFlag_t( (data().getUint8Data(5) >> 4) & 3 );
          t_rearPtoShaftSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t( (data().getUint8Data(5) >> 1) & 0x7 );
        }
        // set last time
        setSelectedDataSourceDevKey(c_tempDevKey);
      }
      else
      { // there is a sender conflict
        getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
        return false;
      }
    }
    return false;
  }

  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool TracPTO_c::isoTimeEventTracMode(  )
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberDevKey(*getDevKey(), true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setIsoSa(b_sa);

    if (Scheduler_c::getAvailableExecTime() == 0) return false;
    // this function is only called in TractorMode
    CANIO_c& c_can = getCanInstance4Comm();
    setSelectedDataSourceDevKey(*getDevKey());
    if ( ( (ci32_now - i32_lastPtoFront ) >= 100) && ( t_frontPtoEngaged == IsoAgLib::IsoActive ) )
    { // it's time to send tractor PTO information and the FRONT PTO is engaged
      data().setIsoPgn(FRONT_PTO_STATE_PGN);
      uint8_t ui8_val;
      data().setUint16Data(0, i16_ptoFront);
      data().setUint16Data(2, ui16_frontPtoSetPoint);
      ui8_val =  (t_frontPtoEngaged             << 6);
      ui8_val |= (t_frontPto1000                << 4);
      ui8_val |= (t_frontPtoEconomy             << 2);
      ui8_val |= (t_frontPtoEngagementReqStatus << 0);
      data().setUint8Data(4, ui8_val);
      ui8_val = 0;
      ui8_val |= (t_frontPtoModeReqStatus         << 6);
      ui8_val |= (t_frontPtoEconomyModeReqStatus  << 4);
      ui8_val |= (t_frontPtoShaftSpeedLimitStatus << 1);
      data().setUint8Data(5, ui8_val);
      //reserved fields
      data().setUint16Data(6, 0);
      data().setLen(8);
      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();

      // update time
      i32_lastPtoFront = Scheduler_c::getLastTimeEventTrigger();
    }
    if ( ( (ci32_now - i32_lastPtoRear ) >= 100) && ( t_rearPtoEngaged == IsoAgLib::IsoActive ) )
    { // it's time to send tractor PTO information and the REAR PTO is engaged
      data().setIsoPgn(REAR_PTO_STATE_PGN);
      uint8_t ui8_val;
      data().setUint16Data(0, i16_ptoRear ); // ISO defines a resolution of 0.125 per bit!!!
      data().setUint16Data(2, ui16_rearPtoSetPoint);

      ui8_val =  (t_rearPtoEngaged             << 6);
      ui8_val |= (t_rearPto1000                << 4);
      ui8_val |= (t_rearPtoEconomy             << 2);
      ui8_val |= (t_rearPtoEngagementReqStatus << 0);
      data().setUint8Data(4, ui8_val);
      ui8_val = 0;
      ui8_val |= (t_rearPtoModeReqStatus         << 6);
      ui8_val |= (t_rearPtoEconomyModeReqStatus  << 4);
      ui8_val |= (t_rearPtoShaftSpeedLimitStatus << 1);
      data().setUint8Data(5, ui8_val);
      //reserved fields
      data().setUint16Data(6, 0);
      data().setLen(8);
      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();

      // update time
      i32_lastPtoRear = Scheduler_c::getLastTimeEventTrigger();
    }
    if (Scheduler_c::getAvailableExecTime() == 0) return false;

    return true;
  }
  #endif

} // End Namespace __IsoAgLib
