/***************************************************************************
               tracfacilities_c - working on facilities data;
                                  data information from CanCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
                                  from BaseCommon_c derived for
                                  interaction with other IsoAgLib objects

                                  This class handles Part 7 - B.25.3
                                  Tractor facilities response message 0xFE09
                                  in both directions (configurable)
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Ing. Antoine Kandera
    email                : a.kandera@osb-ag:de
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include "tracfacilities_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>

using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAglib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracFacilities_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracFacilities_c& getTracFacilitiesInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TracFacilities_c::instance( aui8_instance );
  };
  #else
  /** C-style function, to get access to the unique TracFacilities_c singleton instance */
  TracFacilities_c& getTracFacilitiesInstance( void )
  {
    static TracFacilities_c& c_lbsTracFacilities = TracFacilities_c::instance();
    return c_lbsTracFacilities;
  };
  #endif

  /** HIDDEN constructor for a TracFacilities_c object instance which can optional
      set the configuration for send/receive for a moving msg
      NEVER instantiate a variable of type TracFacilities_c within application
      only access TracFacilities_c via getTracFacilitiesInstance() or getTracFacilitiesInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  TracFacilities_c::TracFacilities_c()
  : mb_facilitiesOnInitSent(false)
  {
    memset(arrui8_facilitiesBitData,0,sizeof(arrui8_facilitiesBitData));
  }

  /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
      void TracFacilities_c::singletonInit()
      { // singletonInit is called, AFTER the initializing instance() function has assigned a suitable
        // singleton vec key - this key value is NOT available at construction time!!!
        BaseCommon_c::singletonInitBase(SINGLETON_VEC_KEY);
      }

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param apc_isoName optional pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TracFacilities_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (apc_isoName, getSingletonVecKey(), at_identMode);
  };

  /** config the TracFacilities_c object after init -> set pointer to isoName and
      config send/receive of a moving msg type
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracFacilities_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    //store old mode to decide to register or unregister from request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

    ///Set time Period for Scheduler_c
    if (at_identMode == IsoAgLib::IdentModeTractor)
    { // SEND facilities at start
      setTimePeriod( (uint16_t) 100);
    }

    if (t_oldMode == IsoAgLib::IdentModeImplement && at_identMode == IsoAgLib::IdentModeTractor)
    {  // a change from Implement mode to Tractor mode occured
      // create FilterBox_c for REQUEST_PGN_MSG_PGN, REQUIRED_TRACTOR_FACILITIES_PGN
      getIsoRequestPgnInstance4Comm().registerPGN (*this, TRACTOR_FACILITIES_PGN); // request for certificate
    }

    if (t_oldMode == IsoAgLib::IdentModeTractor && at_identMode == IsoAgLib::IdentModeImplement)
    {  // a change from Tractor mode to Implement mode occured
      // unregister from request for pgn, because in implement mode no request should be answered
      getIsoRequestPgnInstance4Comm().unregisterPGN (*this, TRACTOR_FACILITIES_PGN);
    }

    /// init all value in facilitiesBitData_t with default Not Available
    memset(arrui8_facilitiesBitData,0,sizeof(arrui8_facilitiesBitData));

    return true;
  }

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracFacilities_c::checkCreateReceiveFilter( )
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CanIo_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // create FilterBox_c for PGN TRACTOR_FACILITIES_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (TRACTOR_FACILITIES_PGN << 8)
      c_can.insertStandardIsoFilter(*this,TRACTOR_FACILITIES_PGN,true);
    }
  }

  bool TracFacilities_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
  {
    // check if we are allowed to send a request for pgn
    if ( ! BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver) ) return false;

    // call TracFacilities_c function to send facilities status of a TECU
    // isoSendMessage checks if this item (identified by ISOName)
    // is configured to send facilities status
    sendFacilities();
    return true;
  };

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TracFacilities_c::processMsg()
  {
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CanPkgExt_c -> resolveSendingInformation
    IsoName_c const& rcc_tempISOName = data().getISONameForSA();

    switch (data().isoPgn() & 0x3FF00)
    {
      case TRACTOR_FACILITIES_PGN:
        if ( checkParseReceived (rcc_tempISOName) )
        { // sender is allowed to send

          for (uint8_t ui8_counter = 0;ui8_counter<sizeof(arrui8_facilitiesBitData)/sizeof(uint8_t);++ui8_counter)
            arrui8_facilitiesBitData[ui8_counter] = data().getUint8Data(ui8_counter);

          setSelectedDataSourceISOName (rcc_tempISOName);
          setUpdateTime( data().time() );
        } else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
        }
      break;
    }
    return true;
  }

  /** send facilities data
      @see  TracFacilities_c::processMsgRequestPGN
      @see  CanIo_c::operator<<
    */
  void TracFacilities_c::sendFacilities( )
  {// there is no need to check for address claim because this is already done in the processMsgRequestPGN;
   // this function is only be called on request!!

    CanIo_c& c_can = getCanInstance4Comm();

    data().setISONameForSA( *getISOName() );
    setSelectedDataSourceISOName(*getISOName());
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);

    data().setIsoPgn(TRACTOR_FACILITIES_PGN);

    for (uint8_t ui8_counter = 0;ui8_counter<sizeof(arrui8_facilitiesBitData)/sizeof(uint8_t);++ui8_counter)
      data().setUint8Data(ui8_counter,arrui8_facilitiesBitData[ui8_counter]);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    c_can << data();
  }

  void TracFacilities_c::setTractorClass(const IsoAgLib::IsoTecuClassFlag_t at_val)
  {
    arrui8_facilitiesBitData[ 0 ] &= ~(0x3    << 6);
    arrui8_facilitiesBitData[ 0 ] |=  (at_val << 6);
  }

  void TracFacilities_c::setFacitity(const IsoAgLib::IsoFacility_t at_facility, const IsoAgLib::IsoFacilityFlag_t at_val)
  {
    if (uint32_t(at_facility/8) < sizeof(arrui8_facilitiesBitData))
    {
      arrui8_facilitiesBitData[ at_facility/8 ] &= ~(0x1    << (7 - (at_facility % 8)));
      arrui8_facilitiesBitData[ at_facility/8 ] |=  (at_val << (7 - (at_facility % 8)));
    }
  }

  IsoAgLib::IsoFacilityFlag_t TracFacilities_c::getFacitity(const IsoAgLib::IsoFacility_t at_facility) const
  {
    if (uint32_t(at_facility/8) < sizeof(arrui8_facilitiesBitData))
    {
      return IsoAgLib::IsoFacilityFlag_t((arrui8_facilitiesBitData[at_facility/8] >> (7 - (at_facility % 8))) & 0x1);
    }
    return IsoAgLib::IsoFacilityNotAvailable;
  }

  /** this is only called when sending ident is configured and it has already claimed an address
  @pre  function is only called in tractor mode
  @see  BaseCommon_c::timeEvent()
   */
  bool TracFacilities_c::timeEventTracMode( )
  {
    if (!mb_facilitiesOnInitSent)
    {
      // send Tractor Facility PGN once at start
      sendFacilities();
      // scheduler not needed anymore -> set period to big value
      setTimePeriod (0x7F00);

      mb_facilitiesOnInitSent = true;
    }
    return true;
  }

///  Used for Debugging Tasks in Scheduler_c
const char*
TracFacilities_c::getTaskName() const
{   return "TracFacilities_c"; }

}// End Namespace __IsoAglib
