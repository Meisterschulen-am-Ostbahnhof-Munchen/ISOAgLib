/***************************************************************************
              tracgeneral_c.cpp - stores, updates  and delivers all base
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
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#include "tracgeneral_c.h"
#include "tracpto_c.h"
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgn_c.h>

using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique TracGeneral_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracGeneral_c& getTracGeneralInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracGeneral_c::instance( rui8_instance );
  };
  #else
  /** C-style function, to get access to the unique TracGeneral_c singleton instance */
  TracGeneral_c& getTracGeneralInstance( void )
  {
    static TracGeneral_c& c_lbsTracGeneral = TracGeneral_c::instance();
    return c_lbsTracGeneral;
  };
  #endif

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_isoName optional pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TracGeneral_c::init_base (const ISOName_c* rpc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t rt_identMode)
  {
    if ( checkAlreadyClosed() )
    {
      // *************************************************************************************************
      // Added by Martin Wodok to accomodate LANGUAGE_PGN Messages:
      b_languageVtReceived = false;
      b_languageTecuReceived = false;
      // *************************************************************************************************
    }

    //call init for handling which is base data independent
    BaseCommon_c::init_base (rpc_isoName, getSingletonVecKey(), rt_identMode);
  };

  /** config the TracGeneral_c object after init -> set pointer to isoName and
      config send/receive of different general base msg types
      @param rpc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracGeneral_c::config_base (const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode, uint16_t rui16_suppressMask)
  { // set configure values
    //store old mode to decide to register or unregister to request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (rpc_isoName, rt_identMode, rui16_suppressMask) ) return false;


    ///Set time Period for Scheduler_c
    if (rt_identMode == IsoAgLib::IdentModeTractor)
    { // SEND data with short period
      setTimePeriod( (uint16_t) 100);
    }
    else
    { // check with long period for timeout after loss of sending node
      setTimePeriod( (uint16_t) 1000 );
    }

    // set the member base msg value vars to NO_VAL codes
    setHitchRear(NO_VAL_8);
    setHitchFront(NO_VAL_8);
    i16_engine = NO_VAL_16S;

    // un-/register to PGN
    if (t_oldMode == IsoAgLib::IdentModeImplement && rt_identMode == IsoAgLib::IdentModeTractor)
    { // a change from Implement mode to Tractor mode occured
      // create FilterBox_c for REQUEST_PGN_MSG_PGN, LANGUAGE_PGN
      getIsoRequestPgnInstance4Comm().registerPGN (*this, LANGUAGE_PGN); // request for language
    }

    if (t_oldMode == IsoAgLib::IdentModeTractor && rt_identMode == IsoAgLib::IdentModeImplement)
    { // a change from Tractor mode to Implement mode occured
      // unregister from request for pgn, because in implement mode no request should be answered
      getIsoRequestPgnInstance4Comm().unregisterPGN (*this, LANGUAGE_PGN);
    }

    i32_lastIsoPositionSimple = 0;

    t_keySwitch = IsoAgLib::IsoNotAvailable; // mark as not available
    ui8_maxPowerTime = ui8_frontLinkForce = ui8_rearLinkForce = NO_VAL_8;
    ui16_frontDraft = ui16_rearDraft = NO_VAL_16;
    ui32_lastMaintainPowerRequest = 0;
    b_maintainEcuPower = b_maintainActuatorPower = false;
    implState.inPark =      IsoAgLib::IsoNotAvailablePark;
    implState.inTransport = IsoAgLib::IsoNotTransported;
    implState.inWork =      IsoAgLib::IsoDisconnect;
    t_frontHitchPosLimitStatus = t_rearHitchPosLimitStatus = IsoAgLib::IsoNotAvailableLimit;

    return true;
  };

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracGeneral_c::checkCreateReceiveFilter( )
  {
    ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CANIO_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // create FilterBox_c for PGN FRONT_HITCH_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, 0x3FFFF00UL,
                        (static_cast<MASK_TYPE>(FRONT_HITCH_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN REAR_HITCH_STATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, 0x3FFFF00UL,
                        (static_cast<MASK_TYPE>(REAR_HITCH_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
     // create FilterBox_c for PGN MAINTAIN_POWER_REQUEST_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, 0x3FFFF00UL,
                        (static_cast<MASK_TYPE>(MAINTAIN_POWER_REQUEST_PGN) << 8), false, Ident_c::ExtendedIdent);

      // *************************************************************************************************
      // Added by Martin Wodok to accomodate LANGUAGE_PGN Messages:
      // create FilterBox_c for LANGUAGE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (LANGUAGE_PGN << 8)
      c_can.insertFilter(*this, 0x3FFFF00UL,
                        (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8), true, Ident_c::ExtendedIdent);

      // *************************************************************************************************
    }
  }

  /** send a ISO11783 general base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    * Timeperiod of 100ms is set in config()
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracGeneral_c::timeEventTracMode( )
  { ///Timeperiod of 100ms is set in ::config
    sendMessage();

    return true;
  }

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CANPkgExt_c::resolveSendingInformation()
    */
  bool TracGeneral_c::processMsg()
  {
    bool b_result = false;
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CANPkgExt_c -> resolveSendingInformation
    ISOName_c c_tempISOName( data().getISONameForSA() );

    switch (data().isoPgn() /* & 0x3FFFF */) // don't need to &0x3FFFF, as this is the whole PGN...
    {
      case FRONT_HITCH_STATE_PGN:
      case REAR_HITCH_STATE_PGN:
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( c_tempISOName ) )
        { // sender is allowed to send
          uint8_t ui8_tempHitch = (( data().getUint8Data( 0 ) * 4) / 10 );
          if ( (ui8_tempHitch != ERROR_VAL_8)
              && (ui8_tempHitch != NO_VAL_8) ) {
            switch ( (data().getUint8Data( 1 ) >> 6) & 3 ) {
            case 0: // not in work
              break;
            case 1: // in work
              ui8_tempHitch |= 0x80;
              break;
            case 2: // Error
              ui8_tempHitch = ERROR_VAL_8;
              break;
            case 3: // Not available
              ui8_tempHitch = NO_VAL_8;
              break;
            }
          }
          if (data().isoPgn() == FRONT_HITCH_STATE_PGN)
          { // front hitch
            setHitchFront(ui8_tempHitch);
            ui8_frontLinkForce = data().getUint8Data( 2 );
            ui16_frontDraft = static_cast<uint16_t>(data().getUint8Data( 3 ) ) + (static_cast<uint16_t>(data().getUint8Data( 4 ) ) << 8);
            t_frontHitchPosLimitStatus = IsoAgLib::IsoLimitFlag_t( ( data().getUint8Data(1) >> 3 ) & 3 );
          }
          else
          { // back hitch
            setHitchRear(ui8_tempHitch);
            ui8_rearLinkForce = data().getUint8Data( 2 );
            ui16_rearDraft = static_cast<uint16_t>(data().getUint8Data( 3 ) ) + (static_cast<uint16_t>(data().getUint8Data( 4 )) << 8);
            t_rearHitchPosLimitStatus = IsoAgLib::IsoLimitFlag_t( ( data().getUint8Data(1) >> 3 ) & 3 );
          }
          setSelectedDataSourceISOName(c_tempISOName);
          //set update time
          setUpdateTime( data().time() );
        }
        else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
        }
        b_result = true;
        break;
      case MAINTAIN_POWER_REQUEST_PGN: // maintain power request
        if ( ( (data().getUint8Data( 0 ) >> 6) & 3) == 1)
          b_maintainEcuPower = true;
        else
          b_maintainEcuPower = false;
        if ( ( (data().getUint8Data( 0 ) >> 4) & 3) == 1)
          b_maintainActuatorPower = true;
        else
          b_maintainActuatorPower = false;

        implState.inTransport = ( (data().getUint8Data( 1 ) >> 6) & 3 );
        implState.inPark =      ( (data().getUint8Data( 1 ) >> 4) & 3 );
        implState.inWork =      ( (data().getUint8Data( 1 ) >> 2) & 3 );

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
              p8ui8_languageVt[i] = data().getUint8Data(i);
            }
            break;
          case 0xF0:
            b_languageTecuReceived = true;
            for (int i=0; i<8; i++)
            {
              p8ui8_languageTecu[i] = data().getUint8Data(i);
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

  bool TracGeneral_c::processMsgRequestPGN (uint32_t rui32_pgn, ISOItem_c* rpc_isoItemSender, ISOItem_c* rpc_isoItemReceiver)
  {
    // check if we are allowed to send a request for pgn
    if ( ! BaseCommon_c::check4ReqForPgn(rui32_pgn, rpc_isoItemSender, rpc_isoItemReceiver) ) return false;

    // call TracGeneral_c function to send language of Tractor-ECU
    // sendLanguage checks if this item (identified by ISOName)
    // is configured to send language
    sendLanguage();
    return true;
  };

  /** send front hitch and rear hitch data msg
      @see  CANIO_c::operator<<
    */
  void TracGeneral_c::sendMessage()
  { //check for isoName and if address claim has yet occured, because this function can also bo called
    //independent from timeEvent() function
    if ( getISOName() == NULL ) return;
    if (!getIsoMonitorInstance4Comm().existIsoMemberISOName(*getISOName(), true)) return;

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    setSelectedDataSourceISOName( *getISOName() );
    CANIO_c& c_can = getCanInstance4Comm();

    uint8_t ui8_temp = 0x7;  /* Pre-load the reserved bits */

    if (mui16_suppressMask & FRONT_HITCH_STATE_PGN_DISABLE_MASK == 0)
    {
      data().setIsoPgn(FRONT_HITCH_STATE_PGN);

      ui8_temp |= t_frontHitchPosLimitStatus << 3;
      switch (hitchFront()) {
        case ERROR_VAL_8:
          data().setUint8Data(0, hitchFront());
          ui8_temp |= ( IsoAgLib::IsoError << 6 );
          data().setUint8Data(1, ui8_temp);
          break;
        case NO_VAL_8:
          data().setUint8Data(0, hitchFront());
          ui8_temp |= ( IsoAgLib::IsoNotAvailable << 6 );
          data().setUint8Data(1, ui8_temp);
          break;
        default:
          data().setUint8Data(0, ((hitchFront() & 0x7F)*10/4));
          if ((hitchFront() & 0x80) != 0)
          {
            ui8_temp |= IsoAgLib::IsoActive << 6;
            data().setUint8Data(1, ui8_temp ); // work
          }
          else
          {
            ui8_temp |= IsoAgLib::IsoInactive << 6;
            data().setUint8Data(1, ui8_temp);
          }
          break;
      }
      data().setUint8Data(2, ui8_frontLinkForce);
      data().setUint8Data(3, ui16_frontDraft& 0xFF);
      data().setUint8Data(4, (ui16_frontDraft >> 8) );

      /* Reserved Bytes */
      data().setUint8Data(5, 0xFF );
      data().setUint16Data(6, 0xFFFF );

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();
    }
    if (mui16_suppressMask & REAR_HITCH_STATE_PGN_DISABLE_MASK == 0)
    {
      data().setIsoPgn(REAR_HITCH_STATE_PGN);
      ui8_temp = 0x7;  /* Pre-load the reserved bits */
      ui8_temp = t_rearHitchPosLimitStatus << 3;
      switch (hitchRear()) {
        case ERROR_VAL_8:
          data().setUint8Data(0, hitchRear());
          ui8_temp |= ( IsoAgLib::IsoError << 6 );
          data().setUint8Data(1, ui8_temp);
          break;
        case NO_VAL_8:
          data().setUint8Data(0, hitchRear());
          ui8_temp |= ( IsoAgLib::IsoNotAvailable << 6 );
          data().setUint8Data(1, ui8_temp);
          break;
        default:
          data().setUint8Data(0, ((hitchRear() & 0x7F)*10/4));
          if ((hitchRear() & 0x80) != 0)
          {
            ui8_temp |= IsoAgLib::IsoActive << 6;
            data().setUint8Data(1, ui8_temp ); // work
          }
          else
          {
            ui8_temp |= IsoAgLib::IsoInactive << 6;
            data().setUint8Data(1, ui8_temp);
          }
          break;
      }
      data().setUint8Data(2, ui8_rearLinkForce);
      data().setUint8Data(3, (ui16_rearDraft& 0xFF) );
      data().setUint8Data(4, ui16_rearDraft >> 8);

      /* Reserved Bytes */
      data().setUint8Data(5, 0xFF );
      data().setUint16Data(6, 0xFFFF );

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      c_can << data();
    }
  }

  /** send iso language data msg
      @see  TracGeneral_c::processMsgRequestPGN
      @see  CANIO_c::operator<<
    */
  void TracGeneral_c::sendLanguage()
  {
    //language is only send in tractor mode
    if ( checkMode(IsoAgLib::IdentModeImplement) ) return;

    if (mui16_suppressMask & LANGUAGE_PGN_DISABLE_MASK != 0) return;

    if (  !b_languageVtReceived
       || ( getISOName()->isUnspecified()  )
       || !getIsoMonitorInstance4Comm().existIsoMemberISOName(*getISOName(), true)
       )
    { //if VT has up to now not send the language command there is no sense to send it
      return;
    } else
    {
      data().setISONameForSA( *getISOName() );
      data().setIdentType(Ident_c::ExtendedIdent);
      data().setIsoPri(6);
      data().setLen(8);

      setSelectedDataSourceISOName( *getISOName() );
      CANIO_c& c_can = getCanInstance4Comm();
      data().setIsoPgn(LANGUAGE_PGN);
      //Bytes 1,2: language command
      data().setUint16Data(0, (p8ui8_languageVt[0] | (p8ui8_languageVt[1] << 8)) );
      //Byte 3: number format; Bit 1-4: reserved, Bit 5,6: time format, Bit 7,8: decimal symbol
      data().setUint8Data(2, p8ui8_languageVt[2]);
      //Byte 4: date format
      data().setUint8Data(3, p8ui8_languageVt[3]);
      //Byte 5: units of measure; Bit 1,2: mass units, Bit 3,4: volume units, Bit 5,6: area units, Bit 7,8: distance units
      data().setUint8Data(4, p8ui8_languageVt[4]);
      //Byte 6: units of measure; Bit 1,2: units system; Bit 3,4; force units, Bit 5,6: pressure units, Bit 7,8: temperature units
      data().setUint8Data(5, p8ui8_languageVt[5]);
      //Bytes 7,8: reserved
      data().setUint8Data(6, p8ui8_languageVt[6] | (p8ui8_languageVt[7] << 8));
      data().setUint8Data(7, 0xFF);

      c_can << data();
    }
  }

 /** force maintain power from tractor
     @see  CANIO_c::operator<<
     @param rb_ecuPower true -> maintain ECU power
     @param rb_actuatorPower true-> maintain actuator power
     @param rt_implState in which state is the implement (transport, park, work)
   */
  void TracGeneral_c::forceMaintainPower( bool rb_ecuPower, bool rb_actuatorPower, IsoAgLib::IsoMaintainPower_t rt_implState)
  {
    if (mui16_suppressMask & MAINTAIN_POWER_REQUEST_PGN_DISABLE_MASK != 0) return;
    // as BaseCommon_c timeEvent() checks only for adr claimed state in TractorMode, we have to perform those checks here,
    // as we reach this function mostly for ImplementMode, where getISOName() might report NULL at least during init time
    if ( ( NULL == getISOName() ) || ( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( *getISOName(), true ) ) )
      return;

     uint8_t val1 = IsoAgLib::IsoInactive,
            val2 = IsoAgLib::IsoInactive;
    if (rb_ecuPower)
      val1 |= ( IsoAgLib::IsoActive   << 6);
    else
      val1 |= ( IsoAgLib::IsoInactive << 6);
    if (rb_actuatorPower)
      val1 |= ( IsoAgLib::IsoActive   << 4);
    else
      val1 |= ( IsoAgLib::IsoInactive << 4);

    switch(rt_implState)
    {
      case IsoAgLib::implInTransport:
        val2 |= ( implState.inTransport         << 6);
        val2 |= ( IsoAgLib::IsoNotAvailablePark << 4);
        val2 |= ( IsoAgLib::IsoNotAvailableWork << 2);
        break;
      case IsoAgLib::implInPark:
        val2 |= ( IsoAgLib::IsoNotAvailableTransport << 6);
        val2 |= ( implState.inPark                   << 4);
        val2 |= ( IsoAgLib::IsoNotAvailableWork      << 2);
      case IsoAgLib::implInWork:
        val2 |= ( IsoAgLib::IsoNotAvailableTransport << 6);
        val2 |= ( IsoAgLib::IsoNotAvailablePark      << 4);
        val2 |= ( implState.inWork                   << 2);
    }
    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(6);

    data().setIsoPgn(MAINTAIN_POWER_REQUEST_PGN);
    data().setUint8Data(0, val1);
    data().setUint8Data(1, val2);
    //reserved fields
    data().setUint16Data(2, 0xFFFFU);
    data().setUint32Data(4, 0xFFFFFFFFUL);
    data().setLen(8);

    // CANIO_c::operator<< retrieves the information with the help of CANPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();
  }
  /** force a request for pgn for language information */
  bool TracGeneral_c::sendRequestUpdateLanguage()
  {
    if ( checkMode(IsoAgLib::IdentModeImplement) )
      return BaseCommon_c::sendPgnRequest(LANGUAGE_PGN);
    else
      return false;
  }

///  Used for Debugging Tasks in Scheduler_c
const char*
TracGeneral_c::getTaskName() const
{   return "TracGeneral_c"; }

} // End Namespace __IsoAgLib
