/***************************************************************************
                          base_c.cpp - working on Base Data Msg Type 1, 2
                                    and Calendar; stores, updates  and
                                    delivers all base data informations
                                    from CANCustomer_c derived for CAN
                                    sending and receiving interaction;
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

  // IsoAgLib_Extension
  #include <IsoAgLib/comm/Multipacket/impl/multireceive_c.h>
  #include <IsoAgLib/comm/Multipacket/istream_c.h>
  #include <IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h>
#endif
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/iutil_funcs.h>
#include "base_c.h"

#include <time.h>

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Base_c& getBaseInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return Base_c::instance( rui8_instance );
  };
#else
  /** C-style function, to get access to the unique Base_c singleton instance */
  Base_c& getBaseInstance( void )
  {
    static Base_c& c_lbsBase = Base_c::instance();
    return c_lbsBase;
  };
#endif

uint8_t bcd2dec(uint8_t rb_bcd);
uint8_t dec2bcd(uint8_t rb_dec);

#if defined(NMEA_2000_FAST_PACKET) && defined(USE_ISO_11783)

/** place next data to send direct into send buffer of pointed
  stream send package - MultiSend_c will send this
  buffer afterwards
*/
void Nmea2000SendStreamer_c::setDataNextStreamPart (__IsoAgLib::MultiSendPkg_c* mspData, uint8_t bytes)
{
  mspData->setDataPart (vec_data, ui16_currentSendPosition, bytes);
  ui16_currentSendPosition += bytes;
}

/** place next data to send direct into send buffer of pointed
    stream send package - MultiSend_c will send this
    buffer afterwards
*/
void Nmea2000SendStreamer_c::setDataNextFastPacketStreamPart (__IsoAgLib::MultiSendPkg_c* mspData, uint8_t bytes, uint8_t rui8_offset )
{
  mspData->setFastPacketDataPart (vec_data, ui16_currentSendPosition, bytes, rui8_offset );
  ui16_currentSendPosition += bytes;
}



/** set cache for data source to stream start */
void Nmea2000SendStreamer_c::resetDataNextStreamPart()
{
  ui16_currentSendPosition = 0;
}

/** save current send position in data source - neeed for resend on send problem */
void Nmea2000SendStreamer_c::saveDataNextStreamPart ()
{
  ui16_storedSendPosition = ui16_currentSendPosition;
}

/** reactivate previously stored data source position - used for resend on problem */
void Nmea2000SendStreamer_c::restoreDataNextStreamPart ()
{
  ui16_currentSendPosition = ui16_storedSendPosition;
}

/** calculate the size of the data source */
uint32_t Nmea2000SendStreamer_c::getStreamSize ()
{
  return vec_data.size();
}

/** get the first byte, which is the COMMAND-byte and should be given back CONST! */
uint8_t Nmea2000SendStreamer_c::getFirstByte ()
{
  return *(vec_data.begin());
}
// END of USE_ISO_11783
#endif


void
Base_c::singletonInit()
{
  setAlreadyClosed();
  init( NULL, IsoAgLib::BaseDataNothing );
}

/**
  initialise element which can't be done during construct;
  above all create the needed FilterBox_c instances, to receive
  the needed CAN msg with base msg type 1,2 and calendar

  possible errors:
      * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
  @param rpc_devKey optional pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
  @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
*/
void Base_c::init(const DevKey_c* rpc_devKey, IsoAgLib::BaseDataGroup_t rt_mySendSelection)
{
  // first register in Scheduler_c
  getSchedulerInstance4Comm().registerClient( this );
  c_data.setSingletonKey( getSingletonVecKey() );

  // set the member base msg value vars to NO_VAL codes

  bit_calendar.year = 0;
  bit_calendar.month = 1;
  bit_calendar.day = 1;
  bit_calendar.hour = 0;
  bit_calendar.minute = 0;
  bit_calendar.second = 0;
  bit_calendar.msec = 0;
  bit_calendar.timezoneMinuteOffset = 0;
  bit_calendar.timezoneHourOffsetMinus24 = 24;

  setHitchRear(NO_VAL_8);
  setHitchFront(NO_VAL_8);
  i16_engine = NO_VAL_16S;
  i16_ptoFront = i16_ptoRear = i16_speedReal = i16_speedTheor = NO_VAL_16S;

  // set distance value to 0
  i16_lastDistReal = i16_lastDistTheor = 0;
  i32_distReal = i32_distTheor = 0;

  // set the timestamps to 0
  i32_lastBase1 = i32_lastBase2 = i32_lastBase3 = i32_lastFuel
      = i32_lastCalendar = 0;
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
  #ifdef NMEA_2000_FAST_PACKET
  i32_lastIsoPositionStream = i32_lastIsoDirectionStream = 0;
  t_multiSendSuccessState = MultiSend_c::SendSuccess;
  #endif

  t_frontPtoEngaged = t_rearPtoEngaged
   = t_frontPto1000 = t_rearPto1000
   = t_frontPtoEconomy = t_rearPtoEconomy = t_keySwitch = IsoAgLib::IsoNotAvailable; // mark as not available
  ui8_maxPowerTime = ui8_frontLinkForce = ui8_rearLinkForce = NO_VAL_8;
  i16_frontDraft = i16_rearDraft = NO_VAL_16S;
  ui32_lastMaintainPowerRequest = 0;
  b_maintainEcuPower = b_maintainActuatorPower = b_maintainPowerForImplInTransport
    = b_maintainPowerForImplInPark = b_maintainPowerForImplInWork = false;

  if ( checkAlreadyClosed() )
  {
    b_isoFilterCreated = false;

  // *************************************************************************************************
  // Added by Brad Cox to accomodate NMEA 2000 GPS Messages:
  // Set GPS Lat/Lon values to 0
  i32_latitudeDegree10Minus7 = i32_longitudeDegree10Minus7 = 0x7FFFFFFF;
  #ifdef NMEA_2000_FAST_PACKET
  ui32_altitudeCm = 0;

  ui8_satelliteCnt = 0;
  getMultiReceiveInstance4Comm().registerClient(NMEA_GPS_POSITON_DATA_PGN,   0xFF, this, true, false, true);
  getMultiReceiveInstance4Comm().registerClient(NMEA_GPS_DIRECTION_DATA_PGN, 0xFF, this, true, false, true);
  #endif
  // *************************************************************************************************

  // *************************************************************************************************
  // Added by Martin Wodok to accomodate LANGUAGE_PGN Messages:
  b_languageVtReceived = false;
  b_languageTecuReceived = false;
  // *************************************************************************************************
  }
  #endif
  i32_lastCalendarSet = 0;

  // set configure values with call for config
  config(rpc_devKey, rt_mySendSelection);

  // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
};

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void Base_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    // unregister from timeEvent() call by Scheduler_c
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
__IsoAgLib::CANPkgExt_c& Base_c::dataBase()
{
  return c_data;
}


/**
  config the Base_c object after init -> set pointer to devKey and
  config send/receive of different base msg types
  @param rpc_devKey pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
  @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
*/
void Base_c::config(const DevKey_c* rpc_devKey, IsoAgLib::BaseDataGroup_t rt_mySendSelection)
{
  // set configure values
  pc_devKey = rpc_devKey;
  t_mySendSelection = rt_mySendSelection;


  // set ui8_sendDevKey to the pointed value, if pointer is valid
  if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup1   ) != 0 ) ) c_sendBase1DevKey = *rpc_devKey;
  else c_sendBase1DevKey.setUnspecified();
  if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2   ) != 0 ) ) c_sendBase2DevKey = *rpc_devKey;
  else c_sendBase2DevKey.setUnspecified();
  if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataCalendar ) != 0 ) ) c_sendCalendarDevKey = *rpc_devKey;
  else c_sendCalendarDevKey.setUnspecified();

  if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup3   ) != 0 ) ) c_sendBase3DevKey = *rpc_devKey;
  else c_sendBase3DevKey.setUnspecified();
  if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataFuel     ) != 0 ) ) c_sendFuelDevKey = *rpc_devKey;
  else c_sendFuelDevKey.setUnspecified();

  #ifdef USE_ISO_11783
  if ((rpc_devKey != NULL) && ( ( t_mySendSelection & IsoAgLib::BaseDataGps     ) != 0 ) )
  {
    c_sendGpsDevKey = *rpc_devKey;
    #ifdef NMEA_2000_FAST_PACKET
    // also remove any previously registered MultiReceive connections
    getMultiReceiveInstance4Comm().deregisterClient( this );
    c_nmea2000Streamer.reset();
    c_nmea2000Streamer.vec_data.resize(0);
    #endif
  }
  else
  {
    c_sendGpsDevKey.setUnspecified();
    #ifdef NMEA_2000_FAST_PACKET
    // make sure that the needed multi receive are registered
    getMultiReceiveInstance4Comm().registerClient(NMEA_GPS_POSITON_DATA_PGN,   0xFF, this, true, false, true);
    getMultiReceiveInstance4Comm().registerClient(NMEA_GPS_DIRECTION_DATA_PGN, 0xFF, this, true, false, true);
    c_nmea2000Streamer.vec_data.reserve(51); // GNSS Position Data with TWO reference stations
    #endif
  }
  #endif
};

/** Retrieve the last update time of the specified information type
  @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
  */
int32_t Base_c::lastUpdate( IsoAgLib::BaseDataGroup_t rt_mySendSelection ) const
{
  const int32_t ci32_now = System_c::getTime();
  switch ( rt_mySendSelection )
  {
    case IsoAgLib::BaseDataGroup1:   return ( ci32_now - i32_lastBase1);
    case IsoAgLib::BaseDataGroup2:   return ( ci32_now - i32_lastBase2);
    case IsoAgLib::BaseDataGroup3:   return ( ci32_now - i32_lastBase3);
    case IsoAgLib::BaseDataCalendar: return ( ci32_now -  i32_lastCalendar);

    case IsoAgLib::BaseDataFuel:     return ( ci32_now - i32_lastFuel);
    #ifdef USE_ISO_11783
    case IsoAgLib::BaseDataGps:
    #ifdef NMEA_2000_FAST_PACKET
      if ( i32_lastIsoPositionStream > i32_lastIsoPositionSimple ) return ( ci32_now - i32_lastIsoPositionStream);
      else return ( ci32_now - i32_lastIsoPositionSimple);
    #endif
    #endif
    default: return 0x7FFFFFFF;
  }
}


/**
  process received base msg and store updated value for later reading access;
  called by FilterBox_c::processMsg after receiving a msg

  possible errors:
      * LibErr_c::LbsBaseSenderConflict base msg recevied from different member than before
  @see FilterBox_c::processMsg
  @see CANIO_c::processMsg
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
*/
bool Base_c::processMsg(){
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


/**
  functions with actions, which must be performed periodically
  -> called periodically by Scheduler_c
  ==> sends base msg if configured in the needed rates

  possible errors:
      * dependant error in CANIO_c on CAN send problems
  @see CANPkg_c::getData
  @see CANPkgExt_c::getData
  @see CANIO_c::operator<<
  @return true -> all planned activities performed in allowed time
*/
bool Base_c::timeEvent( void ) {

  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  checkCreateReceiveFilter();
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

  // check for different base data types whether the previously
  // sending node stopped sending -> other nodes can now step in
  if ( ( ( t_mySendSelection & IsoAgLib::BaseDataGroup1 ) == 0 )
    && ( ( ci32_now - i32_lastBase1 ) >= 3000                  )
    && ( c_sendBase1DevKey.isSpecified()                       ) )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    c_sendBase1DevKey.setUnspecified();
  }
  if ( ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2 ) == 0 )
    && ( ( ci32_now - i32_lastBase2 ) >= 3000                  )
    && ( c_sendBase2DevKey.isSpecified()                       ) )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    c_sendBase2DevKey.setUnspecified();
  }
  if ( ( ( t_mySendSelection & IsoAgLib::BaseDataGroup3 ) == 0 )
    && ( ( ci32_now - i32_lastBase3 ) >= 3000                  )
    && ( c_sendBase3DevKey.isSpecified()                       ) )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    c_sendBase3DevKey.setUnspecified();
  }
  if ( ( ( t_mySendSelection & IsoAgLib::BaseDataFuel ) == 0  )
    && ( ( ci32_now - i32_lastFuel ) >= 3000                  )
    && ( c_sendFuelDevKey.isSpecified()                       ) )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    c_sendFuelDevKey.setUnspecified();
  }
  if ( ( ( t_mySendSelection & IsoAgLib::BaseDataCalendar ) == 0           )
    && ( ( ( ci32_now - i32_lastCalendar ) >= 3000 ) || ( yearUtc() == 0 ) )
    && ( c_sendCalendarDevKey.isSpecified()                                ) )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    c_sendCalendarDevKey.setUnspecified();
    bit_calendar.year = bit_calendar.hour = bit_calendar.minute = bit_calendar.second = 0;
    bit_calendar.month = bit_calendar.day = 1;
  }
  #ifdef USE_ISO_11783
  if ( ( ( t_mySendSelection & IsoAgLib::BaseDataGps ) == 0 )
    #ifdef NMEA_2000_FAST_PACKET
    && ( ( ci32_now - i32_lastIsoPositionStream  ) >= 3000  )
    && ( ( ci32_now - i32_lastIsoDirectionStream ) >= 3000  )
    #endif
    && ( ( ci32_now - i32_lastIsoPositionSimple  ) >= 3000  )
    && ( c_sendGpsDevKey.isSpecified()                      ) )
  { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
    c_sendGpsDevKey.setUnspecified();
    i32_latitudeDegree10Minus7 = i32_longitudeDegree10Minus7 = 0x7FFFFFFF;
    #ifdef NMEA_2000_FAST_PACKET
    ui8_satelliteCnt = 0;
    t_gnssMethod = IsoAgLib::IsoNoGps;
    t_gnssType = IsoAgLib::IsoGnssGps;
    ui32_altitudeCm = 0;
    #endif
  }
  #endif


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
void Base_c::checkCreateReceiveFilter( void )
{
  SystemMgmt_c& c_systemMgmt = getSystemMgmtInstance4Comm();
  CANIO_c &c_can = getCanInstance4Comm();
  #ifdef USE_DIN_9684
  if ( ( !b_dinFilterCreated ) && (c_systemMgmt.existActiveLocalDinMember() ) )
  { // check if needed receive filters for DIN are active
    b_dinFilterCreated = true;
    // filter for base data 1
    c_can.insertFilter(*this, (0x7F << 4),(0x14 << 4), false);
    // filter for base data 2
    c_can.insertFilter(*this, (0x7F << 4),(0x15 << 4), false);
    // NEW filter for base data 3
    c_can.insertFilter(*this, (0x7F << 4),(0x16 << 4), false);
    // filter for lower priority base data fuel consumption & base data data calendar
    c_can.insertFilter(*this, (0x7C << 4),(0x1C << 4), true);
  }
  #endif
  #ifdef USE_ISO_11783

  if ( ( ! b_isoFilterCreated ) && ( c_systemMgmt.existActiveLocalIsoMember() ) )
  { // check if needed receive filters for ISO are active
    b_isoFilterCreated = true;
    // create FilterBox_c for PGN TIME_DATE_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(TIME_DATE_PGN) << 8), false, Ident_c::ExtendedIdent);
    // create FilterBox_c for PGN GROUND_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(GROUND_BASED_SPEED_DIST_PGN) << 8), false, Ident_c::ExtendedIdent);
    // create FilterBox_c for PGN WHEEL_BASED_SPEED_DIST_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(WHEEL_BASED_SPEED_DIST_PGN) << 8), false, Ident_c::ExtendedIdent);
    // create FilterBox_c for PGN FRONT_HITCH_STATE_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(FRONT_HITCH_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
    // create FilterBox_c for PGN BACK_HITCH_STATE_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(BACK_HITCH_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
    // create FilterBox_c for PGN FRONT_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(FRONT_PTO_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);
    // create FilterBox_c for PGN BACK_PTO_STATE_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(BACK_PTO_STATE_PGN) << 8), false, Ident_c::ExtendedIdent);

    // *************************************************************************************************
    // Added by Martin Wodok to accomodate LANGUAGE_PGN Messages:
    // create FilterBox_c for LANGUAGE_PGN, PF 254 - mask for DP, PF and PS
    // mask: (0x1FFFF << 8) filter: (LANGUAGE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                      (static_cast<MASK_TYPE>(LANGUAGE_PGN) << 8), false, Ident_c::ExtendedIdent);

    // *************************************************************************************************
    // Added by Brad Cox to accomodate NMEA 2000 GPS Messages:

    // GNSS Position Rapid Update
    // mask: (0x1FFFF << 8) filter: (NMEA_GPS_POSITON_RAPID_UPDATE_PGN << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                  (static_cast<MASK_TYPE>(NMEA_GPS_POSITON_RAPID_UPDATE_PGN) << 8), true, Ident_c::ExtendedIdent);

    // *************************************************************************************************
  }
  #endif
}

#ifdef USE_DIN_9684
/** send a DIN9684 base information PGN
 * this is only called when sending ident is configured and it has already claimed an address
 */
bool Base_c::dinTimeEvent( void )
{
  CANIO_c& c_can = getCanInstance4Comm();
  const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
  // retreive the actual dynamic sender no of the member with the registered devKey
  uint8_t b_send = getDinMonitorInstance4Comm().dinMemberDevKey(*pc_devKey, true).nr();

  if ( ( ( ci32_now - i32_lastBase1 ) >= 100                   )
    && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup1 ) != 0 ) )
  { // send actual base1 data
    c_sendBase1DevKey = *pc_devKey;
    data().setBabo(4);
    data().setSend(b_send);
    data().setVal12(i16_speedReal);
    data().setVal34(i16_speedTheor);
    data().setVal56(long2int(i32_distReal));
    data().setVal78(long2int(i32_distTheor));

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    i32_lastBase1 = ci32_now;
  }
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  if ( ( ( ci32_now - i32_lastBase2 ) >= 100                   )
    && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2 ) != 0 ) )
  { // send actual base2 data
    c_sendBase2DevKey = *pc_devKey;
    data().setBabo(5);
    data().setSend(b_send);
    data().setVal12(i16_ptoRear);
    data().setVal34(i16_ptoFront);
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

  if ( ( ( ci32_now - i32_lastCalendar) >= 1000                  )
    && ( ( t_mySendSelection & IsoAgLib::BaseDataCalendar ) != 0 ) )
  { // send actual calendar data
    c_sendCalendarDevKey = *pc_devKey;
    data().setBabo(0xF);
    data().setSend(b_send);
    data().setVal1(dec2bcd(yearLocal() / 100));
    data().setVal2(dec2bcd(yearLocal() % 100));
    data().setVal3(dec2bcd(monthLocal()));
    data().setVal4(dec2bcd(dayLocal()));
    data().setVal5(dec2bcd(hourLocal()));
    data().setVal6(dec2bcd(minuteLocal()));
    data().setVal7(dec2bcd(second()));

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    c_can << data();

    // update time
    i32_lastCalendar = ci32_now;
  }
  return true;
}

/** process a DIN9684 base information PGN */
bool Base_c::dinProcessMsg()
{ // a DIN9684 base information msg received
  DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
  bool b_result = false;
  // store the devKey of the sender of base data
  const uint16_t ui16_actTime100ms = (data().time() / 100);
  if (getDinMonitorInstance4Comm().existDinMemberNr(data().send()))
  { // the corresponding sender entry exist in the monitor list
    c_tempDevKey = getDinMonitorInstance4Comm().dinMemberNr(data().send()).devKey();
  }

  // interprete data accordingto BABO
  switch (data().babo())
  {
    case 4: // base data 1: speed, dist
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey, c_sendBase1DevKey, IsoAgLib::BaseDataGroup1 ) )
      { // sender is allowed to send
        // real speed
        setSpeedReal(data().val12());
        // theor speed
        setSpeedTheor(data().val34());
        // real dist -> react on 16bit int16_t overflow
        setOverflowSecure(i32_distReal, i16_lastDistReal, data().val56());
        // theor dist -> react on 16bit int16_t overflow
        setOverflowSecure(i32_distTheor, i16_lastDistTheor, data().val78());

        // set last time
        i32_lastBase1 = ui16_actTime100ms;
        c_sendBase1DevKey = c_tempDevKey;
      }
      else
      { // there is a sender conflict
        getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
      }
      b_result = true;
      break;
    case 5: // base data 2: pto, hitch
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey, c_sendBase2DevKey, IsoAgLib::BaseDataGroup2 ) )
      { // sender is allowed to send
        // rear pto
        setPtoRear(data().val12());
        // front pto
        setPtoFront(data().val34());
        // engine speed
        setEngine(data().val56());
        // rear hitch
        setHitchRear(data().val7());
        // front hitch
        setHitchFront(data().val8());

        // set last time
        i32_lastBase2 = ui16_actTime100ms;
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
        i32_lastBase3 = ui16_actTime100ms;
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
        i32_lastFuel = ui16_actTime100ms;
        c_sendFuelDevKey = c_tempDevKey;
      }
      else
      { // there is a sender conflict
        getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
      }
      b_result = true;
      break;
    case 0xF: // calendar
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey, c_sendCalendarDevKey, IsoAgLib::BaseDataCalendar ) )
      { // sender is allowed to send
        // only store date with valid year ( != 0 ) or if last received
        // year is also 0
        if ( ( data().val1() != 0 ) || ( data().val2() != 0 ) || ( yearUtc() == 0 ) )
        { // store new calendar setting
          #ifdef USE_ISO_11783
          if ( ( ( t_mySendSelection & IsoAgLib::BaseDataGps ) == 0 )
            && ( c_sendGpsDevKey.isUnspecified()                    ) )
          #endif
          { // neither this item now another item is sending GPS data -> this is the best time source
            setCalendarLocal(
              bcd2dec(data().val1()) * 100 + bcd2dec(data().val2()),
              bcd2dec(data().val3()), bcd2dec(data().val4()),
              bcd2dec(data().val5()), bcd2dec(data().val6()),
              bcd2dec(data().val7())
            );
          }
          #ifdef USE_ISO_11783
          else
          { // only fetch the date, as this information might not be defined by GPS
            setDateLocal(bcd2dec(data().val1()) * 100 + bcd2dec(data().val2()),
              bcd2dec(data().val3()), bcd2dec(data().val4()));
          }
          #endif
        }
        // only handle this data source as reference data source, if year is valid
        // ( i.e. year != 0 )
        if ( ( data().val1() != 0 ) || ( data().val2() != 0 ) )
        { // set last time
          i32_lastCalendar = ui16_actTime100ms;
          c_sendCalendarDevKey = c_tempDevKey;
        }
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
bool Base_c::isoProcessMsg()
{
  bool b_result = false;
  DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
  const uint16_t ui16_actTime100ms = (data().time()/100);
  // store the devKey of the sender of base data
  if (getIsoMonitorInstance4Comm().existIsoMemberNr(data().isoSa()))
  { // the corresponding sender entry exist in the monitor list
    c_tempDevKey = getIsoMonitorInstance4Comm().isoMemberNr(data().isoSa()).devKey();
  }

  switch (data().isoPgn() & 0x1FFFF)
  {
    case TIME_DATE_PGN:
      // time - date
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey, c_sendCalendarDevKey, IsoAgLib::BaseDataCalendar ) )
      { // sender is allowed to send
        // store new calendar setting
        if ( ( ( t_mySendSelection & IsoAgLib::BaseDataGps ) == 0 )
          && ( c_sendGpsDevKey.isUnspecified()                       ) )
        { // neither this item nor another item is sending GPS data -> this is the best time source
          setCalendarUtc(
          (data().val6() + 1985), data().val4(), (data().val5() / 4), (data().val3()),
           (data().val2()), (data().val1() / 4));
        }
        else
        { // only fetch the date, as this information might not be defined by GPS
          setDateUtc((data().val6() + 1985), data().val4(), (data().val5() / 4));
        }
        // take local timezone offset in all cases
        bit_calendar.timezoneMinuteOffset = data().val7();
        bit_calendar.timezoneHourOffsetMinus24 = data().val8();
        // set last time
        i32_lastCalendar = ui16_actTime100ms;
        c_sendCalendarDevKey = c_tempDevKey;
      }
      else
      { // there is a sender conflict
        getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
      }
      b_result = true;
      break;
    case GROUND_BASED_SPEED_DIST_PGN:
    case WHEEL_BASED_SPEED_DIST_PGN:
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey, c_sendBase1DevKey, IsoAgLib::BaseDataGroup1 ) )
      { // sender is allowed to send
        int16_t i16_tempSpeed = data().val12();
        switch (data().val8() & 3 ) {
         case 0: // driving reverse
          if ( (i16_tempSpeed != ERROR_VAL_16S)
            && (i16_tempSpeed != NO_VAL_16S) ) i16_tempSpeed *= -1;
          break;
         case 1: // driving forward
          break;
         case 2: // ERROR
          i16_tempSpeed = ERROR_VAL_16S;
          break;
         case 3: // not available
          i16_tempSpeed = NO_VAL_16S;
          break;
        }

        if (data().isoPgn() == GROUND_BASED_SPEED_DIST_PGN)
        { // real speed
          setSpeedReal(i16_tempSpeed);
          // real dist
          setDistReal(static_cast<int32_t>(data().val36()));
        }
        else
        { // wheel based speed
          setSpeedTheor(i16_tempSpeed);
          // wheel based dist
          setDistTheor(static_cast<int32_t>(data().val36()));
          // additionally scan for key switch and maximum power time
          t_keySwitch = IsoAgLib::IsoActiveFlag_t( ( data().val8() >> 2 ) & 3 );
          ui8_maxPowerTime = data().val7();
        }
        // set last time
        i32_lastBase1 = ui16_actTime100ms;
        c_sendBase1DevKey = c_tempDevKey;
      }
      else
      { // there is a sender conflict
        getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
      }
      b_result = true;
      break;
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
        i32_lastBase2 = ui16_actTime100ms;
        c_sendBase2DevKey = c_tempDevKey;
      }
      else
      { // there is a sender conflict
        getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
      }
      b_result = true;
      break;
    case FRONT_PTO_STATE_PGN:
    case BACK_PTO_STATE_PGN:
      // only take values, if i am not the regular sender
      // and if actual sender isn't in conflict to previous sender
      if ( checkParseReceived( c_tempDevKey, c_sendBase2DevKey, IsoAgLib::BaseDataGroup2 ) )
      { // sender is allowed to send
        if (data().isoPgn() == FRONT_PTO_STATE_PGN)
        { // front PTO
          setPtoFront(data().val12());
          t_frontPtoEngaged = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 6) & 3);
          t_frontPto1000 = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 4) & 3);
          t_frontPtoEconomy = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 2) & 3);
        }
        else
        { // back PTO
          setPtoRear(data().val12());
          t_rearPtoEngaged = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 6) & 3);
          t_rearPto1000 = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 4) & 3);
          t_rearPtoEconomy = IsoAgLib::IsoActiveFlag_t( (data().val5() >> 2) & 3);
        }
        // set last time
        i32_lastBase2 = ui16_actTime100ms;
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
    // Added by Brad Cox for NMEA 2000 GPS Messages:
    case NMEA_GPS_POSITON_RAPID_UPDATE_PGN:
      if ( checkParseReceived( c_tempDevKey, c_sendGpsDevKey, IsoAgLib::BaseDataGps ) )
      { // sender is allowed to send
        i32_lastIsoPositionSimple = ui16_actTime100ms;
        i32_latitudeDegree10Minus7  = data().getInt32Data( 0 );
        i32_longitudeDegree10Minus7 = data().getInt32Data( 4 );
        // set last time
        i32_lastIsoPositionSimple = ui16_actTime100ms;
        c_sendGpsDevKey = c_tempDevKey;
      }
      b_result = true;
      break;
      // **********************************************************
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
#if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
/** check if an NMEA2000 position signal was received */
bool Base_c::isPositionReceived() const
{
  if ( ( i32_latitudeDegree10Minus7 == 0x7FFFFFFF  )
    && ( i32_longitudeDegree10Minus7 == 0x7FFFFFFF )
    #ifdef NMEA_2000_FAST_PACKET
    && ( ui32_altitudeCm == 0                      )
    #endif
     )
  { // there is no valid GPS information
    return false;
  }
  else
  {
    return true;
  }
}
#endif

#ifdef NMEA_2000_FAST_PACKET
/** deliver GPS receive qualitiy */
void Base_c::setGnssMode( IsoAgLib::IsoGnssMethod_t rt_newVal )
{
  if(rt_newVal<=IsoAgLib::IsoGnssMethodMAX) t_gnssMethod = rt_newVal;
  // set the upper 4 bits to unused as the Set/COG/Heading Ref is not used
  ui8_dataModeAndHeadingReference = 0xF0;
  switch ( rt_newVal )
  {
    case IsoAgLib::IsoDgnssFix:
    case IsoAgLib::IsoGnssPrecise:
    case IsoAgLib::IsoRtkFixedInteger:
    case IsoAgLib::IsoRtkFloat:
      ui8_dataModeAndHeadingReference |= 0x1; // Differential enhanced mode
      break;
    case IsoAgLib::IsoDrEstimated:
      ui8_dataModeAndHeadingReference |= 0x2; // Estimated Mode
      break;
    case IsoAgLib::IsoGnssSimulated:
      ui8_dataModeAndHeadingReference |= 0x3; // Simulated Mode
      break;
    case IsoAgLib::IsoGnssManual:
      ui8_dataModeAndHeadingReference |= 0x4; // Manual Mode
      break;
    case IsoAgLib::IsoGnssError:
      ui8_dataModeAndHeadingReference |= 0xE; // Error
      break;
    case IsoAgLib::IsoGnssNull:
      ui8_dataModeAndHeadingReference |= 0xF; // Error
      break;
    default:
      // let the lower 4 bits at value "0" as currently initialized --> Autonomous Mode
      break;

  }
}

// //////////////////////////////// +X2C Operation 2432 : reactOnStreamStart
//! Parameter:
//! @param rc_ident:
//! @param rui32_totalLen:
bool Base_c::reactOnStreamStart(IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                                uint32_t /*rui32_totalLen */)
{ // ~X2C
  if ( ( ( rc_ident.getPgn() == NMEA_GPS_POSITON_DATA_PGN   )
           || ( rc_ident.getPgn() == NMEA_GPS_DIRECTION_DATA_PGN ) )
           && ( ( t_mySendSelection & IsoAgLib::BaseDataGps   ) == 0 ) )
  { // this a NMEA multi stream of interest where we don't send it
    return true;
  }
  else
  { // this is not of interest for us
    return false;
  }
} // -X2C

// //////////////////////////////// +X2C Operation 5692 : reactOnAbort
void Base_c::reactOnAbort(IsoAgLib::ReceiveStreamIdentifier_c /*rc_ident*/)
{ // ~X2C
  // as we don't perform an on-the-fly parse of the pool, nothing has to be done here
  // - this is only important with on-the-fly parse, where everything parsed already has to be invalidated
} // -X2C

// //////////////////////////////// +X2C Operation 5688 : processPartStreamDataChunk
//! Parameter:
//! @param rc_ident:
//! @param rb_isFirstChunk:
//! @param rb_isLastChunkAndACKd:
bool Base_c::processPartStreamDataChunk(IsoAgLib::iStream_c* rpc_stream,
                                        bool /*rb_isFirstChunk*/,
                                        bool rb_isLastChunkAndACKd)
{ // ~X2C
  IsoAgLib::ReceiveStreamIdentifier_c rc_ident = rpc_stream->getIdent();

  // >>>Last Chunk<<< Processing
  if (rb_isLastChunkAndACKd)
  {  /** let reactOnLastChunk decide whether the pool should be kept in memory */
    return reactOnLastChunk(rc_ident, *rpc_stream);
  }
  // default - don't keep it
  return false;
} // -X2C


//  Operation: reactOnAbort
void Base_c::reactOnAbort(IsoAgLib::iStream_c* /*rpc_stream*/)
{ // as we don't perform on-the-fly parse, there is nothing special to do
}

void getDegree10Minus7FromStream( IsoAgLib::iStream_c& refc_stream, int32_t& refi32_result )
{
  #if SIZEOF_INT == 4
  // use 64 bit variable
  int64_t i64_temp;
  IsoAgLib::convertIstream( refc_stream, i64_temp );
  double d_temp = double(i64_temp);
  refi32_result = int32_t( d_temp * 1.0e-9 );
 #else
  // only take higher 4 bytes
  int32_t i32_temp;

  // ignore the result of the following call
  IsoAgLib::convertIstream( refc_stream, i32_temp );
  // only take this part
  IsoAgLib::convertIstream( refc_stream, i32_temp );
  // NMEA sends with 1.0e-16, while normally 1.0e-7 is enough -> mult with 1.0e-9
  double d_temp = ( double(i32_temp) * 4294967296.0 * 1.0e-9 );
  refi32_result = int32_t( d_temp );
  #endif
}

void getAltitude10Minus2FromStream( IsoAgLib::iStream_c& refc_stream, uint32_t& refui32_result )
{
#if SIZEOF_INT == 4
  // use 64 bit variable
  int64_t i64_temp;
  IsoAgLib::convertIstream( refc_stream, i64_temp );
  double d_temp = double(i64_temp);
  // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
  refui32_result = int32_t( d_temp * 1.0e-4 );
  #else
  // only take higher 4 bytes
  int32_t i32_temp;

  // ignore the result of the following call
  IsoAgLib::convertIstream( refc_stream, i32_temp );
  // only take this part
  IsoAgLib::convertIstream( refc_stream, i32_temp );
  // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
  double d_temp = ( double(i32_temp) * 4294967296.0 * 1.0e-4 );
  refi32_result = int32_t( d_temp );
  #endif
}

// //////////////////////////////// +X2C Operation 5703 : reactOnLastChunk
//! Parameter:
//! @param rc_ident:
//! @param rpc_stream:
bool Base_c::reactOnLastChunk( IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                               IsoAgLib::iStream_c& refc_stream)
{ // ~X2C
  // see if it's a pool upload, string upload or whatsoever! (First byte is already read by MultiReceive!)
  const uint8_t cui8_sa = refc_stream.getIdent().getSa();
  DevKey_c c_tempDevKey( DevKey_c::DevKeyUnspecified );
  if (getIsoMonitorInstance4Comm().existIsoMemberNr(cui8_sa))
  { // the corresponding sender entry exist in the monitor list
    c_tempDevKey = getIsoMonitorInstance4Comm().isoMemberNr(cui8_sa).devKey();
  }

  // check if we want to process the information
  if (
      ( ( t_mySendSelection & IsoAgLib::BaseDataGps   ) != 0 ) // I'm the sender
      || ( // one of the following conditions must be true
         (c_sendGpsDevKey != c_tempDevKey) // actual sender different to last
      && (c_sendGpsDevKey.isSpecified() ) // last sender has correctly claimed address member
         )
     )
  { // DO NOT take this message, as this might be a falsly double source
    return false;
  }

  // set last time and DEVKEY information
  c_sendGpsDevKey = c_tempDevKey;


  uint8_t ui8_tempValue;

  switch ( rc_ident.getPgn() )
  {
    case NMEA_GPS_POSITON_DATA_PGN: // 0x01F805LU -> 129 029
    {
      i32_lastIsoPositionStream = data().time();
      // fetch sequence number from Byte1
      IsoAgLib::convertIstream( refc_stream, ui8_positionSequenceID );
      // --> continue with Byte2 ...
      uint16_t ui16_daysSince1970;
      uint32_t ui32_milliseconds;
      // read 2-bytes of Generic date as days since 1-1-1970 ( UNIX date )
      IsoAgLib::convertIstream( refc_stream, ui16_daysSince1970 );
      IsoAgLib::convertIstream( refc_stream, ui32_milliseconds );
      // NMEA NMEA_GPS_POSITON_DATA_PGN sends with 0.1 msec
      ui32_milliseconds /= 10;

      const time_t t_tempUnixTime = ( time_t(ui16_daysSince1970) * time_t(60 * 60 * 24) ) + (ui32_milliseconds/1000);
      tm* UtcNow = gmtime( &t_tempUnixTime );
      if ( ( UtcNow != NULL ) && ( ( t_mySendSelection & IsoAgLib::BaseDataCalendar   ) != 0  ))
      { // now read the tm data strcture as I'm currently sending time
        setCalendarUtc((UtcNow->tm_year+1900), UtcNow->tm_mon, UtcNow->tm_mday,
                        UtcNow->tm_hour, UtcNow->tm_min, UtcNow->tm_sec, (ui32_milliseconds%1000));
      }
      // now read Latitude --> convert into double [degree]
      getDegree10Minus7FromStream( refc_stream, i32_latitudeDegree10Minus7 );
      // now read Longitude --> convert into double [degree]
      getDegree10Minus7FromStream( refc_stream, i32_longitudeDegree10Minus7 );
      // now read Altitude --> convert into double [meter]
      getAltitude10Minus2FromStream( refc_stream, ui32_altitudeCm );
      // now fetch Quality - gps-mode
      refc_stream >> ui8_tempValue;
      if ( ( ui8_tempValue >> 4  ) <= IsoAgLib::IsoGnssMethodMAX ) t_gnssMethod = IsoAgLib::IsoGnssMethod_t(ui8_tempValue >> 4 );
      if ( ( ui8_tempValue & 0xF ) <= IsoAgLib::IsoGnssTypeMAX   ) t_gnssType   = IsoAgLib::IsoGnssType_t(ui8_tempValue & 0xF );
      // GNSS Integrity
      refc_stream >> ui8_integrity;
      ui8_integrity &= 0x3; // mask reserved bits out
      // now fetch the number of satelites
      refc_stream >> ui8_satelliteCnt;
      // now fetch HDOP from raw uint16_t to float [1.0*1.0e-2)
      IsoAgLib::convertIstream( refc_stream, i16_hdop );
      // now fetch PDOP from raw uint16_t to float [1.0*1.0e-2)
      IsoAgLib::convertIstream( refc_stream, i16_pdop );
      // Geodial Separation
      IsoAgLib::convertIstream( refc_stream, i32_geoidalSeparation );
      // number of reference stations
      IsoAgLib::convertIstream( refc_stream, ui8_noRefStations );
      // now read the type and age of all following reference stations
      for ( unsigned int ind = 0; ((ind < ui8_noRefStations) &&(!refc_stream.eof())); ind++ )
      { // push new items at the end or simply update the corresponding value
        if ( vec_refStationTypeAndStation.size() < (ind+1) ) vec_refStationTypeAndStation.push_back( __IsoAgLib::convertIstreamUi16( refc_stream ) );
        else IsoAgLib::convertIstream( refc_stream, vec_refStationTypeAndStation[ind] );
        if ( vec_refStationDifferentialAge10Msec.size() < (ind+1) ) vec_refStationDifferentialAge10Msec.push_back( __IsoAgLib::convertIstreamUi16( refc_stream ) );
        else IsoAgLib::convertIstream( refc_stream, vec_refStationDifferentialAge10Msec[ind] );
      }
    }
    break;
    case NMEA_GPS_DIRECTION_DATA_PGN: // 0x01FA06LU - 130577 with Heading and Speed
      i32_lastIsoDirectionStream = data().time();
      IsoAgLib::convertIstream( refc_stream, ui8_dataModeAndHeadingReference );
      ui8_dataModeAndHeadingReference &= 0x3F;
      IsoAgLib::convertIstream( refc_stream, ui8_directionSequenceID );
      IsoAgLib::convertIstream( refc_stream, ui16_courseOverGroundRad10Minus4 );
      IsoAgLib::convertIstream( refc_stream, ui16_speedOverGroundCmSec );
      IsoAgLib::convertIstream( refc_stream, ui16_headingRad10Minus4 );
      IsoAgLib::convertIstream( refc_stream, ui16_speedCmSec );
      IsoAgLib::convertIstream( refc_stream, ui16_flowDirectionRad10Minus4 );
      IsoAgLib::convertIstream( refc_stream, ui16_driftSpeedCmSec );
      break;
  }

  return false;
} // -X2C

// END NMEA_2000_FAST_PACKET
#endif

/** send a ISO11783 base information PGN.
 * this is only called when sending ident is configured and it has already claimed an address
 */
bool Base_c::isoTimeEvent( void )
{
  const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

  // retreive the actual dynamic sender no of the member with the registered devKey
  uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberDevKey(*pc_devKey, true).nr();
  data().setIdentType(Ident_c::ExtendedIdent);
  data().setIsoPri(3);
  data().setIsoSa(b_sa);

  if ( ( ( ci32_now - i32_lastBase1 ) >= 100                   )
    && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup1 ) != 0 ) )
  { // it's time to send Base1
    isoSendBase1Update();
  }
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  if ( ( ( ci32_now - i32_lastBase2 ) >= 100                   )
    && ( ( t_mySendSelection & IsoAgLib::BaseDataGroup2 ) != 0 ) )
  { // it's time to send Base2
    isoSendBase2Update();
  }
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

  if ( ( ( ci32_now - i32_lastCalendar) >= 1000                  )
    && ( ( t_mySendSelection & IsoAgLib::BaseDataCalendar ) != 0 ) )
  { // send actual calendar data
    c_sendCalendarDevKey = *pc_devKey;
    isoSendCalendar(*pc_devKey);
  }

  if ( ( t_mySendSelection & IsoAgLib::BaseDataGps ) != 0 )
  {
    if ( ( ci32_now - i32_lastIsoPositionSimple ) >= 100 )
    {
      if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
      isoSendPositionRapidUpdate();
    }

    #ifdef NMEA_2000_FAST_PACKET
    if ( ( ( ci32_now - i32_lastIsoPositionStream ) >= 1000 )
      && ( t_multiSendSuccessState != MultiSend_c::Running  ) )
    {
      if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
      isoSendPositionStream();
    }

    if ( ( ( ci32_now - i32_lastIsoDirectionStream ) >= 1000 )
      && ( t_multiSendSuccessState != MultiSend_c::Running   ) )
    {
      if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
      isoSendDirectionStream();
    }
    #endif
  }
  return true;
}

/** send Base1 Group data with ground&theor speed&dist */
void Base_c::isoSendBase1Update( void )
{ // send actual base1 data: ground/wheel based speed/dist
  CANIO_c& c_can = getCanInstance4Comm();
  c_sendBase1DevKey = *pc_devKey;
  data().setIsoPgn(GROUND_BASED_SPEED_DIST_PGN);
#ifdef SYSTEM_PC_VC
  data().setVal12(abs(i16_speedReal));
#else
  data().setVal12(CNAMESPACE::abs(i16_speedReal));
#endif
  data().setVal36(i32_distReal);
  switch (i16_speedReal) {
    case ERROR_VAL_16S:
      data().setVal8(IsoAgLib::IsoError);
      break;
    case NO_VAL_16S:
      data().setVal8(IsoAgLib::IsoNotAvailable);
      break;
    default:
      if (i16_speedReal < 0) data().setVal8(0);
      else data().setVal8(1);
      break;
  }
    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
  c_can << data();

  data().setIsoPgn(WHEEL_BASED_SPEED_DIST_PGN);
#ifdef SYSTEM_PC_VC
  data().setVal12(abs(i16_speedTheor));
#else
  data().setVal12(CNAMESPACE::abs(i16_speedTheor));
#endif
  data().setVal36(i32_distTheor);

  data().setVal7(ui8_maxPowerTime);

  uint8_t b_val8 = IsoAgLib::IsoInactive;
  switch (i16_speedTheor) {
    case ERROR_VAL_16S:
      b_val8 |= IsoAgLib::IsoError;
      break;
    case NO_VAL_16S:
      b_val8 |= IsoAgLib::IsoNotAvailable;
      break;
    default:
      if (i16_speedTheor >= 0) b_val8 |= IsoAgLib::IsoActive;
      break;
  }
  b_val8 |= (t_keySwitch << 2);
  data().setVal8(b_val8);
  // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
  // then it sends the data
  c_can << data();


  // update time
  i32_lastBase1 = Scheduler_c::getLastTimeEventTrigger();
}

/** send Base2 Group data with hitch and PTO state */
void Base_c::isoSendBase2Update( void )
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

  data().setIsoPgn(FRONT_PTO_STATE_PGN);
  data().setVal12(ptoFront());
  data().setVal12(NO_VAL_16);

  uint8_t ui8_val5 = (t_frontPtoEngaged << 6);
  ui8_val5 |= (t_frontPto1000 << 4);
  ui8_val5 |= (t_frontPtoEconomy << 2);
  data().setVal5(ui8_val5);
  // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
  // then it sends the data
  c_can << data();

  data().setIsoPgn(BACK_PTO_STATE_PGN);
  data().setVal12(ptoRear());
  data().setVal12(NO_VAL_16);

  ui8_val5 = (t_frontPtoEngaged << 6);
  ui8_val5 |= (t_frontPto1000 << 4);
  ui8_val5 |= (t_frontPtoEconomy << 2);
  data().setVal5(ui8_val5);
  // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
  // then it sends the data
  c_can << data();

  // update time
  i32_lastBase2 = Scheduler_c::getLastTimeEventTrigger();
}


/** send position rapid update message */
void Base_c::isoSendPositionRapidUpdate( void )
{
  data().setIsoPgn(NMEA_GPS_POSITON_RAPID_UPDATE_PGN);
  data().setInt32Data(0, i32_latitudeDegree10Minus7 );
  data().setInt32Data(4, i32_longitudeDegree10Minus7);

  // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
  // then it sends the data
  getCanInstance4Comm() << data();

  // update time
  i32_lastIsoPositionSimple = Scheduler_c::getLastTimeEventTrigger();
}




#if defined(NMEA_2000_FAST_PACKET)
void setDegree10Minus7ToStream( const int32_t& refi32_src, std::vector<uint8_t>& writeRef )
{
#if SIZEOF_INT == 4
  // use 64 bit variable
  const double d_temp = double(refi32_src) * 1.0e+9;
  int64_t i64_temp = int64_t(d_temp);
  number2LittleEndianString( i64_temp, writeRef );
#else
  // only take higher 4 bytes ( write the lower four byte with 0xFF in case the value is negative )
  int32_t i32_temp = (refi32_src >= 0)?0:-1;
  number2LittleEndianString( i32_temp, writeRef );

  i32_temp = int32_t(double(refi32_src) * 1.0e+9 / 4294967296.0);
  number2LittleEndianString( i32_temp, writeRef );
#endif
}

void setAltitude10Minus2ToStream( const uint32_t& refui32_result, std::vector<uint8_t>& writeRef )
{
#if SIZEOF_INT == 4
  // use 64 bit variable
  // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
  const double d_temp = double(refui32_result) * 1.0e+4;
  const int64_t i64_temp = int64_t(d_temp);
  number2LittleEndianString( i64_temp, writeRef );
#else
  // only take higher 4 bytes
  int32_t i32_temp = 0;
  number2LittleEndianString( i32_temp, writeRef );

  // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
  i32_temp = int32_t(double(refi32_src) * 1.0e+4 / 4294967296.0);
  number2LittleEndianString( i32_temp, writeRef );
#endif
}


/** send position as detailed stream */
void Base_c::isoSendPositionStream( void )
{
  const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
  ISOMonitor_c& c_iso_monitor = getIsoMonitorInstance4Comm();
  // set data in Nmea2000SendStreamer_c
  c_nmea2000Streamer.reset();
  std::vector<uint8_t>& writeRef = c_nmea2000Streamer.vec_data;
  // use helper function to transfer value to the byte vector
  number2LittleEndianString( ui8_positionSequenceID, writeRef );


  struct tm testTime = { second(), minuteUtc(), hourUtc(), dayUtc(),(monthUtc()-1),(yearUtc()-1900),0,0,0 };
  const time_t secondsSince1970 = mktime( &testTime );
  // calculate the days
  const uint16_t ui16_daysSince1970 = secondsSince1970 / ( 60 * 60 *24 );
  const uint32_t ui32_milliseconds = ( ( ( ( ( ( hourUtc() * 60 ) + minuteUtc() ) * 60 ) + second() ) * 1000 ) + millisecond()) * 10;
  // write Position Date as Days since 1.1.1970
  number2LittleEndianString( ui16_daysSince1970, writeRef );
  // write Position Time as Milliseconds*10 per day
  number2LittleEndianString( ui32_milliseconds, writeRef );
  // write Latitude as uint64_t value
  setDegree10Minus7ToStream( i32_latitudeDegree10Minus7, writeRef );
  // write Longitude as uint64_t value
  setDegree10Minus7ToStream( i32_longitudeDegree10Minus7, writeRef );
  // write Altitude as uint64_t value
  setAltitude10Minus2ToStream( ui32_altitudeCm, writeRef );

  // write type and method
  const uint8_t cu8_tempTypeMethod = ( t_gnssType | ( t_gnssMethod << 4 ) );
  number2LittleEndianString( cu8_tempTypeMethod, writeRef );
  // write integrity ( set unused highest 6 bits to "1" )
  number2LittleEndianString( uint8_t(ui8_integrity| 0xFC), writeRef );
  // write #satelites
  number2LittleEndianString( ui8_satelliteCnt, writeRef );
  // write HDOP
  number2LittleEndianString( i16_hdop, writeRef );
  // write PDOP
  number2LittleEndianString( i16_pdop, writeRef );
  // write geodial separation
  number2LittleEndianString( i32_geoidalSeparation, writeRef );

  // write number of reference stations
  unsigned int limit = ui8_noRefStations;
  if ( vec_refStationTypeAndStation.size() < limit ) limit = vec_refStationTypeAndStation.size();
  if ( vec_refStationDifferentialAge10Msec.size() < limit ) limit = vec_refStationDifferentialAge10Msec.size();

  number2LittleEndianString( limit, writeRef );

  for ( unsigned int ind = 0; ind < limit; ind++ )
  {
    number2LittleEndianString( vec_refStationTypeAndStation[ind], writeRef );
    number2LittleEndianString( vec_refStationDifferentialAge10Msec[ind], writeRef );
  }

  //now trigger sending
  // retreive the actual dynamic sender no of the member with the registered devKey
  const uint8_t b_send = c_iso_monitor.isoMemberDevKey(c_sendGpsDevKey, true).nr();
  if ( getMultiSendInstance4Comm().sendIsoFastPacket(b_send, 0xFF, &c_nmea2000Streamer, NMEA_GPS_POSITON_DATA_PGN, t_multiSendSuccessState) )
  { // update time
    i32_lastIsoPositionStream = ci32_now;
    ui8_positionSequenceID++;
  }
}
/** send direction as detailed stream */
void Base_c::isoSendDirectionStream( void )
{
  const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
  ISOMonitor_c& c_iso_monitor = getIsoMonitorInstance4Comm();
  // set data in Nmea2000SendStreamer_c
  c_nmea2000Streamer.reset();
  std::vector<uint8_t>& writeRef = c_nmea2000Streamer.vec_data;
  // use helper function to transfer value to the byte vector
  number2LittleEndianString( uint8_t (ui8_dataModeAndHeadingReference|0xC0), writeRef );
  number2LittleEndianString( ui8_directionSequenceID, writeRef );
  number2LittleEndianString( ui16_courseOverGroundRad10Minus4, writeRef );
  number2LittleEndianString( ui16_speedOverGroundCmSec, writeRef );

  number2LittleEndianString( ui16_headingRad10Minus4, writeRef );
  number2LittleEndianString( ui16_speedCmSec, writeRef );

  number2LittleEndianString( ui16_flowDirectionRad10Minus4, writeRef );
  number2LittleEndianString( ui16_driftSpeedCmSec, writeRef );

  //now trigger sending
  // retreive the actual dynamic sender no of the member with the registered devKey
  const uint8_t b_send = c_iso_monitor.isoMemberDevKey(c_sendGpsDevKey, true).nr();
  if ( getMultiSendInstance4Comm().sendIsoFastPacket(b_send, 0xFF, &c_nmea2000Streamer, NMEA_GPS_DIRECTION_DATA_PGN, t_multiSendSuccessState) )
  { // update time
    i32_lastIsoDirectionStream = ci32_now;
    ui8_directionSequenceID++;
  }
}
// END OF FAST PACKET IN ISO11783
#endif

/**
  send ISO11783 calendar PGN
  @param rc_devKey DEV_KEY code off calling item which wants to send
  @param ri32_time timestamp where calendar was last sent (default autodetect)

  possible errors:
      * dependant error in CANIO_c on CAN send problems
  @see CANPkg_c::getData
  @see CANPkgExt_c::getData
  @see CANIO_c::operator<<
*/
void Base_c::isoSendCalendar(const DevKey_c& rc_devKey)
{
  if ( ( ( t_mySendSelection & IsoAgLib::BaseDataCalendar ) != 0 )
    && ( c_sendCalendarDevKey == rc_devKey                   ) )
  { // this item (identified by DEV_KEY is configured to send
    data().setIsoPgn(TIME_DATE_PGN);
    data().setVal1(second() * 4);
    data().setVal2(minuteUtc());
    data().setVal3(hourUtc());
    data().setVal4(monthUtc());
    data().setVal5(dayUtc() * 4);
    data().setVal6(yearUtc() - 1985);
    data().setVal7( bit_calendar.timezoneMinuteOffset );
    data().setVal8( bit_calendar.timezoneHourOffsetMinus24 );

    // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();

    // update time
    i32_lastCalendar = Scheduler_c::getLastTimeEventTrigger()/100;
  }
}
/** force maintain power from tractor
  * @param rb_ecuPower true -> maintain ECU power
  * @param rb_actuatorPower -> maintain actuator power
  * @param rt_implTransport true -> implement is in transport state
  * @param rt_implPark true -> implement is in park state
  * @param rt_implWork true -> implement is in work state
  */
void Base_c::forceMaintainPower( bool rb_ecuPower, bool rb_actuatorPower, IsoAgLib::IsoActiveFlag_t rt_implTransport,
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




/**
  update distance values ; react on int16_t overflow
  @param reflVal to be updated value as int32_t variable
  @param refiVal to be updated value as 16bit int16_t variable
  @param rrefiNewVal new value given as reference to 16bit int
*/
void Base_c::setOverflowSecure(int32_t& reflVal, int16_t& refiVal, const int16_t& rrefiNewVal)
{
  // definition area [0..i32_maxVal] with overflow
  // 10000 of Fendt; 32767 of DIN
  const int32_t i32_maxDefFendt = 10000,
                i32_maxDefDin = 32767;
  int32_t i32_newValFendt = reflVal,
          i32_newValDin = reflVal;
  int16_t i16_diff = rrefiNewVal - refiVal;

  // check if there was an overflow = diff is greater than half of def area (per sign side)
#ifdef SYSTEM_PC_VC
  if ((abs(i16_diff) > i32_maxDefFendt/2) || (abs(i16_diff) > i32_maxDefDin/2))
  { // one of the overflow checks triggers
    if (abs(i16_diff) > i32_maxDefFendt/2)
#else
  if ((CNAMESPACE::abs(i16_diff) > i32_maxDefFendt/2) || (CNAMESPACE::abs(i16_diff) > i32_maxDefDin/2))
  { // one of the overflow checks triggers
    if (CNAMESPACE::abs(i16_diff) > i32_maxDefFendt/2)
#endif
    { // the old wrong fendt limit triggers
      if (rrefiNewVal > refiVal)
      { // dist decreased lower than 0 -> lower underflow
        i32_newValFendt -= refiVal; // max reducable before underflow
        i32_newValFendt -= (i32_maxDefFendt - rrefiNewVal); // decreased after underflow (jump to int16 max pos val)
      }
      else
      { // dist increased upper than 0x7FFF -> overflow
        i32_newValFendt += (i32_maxDefFendt - refiVal); // max decrease before overflow
        i32_newValFendt += rrefiNewVal; // decreased after overflow
      }
    }
#ifdef SYSTEM_PC_VC
    if (abs(i16_diff) > i32_maxDefDin/2)
#else
    if (CNAMESPACE::abs(i16_diff) > i32_maxDefDin/2)
#endif
  { // the correct DIN limit triggers
      if (rrefiNewVal > refiVal)
      { // dist decreased lower than 0 -> lower underflow
        i32_newValDin -= refiVal; // max reducable before underflow
        i32_newValDin -= (i32_maxDefDin - rrefiNewVal); // decreased after underflow (jump to int16 max pos val)
      }
      else
      { // dist increased upper than 0x7FFF -> overflow
        i32_newValDin += (i32_maxDefDin - refiVal); // max decrease before overflow
        i32_newValDin += rrefiNewVal; // decreased after overflow
      }
    }
    // check, which new value should be used -> take the value which mean less distance to old val
    if (CNAMESPACE::labs(i32_newValFendt - reflVal) < CNAMESPACE::labs(i32_newValDin - reflVal))
    {
      reflVal = i32_newValFendt;
    }
    else
    {
      reflVal = i32_newValDin;
    }
  }
  else
  { // there's no overflow -> easy increment reflVal
    reflVal += i16_diff;
  }
  // store the given int16 val as last in16 val in refiVal
  refiVal = rrefiNewVal;
};

/**
  deliver the devKey of the sender of the base data

  possible errors:
      * Err_c::range rui8_typeNr doesn't match valid base msg type number
  @param rt_typeGrp base msg type no of interest: BaseDataGroup1 | BaseDataGroup2 | BaseDataCalendar
  @return DEV_KEY code of member who is sending the intereested base msg type
*/
const DevKey_c& Base_c::senderDevKey(IsoAgLib::BaseDataGroup_t rt_typeGrp) {
  // simply answer first matching result if more than one type is selected
  if ( ( rt_typeGrp & IsoAgLib::BaseDataGroup1   ) != 0 ) return c_sendBase1DevKey;
  if ( ( rt_typeGrp & IsoAgLib::BaseDataGroup2   ) != 0 ) return c_sendBase2DevKey;
  if ( ( rt_typeGrp & IsoAgLib::BaseDataGroup3   ) != 0 ) return c_sendBase3DevKey;
  if ( ( rt_typeGrp & IsoAgLib::BaseDataFuel     ) != 0 ) return c_sendFuelDevKey;
  if ( ( rt_typeGrp & IsoAgLib::BaseDataCalendar ) != 0 ) return c_sendCalendarDevKey;
  #if defined(USE_ISO_11783) && defined(NMEA_2000_FAST_PACKET)
  if ( ( rt_typeGrp & IsoAgLib::BaseDataGps      ) != 0 ) return c_sendGpsDevKey;
  #endif
  else return DevKey_c::DevKeyUnspecified;
}

/**
  translate BCD to normal value
*/
uint8_t bcd2dec(uint8_t rb_bcd)
{
  return ((rb_bcd >> 4) * 10) + (rb_bcd & 0xF);
}
/**
  translate normal value to BCD
*/
uint8_t dec2bcd(uint8_t rb_dec)
{
  const uint8_t ui8_v10 = rb_dec / 10;
  const uint8_t ui8_v0  = rb_dec % 10;
  const uint8_t ui8_result = ( ui8_v10 << 4 ) + ui8_v0;
  return ui8_result;
}

/**
  set the real (radar measured) driven distance with int32_t val
  @param rreflVal value to store as real radar measured distance
*/
void Base_c::setDistReal(const int32_t& rreflVal)
{
  i32_distReal = rreflVal;
  // set the int16_t value regarding the overflow counting
  i16_lastDistReal = long2int(rreflVal);
};

/**
  set the theoretical (gear calculated) driven distance with int32_t val
  @param rreflVal value to store as theoretical (gear calculated) driven distance
*/
void Base_c::setDistTheor(const int32_t& rreflVal)
{
  i32_distTheor = rreflVal;
  // set the int16_t value regarding the overflow counting
  i16_lastDistTheor = long2int(rreflVal);
};



/** check if a calendar information was received since init */
bool Base_c::isCalendarReceived() const
{ // check if default data from init is still in vars
  return ( ( bit_calendar.day == 1 ) && ( bit_calendar.month == 1 ) && ( bit_calendar.year == 0 )
      && ( bit_calendar.second == 0 ) && ( bit_calendar.minute == 0 ) && ( bit_calendar.hour == 0 ) )?false:true;
}


/**
  set the calendar value
  @param ri16_year value to store as year
  @param rb_month value to store as month
  @param rb_day value to store as day
  @param rb_hour value to store as hour
  @param rb_minute value to store as minute
  @param rb_second value to store as second
*/
void Base_c::setCalendarUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec)
{
  i32_lastCalendarSet = System_c::getTime();

  bit_calendar.year   = ri16_year;
  bit_calendar.month  = rb_month;
  bit_calendar.day    = rb_day;
  bit_calendar.hour   = rb_hour;
  bit_calendar.minute = rb_minute;
  bit_calendar.second = rb_second;
  bit_calendar.msec   = rui16_msec;
};
/**
  set the calendar value as local time ( take local time offsets into account )
  @param ri16_year value to store as year
  @param rb_month value to store as month
  @param rb_day value to store as day
  @param rb_hour value to store as hour
  @param rb_minute value to store as minute
  @param rb_second value to store as second
  */
void Base_c::setCalendarLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec )
{
  i32_lastCalendarSet = System_c::getTime();

  struct tm testTime = { rb_second, int(rb_minute)-int(bit_calendar.timezoneMinuteOffset), (int(rb_hour)-(int(bit_calendar.timezoneHourOffsetMinus24)-24)),
                         rb_day,(rb_month-1),(ri16_year-1900),0,0,0 };

  const time_t middle = mktime( &testTime );
  const struct tm* normalizedTime = localtime( &middle );

  bit_calendar.year   = normalizedTime->tm_year+1900;
  bit_calendar.month  = (normalizedTime->tm_mon+1);
  bit_calendar.day    = normalizedTime->tm_mday;
  bit_calendar.hour   = normalizedTime->tm_hour;
  bit_calendar.minute = normalizedTime->tm_min;
  bit_calendar.second = normalizedTime->tm_sec;
  bit_calendar.msec   = rui16_msec;
};

/** set the date in local timezone */
void Base_c::setDateLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day)
{
  i32_lastCalendarSet = System_c::getTime();

  struct tm testTime = { bit_calendar.second, bit_calendar.minute, bit_calendar.hour,
    rb_day,(rb_month-1),(ri16_year-1900),0,0,0 };

  const time_t middle = mktime( &testTime );
  const struct tm* normalizedTime = localtime( &middle );

  bit_calendar.year   = normalizedTime->tm_year+1900;
  bit_calendar.month  = (normalizedTime->tm_mon+1);
  bit_calendar.day    = normalizedTime->tm_mday;
}

/** set the date in UTC timezone */
void Base_c::setDateUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day)
{
  i32_lastCalendarSet = System_c::getTime();
  bit_calendar.year   = ri16_year;
  bit_calendar.month  = rb_month;
  bit_calendar.day    = rb_day;
}

/** set the time in local timezone */
void Base_c::setTimeLocal(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec)
{
  i32_lastCalendarSet = System_c::getTime();

  struct tm testTime = { rb_second, int(rb_minute)-int(bit_calendar.timezoneMinuteOffset), (int(rb_hour)-(int(bit_calendar.timezoneHourOffsetMinus24)-24)),
    bit_calendar.day,(bit_calendar.month-1),(bit_calendar.year-1900),0,0,0 };

  const time_t middle = mktime( &testTime );
  const struct tm* normalizedTime = localtime( &middle );

  bit_calendar.hour   = normalizedTime->tm_hour;
  bit_calendar.minute = normalizedTime->tm_min;
  bit_calendar.second = normalizedTime->tm_sec;
  bit_calendar.msec   = rui16_msec;
}
/** set the time in UTC timezone */
void Base_c::setTimeUtc(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec)
{
  i32_lastCalendarSet = System_c::getTime();
  bit_calendar.hour   = rb_hour;
  bit_calendar.minute = rb_minute;
  bit_calendar.second = rb_second;
  bit_calendar.msec   = rui16_msec;
}


const struct tm* Base_c::Utc2LocalTime() const
{
  struct tm testTime = {
    bit_calendar.second + ((calendarSetAge() / 1000)%60),
    bit_calendar.minute+bit_calendar.timezoneMinuteOffset+ ((calendarSetAge() / 60000)%60),
    (int(bit_calendar.hour)+int(bit_calendar.timezoneHourOffsetMinus24)-24) + ((calendarSetAge() / 3600000)%24),
    bit_calendar.day + (calendarSetAge() / 86400000),
    (bit_calendar.month-1),
    (bit_calendar.year-1900),
    0,0,0 };

  const time_t middle = mktime( &testTime );
  return localtime( &middle );
}

/**
  get the calendar year value
  @return actual calendar year value
  */
int16_t Base_c::yearLocal() const
{
  return Utc2LocalTime()->tm_year+1900;
}

/**
  get the calendar month value
  @return actual calendar month value
  */
uint8_t Base_c::monthLocal() const
{ // month is delivered with range [0..11] -> add 1
  return Utc2LocalTime()->tm_mon+1;
}

/**
  get the calendar day value
  @return actual calendar day value
  */
uint8_t Base_c::dayLocal() const
{
  return Utc2LocalTime()->tm_mday;
}

/**
  get the calendar hour value
  @return actual calendar hour value
  */
uint8_t Base_c::hourLocal() const
{
  return Utc2LocalTime()->tm_hour;
}

/**
  get the calendar minute value
  @return actual calendar minute value
  */
uint8_t Base_c::minuteLocal() const
{
  return Utc2LocalTime()->tm_min;
};




/**
  get int16_t overflowed val from long
  @param rreflVal value as int32_t (32bit) variable
  @return 16bit int16_t calculated with counting overflow from 32767 to (-32766)
*/
int16_t Base_c::long2int(const int32_t& rreflVal)
{
  // Version mit [0..10000] und Uberlauf a la Fendt Vario
  //int32_t i32_mod =  rreflVal % 10000;
  // Version mit [0..32767] und Uberlauf a la Scheduler_c DIN 9684
  int32_t i32_mod =  rreflVal % 32767;

  return i32_mod;
};
} // end of namespace __IsoAgLib
