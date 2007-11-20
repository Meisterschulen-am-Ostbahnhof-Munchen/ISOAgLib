/***************************************************************************
             timeposgps_c.cpp - working on Base Data Msg Type NMEA 2000 GPS
                                and Calendar; stores, updates  and
                                delivers all base data informations
                                from CanCustomer_c derived for CAN
                                sending and receiving interaction;
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
#include "timeposgps_c.h"

#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
// IsoAgLib_Extension
#include <IsoAgLib/comm/Multipacket/imultireceive_c.h>
#include <IsoAgLib/comm/Multipacket/istream_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h>
#include <IsoAgLib/util/iutil_funcs.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/comm/Base/itracmove_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgn_c.h>

#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/irs232io_c.h>
  #endif
#endif



using namespace std;

#ifdef ENABLE_NMEA_2000_MULTI_PACKET
// Off-class/namespace c-style helper functions
void getDegree10Minus7FromStream( IsoAgLib::iStream_c& rc_stream, int32_t& ri32_result )
{
  #if SIZEOF_INT == 4
  // use 64 bit variable
  int64_t i64_temp;
  IsoAgLib::convertIstream( rc_stream, i64_temp );
  double d_temp = double(i64_temp);
  ri32_result = int32_t( d_temp * 1.0e-9 );
  #else
  // only take higher 4 bytes
  int32_t i32_temp;

  // ignore the result of the following call
  IsoAgLib::convertIstream( rc_stream, i32_temp );
  // only take this part
  IsoAgLib::convertIstream( rc_stream, i32_temp );
  // NMEA sends with 1.0e-16, while normally 1.0e-7 is enough -> mult with 1.0e-9
  double d_temp = ( double(i32_temp) * 4294967296.0 * 1.0e-9 );
  ri32_result = int32_t( d_temp );
  #endif
}

void getAltitude10Minus2FromStream( IsoAgLib::iStream_c& rc_stream, int32_t& ri32_result )
{
  #if SIZEOF_INT == 4
  // use 64 bit variable
  int64_t i64_temp;
  IsoAgLib::convertIstream( rc_stream, i64_temp );
  double d_temp = double(i64_temp);
  // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
  ri32_result = int32_t( d_temp * 1.0e-4 );
  #else
  // only take higher 4 bytes
  int32_t i32_temp;

  // ignore the result of the following call
  IsoAgLib::convertIstream( rc_stream, i32_temp );
  // only take this part
  IsoAgLib::convertIstream( rc_stream, i32_temp );
  // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
  double d_temp = ( double(i32_temp) * 4294967296.0 * 1.0e-4 );
  ri32_result = int32_t( d_temp );
  #endif
}
#endif // END of ENABLE_NMEA_2000_MULTI_PACKET

namespace __IsoAgLib {

  #if defined(ENABLE_NMEA_2000_MULTI_PACKET)
  /** place next data to send direct into send buffer of pointed
    stream send package - MultiSend_c will send this
    buffer afterwards
  */
  void Nmea2000SendStreamer_c::setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes)
  {
    mspData->setDataPart (vec_data, ui16_currentSendPosition, bytes);
    ui16_currentSendPosition += bytes;
  }

#ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards
  */
  void Nmea2000SendStreamer_c::setDataNextFastPacketStreamPart (MultiSendPkg_c* mspData, uint8_t bytes, uint8_t aui8_offset )
  {
    mspData->setFastPacketDataPart (vec_data, ui16_currentSendPosition, bytes, aui8_offset );
    ui16_currentSendPosition += bytes;
  }
#endif

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
  #endif // END of ENABLE_NMEA_2000_MULTI_PACKET


  /** HIDDEN constructor for a TimePosGPS_c object instance which can optional
  set the configuration for send/receive for base msg type NMEA 2000 GPS
  and calendar
  NEVER instantiate a variable of type TimePosGPS_c within application
  only access TimePosGPS_c via getTimePosGpsInstance() or getTimePosGpsInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
   */
  TimePosGPS_c::TimePosGPS_c()
  : mc_sendGpsISOName(),
  mpc_isoNameGps(NULL),
  mt_identModeGps( IsoAgLib::IdentModeImplement )
  {}


  /** functions with actions, which must be performed periodically
      -> called periodically by Scheduler_c
      ==> sends base msg if configured in the needed rates
      possible errors:
          * dependant error in CanIo_c on CAN send problems
      @see CanPkg_c::getData
      @see CanPkgExt_c::getData
      @see CanIo_c::operator<<
      @return true -> all planned activities performed in allowed time
    */
  bool TimePosGPS_c::timeEvent(  )
  {
    checkCreateReceiveFilter();

    const int32_t ci32_now = getLastRetriggerTime();

    // check for different base data types whether the previously
    if ( ( checkMode(IsoAgLib::IdentModeImplement)      )
      && ( getSelectedDataSourceISOName().isSpecified() )
      && ( (lastedTimeSinceUpdate() >= TIMEOUT_SENDING_NODE) || (yearUtc() == 0) ) // yearUtc means ERROR and NOT year0/1900/1970/whatever...
       )
    { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
      getSelectedDataSourceISOName().setUnspecified();

      /// Set date to NO_VAL, i.e. 01.01.0000
      bit_calendar.year = 0;
      bit_calendar.month = 1;
      bit_calendar.day = 1;
      /// There's no NO_VAL for time yet, so set it to 00:00:00
      bit_calendar.hour = 0;
      bit_calendar.minute = 0;
      bit_calendar.second = 0;
      // @todo maybe later also reset those values here, too...
      //bit_calendar.msec = 0;
      //bit_calendar.timezoneMinuteOffset = 0;
      //bit_calendar.timezoneHourOffsetMinus24 = 24;
    }

    if ( ( checkModeGps(IsoAgLib::IdentModeImplement) )
      && ( mc_sendGpsISOName.isSpecified() ) )
    { // we are receiving GPS data -> check whether the old sender is still active
      // and we were already receiving sometimes in past GPS data
      bool b_noPosition = false;
      if (
          ( (ci32_now - mi32_lastIsoPositionSimple) >= TIMEOUT_SENDING_NODE )
          #ifdef ENABLE_NMEA_2000_MULTI_PACKET
          &&
          ( (ci32_now - mi32_lastIsoPositionStream) >= TIMEOUT_SENDING_NODE )
          #endif
        )
      { // the previously sending node didn't send POSITION information for 3 seconds -> give other items a chance
        mi32_latitudeDegree10Minus7 = mi32_longitudeDegree10Minus7 = 0x7FFFFFFF;
        mt_gnssMethod = IsoAgLib::IsoNoGps;
        #ifdef ENABLE_NMEA_2000_MULTI_PACKET
        mt_gnssType = IsoAgLib::IsoGnssGps;
        mui8_satelliteCnt = 0;
        mi32_altitudeCm = 0;
        #endif
        b_noPosition = true;
      }
      if ( (ci32_now - mi32_lastIsoDirection) >= TIMEOUT_SENDING_NODE )
      { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
        mui16_speedOverGroundCmSec = mui16_courseOverGroundRad10Minus4 = 0xFFFF;

        if (b_noPosition)
        { // neither Pos nor Dir are specified, so kick the sender!
          mc_sendGpsISOName.setUnspecified();
        }
        /** @todo Maybe make it 2 GpsIsoNames: One for Position and one for Direction!
        * Then we don't have to wait for both to be silent in order to kick the mc_sendGpsISOName.
        * Naming:       Gps for Position
        *         Direction for Direction */
      }
    }


    if ( ( ( getISOName() != NULL )  && (getIsoMonitorInstance4Comm().existIsoMemberISOName(*getISOName(), true))  && ( checkMode(IsoAgLib::IdentModeTractor) ) )
      || ( ( mpc_isoNameGps != NULL ) && (getIsoMonitorInstance4Comm().existIsoMemberISOName(*mpc_isoNameGps, true)) && (checkModeGps(IsoAgLib::IdentModeTractor) ) ) )
    { // there is at least something configured for send where the time sending or GPS sending is activated
      return timeEventTracMode();
    }
    return true;
  }

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TimePosGPS_c::checkCreateReceiveFilter( )
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CanIo_c &c_can = getCanInstance4Comm();

    if ( ( ! checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();

      c_can.insertStandardIsoFilter(*this,TIME_DATE_PGN,false);
      c_can.insertStandardIsoFilter(*this,NMEA_GPS_POSITION_RAPID_UPDATE_PGN,false);
      c_can.insertStandardIsoFilter(*this,NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN,true);

    }
  }

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param apc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TimePosGPS_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    BaseCommon_c::init_base( apc_isoName, getSingletonVecKey(), at_identMode );

    mpc_isoNameGps = NULL;
    // set the GPS mode always to non-sending
    configGps( NULL, IsoAgLib::IdentModeImplement );

    // 01.01.1970 00:00:00
    struct tm t_testTime = {0, 0, 0, 1, 0, 70, 0 , 0 ,-1
                            #if defined(__USE_BSD) || defined(__GNU_LIBRARY__) || defined(__GLIBC__) || defined(__GLIBC_MINOR__)
                            , 0, NULL
                            #endif
                          };

    mt_tzOffset = - mktime(&t_testTime);
    if (1 == mt_tzOffset)
    { // mktime returned -1 => error
      mt_tzOffset = 0;
    }
  }

  /** config the TimePosGPS_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TimePosGPS_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    //store old mode to decide to register or unregister to request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;


    if ( checkMode( IsoAgLib::IdentModeTractor ) || checkModeGps( IsoAgLib::IdentModeTractor ) )
    { // we are in sending state for at least one type
      ///setTimePeriod for Scheduler_c 100ms is minimal periode in GPSmodul up to now
      setTimePeriod( (uint16_t) 100 );
    }
    else
    { // we are only in receiving mode for all types
      setTimePeriod( 1000 );
    }


    if (t_oldMode == IsoAgLib::IdentModeImplement && at_identMode == IsoAgLib::IdentModeTractor)
    {  // a change from Implement mode to Tractor mode occured
      getIsoRequestPgnInstance4Comm().registerPGN (*this, TIME_DATE_PGN); // request for time and date
    }

    if (t_oldMode == IsoAgLib::IdentModeTractor && at_identMode == IsoAgLib::IdentModeImplement)
    {  // a change from Tractor mode to Implement mode occured
      // unregister from request for pgn, because in implement mode no request should be answered
      getIsoRequestPgnInstance4Comm().unregisterPGN (*this, TIME_DATE_PGN);
    }

    /// Set date to NO_VAL, i.e. 01.01.0000
    bit_calendar.year = 0;
    bit_calendar.month = 1;
    bit_calendar.day = 1;
    /// There's no NO_VAL for time yet, so set it to 00:00:00
    bit_calendar.hour = 0;
    bit_calendar.minute = 0;
    bit_calendar.second = 0;
    // additionally set the other field, too...
    bit_calendar.msec = 0;
    bit_calendar.timezoneMinuteOffset = 0;
    bit_calendar.timezoneHourOffsetMinus24 = 24;

    mi32_lastCalendarSet = 0;

    mt_cachedLocalSeconds1970AtLastSet = 0;

    return true;
  };

  /** force a request for pgn for time/date information */
  bool TimePosGPS_c::sendRequestUpdateTimeDate()
  {
    if ( checkMode(IsoAgLib::IdentModeImplement) )
      return BaseCommon_c::sendPgnRequest(TIME_DATE_PGN);
    else
      return false;
  }
  /** config the Base_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param ab_implementMode implement mode (true) or tractor mode (false)!!!
      @return true -> configuration was successfull
    */
  bool TimePosGPS_c::configGps(const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t  at_identModeGps)
  {
    if (   at_identModeGps == IsoAgLib::IdentModeTractor
        && apc_isoName == NULL
      )
    { // the application is in tractor mode but has no valid isoName
      // IMPORTANT: if we are in tractor mode we MUST have a valid isoName otherwise the configuration makes no sense
      #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "CONFIG FAILURE. The config function was called with isoName == NULL and\
                                  IdentModeTractor. Is is not allowed that the isoName ist NULL in combination\
                                  with tractor mode." << "\n";
      #endif
      #if defined(DEBUG) && defined(SYSTEM_PC)
        abort();
      #endif
      getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::Base );
      return false;
    }
    //set configure values
    mi32_lastIsoPositionSimple = 0;
    mi32_lastIsoDirection = 0;
#ifdef ENABLE_NMEA_2000_MULTI_PACKET
    mi32_lastIsoPositionStream = 0;
    mt_multiSendSuccessState = MultiSend_c::SendSuccess;
    mi32_altitudeCm = 0x7FFFFFFF;

    mui8_satelliteCnt = 0;
#endif // END of ENABLE_NMEA_2000_MULTI_PACKET

    mpc_isoNameGps = apc_isoName;
    mt_identModeGps = at_identModeGps;

    mi32_latitudeDegree10Minus7 = mi32_longitudeDegree10Minus7 = 0x7FFFFFFF;
    mui16_speedOverGroundCmSec = mui16_courseOverGroundRad10Minus4 = 0xFFFF;

    if ( at_identModeGps == IsoAgLib::IdentModeTractor )
    { // GPS send from now on
      // because wer are in tractor mode the apc_isoName cannot be NULL
      mc_sendGpsISOName = *apc_isoName;
      #ifdef ENABLE_NMEA_2000_MULTI_PACKET
      // also remove any previously registered MultiReceive connections
      getMultiReceiveInstance4Comm().deregisterClient( *this );
      mc_nmea2000Streamer.reset();
      mc_nmea2000Streamer.vec_data.resize(0);
      #endif // END of ENABLE_NMEA_2000_MULTI_PACKET
    }
    else
    { // IdentModeImplement
      mc_sendGpsISOName.setUnspecified();
      // register Broadcast-TP/FP receive of NMEA 2000 data
      // make sure that the needed multi receive are registered
      #ifdef ENABLE_NMEA_2000_MULTI_PACKET
      getMultiReceiveInstance4Comm().registerClient(*this, __IsoAgLib::IsoName_c::IsoNameUnspecified(), NMEA_GPS_POSITION_DATA_PGN,  0x3FFFF00, true, false
        #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
          , false
        #endif
            );
      getMultiReceiveInstance4Comm().registerClient(*this, __IsoAgLib::IsoName_c::IsoNameUnspecified(), NMEA_GPS_DIRECTION_DATA_PGN, 0x3FFFF00, true, false
        #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
        , false
        #endif
        );

      #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
      getMultiReceiveInstance4Comm().registerClient(*this, __IsoAgLib::IsoName_c::IsoNameUnspecified(), NMEA_GPS_POSITION_DATA_PGN,  0x3FFFF00, true, false, true);
      getMultiReceiveInstance4Comm().registerClient(*this, __IsoAgLib::IsoName_c::IsoNameUnspecified(), NMEA_GPS_DIRECTION_DATA_PGN, 0x3FFFF00, true, false, true);
      #endif
      mc_nmea2000Streamer.vec_data.reserve(51); // GNSS Position Data with TWO reference stations
      #endif // END of ENABLE_NMEA_2000_MULTI_PACKET
    }

    if ( checkMode( IsoAgLib::IdentModeTractor ) || checkModeGps( IsoAgLib::IdentModeTractor ) )
    { // we are in sending state for at least one type
      ///setTimePeriod for Scheduler_c 100ms is minimal periode in GPSmodul up to now
      setTimePeriod( (uint16_t) 100 );
    }
    else
    { // we are only in receiving mode for all types
      setTimePeriod( 1000 );
    }

    return true;
  }

  /** Retrieve the last update time of the specified information type
    */
  int32_t TimePosGPS_c::lastedTimeSinceUpdateGps() const
  {
    const int32_t ci32_now = getLastRetriggerTime();
    #ifdef ENABLE_NMEA_2000_MULTI_PACKET
    if ( mi32_lastIsoPositionStream > mi32_lastIsoPositionSimple ) return ( ci32_now - mi32_lastIsoPositionStream);
    else return ( ci32_now - mi32_lastIsoPositionSimple);
    #else
    return ( ci32_now - mi32_lastIsoPositionSimple);
    #endif
  }
  /** Retrieve the time of last update */
  int32_t TimePosGPS_c::lastUpdateTimeGps() const
  {
    #ifdef ENABLE_NMEA_2000_MULTI_PACKET
    if ( mi32_lastIsoPositionStream > mi32_lastIsoPositionSimple ) return mi32_lastIsoPositionStream;
    else return mi32_lastIsoPositionSimple;
    #else
    return mi32_lastIsoPositionSimple;
    #endif
  }

  /** Retrieve the last update time of the specified information type
    */
  int32_t TimePosGPS_c::lastedTimeSinceUpdateDirection() const
  {
    const int32_t ci32_now = getLastRetriggerTime();
    return ( ci32_now - mi32_lastIsoDirection);
  }
  /** Retrieve the time of last update */
  int32_t TimePosGPS_c::lastUpdateTimeDirection() const
  {
    return mi32_lastIsoDirection;
  }

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TimePosGPS_c& getTimePosGpsInstance( uint8_t aui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TimePosGPS_c::instance( aui8_instance );
  };
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TimePosGPS_c& getTimePosGpsInstance( void )
  {
    static TimePosGPS_c& c_lbsTimePosGps = TimePosGPS_c::instance();
    return c_lbsTimePosGps;
  };
  #endif


  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TimePosGPS_c::processMsg()
  {
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CanPkgExt_c -> resolveSendingInformation
    IsoName_c const& rcc_tempISOName = data().getISONameForSA();

    const int32_t ci32_now = data().time();

    switch (data().isoPgn() /*& 0x3FFFF*/) // don't need to &, we're interested in the whole PGN.
    {
      case TIME_DATE_PGN:
        // time - date
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( rcc_tempISOName ) )
        { // sender is allowed to send
          // store new calendar setting
          if ( mc_sendGpsISOName.isUnspecified()  )
          { // neither this item nor another item is sending GPS data -> this is the best time source
            setCalendarUtc(
            (data().getUint8Data(5) + 1985), data().getUint8Data(3), (data().getUint8Data(4) / 4), (data().getUint8Data(2)),
            (data().getUint8Data(1)), (data().getUint8Data(0) / 4));
          }
          else
          { // only fetch the date, as this information might not be defined by GPS
            setDateUtc((data().getUint8Data(5) + 1985), data().getUint8Data(3), (data().getUint8Data(4) / 4));
          }
          // take local timezone offset in all cases
          bit_calendar.timezoneMinuteOffset = data().getUint8Data(6);
          bit_calendar.timezoneHourOffsetMinus24 = data().getUint8Data(7);
          // set last time
          setUpdateTime(ci32_now);
          setSelectedDataSourceISOName (rcc_tempISOName);
          return true;
        }
        else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
          return false; /** @todo why not return true here?! */
        }
        break;

      case NMEA_GPS_POSITION_RAPID_UPDATE_PGN:
        if ( checkParseReceived( rcc_tempISOName ) )
        { // sender is allowed to send
          mi32_latitudeDegree10Minus7  = data().getInt32Data( 0 );
          mi32_longitudeDegree10Minus7 = data().getInt32Data( 4 );
          mi32_lastIsoPositionSimple = ci32_now;
          mc_sendGpsISOName = rcc_tempISOName;
          if (getGnssMode() == IsoAgLib::IsoNoGps)
          { /// @todo Allow Rapid Update without Complete Position TP/FP before? Is is just an update or can it be standalone?
              /// for now, allow it as standalone and set GpsMethod simply to IsoGnssNull as we don't have reception info...
            mt_gnssMethod = IsoAgLib::IsoGnssNull; // was IsoGnssFix before. Actually, noone knows what to set here ;-)
            #ifdef ENABLE_NMEA_2000_MULTI_PACKET
            mt_gnssType = IsoAgLib::IsoGnssGps;
            #endif
          }
        }
        return true;

      case NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN:
        if ( checkParseReceived( rcc_tempISOName ) )
        { // sender is allowed to send
          mui8_directionSequenceID          = data().getUint8Data ( 0 );
          mui8_courseOverGroundReference    = data().getUint8Data ( 1 ) & 0x03;
          mui16_courseOverGroundRad10Minus4 = data().getUint16Data( 2 );
          mui16_speedOverGroundCmSec        = data().getUint16Data( 4 );
          // always update values to know if the information is there or not!

          // set last time (also always, because if the sender's sending it's sending so we can't send!!
          mi32_lastIsoDirection = ci32_now;
          mc_sendGpsISOName = rcc_tempISOName;

          if ( (mui16_courseOverGroundRad10Minus4 <= (62855)) /// @todo check for the REAL max, 62855 is a little bigger than 62831 or alike that could be calculated. but anyway...
            && (mui16_speedOverGroundCmSec        <= (65532))
             )
          {
#if defined (USE_TRACTOR_MOVE) || defined (USE_BASE)
            IsoAgLib::iTracMove_c& c_tracmove = IsoAgLib::getITracMoveInstance();
            c_tracmove.updateSpeed(IsoAgLib::GpsBasedSpeed);
#endif
          }
          // else: Regard this as NO (valid) COG/SOG, so it's just like nothing meaningful got received!
        }
    break;

    }
    return false;
  }

  bool TimePosGPS_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
  {
    // check if we are allowed to send a request for pgn
    if ( ! BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver) ) return false;

    // call TimePosGPS_c function to send time/date
    // isoSendCalendar checks if this item (identified by ISOName)
    // is configured to send time/date
    sendCalendar(*getISOName());
    return true;
  };

  /** check if an NMEA2000 position signal was received - it does NOT indicate that this is an up2date signal */
  /** @todo improve with isPositionStreamReceived(), so we know that e.g. Altitude is there, too... */
  bool TimePosGPS_c::isPositionReceived() const
  {
    if ( (mi32_latitudeDegree10Minus7  >= ( -90*10000000)) && (mi32_latitudeDegree10Minus7  <= ( 90*10000000))
      && (mi32_longitudeDegree10Minus7 >= (-180*10000000)) && (mi32_longitudeDegree10Minus7 <= (180*10000000))
      && (mt_gnssMethod != IsoAgLib::IsoNoGps) /// @todo improve the checking on what's valid!
      )
    { // yep, valid GPS information
      return true;
    }
    else
    { // there is no valid GPS information
      return false;
    }
  }

  /** check if an NMEA2000 direction signal was received - it does NOT indicate that this is an up2date signal */
  bool TimePosGPS_c::isDirectionReceived() const
  {
    if ( (mui16_courseOverGroundRad10Minus4 <= 62855) /// @todo check for the REAL max, 62855 is a little bigger than 62831 or alike that could be calculated. but anyway...
      && (mui16_speedOverGroundCmSec        <= 65532)
       )
    { // yep, valid GPS-direction information
      return true;
    }
    else
    { // there is no valid GPS-direction information
      return false;
    }
  }


  #ifdef ENABLE_NMEA_2000_MULTI_PACKET
  /** set the GPS time in UTC timezone.
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  */
  void TimePosGPS_c::setTimeUtcGps(uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec )
  {
    bit_gpsTime.hour = ab_hour;
    bit_gpsTime.minute = ab_minute;
    bit_gpsTime.second = ab_second;
    bit_gpsTime.msec = aui16_msec;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setTimeUtc( ab_hour, ab_minute, ab_second, aui16_msec );
    }
  }
  /** set the calendar hour value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param ab_hour actual calendar hour value
  */
  void TimePosGPS_c::setHourUtcGps(uint8_t ab_hour)
  {
    bit_gpsTime.hour = ab_hour;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setHourUtc( ab_hour);
    }
  }
  /** set the calendar minute value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param ab_minute actual calendar minute value
  */
  void TimePosGPS_c::setMinuteUtcGps(uint8_t ab_minute)
  {
    bit_gpsTime.minute = ab_minute;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setMinuteUtc( ab_minute );
    }
  }
  /** set the calendar second value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param ab_second actual calendar second value
  */
  void TimePosGPS_c::setSecondUtcGps(uint8_t ab_second)
  {
    bit_gpsTime.second = ab_second;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setSecond( ab_second );
    }
  }
  /** set the calendar millisecond value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param ab_millisecond actual calendar second value
  */
  void TimePosGPS_c::setMillisecondUtcGps(uint16_t aui16_millisecond)
  {
    bit_gpsTime.msec = aui16_millisecond;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setMillisecond( aui16_millisecond );
    }
  }

  /** deliver GPS receive qualitiy */
  void TimePosGPS_c::setGnssMode( IsoAgLib::IsoGnssMethod_t at_newVal )
  {
    if(at_newVal<=IsoAgLib::IsoGnssMethodMAX) mt_gnssMethod = at_newVal;
/**
 * not using data-mode anymore as we changed from PGN 130577 to 129026
 *
    // set the upper 4 bits to unused as the Set/COG/Heading Ref is not used
    ui8_dataModeAndHeadingReference = 0xF0;
    switch ( at_newVal )
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
*/
  }

  //! Parameter:
  //! @param ac_ident:
  //! @param aui32_totalLen:
  bool TimePosGPS_c::reactOnStreamStart (const IsoAgLib::ReceiveStreamIdentifier_c& ac_ident,
                                         uint32_t /*aui32_totalLen */)
  {
    if ( ( ( ac_ident.getPgn() == NMEA_GPS_POSITION_DATA_PGN  )
        || ( ac_ident.getPgn() == NMEA_GPS_DIRECTION_DATA_PGN ) )
        &&  checkModeGps(IsoAgLib::IdentModeImplement) )
    { // this a NMEA multi stream of interest where we don't send it
      return true;
    }
    else
    { // this is not of interest for us
      return false; /** @todo check if we're NACKing it and if that's correct?? maybe we should just ignore it??? */
    }
  }

  //! Parameter:
  //! @param ac_ident:
  //! @param ab_isFirstChunk:
  //! @param ab_isLastChunkAndACKd:
  bool TimePosGPS_c::processPartStreamDataChunk (IsoAgLib::iStream_c& arc_stream,
                                          bool /*ab_isFirstChunk*/,
                                          bool ab_isLastChunkAndACKd)
  {
    const IsoAgLib::ReceiveStreamIdentifier_c& ac_ident = arc_stream.getIdent();

    // >>>Last Chunk<<< Processing
    if (ab_isLastChunkAndACKd)
    {  /** let reactOnLastChunk decide whether the pool should be kept in memory */
      return reactOnLastChunk(ac_ident, arc_stream);
    }
    // default - don't keep it
    return false;
  }

  void TimePosGPS_c::reactOnAbort (IsoAgLib::iStream_c& /*arc_stream*/)
  { // as we don't perform on-the-fly parse, there is nothing special to do
  }

  //! Parameter:
  //! @param ac_ident:
  //! @param apc_stream:
  bool TimePosGPS_c::reactOnLastChunk (const IsoAgLib::ReceiveStreamIdentifier_c& ac_ident,
                                       IsoAgLib::iStream_c& rc_stream)
  { // see if it's a pool upload, string upload or whatsoever! (First byte is already read by MultiReceive!)
    IsoName_c const& rcc_tempISOName = rc_stream.getIdent().getSaIsoName();

    // check if we want to process the information
    if (
        ( checkModeGps(IsoAgLib::IdentModeTractor) ) // I'm the sender
        ||
        ( // one of the following conditions must be true
         (mc_sendGpsISOName != rcc_tempISOName) // actual sender different to last
          &&
         (mc_sendGpsISOName.isSpecified() ) // last sender has correctly claimed address member
        )
       )
    { // DO NOT take this message, as this might be a falsly double source
      return false;
    }

    // set last time and ISOName information
    mc_sendGpsISOName = rcc_tempISOName;
    uint8_t ui8_tempValue;

    switch ( ac_ident.getPgn() )
    {
      case NMEA_GPS_POSITION_DATA_PGN: // 0x01F805LU -> 129029
      {
        mi32_lastIsoPositionStream = data().time();
        // fetch sequence number from Byte1
        IsoAgLib::convertIstream( rc_stream, mui8_positionSequenceID );
        // --> continue with Byte2 ...
        uint16_t ui16_daysSince1970;
        uint32_t ui32_milliseconds;
        // read 2-bytes of Generic date as days since 1-1-1970 ( UNIX date )
        IsoAgLib::convertIstream( rc_stream, ui16_daysSince1970 );
        IsoAgLib::convertIstream( rc_stream, ui32_milliseconds );
        // NMEA NMEA_GPS_POSITON_DATA_PGN sends with 0.1 msec
        ui32_milliseconds /= 10;

        const time_t t_tempUnixTime = ( time_t(ui16_daysSince1970) * time_t(60 * 60 * 24) ) + (ui32_milliseconds/1000);
        tm* UtcNow = gmtime( &t_tempUnixTime );
        if ( UtcNow != NULL )
        {
          if ( checkMode(IsoAgLib::IdentModeTractor) || getSelectedDataSourceISOName().isUnspecified())
          { // update the normal UTC time from GPS time, as we are either sending the calendar + time PGN _or_
            // there is currently no other active sender of this PGN --> other getter functions of this application should get
            // the GPS time as notmal UTC time
            setCalendarUtc((UtcNow->tm_year+1900), UtcNow->tm_mon + 1, UtcNow->tm_mday,
                            UtcNow->tm_hour, UtcNow->tm_min, UtcNow->tm_sec, (ui32_milliseconds%1000));
          }
          // now set also the pure GPS time
          setTimeUtcGps(UtcNow->tm_hour, UtcNow->tm_min, UtcNow->tm_sec, (ui32_milliseconds%1000));

          if (getSelectedDataSourceISOName().isSpecified())
          { // set also UTC time (in this case setTimeUtcGps() doesn't update the UTC time)
            setTimeUtc(UtcNow->tm_hour, UtcNow->tm_min, UtcNow->tm_sec, (ui32_milliseconds%1000));
          }
        }
        // now read Latitude --> convert into double [degree]
        getDegree10Minus7FromStream( rc_stream, mi32_latitudeDegree10Minus7 );
        // now read Longitude --> convert into double [degree]
        getDegree10Minus7FromStream( rc_stream, mi32_longitudeDegree10Minus7 );
        // now read Altitude --> convert into double [meter]
        getAltitude10Minus2FromStream( rc_stream, mi32_altitudeCm );
        // now fetch Quality - gps-mode
        rc_stream >> ui8_tempValue;
        if ( ( ui8_tempValue >> 4  ) <= IsoAgLib::IsoGnssMethodMAX ) mt_gnssMethod = IsoAgLib::IsoGnssMethod_t(ui8_tempValue >> 4 );
        if ( ( ui8_tempValue & 0xF ) <= IsoAgLib::IsoGnssTypeMAX   ) mt_gnssType   = IsoAgLib::IsoGnssType_t(ui8_tempValue & 0xF );
        // GNSS Integrity
        rc_stream >> mui8_integrity;
        mui8_integrity &= 0x3; // mask reserved bits out
        // now fetch the number of satelites
        rc_stream >> mui8_satelliteCnt;
        // now fetch HDOP from raw uint16_t to float [1.0*1.0e-2)
        IsoAgLib::convertIstream( rc_stream, mi16_hdop );
        // now fetch PDOP from raw uint16_t to float [1.0*1.0e-2)
        IsoAgLib::convertIstream( rc_stream, mi16_pdop );
        // Geodial Separation
        IsoAgLib::convertIstream( rc_stream, mi32_geoidalSeparation );
        // number of reference stations
        IsoAgLib::convertIstream( rc_stream, mui8_noRefStations );
        // now read the type and age of all following reference stations
        for ( unsigned int ind = 0; ((ind < mui8_noRefStations) &&(!rc_stream.eof())); ind++ )
        { // push new items at the end or simply update the corresponding value
          if ( mvec_refStationTypeAndStation.size() < (ind+1) ) mvec_refStationTypeAndStation.push_back(__IsoAgLib::convertIstreamUi16( rc_stream ) );
          else IsoAgLib::convertIstream( rc_stream, mvec_refStationTypeAndStation[ind] );
          if ( mvec_refStationDifferentialAge10Msec.size() < (ind+1) ) mvec_refStationDifferentialAge10Msec.push_back(__IsoAgLib::convertIstreamUi16( rc_stream ) );
          else IsoAgLib::convertIstream( rc_stream, mvec_refStationDifferentialAge10Msec[ind] );
        }
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "process NMEA_GPS_POSITON_DATA_PGN Lat: " << mi32_latitudeDegree10Minus7
          << ", Lon: " << mi32_longitudeDegree10Minus7 << ", Alt: " << mi32_altitudeCm
          << ", TotalSize: " << rc_stream.getByteTotalSize() << ", resceived: " << rc_stream.getByteAlreadyReceived()
          << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;

      case NMEA_GPS_DIRECTION_DATA_PGN: // 0x01FE11LU - 130577 with Heading and Speed
        uint8_t ui8_dummy;
        uint16_t ui16_newCOG, ui16_newSOG;
        IsoAgLib::convertIstream( rc_stream, ui8_dummy ); //ui8_dataModeAndHeadingReference ); //ui8_dataModeAndHeadingReferenceDUMMY &= 0x3F;
        IsoAgLib::convertIstream( rc_stream, mui8_directionSequenceID );
        IsoAgLib::convertIstream( rc_stream, ui16_newCOG );
        IsoAgLib::convertIstream( rc_stream, ui16_newSOG );
      //IsoAgLib::convertIstream( rc_stream, ui16_dummy ); //ui16_headingRad10Minus4 );
      //IsoAgLib::convertIstream( rc_stream, ui16_dummy ); //ui16_speedCmSec );
      //IsoAgLib::convertIstream( rc_stream, ui16_dummy ); //ui16_flowDirectionRad10Minus4 );
      //IsoAgLib::convertIstream( rc_stream, ui16_dummy ); //ui16_driftSpeedCmSec );
        /** @todo Do we need those valus above?? */

        if ( (ui16_newCOG <= (62855)) /// @todo check for the REAL max, 62855 is a little bigger than 62831 or alike that could be calculated. but anyway...
          && (ui16_newSOG <= (65532))
           )
        {
          mui16_courseOverGroundRad10Minus4 = ui16_newCOG;
          mui16_speedOverGroundCmSec = ui16_newSOG;
          mi32_lastIsoDirection = data().time();
#if defined (USE_TRACTOR_MOVE) || defined (USE_BASE)
          IsoAgLib::iTracMove_c& c_tracmove = IsoAgLib::getITracMoveInstance();
          c_tracmove.updateSpeed(IsoAgLib::GpsBasedSpeed);
#endif
        }

        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "process NMEA_GPS_DIRECTION_DATA_PGN: CourseOverGround: " << mui16_courseOverGroundRad10Minus4
          << ", SpeedOverGround [cm/sec]: " << mui16_speedOverGroundCmSec
          << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        break;
    }
    return false;
  }
  #endif // END ENABLE_NMEA_2000_MULTI_PACKET

  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TimePosGPS_c::timeEventTracMode()
  {
    const int32_t ci32_now = getLastRetriggerTime();

    // time/date is only sent on request

    if ( checkModeGps(IsoAgLib::IdentModeTractor) )
    { // mpc_isoNameGps must be != NULL, because we are in tractor mode
      if ( isPositionSimpleToSend() && ((ci32_now - mi32_lastIsoPositionSimple) >= 100) )
      {
        sendPositionRapidUpdate();
      }

      if ( isDirectionToSend() && ((ci32_now - mi32_lastIsoDirection) >= 250) )
      {
        isoSendDirection();
      }

      #ifdef ENABLE_NMEA_2000_MULTI_PACKET
      if ( ( isPositionStreamToSend() )
        && ( ( ci32_now - mi32_lastIsoPositionStream ) >= 1000 )
        && ( mt_multiSendSuccessState != MultiSend_c::Running  ) )
      {
        if ( getAvailableExecTime() == 0 ) return false;
        isoSendPositionStream();
      }
      #endif // END of ENABLE_NMEA_2000_MULTI_PACKET
    }
    return true;
  }

  /** send position rapid update message */
  void TimePosGPS_c::sendPositionRapidUpdate( void )
  {
    // retreive the actual dynamic sender no of the member with the registered isoName
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberISOName(*mpc_isoNameGps, true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(2);
    data().setLen(8);
    data().setIsoSa(b_sa);

    data().setIsoPgn(NMEA_GPS_POSITION_RAPID_UPDATE_PGN);
    data().setInt32Data(0, mi32_latitudeDegree10Minus7 );
    data().setInt32Data(4, mi32_longitudeDegree10Minus7);

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    getCanInstance4Comm() << data();

    // update time
    mi32_lastIsoPositionSimple = getLastRetriggerTime();
  }

/** send COG and SOG as 250ms rapid update, but with single packet only */
/**
** not using anymore as we changed from PGN 130577 to 129026
**
  void TimePosGPS_c::isoSendDirectionStream( void )
  {
    const int32_t ci32_now = getLastRetriggerTime();
    // set data in Nmea2000SendStreamer_c
    mc_nmea2000Streamer.reset();
    STL_NAMESPACE::vector<uint8_t>& writeRef = mc_nmea2000Streamer.vec_data;
    // use helper function to transfer value to the byte vector
    number2LittleEndianString( uint8_t (ui8_dataModeAndHeadingReference|0xC0), writeRef );     /// NOT there in the RAPID UPDATE one
    number2LittleEndianString( mui8_directionSequenceID, writeRef );
    number2LittleEndianString( mui16_courseOverGroundRad10Minus4, writeRef );
    number2LittleEndianString( mui16_speedOverGroundCmSec, writeRef );

    number2LittleEndianString( ui16_headingRad10Minus4, writeRef );           /// not init'ed  /// NOT there in the RAPID UPDATE one
    number2LittleEndianString( ui16_speedCmSec, writeRef );                   /// not init'ed  /// NOT there in the RAPID UPDATE one

    number2LittleEndianString( ui16_flowDirectionRad10Minus4, writeRef );     /// not init'ed   /// NOT there in the RAPID UPDATE one
    number2LittleEndianString( ui16_driftSpeedCmSec, writeRef );              /// not init'ed  /// NOT there in the RAPID UPDATE one

    //now trigger sending
    #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
    if ( getMultiSendInstance4Comm().sendIsoFastPacket(mc_sendGpsISOName, &mc_nmea2000Streamer, NMEA_GPS_DIRECTION_DATA_PGN, mt_multiSendSuccessState) )
    #else
    if ( getMultiSendInstance4Comm().sendIsoBroadcast(mc_sendGpsISOName, &mc_nmea2000Streamer, NMEA_GPS_DIRECTION_DATA_PGN, mt_multiSendSuccessState) )
    #endif
    { // update time
      mi32_lastIsoDirectionStream = ci32_now;
      mui8_directionSequenceID++;
    }
  }
*/

void TimePosGPS_c::isoSendDirection( void )
{
  // little pre-Setup
  mui8_directionSequenceID = 0xFF; // not using tied-together-packaging right now
  mui8_courseOverGroundReference = 0; // for now, we only send TRUE NORTH

  data().setIdentType(Ident_c::ExtendedIdent);
  data().setIsoPri(2);
  data().setISONameForSA(*mpc_isoNameGps);

  data().setIsoPgn (NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN);
  data().setUint8Data (0, mui8_directionSequenceID );
  data().setUint8Data (1, mui8_courseOverGroundReference);
  data().setUint16Data(2, mui16_courseOverGroundRad10Minus4 );
  data().setUint16Data(4, mui16_speedOverGroundCmSec );
  data().setUint16Data(6, 0xFFFF );

  // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
  // then it sends the data
  getCanInstance4Comm() << data();

  // update time
  mi32_lastIsoDirection = Scheduler_c::getLastTimeEventTrigger();
}


#if defined(ENABLE_NMEA_2000_MULTI_PACKET)
  void setDegree10Minus7ToStream( const int32_t& ri32_src, STL_NAMESPACE::vector<uint8_t>& writeRef )
  {
    #if SIZEOF_INT == 4
    // use 64 bit variable
    const double d_temp = double(ri32_src) * 1.0e+9;
    int64_t i64_temp = int64_t(d_temp);
    number2LittleEndianString( i64_temp, writeRef );
    #else
    // only take higher 4 bytes ( write the lower four byte with 0xFF in case the value is negative )
    int32_t i32_temp = (ri32_src >= 0)?0:-1;
    number2LittleEndianString( i32_temp, writeRef );
    i32_temp = int32_t(double(ri32_src) * 1.0e+9 / 4294967296.0);
    number2LittleEndianString( i32_temp, writeRef );
    #endif
  }

  void setAltitude10Minus2ToStream( const int32_t& ri32_result, STL_NAMESPACE::vector<uint8_t>& writeRef )
  {
    #if SIZEOF_INT == 4
    // use 64 bit variable
    // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
    const double d_temp = double(ri32_result) * 1.0e+4;
    const int64_t i64_temp = int64_t(d_temp);
    number2LittleEndianString( i64_temp, writeRef );
    #else
    // only take higher 4 bytes
    int32_t i32_temp = 0;
    number2LittleEndianString( i32_temp, writeRef );
    // NMEA sends with 1.0e-6, while normally 1.0e-2 is enough -> mult with 1.0e-4
    i32_temp = int32_t(double(ri32_result) * 1.0e+4 / 4294967296.0);
    number2LittleEndianString( i32_temp, writeRef );
    #endif
  }

  /** send position as detailed stream */
  void TimePosGPS_c::isoSendPositionStream( void )
  {
    // little pre-Setup
    mui8_positionSequenceID = 0xFF; // not using tied-together-packaging right now

    const int32_t ci32_now = getLastRetriggerTime();
    // set data in Nmea2000SendStreamer_c
    mc_nmea2000Streamer.reset();
    STL_NAMESPACE::vector<uint8_t>& writeRef = mc_nmea2000Streamer.vec_data;
    // use helper function to transfer value to the byte vector
    number2LittleEndianString( mui8_positionSequenceID, writeRef );

    uint16_t ui16_daysSince1970 = 0; // standard value (or 0xFFFF?), if UTC date is not set (= 01.01.1900)

    const struct CNAMESPACE::tm* p_tm = currentUtcTm();

    if (p_tm)
    { // testTime is only used for calculation of ui16_daysSince1970 => use time 12:00:00 to avoid daylight setting influence
      struct tm testTime = {0, 0, 12, p_tm->tm_mday, p_tm->tm_mon, p_tm->tm_year, 0,0,-1
                            #if defined(__USE_BSD) || defined(__GNU_LIBRARY__) || defined(__GLIBC__) || defined(__GLIBC_MINOR__)
                            , 0, NULL
                            #endif
                            };
      // compensate mktime() time zone influence:
      const time_t secondsSince1970 = mktime( &testTime ) + mt_tzOffset;
      // calculate the days
      ui16_daysSince1970 = secondsSince1970 / ( 60 * 60 *24 );
    }
    const uint32_t ui32_milliseconds = ( ( ( ( ( ( hourUtcGps() * 60 ) + minuteUtcGps() ) * 60 ) + secondUtcGps() ) * 1000 ) + millisecondUtcGps()) * 10;
    // write Position Date as Days since 1.1.1970
    number2LittleEndianString( ui16_daysSince1970, writeRef );
    // write Position Time as Milliseconds*10 per day
    number2LittleEndianString( ui32_milliseconds, writeRef );
    // write Latitude as uint64_t value
    setDegree10Minus7ToStream( mi32_latitudeDegree10Minus7, writeRef );
    // write Longitude as uint64_t value
    setDegree10Minus7ToStream( mi32_longitudeDegree10Minus7, writeRef );
    // write Altitude as uint64_t value
    setAltitude10Minus2ToStream( mi32_altitudeCm, writeRef );

    // write type and method
    const uint8_t cu8_tempTypeMethod = ( mt_gnssType | ( mt_gnssMethod << 4 ) );
    number2LittleEndianString( cu8_tempTypeMethod, writeRef );
    // write integrity ( set unused highest 6 bits to "1" )
    number2LittleEndianString( uint8_t(mui8_integrity| 0xFC), writeRef );
    // write #satelites
    number2LittleEndianString( mui8_satelliteCnt, writeRef );
    // write HDOP
    number2LittleEndianString( mi16_hdop, writeRef );
    // write PDOP
    number2LittleEndianString( mi16_pdop, writeRef );
    // write geodial separation
    number2LittleEndianString( mi32_geoidalSeparation, writeRef );

    // write number of reference stations
    uint8_t ui8_limit = mui8_noRefStations;
    if ( mvec_refStationTypeAndStation.size() < ui8_limit ) ui8_limit = mvec_refStationTypeAndStation.size();
    if ( mvec_refStationDifferentialAge10Msec.size() < ui8_limit ) ui8_limit = mvec_refStationDifferentialAge10Msec.size();

    number2LittleEndianString( ui8_limit, writeRef );

    for ( unsigned int ind = 0; ind < ui8_limit; ind++ )
    {
      number2LittleEndianString( mvec_refStationTypeAndStation[ind], writeRef );
      number2LittleEndianString( mvec_refStationDifferentialAge10Msec[ind], writeRef );
    }

    //now trigger sending
    #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
    if ( getMultiSendInstance4Comm().sendIsoFastPacketBroadcast(mc_sendGpsISOName, &mc_nmea2000Streamer, NMEA_GPS_POSITION_DATA_PGN, mt_multiSendSuccessState) )
    #else
    if ( getMultiSendInstance4Comm().sendIsoBroadcast(mc_sendGpsISOName, &mc_nmea2000Streamer, NMEA_GPS_POSITION_DATA_PGN, mt_multiSendSuccessState) )
    #endif
    { // update time
      mi32_lastIsoPositionStream = ci32_now;
   }
  }
  #endif // END OF ENABLE_NMEA_2000_MULTI_PACKET

  /** send ISO11783 calendar PGN
      @param ac_isoName ISOName code off calling item which wants to send
      possible errors:
          * dependant error in CanIo_c on CAN send problems
      @see CanPkg_c::getData
      @see CanPkgExt_c::getData
      @see CanIo_c::operator<<
    */
  void TimePosGPS_c::sendCalendar(const IsoName_c& ac_isoName)
  {
    if (!getIsoMonitorInstance4Comm().existIsoMemberISOName(ac_isoName, true)) return;

    data().setISONameForSA( ac_isoName );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(6);
    data().setLen(8);

    if ( ( getSelectedDataSourceISOName() == ac_isoName ) )
    { // this item (identified by ISOName is configured to send
      if ( checkMode(IsoAgLib::IdentModeTractor) )
        setSelectedDataSourceISOName(*getISOName());

      data().setIsoPgn(TIME_DATE_PGN);

      const struct CNAMESPACE::tm* p_tm = currentUtcTm();
      if (NULL != p_tm)
      {
        data().setUint8Data(0, (p_tm->tm_sec * 4) );
        data().setUint8Data(1, p_tm->tm_min);
        data().setUint8Data(2, p_tm->tm_hour);
        data().setUint8Data(3, p_tm->tm_mon + 1);
        data().setUint8Data(4, p_tm->tm_mday * 4);
        data().setUint8Data(5, p_tm->tm_year + 1900 - 1985);
      }
      else
      {
        data().setUint16Data( 0, 0U );
        data().setUint8Data(2, 0U);
        data().setUint8Data(3, 1U);
        data().setUint8Data(4, 4U);
        data().setUint8Data(5, 0U);
      }
      data().setUint8Data(6, bit_calendar.timezoneMinuteOffset );
      data().setUint8Data(7, bit_calendar.timezoneHourOffsetMinus24 );

      // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
      // then it sends the data
      getCanInstance4Comm() << data();

      // update time
      setUpdateTime(getLastRetriggerTime() );
    }
  }

  /** check if a calendar information was received since init
   */
  bool TimePosGPS_c::isCalendarReceived() const
  {
    return ( ( bit_calendar.day == 1 ) && ( bit_calendar.month == 1 ) && ( bit_calendar.year == 0 ) // year 0 (absolute) indicates NO_VAL!
        && ( bit_calendar.second == 0 ) && ( bit_calendar.minute == 0 ) && ( bit_calendar.hour == 0 ) )?false:true;
  }

  /** check if a calendar's DATE information that was received is VALID */
  bool TimePosGPS_c::isCalendarDateValid() const
  { // check if default data from init is still in vars
    return ( (bit_calendar.day > 0) && (bit_calendar.month > 0) && (bit_calendar.year > 0) );
  }

  /** check if a calendar's TIME information that was received is VALID
   * @todo Add this check if it's ensured everywhere that NO_VAL times
   *       don't screw around (with some mktime() or whatever functions...)
  bool TimePosGPS_c::isCalendarTimeValid() const
   */

  /**
  set the calendar value
  @param ai16_year value to store as year
  @param ab_month value to store as month
  @param ab_day value to store as day
  @param ab_hour value to store as hour
  @param ab_minute value to store as minute
  @param ab_second value to store as second
  */
  void TimePosGPS_c::setCalendarUtc(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day, uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec)
  {
    mi32_lastCalendarSet = System_c::getTime();
    mt_cachedLocalSeconds1970AtLastSet = 0;

    bit_calendar.year   = ai16_year;
    bit_calendar.month  = ab_month;
    bit_calendar.day    = ab_day;
    bit_calendar.hour   = ab_hour;
    bit_calendar.minute = ab_minute;
    bit_calendar.second = ab_second;
    bit_calendar.msec   = aui16_msec;
  };

  /**
  set the calendar value as local time ( take local time offsets into account )
  @param ai16_year value to store as year
  @param ab_month value to store as month
  @param ab_day value to store as day
  @param ab_hour value to store as hour
  @param ab_minute value to store as minute
  @param ab_second value to store as second
  */
  void TimePosGPS_c::setCalendarLocal(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day, uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec )
  {
    mi32_lastCalendarSet = System_c::getTime();
    mt_cachedLocalSeconds1970AtLastSet = 0;
    struct ::tm testTime = { ab_second, int(ab_minute)-int(bit_calendar.timezoneMinuteOffset), (int(ab_hour)-(int(bit_calendar.timezoneHourOffsetMinus24)-24)),
                            ab_day,(ab_month-1),(ai16_year-1900),0,0,-1
                            #if defined(__USE_BSD) || defined(__GNU_LIBRARY__) || defined(__GLIBC__) || defined(__GLIBC_MINOR__)
                            , 0, NULL
                            #endif
                            };
    // argument of mktime is interpreted as local time (system time zone influence!)
    const time_t middle = mktime( &testTime );
    // compensate system time zone setting: call localtime() and not gmtime()
    const struct ::tm* normalizedTime = localtime( &middle );

    bit_calendar.year   = normalizedTime->tm_year+1900;
    bit_calendar.month  = (normalizedTime->tm_mon+1);
    bit_calendar.day    = normalizedTime->tm_mday;
    bit_calendar.hour   = normalizedTime->tm_hour;
    bit_calendar.minute = normalizedTime->tm_min;
    bit_calendar.second = normalizedTime->tm_sec;
    bit_calendar.msec   = aui16_msec;
  };

  /** set the date in local timezone */
  void TimePosGPS_c::setDateLocal(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day)
  {
    mi32_lastCalendarSet = System_c::getTime();
    mt_cachedLocalSeconds1970AtLastSet = 0;

    // @todo: calender time consists of UTC time and local date?
    struct ::tm testTime = { bit_calendar.second, bit_calendar.minute, bit_calendar.hour,
                            ab_day,(ab_month-1),(ai16_year-1900),0,0,-1
                            #if defined(__USE_BSD) || defined(__GNU_LIBRARY__) || defined(__GLIBC__) || defined(__GLIBC_MINOR__)
                            , 0, NULL
                            #endif
                          };

    const time_t middle = mktime( &testTime );
    const struct ::tm* normalizedTime = localtime( &middle );

    bit_calendar.year   = normalizedTime->tm_year+1900;
    bit_calendar.month  = (normalizedTime->tm_mon+1);
    bit_calendar.day    = normalizedTime->tm_mday;
  }

  /** set the date in UTC timezone */
  void TimePosGPS_c::setDateUtc(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day)
  {
    mi32_lastCalendarSet = System_c::getTime();
    mt_cachedLocalSeconds1970AtLastSet = 0;
    bit_calendar.year   = ai16_year;
    bit_calendar.month  = ab_month;
    bit_calendar.day    = ab_day;
  }

  /** set the time in local timezone */
  void TimePosGPS_c::setTimeLocal(uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec)
  {
    mi32_lastCalendarSet = System_c::getTime();
    mt_cachedLocalSeconds1970AtLastSet = 0;

    struct ::tm testTime = { ab_second, int(ab_minute)-int(bit_calendar.timezoneMinuteOffset), (int(ab_hour)-(int(bit_calendar.timezoneHourOffsetMinus24)-24)),
                            bit_calendar.day,(bit_calendar.month-1),
                            (bit_calendar.year == 0) ? 70 : (bit_calendar.year-1900), // in case bit_calendar.year is not yet set: use 1970
                            0,0,-1
                            #if defined(__USE_BSD) || defined(__GNU_LIBRARY__) || defined(__GLIBC__) || defined(__GLIBC_MINOR__)
                            , 0, NULL
                            #endif
                          };

    // argument of mktime is interpreted as local time (system time zone influence!)
    const time_t middle = mktime( &testTime );
    // compensate system time zone setting: call localtime() and not gmtime()
    const struct ::tm* normalizedTime = localtime( &middle );

    bit_calendar.hour   = normalizedTime->tm_hour;
    bit_calendar.minute = normalizedTime->tm_min;
    bit_calendar.second = normalizedTime->tm_sec;
    bit_calendar.msec   = aui16_msec;
  }

  /** set the time in UTC timezone */
  void TimePosGPS_c::setTimeUtc(uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec, bool b_updateDate)
  {
    if (b_updateDate)
    {
      struct CNAMESPACE::tm* p_tm = currentUtcTm();
      bit_calendar.year   = p_tm->tm_year + 1900;
      bit_calendar.month  = p_tm->tm_mon +1;
      bit_calendar.day    = p_tm->tm_mday;
    }

    mi32_lastCalendarSet = System_c::getTime();
    mt_cachedLocalSeconds1970AtLastSet = 0;
    bit_calendar.hour   = ab_hour;
    bit_calendar.minute = ab_minute;
    bit_calendar.second = ab_second;
    bit_calendar.msec   = aui16_msec;
  }

  const struct ::tm* TimePosGPS_c::Utc2LocalTime()
  {
    if (0 == mt_cachedLocalSeconds1970AtLastSet)
    {
      // set mt_cachedLocalSeconds1970AtLastSet
      currentUtcTm();
    }

    const time_t t_secondsSince1970Local = mt_cachedLocalSeconds1970AtLastSet + calendarSetAge()/1000
                                          + (bit_calendar.timezoneHourOffsetMinus24 - 24) * 60 * 60  // negative offsets => increased local time
                                          + bit_calendar.timezoneMinuteOffset * 60;

    // compensate system time zone setting: call localtime() and not gmtime()
    return localtime( &t_secondsSince1970Local );
  }

  /**
  get the calendar year value
  @return actual calendar year value
  */
  int16_t TimePosGPS_c::yearLocal()
  {
    return Utc2LocalTime()->tm_year+1900;
  }

  /**
  get the calendar month value
  @return actual calendar month value
  */
  uint8_t TimePosGPS_c::monthLocal()
  { // month is delivered with range [0..11] -> add 1
    return Utc2LocalTime()->tm_mon+1;
  }

  /**
  get the calendar day value
  @return actual calendar day value
  */
  uint8_t TimePosGPS_c::dayLocal()
  {
    return Utc2LocalTime()->tm_mday;
  }

  /**
  get the calendar hour value
  @return actual calendar hour value
  */
  uint8_t TimePosGPS_c::hourLocal()
  {
    return Utc2LocalTime()->tm_hour;
  }

  /**
  get the calendar minute value
  @return actual calendar minute value
  */
  uint8_t TimePosGPS_c::minuteLocal()
  {
    return Utc2LocalTime()->tm_min;
  };

  int16_t TimePosGPS_c::yearUtc()
  {
    const struct CNAMESPACE::tm* p_tm = currentUtcTm();
    return (NULL != p_tm) ? p_tm->tm_year + 1900 : 0;
  }
  uint8_t TimePosGPS_c::monthUtc()
  {
    const struct CNAMESPACE::tm* p_tm = currentUtcTm();
    return (NULL != p_tm) ? p_tm->tm_mon + 1 : 0;
  }
  uint8_t TimePosGPS_c::dayUtc()
  {
    const struct CNAMESPACE::tm* p_tm = currentUtcTm();
    return (NULL != p_tm) ? p_tm->tm_mday : 0;
  }
  uint8_t TimePosGPS_c::hourUtc()
  {
    const struct CNAMESPACE::tm* p_tm = currentUtcTm();
    return (NULL != p_tm) ? p_tm->tm_hour : 0;
  }
  uint8_t TimePosGPS_c::minuteUtc()
  {
    const struct CNAMESPACE::tm* p_tm = currentUtcTm();
    return (NULL != p_tm) ? p_tm->tm_min : 0;
  }
  uint8_t TimePosGPS_c::second()
  {
    const struct CNAMESPACE::tm* p_tm = currentUtcTm();
    return (NULL != p_tm) ? p_tm->tm_sec : 0;
  }

  struct CNAMESPACE::tm* TimePosGPS_c::currentUtcTm()
  {
    if ( 0 == mt_cachedLocalSeconds1970AtLastSet)
    { // recalculate seconds from bit_calendar struct
      // compensate system time zone setting (part 1)
      struct tm testTime = { bit_calendar.second, bit_calendar.minute, bit_calendar.hour,
                            bit_calendar.day, bit_calendar.month-1,
                            (bit_calendar.year == 0) ? 70 : (bit_calendar.year-1900), // in case bit_calendar.year is not yet set: use 1970
                            0,0,-1
                            #if defined(__USE_BSD) || defined(__GNU_LIBRARY__) || defined(__GLIBC__) || defined(__GLIBC_MINOR__)
                            , 0, NULL
                            #endif
                            };
      mt_cachedLocalSeconds1970AtLastSet = mktime( &testTime );
      if (-1 == mt_cachedLocalSeconds1970AtLastSet) return NULL;
    }

    const time_t t_secondsSince1970 = mt_cachedLocalSeconds1970AtLastSet + calendarSetAge()/1000;

    // compensate system time zone setting (part 2)
    return localtime( &t_secondsSince1970 );
  }

///  Used for Debugging Tasks in Scheduler_c
const char*
TimePosGPS_c::getTaskName() const
{   return "TimePosGPS_c"; }

} // namespace __IsoAgLib

