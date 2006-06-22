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
#ifdef USE_ISO_11783
 #include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgn_c.h>
#endif

using namespace std;

#define TIMEOUT_PTO_DISENGAGED 3000

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
      @return true -> configuration was successfull
   */
  bool TracPTO_c::config(const DevKey_c* rpc_devKey, IsoAgLib::IdentMode_t rt_identMode)
  {
    #ifdef USE_ISO_11783
    //store old mode to decide to register or unregister from request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();
    #endif
    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config(rpc_devKey, rt_identMode) ) return false;

    // set the member base msg value vars to NO_VAL codes
    t_ptoFront.ui16_pto8DigitPerRpm = t_ptoRear.ui16_pto8DigitPerRpm = NO_VAL_16S;
    // set the timestamps to 0
    t_ptoFront.i32_lastPto = t_ptoRear.i32_lastPto = 0;
    #ifdef USE_ISO_11783
    if (   (t_oldMode == IsoAgLib::IdentModeImplement && rt_identMode == IsoAgLib::IdentModeTractor)
        || (t_oldMode == IsoAgLib::IdentModeTractor && rt_identMode == IsoAgLib::IdentModeImplement)
       )
    { // a change from Implement to Tractor mode or a change from Tractor to Implement mode occured
      const uint8_t pgnCount = 2;
      const uint32_t pgnArray[pgnCount] = {FRONT_PTO_STATE_PGN, REAR_PTO_STATE_PGN};

      if (t_oldMode == IsoAgLib::IdentModeImplement)
        // register to request for pgn
        // create FilterBox_c for REQUEST_PGN_MSG_PGN, FRONT_PTO_STATE_PGN
        getIsoRequestPgnInstance4Comm().registerPGN (*this, pgnCount, pgnArray); // request for front pto state
      else
        // unregister from request for pgn, because in implement mode no requests should be answered
        getIsoRequestPgnInstance4Comm().unregisterPGN (*this, pgnCount, pgnArray);
    }

    t_ptoFront.t_ptoEngaged = t_ptoRear.t_ptoEngaged
    = t_ptoFront.t_pto1000 = t_ptoRear.t_pto1000
    = t_ptoFront.t_ptoEconomy = t_ptoRear.t_ptoEconomy = IsoAgLib::IsoNotAvailable; // mark as not available
    t_ptoFront.ui16_ptoSetPoint8DigitPerRpm = 0;
    t_ptoRear.ui16_ptoSetPoint8DigitPerRpm = 0;
    t_ptoFront.t_ptoEngagementReqStatus = t_ptoRear.t_ptoEngagementReqStatus
    = t_ptoFront.t_ptoModeReqStatus = t_ptoRear.t_ptoModeReqStatus
    = t_ptoFront.t_ptoEconomyModeReqStatus = t_ptoRear.t_ptoEconomyModeReqStatus = IsoAgLib::IsoNotAvailableReq;
    t_ptoFront.t_ptoShaftSpeedLimitStatus = t_ptoRear.t_ptoShaftSpeedLimitStatus = IsoAgLib::IsoNotAvailableLimit;
    #endif
    return true;
  };

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
    const DevKey_c& c_tempDevKey = getDinMonitorInstance4Comm().dinMemberNr(data().dinSa()).devKey();

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
        setUpdateTime( Scheduler_c::getLastTimeEventTrigger() );
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
    setPtoRear( rrefc_pkg.getUint16Data(0) );
    // front pto
    setPtoFront(rrefc_pkg.getUint16Data(2) );

    // set last time
    setUpdateTime(Scheduler_c::getLastTimeEventTrigger());

  }
  /** helper function to set the Hitch and Engine flags of a DIN base data message */
  void TracPTO_c::dinSetPtoFlags(CANPkgExt_c& rrefc_pkg)
  {
    rrefc_pkg.setUint16Data(0, ptoRear() );
    rrefc_pkg.setUint16Data(2, ptoFront() );

    // set last time
    setUpdateTime(Scheduler_c::getLastTimeEventTrigger());
  }
  #endif

  #ifdef USE_ISO_11783
  bool TracPTO_c::processMsgRequestPGN (uint32_t rui32_pgn, uint8_t /*rui8_sa**/, uint8_t rui8_da)
  {
    if ( NULL == getDevKey() ) return false;
    if ( ! getIsoMonitorInstance4Comm().existIsoMemberDevKey( *getDevKey(), true ) ) return false;

    // now we can be sure, that we are in tractor mode, and the registered tractor device key
    // belongs to an already claimed IsoItem_c --> we are allowed to send
    if ( ( getIsoMonitorInstance4Comm().isoMemberDevKey( *getDevKey() ).nr() == rui8_da ) || ( rui8_da == 0xFF ) )
    { // the REQUEST was directed to the SA that belongs to the tractor IdentItem_c that is matched by the registrated
      // DevKey_c (getDevKey())
      // call TracPto_c function to send pto informtation
      // isoSendMessage checks if this item (identified by DEV_KEY)
      // is configured to send pto information
      if ( rui32_pgn == FRONT_PTO_STATE_PGN  && t_ptoFront.t_ptoEngaged != IsoAgLib::IsoActive)
      {
        isoSendMessage(sendFrontPto);
      }
      if ( rui32_pgn == REAR_PTO_STATE_PGN && t_ptoRear.t_ptoEngaged != IsoAgLib::IsoActive)
      {
        isoSendMessage(sendRearPto);
      }
      return true;
    }
    return false;
  }

  /**
    process a ISO11783 base information PGN
  */
  bool TracPTO_c::isoProcessMsg()
  {
    DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
    // store the devKey of the sender of base data
    if (getIsoMonitorInstance4Comm().existIsoMemberNr(data().isoSa()))
    { // the corresponding sender entry exist in the monitor list
      c_tempDevKey = getIsoMonitorInstance4Comm().isoMemberNr(data().isoSa()).devKey();
    }

    if (((data().isoPgn() & 0x1FFFF) == FRONT_PTO_STATE_PGN) || ((data().isoPgn() & 0x1FFFF) == REAR_PTO_STATE_PGN))
    {
      const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey ) )
      { // sender is allowed to send
        PtoData_t* pt_ptoData = NULL;
        if (data().isoPgn() == FRONT_PTO_STATE_PGN)
        { // front PTO
          pt_ptoData = &t_ptoFront;
        }
        else
        { // rear PTO
          pt_ptoData = &t_ptoRear;
        }

        pt_ptoData->i32_lastPto = ci32_now;
        pt_ptoData->ui16_pto8DigitPerRpm         = data().getUint16Data(0);
        pt_ptoData->ui16_ptoSetPoint8DigitPerRpm = data().getUint16Data(2);
        pt_ptoData->t_ptoEngaged = IsoAgLib::IsoActiveFlag_t(          (    data().getUint8Data(4) >> 6) & 3 );
        pt_ptoData->t_pto1000    = IsoAgLib::IsoActiveFlag_t(          (    data().getUint8Data(4) >> 4) & 3 );
        pt_ptoData->t_ptoEconomy = IsoAgLib::IsoActiveFlag_t(          (    data().getUint8Data(4) >> 2) & 3 );
        pt_ptoData->t_ptoEngagementReqStatus   = IsoAgLib::IsoReqFlag_t(    data().getUint8Data(4)       & 3 );
        pt_ptoData->t_ptoModeReqStatus         = IsoAgLib::IsoReqFlag_t(   (data().getUint8Data(5) >> 6) & 3 );
        pt_ptoData->t_ptoEconomyModeReqStatus  = IsoAgLib::IsoReqFlag_t(   (data().getUint8Data(5) >> 4) & 3 );
        pt_ptoData->t_ptoShaftSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t( (data().getUint8Data(5) >> 1) & 0x7 );

        // set last time
        setSelectedDataSourceDevKey(c_tempDevKey);
        // update time
        pt_ptoData->i32_lastPto = Scheduler_c::getLastTimeEventTrigger();
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
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracPTO_c::isoTimeEventTracMode( )
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

    if ( ( (ci32_now - t_ptoFront.i32_lastPto ) >= 100) && ( t_ptoFront.t_ptoEngaged == IsoAgLib::IsoActive ) )
    { // it's time to send tractor PTO information and the FRONT PTO is engaged
      isoSendMessage(sendFrontPto);
    }

    if (Scheduler_c::getAvailableExecTime() == 0) return false;

    if ( ( (ci32_now - t_ptoRear.i32_lastPto ) >= 100) && ( t_ptoRear.t_ptoEngaged == IsoAgLib::IsoActive ) )
    { // it's time to send tractor PTO information and the REAR PTO is engaged
      isoSendMessage(sendRearPto);
    }

    return true;
  }

  /** Detect stop of PTO update from tractor -> indication for stopped PTO */
  bool TracPTO_c::isoTimeEventImplMode()
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // check for different pto data types whether the previously
    // sending node stopped sending -> other nodes can now step in
    if ( ( ( ci32_now - t_ptoFront.i32_lastPto ) >= TIMEOUT_PTO_DISENGAGED ) || (getSelectedDataSourceDevKey().isUnspecified() ) )
    { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
      // --> switch values to ZERO
      t_ptoFront.ui16_pto8DigitPerRpm = 0;
      t_ptoFront.t_ptoEngaged = IsoAgLib::IsoInactive;
    }
    if ( ( ( ci32_now - t_ptoRear.i32_lastPto ) >= TIMEOUT_PTO_DISENGAGED ) || (getSelectedDataSourceDevKey().isUnspecified() ) )
    { // TECU stoppped its PTO and doesn'T send PTO updates - as defined by ISO 11783
      // --> switch values to ZERO
      t_ptoRear.ui16_pto8DigitPerRpm = 0;
      t_ptoRear.t_ptoEngaged = IsoAgLib::IsoInactive;
    }
    return true;
  }

  /** send pto data message
      @param t_sendptodata  send pto front or pto rear
      @see  TracCert_c::processMsgRequestPGN
      @see  CANIO_c::operator<<
    */
  void TracPTO_c::isoSendMessage(SendPtoData_t t_sendptodata)
  {
    if ( getDevKey() == NULL ) return;
    if (!getIsoMonitorInstance4Comm().existIsoMemberDevKey(*getDevKey(), true)) return;

    CANIO_c& c_can = getCanInstance4Comm();

    // retreive the actual dynamic sender no of the member with the registered devKey
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberDevKey(*getDevKey(), true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setIsoSa(b_sa);
    data().setLen(8);

    setSelectedDataSourceDevKey(*getDevKey());

    PtoData_t* pt_ptoData = NULL;
    if (t_sendptodata == sendFrontPto)
    {
      data().setIsoPgn(FRONT_PTO_STATE_PGN);
      pt_ptoData = &t_ptoFront;
    }
    else
    { //sendRearPto
      data().setIsoPgn(REAR_PTO_STATE_PGN);
      pt_ptoData = &t_ptoRear;
    }

    uint8_t ui8_val;
    data().setUint16Data(0, pt_ptoData->ui16_pto8DigitPerRpm);
    data().setUint16Data(2, pt_ptoData->ui16_ptoSetPoint8DigitPerRpm);
    ui8_val =  (pt_ptoData->t_ptoEngaged             << 6);
    ui8_val |= (pt_ptoData->t_pto1000                << 4);
    ui8_val |= (pt_ptoData->t_ptoEconomy             << 2);
    ui8_val |= (pt_ptoData->t_ptoEngagementReqStatus << 0);
    data().setUint8Data(4, ui8_val);
    ui8_val = 0;
    ui8_val |= (pt_ptoData->t_ptoModeReqStatus         << 6);
    ui8_val |= (pt_ptoData->t_ptoEconomyModeReqStatus  << 4);
    ui8_val |= (pt_ptoData->t_ptoShaftSpeedLimitStatus << 1);
    data().setUint8Data(5, ui8_val);
    //reserved fields
    data().setUint16Data(6, 0);

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    pt_ptoData->i32_lastPto = Scheduler_c::getLastTimeEventTrigger();
  }

  /** force a request for pgn for front pto state */
  bool TracPTO_c::sendRequestUpdateFront()
  {
    if ( checkMode(IsoAgLib::IdentModeImplement) )
      return BaseCommon_c::sendPgnRequest(FRONT_PTO_STATE_PGN);
    else
      return false;
  }
  /** force a request for pgn for rear pto state */
  bool TracPTO_c::sendRequestUpdateRear()
  {
    if ( checkMode(IsoAgLib::IdentModeImplement) )
      return BaseCommon_c::sendPgnRequest(REAR_PTO_STATE_PGN);
    else
      return false;
  }
  #endif

} // End Namespace __IsoAgLib
