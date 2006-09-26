/***************************************************************************
             timeposgps_c.cpp - working on Base Data Msg Type NMEA 2000 GPS
                                and Calendar; stores, updates  and
                                delivers all base data informations
                                from CANCustomer_c derived for CAN
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
#include <IsoAgLib/comm/Multipacket/impl/multireceive_c.h>
#include <IsoAgLib/comm/Multipacket/istream_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h>
#include <IsoAgLib/util/iutil_funcs.h>
#include <IsoAgLib/comm/Base/itracmove_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgn_c.h>


using namespace std;

#ifdef NMEA_2000_FAST_PACKET
// Off-class/namespace c-style helper functions
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
  refui32_result = int32_t( d_temp );
  #endif
}
#endif // END of NMEA_2000_FAST_PACKET

namespace __IsoAgLib {

  uint8_t bcd2dec(uint8_t rb_bcd);
  uint8_t dec2bcd(uint8_t rb_dec);

  #if defined(NMEA_2000_FAST_PACKET)
  /** place next data to send direct into send buffer of pointed
    stream send package - MultiSend_c will send this
    buffer afterwards
  */
  void Nmea2000SendStreamer_c::setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes)
  {
    mspData->setDataPart (vec_data, ui16_currentSendPosition, bytes);
    ui16_currentSendPosition += bytes;
  }

  /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards
  */
  void Nmea2000SendStreamer_c::setDataNextFastPacketStreamPart (MultiSendPkg_c* mspData, uint8_t bytes, uint8_t rui8_offset )
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
  #endif // END of NMEA_2000_FAST_PACKET

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
  bool TimePosGPS_c::timeEvent(  )
  {
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    checkCreateReceiveFilter();
    if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;

    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

    // check for different base data types whether the previously
    if ( checkMode(IsoAgLib::IdentModeImplement)
      && ( (lastedTimeSinceUpdate() >= 3000 ) || ( yearUtc() == 0 ) )
      && ( getSelectedDataSourceISOName().isSpecified()           )
        )
    { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
      getSelectedDataSourceISOName().setUnspecified();
      bit_calendar.year = bit_calendar.hour = bit_calendar.minute = bit_calendar.second = 0;
      bit_calendar.month = bit_calendar.day = 1;
    }
    if ( checkModeGps(IsoAgLib::IdentModeImplement)
      #ifdef NMEA_2000_FAST_PACKET
      && ( ( ci32_now - i32_lastIsoPositionStream  ) >= TIMEOUT_SENDING_NODE  )
      && ( ( ci32_now - i32_lastIsoDirectionStream ) >= TIMEOUT_SENDING_NODE  )
      #endif
      && ( ( ci32_now - i32_lastIsoPositionSimple  ) >= TIMEOUT_SENDING_NODE  )
      && ( c_sendGpsISOName.isSpecified()                      ) )
    { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
      c_sendGpsISOName.setUnspecified();
      i32_latitudeDegree10Minus7 = i32_longitudeDegree10Minus7 = 0x7FFFFFFF;
      #ifdef NMEA_2000_FAST_PACKET
      ui8_satelliteCnt = 0;
      t_gnssMethod = IsoAgLib::IsoNoGps;
      t_gnssType = IsoAgLib::IsoGnssGps;
      ui32_altitudeCm = 0;
      #endif
    }

    if ( getISOName() != NULL )
    { // there is at least something configured to send
      if (getIsoMonitorInstance4Comm().existIsoMemberISOName(*getISOName(), true))
      { // stored base information sending ISO member has claimed address
        if ( checkMode(IsoAgLib::IdentModeTractor) ) timeEventTracMode();
      }
      if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
    }
    if ( pc_isoNameGps != NULL )
    { // there is at least something configured to send

      if (getIsoMonitorInstance4Comm().existIsoMemberISOName(*pc_isoNameGps, true))
      { // stored base information sending ISO member has claimed address
        if (checkModeGps(IsoAgLib::IdentModeTractor) ) timeEventTracMode();
      }
    }
    return true;
  }

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TimePosGPS_c::checkCreateReceiveFilter( )
  {
    ISOMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CANIO_c &c_can = getCanInstance4Comm();

    if ( ( ! checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();

      // create FilterBox_c for PGN TIME_DATE_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x1FFFF << 8) filter: (TIME_DATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                        (static_cast<MASK_TYPE>(TIME_DATE_PGN) << 8), false, Ident_c::ExtendedIdent);
      // *************************************************************************************************
      // Added by Brad Cox to accomodate NMEA 2000 GPS Messages:

      // GNSS Position Rapid Update
      // mask: (0x1FFFF << 8) filter: (NMEA_GPS_POSITON_RAPID_UPDATE_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x1FFFF) << 8),
                    (static_cast<MASK_TYPE>(NMEA_GPS_POSITON_RAPID_UPDATE_PGN) << 8), true, Ident_c::ExtendedIdent);

      // *************************************************************************************************
    }
  }

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TimePosGPS_c::init(const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode)
  {
    BaseCommon_c::init( rpc_isoName, rt_identMode );

    pc_isoNameGps = NULL;
    // set the GPS mode always to non-sending
    configGps( NULL, IsoAgLib::IdentModeImplement );

    // 01.01.1970 00:00:00
    struct tm t_testTime = {0, 0, 0, 1, 0, 70, 0 , 0 ,-1
                            #if defined(__USE_BSD) || defined(__GNU_LIBRARY__) || defined(__GLIBC__) || defined(__GLIBC_MINOR__)
                            , 0, NULL
                            #endif
                          };

    t_tzOffset = - mktime(&t_testTime);
    if (1 == t_tzOffset)
    { // mktime returned -1 => error
      t_tzOffset = 0;
    }
  }

  /** config the TimePosGPS_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param rpc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TimePosGPS_c::config(const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode)
  {
    //store old mode to decide to register or unregister to request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();

    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config(rpc_isoName, rt_identMode) ) return false;

    if (t_oldMode == IsoAgLib::IdentModeImplement && rt_identMode == IsoAgLib::IdentModeTractor)
    {  // a change from Implement mode to Tractor mode occured
      // create FilterBox_c for REQUEST_PGN_MSG_PGN, TIME_DATE_PGN
      getIsoRequestPgnInstance4Comm().registerPGN (*this, TIME_DATE_PGN); // request for time and date
    }

    if (t_oldMode == IsoAgLib::IdentModeTractor && rt_identMode == IsoAgLib::IdentModeImplement)
    {  // a change from Tractor mode to Implement mode occured
      // unregister from request for pgn, because in implement mode no request should be answered
      getIsoRequestPgnInstance4Comm().unregisterPGN (*this, TIME_DATE_PGN);
    }

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

    i32_lastCalendarSet = 0;

    t_cachedLocalSeconds1970AtLastSet = 0;

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
      @param rpc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rb_implementMode implement mode (true) or tractor mode (false)!!!
      @return true -> configuration was successfull
    */
  bool TimePosGPS_c::configGps(const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t  rt_identModeGps)
  {
    if (   rt_identModeGps == IsoAgLib::IdentModeTractor
        && rpc_isoName == NULL
      )
    { // the application is in tractor mode but has no valid isoName
      // IMPORTANT: if we are in tractor mode we MUST have a valid isoName otherwise the configuration makes no sense
      #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "CONFIG FAILURE. The config function was called with isoName == NULL and\
                                  IdentModeTractor. Is is not allowed that the isoName ist NULL in combination\
                                  with tractor mode." << "\n";
      #endif
      #if defined(DEBUG) && defined(SYSTEM_PC)
        abort();
      #endif
      getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::LbsBase );
      return false;
    }
    //set configure values
    i32_lastIsoPositionSimple = 0;

    pc_isoNameGps = rpc_isoName;
    t_identModeGps = rt_identModeGps;

    i32_latitudeDegree10Minus7 = i32_longitudeDegree10Minus7 = 0x7FFFFFFF;
    #ifdef NMEA_2000_FAST_PACKET
    i32_lastIsoPositionStream = i32_lastIsoDirectionStream = 0;
    t_multiSendSuccessState = MultiSend_c::SendSuccess;
    ui32_altitudeCm = 0;

    ui8_satelliteCnt = 0;
    #endif // END of NMEA_2000_FAST_PACKET

    if ( rt_identModeGps == IsoAgLib::IdentModeTractor )
    { // GPS send from now on
      // because wer are in tractor mode the rpc_isoName cannot be NULL
      c_sendGpsISOName = *rpc_isoName;
      #ifdef NMEA_2000_FAST_PACKET
      // also remove any previously registered MultiReceive connections
      getMultiReceiveInstance4Comm().deregisterClient( this );
      c_nmea2000Streamer.reset();
      c_nmea2000Streamer.vec_data.resize(0);
      #endif // END of NMEA_2000_FAST_PACKET
    }
    else
    { // IdentModeImplement
      c_sendGpsISOName.setUnspecified();
      #ifdef NMEA_2000_FAST_PACKET
      // make sure that the needed multi receive are registered
      getMultiReceiveInstance4Comm().registerClient(NMEA_GPS_POSITON_DATA_PGN,   0xFF, this, true, false, true);
      getMultiReceiveInstance4Comm().registerClient(NMEA_GPS_DIRECTION_DATA_PGN, 0xFF, this, true, false, true);
      c_nmea2000Streamer.vec_data.reserve(51); // GNSS Position Data with TWO reference stations
      #endif // END of NMEA_2000_FAST_PACKET
    }
    return true;
  }

  /** Retrieve the last update time of the specified information type
    */
  int32_t TimePosGPS_c::lastedTimeSinceUpdateGps() const
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    #ifdef NMEA_2000_FAST_PACKET
    if ( i32_lastIsoPositionStream > i32_lastIsoPositionSimple ) return ( ci32_now - i32_lastIsoPositionStream);
    else return ( ci32_now - i32_lastIsoPositionSimple);
    #else
    return ( ci32_now - i32_lastIsoPositionSimple);
    #endif
  }
  /** Retrieve the time of last update */
  int32_t TimePosGPS_c::lastUpdateTimeGps() const
  {
    #ifdef NMEA_2000_FAST_PACKET
    if ( i32_lastIsoPositionStream > i32_lastIsoPositionSimple ) return i32_lastIsoPositionStream;
    else return i32_lastIsoPositionSimple;
    #else
    return i32_lastIsoPositionSimple;
    #endif
  }

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TimePosGPS_c& getTimePosGpsInstance( uint8_t rui8_instance )
  { // if > 1 singleton instance is used, no static reference can be used
    return TimePosGPS_c::instance( rui8_instance );
  };
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TimePosGPS_c& getTimePosGpsInstance( void )
  {
    static TimePosGPS_c& c_lbsTimePosGps = TimePosGPS_c::instance();
    return c_lbsTimePosGps;
  };
  #endif


  /** process a ISO11783 base information PGN */
  bool TimePosGPS_c::processMsg()
  {
    ISOName_c c_tempISOName( ISOName_c::ISONameUnspecified );
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    // store the isoName of the sender of base data
    if (getIsoMonitorInstance4Comm().existIsoMemberNr(data().isoSa()))
    { // the corresponding sender entry exist in the monitor list
      c_tempISOName = getIsoMonitorInstance4Comm().isoMemberNr(data().isoSa()).isoName();
    }

    switch (data().isoPgn() & 0x1FFFF)
    {
      case TIME_DATE_PGN:
        // time - date
        // only take values, if i am not the regular sender
        // and if actual sender isn't in conflict to previous sender
        if ( checkParseReceived( c_tempISOName ) )
        { // sender is allowed to send
          // store new calendar setting
          if ( c_sendGpsISOName.isUnspecified()  )
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
          setSelectedDataSourceISOName( c_tempISOName);
        }
        else
        { // there is a sender conflict
          getLbsErrInstance().registerError( LibErr_c::LbsBaseSenderConflict, LibErr_c::LbsBase );
          return false;
        }
        break;
      // **********************************************************
      // Added by Brad Cox for NMEA 2000 GPS Messages:
      case NMEA_GPS_POSITON_RAPID_UPDATE_PGN:
        if ( checkParseReceived( c_tempISOName ) )
        { // sender is allowed to send
          i32_lastIsoPositionSimple = ci32_now;
          i32_latitudeDegree10Minus7  = data().getInt32Data( 0 );
          i32_longitudeDegree10Minus7 = data().getInt32Data( 4 );
          // set last time
          i32_lastIsoPositionSimple = ci32_now;
          c_sendGpsISOName = c_tempISOName;
          setUpdateTime( ci32_now );

          #if defined (USE_TRACTOR_MOVE) || defined (USE_BASE)
          IsoAgLib::iTracMove_c& c_tracmove = IsoAgLib::getITracMoveInstance();
          c_tracmove.updateSpeed(IsoAgLib::GpsBasedSpeed);
          #endif
        }
        break;
      // **********************************************************
    }
    return true;
  }

  bool TimePosGPS_c::processMsgRequestPGN (uint32_t rui32_pgn, uint8_t rui8_sa, uint8_t rui8_da)
  {
    // check if we are allowed to send a request for pgn
    if ( ! BaseCommon_c::check4ReqForPgn(rui32_pgn, rui8_sa, rui8_da) ) return false;

    // call TimePosGPS_c function to send time/date
    // isoSendCalendar checks if this item (identified by ISOName)
    // is configured to send time/date
    sendCalendar(*getISOName());
    return true;
  };

  #if defined(USE_FLOAT_DATA_TYPE)
  /** check if an NMEA2000 position signal was received */
  bool TimePosGPS_c::isPositionReceived() const
  {
    if ( ( i32_latitudeDegree10Minus7 == 0x7FFFFFFF  )
      && ( i32_longitudeDegree10Minus7 == 0x7FFFFFFF )
      #ifdef NMEA_2000_FAST_PACKET
      && ( ui32_altitudeCm == 0                      )
      #endif // END of NMEA_2000_FAST_PACKET
      )
    { // there is no valid GPS information
      return false;
    }
    else
    {
      return true;
    }
  }
  #endif // END of USE_FLOAT_DATA_TYPE

  #ifdef NMEA_2000_FAST_PACKET
  /** set the GPS time in UTC timezone.
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  */
  void TimePosGPS_c::setTimeUtcGps(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec )
  {
    bit_gpsTime.hour = rb_hour;
    bit_gpsTime.minute = rb_minute;
    bit_gpsTime.second = rb_second;
    bit_gpsTime.msec = rui16_msec;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setTimeUtc( rb_hour, rb_minute, rb_second, rui16_msec );
    }
  }
  /** set the calendar hour value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param rb_hour actual calendar hour value
  */
  void TimePosGPS_c::setHourUtcGps(uint8_t rb_hour)
  {
    bit_gpsTime.hour = rb_hour;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setHourUtc( rb_hour);
    }
  }
  /** set the calendar minute value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param rb_minute actual calendar minute value
  */
  void TimePosGPS_c::setMinuteUtcGps(uint8_t rb_minute)
  {
    bit_gpsTime.minute = rb_minute;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setMinuteUtc( rb_minute );
    }
  }
  /** set the calendar second value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param rb_second actual calendar second value
  */
  void TimePosGPS_c::setSecondUtcGps(uint8_t rb_second)
  {
    bit_gpsTime.second = rb_second;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setSecond( rb_second );
    }
  }
  /** set the calendar millisecond value
  *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
  *  setTimeUtc().
  *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
  *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
  *  @param rb_millisecond actual calendar second value
  */
  void TimePosGPS_c::setMillisecondUtcGps(uint16_t rui16_millisecond)
  {
    bit_gpsTime.msec = rui16_millisecond;
    if ( getSelectedDataSourceISOName().isUnspecified() )
    { // no active source for the GPS independent time / date is specified -> so update the other UTC time also from GPS
      setMillisecond( rui16_millisecond );
    }
  }

  /** deliver GPS receive qualitiy */
  void TimePosGPS_c::setGnssMode( IsoAgLib::IsoGnssMethod_t rt_newVal )
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

  //! Parameter:
  //! @param rc_ident:
  //! @param rui32_totalLen:
  bool TimePosGPS_c::reactOnStreamStart(IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                                  uint32_t /*rui32_totalLen */)
  {
    if ( ( ( rc_ident.getPgn() == NMEA_GPS_POSITON_DATA_PGN   )
            || ( rc_ident.getPgn() == NMEA_GPS_DIRECTION_DATA_PGN ) )
            &&  checkModeGps(IsoAgLib::IdentModeImplement) )
    { // this a NMEA multi stream of interest where we don't send it
      return true;
    }
    else
    { // this is not of interest for us
      return false; /** @todo check if we're NACKing it and if that's correct?? maybe we should just ignore it??? */
    }
  }

  void TimePosGPS_c::reactOnAbort(IsoAgLib::ReceiveStreamIdentifier_c /*rc_ident*/)
  {
    // as we don't perform an on-the-fly parse of the pool, nothing has to be done here
    // - this is only important with on-the-fly parse, where everything parsed already has to be invalidated
  }

  //! Parameter:
  //! @param rc_ident:
  //! @param rb_isFirstChunk:
  //! @param rb_isLastChunkAndACKd:
  bool TimePosGPS_c::processPartStreamDataChunk(IsoAgLib::iStream_c* rpc_stream,
                                          bool /*rb_isFirstChunk*/,
                                          bool rb_isLastChunkAndACKd)
  {
    IsoAgLib::ReceiveStreamIdentifier_c rc_ident = rpc_stream->getIdent();

    // >>>Last Chunk<<< Processing
    if (rb_isLastChunkAndACKd)
    {  /** let reactOnLastChunk decide whether the pool should be kept in memory */
      return reactOnLastChunk(rc_ident, *rpc_stream);
    }
    // default - don't keep it
    return false;
  }

  void TimePosGPS_c::reactOnAbort(IsoAgLib::iStream_c* /*rpc_stream*/)
  { // as we don't perform on-the-fly parse, there is nothing special to do
  }

  //! Parameter:
  //! @param rc_ident:
  //! @param rpc_stream:
  bool TimePosGPS_c::reactOnLastChunk( IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                                IsoAgLib::iStream_c& refc_stream)
  { // see if it's a pool upload, string upload or whatsoever! (First byte is already read by MultiReceive!)
    const uint8_t cui8_sa = refc_stream.getIdent().getSa();
    ISOName_c c_tempISOName( ISOName_c::ISONameUnspecified );
    if (getIsoMonitorInstance4Comm().existIsoMemberNr(cui8_sa))
    { // the corresponding sender entry exist in the monitor list
      c_tempISOName = getIsoMonitorInstance4Comm().isoMemberNr(cui8_sa).isoName();
    }

    // check if we want to process the information
    if (
        ( checkModeGps(IsoAgLib::IdentModeTractor) ) // I'm the sender
        || ( // one of the following conditions must be true
          (c_sendGpsISOName != c_tempISOName) // actual sender different to last
        && (c_sendGpsISOName.isSpecified() ) // last sender has correctly claimed address member
          )
      )
    { // DO NOT take this message, as this might be a falsly double source
      return false;
    }

    // set last time and ISOName information
    c_sendGpsISOName = c_tempISOName;
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
          if ( vec_refStationTypeAndStation.size() < (ind+1) ) vec_refStationTypeAndStation.push_back(__IsoAgLib::convertIstreamUi16( refc_stream ) );
          else IsoAgLib::convertIstream( refc_stream, vec_refStationTypeAndStation[ind] );
          if ( vec_refStationDifferentialAge10Msec.size() < (ind+1) ) vec_refStationDifferentialAge10Msec.push_back(__IsoAgLib::convertIstreamUi16( refc_stream ) );
          else IsoAgLib::convertIstream( refc_stream, vec_refStationDifferentialAge10Msec[ind] );
        }
      }
      break;
      case NMEA_GPS_DIRECTION_DATA_PGN: // 0x01FE11LU - 130577 with Heading and Speed
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
  }
  #endif // END NMEA_2000_FAST_PACKET

  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TimePosGPS_c::timeEventTracMode()
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();

    if ( ( lastedTimeSinceUpdate() >= 1000 )
      && checkMode(IsoAgLib::IdentModeTractor) )
    { // getISOName() must be != NULL, because we are in tractor mode
      // send actual calendar data
      setSelectedDataSourceISOName(*getISOName());
      sendCalendar(*getISOName());
    }

    if ( checkModeGps(IsoAgLib::IdentModeTractor) )
    { // pc_isoNameGps must be != NULL, because we are in tractor mode
      if ( ( ci32_now - i32_lastIsoPositionSimple ) >= 100 )
      {
        if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
        sendPositionRapidUpdate();
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
      #endif // END of NMEA_2000_FAST_PACKET
    }
    return true;
  }

  /** send position rapid update message */
  void TimePosGPS_c::sendPositionRapidUpdate( void )
  {
    // retreive the actual dynamic sender no of the member with the registered isoName
    uint8_t b_sa = getIsoMonitorInstance4Comm().isoMemberISOName(*pc_isoNameGps, true).nr();
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);
    data().setIsoSa(b_sa);

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
  /** send direction as detailed stream */
  void TimePosGPS_c::isoSendDirectionStream( void )
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    ISOMonitor_c& c_iso_monitor = getIsoMonitorInstance4Comm();
    // set data in Nmea2000SendStreamer_c
    c_nmea2000Streamer.reset();
    std::vector<uint8_t>& writeRef = c_nmea2000Streamer.vec_data;
    // use helper function to transfer value to the byte vector
    number2LittleEndianString( uint8_t (ui8_dataModeAndHeadingReference|0xC0), writeRef );     /// NOT there in the RAPID UPDATE one
    number2LittleEndianString( ui8_directionSequenceID, writeRef );
    number2LittleEndianString( ui16_courseOverGroundRad10Minus4, writeRef );
    number2LittleEndianString( ui16_speedOverGroundCmSec, writeRef );

    number2LittleEndianString( ui16_headingRad10Minus4, writeRef );           /// not init'ed  /// NOT there in the RAPID UPDATE one
    number2LittleEndianString( ui16_speedCmSec, writeRef );                   /// not init'ed  /// NOT there in the RAPID UPDATE one

    number2LittleEndianString( ui16_flowDirectionRad10Minus4, writeRef );     /// not init'ed   /// NOT there in the RAPID UPDATE one
    number2LittleEndianString( ui16_driftSpeedCmSec, writeRef );              /// not init'ed  /// NOT there in the RAPID UPDATE one

    //now trigger sending
    // retreive the actual dynamic sender no of the member with the registered isoName
    const uint8_t b_send = c_iso_monitor.isoMemberISOName(c_sendGpsISOName, true).nr();
    if ( getMultiSendInstance4Comm().sendIsoFastPacket(b_send, 0xFF, &c_nmea2000Streamer, NMEA_GPS_DIRECTION_DATA_PGN, t_multiSendSuccessState) )
    { // update time
      i32_lastIsoDirectionStream = ci32_now;
      ui8_directionSequenceID++;
    }
  }

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
    i32_temp = int32_t(double(refui32_result) * 1.0e+4 / 4294967296.0);
    number2LittleEndianString( i32_temp, writeRef );
    #endif
  }

  /** send position as detailed stream */
  void TimePosGPS_c::isoSendPositionStream( void )
  {
    const int32_t ci32_now = Scheduler_c::getLastTimeEventTrigger();
    ISOMonitor_c& c_iso_monitor = getIsoMonitorInstance4Comm();
    // set data in Nmea2000SendStreamer_c
    c_nmea2000Streamer.reset();
    std::vector<uint8_t>& writeRef = c_nmea2000Streamer.vec_data;
    // use helper function to transfer value to the byte vector
    number2LittleEndianString( ui8_positionSequenceID, writeRef );

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
      const time_t secondsSince1970 = mktime( &testTime ) + t_tzOffset;
      // calculate the days
      ui16_daysSince1970 = secondsSince1970 / ( 60 * 60 *24 );
    }
    const uint32_t ui32_milliseconds = ( ( ( ( ( ( hourUtcGps() * 60 ) + minuteUtcGps() ) * 60 ) + secondUtcGps() ) * 1000 ) + millisecondUtcGps()) * 10;
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
    // retreive the actual dynamic sender no of the member with the registered isoName
    const uint8_t b_send = c_iso_monitor.isoMemberISOName(c_sendGpsISOName, true).nr();
    if ( getMultiSendInstance4Comm().sendIsoFastPacket(b_send, 0xFF, &c_nmea2000Streamer, NMEA_GPS_POSITON_DATA_PGN, t_multiSendSuccessState) )
    { // update time
      i32_lastIsoPositionStream = ci32_now;
      ui8_positionSequenceID++;
    }
  }
  #endif // END OF NMEA_2000_FAST_PACKET

  /** send ISO11783 calendar PGN
      @param rc_isoName ISOName code off calling item which wants to send
      @param ri32_time timestamp where calendar was last sent (default autodetect)
      possible errors:
          * dependant error in CANIO_c on CAN send problems
      @see CANPkg_c::getData
      @see CANPkgExt_c::getData
      @see CANIO_c::operator<<
    */
  void TimePosGPS_c::sendCalendar(const ISOName_c& rc_isoName)
  {
    if (!getIsoMonitorInstance4Comm().existIsoMemberISOName(rc_isoName, true)) return;

    data().setISONameForSA( rc_isoName );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(6);
    data().setLen(8);

    if ( ( getSelectedDataSourceISOName() == rc_isoName ) )
    { // this item (identified by ISOName is configured to send
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

      // CANIO_c::operator<< retreives the information with the help of CANPkg_c::getData
      // then it sends the data
      getCanInstance4Comm() << data();

      // update time
      setUpdateTime(Scheduler_c::getLastTimeEventTrigger() );
    }
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

  /** check if a calendar information was received since init */
  bool TimePosGPS_c::isCalendarReceived() const
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
  void TimePosGPS_c::setCalendarUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec)
  {
    i32_lastCalendarSet = System_c::getTime();
    t_cachedLocalSeconds1970AtLastSet = 0;

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
  void TimePosGPS_c::setCalendarLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec )
  {
    i32_lastCalendarSet = System_c::getTime();
    t_cachedLocalSeconds1970AtLastSet = 0;
    struct ::tm testTime = { rb_second, int(rb_minute)-int(bit_calendar.timezoneMinuteOffset), (int(rb_hour)-(int(bit_calendar.timezoneHourOffsetMinus24)-24)),
                            rb_day,(rb_month-1),(ri16_year-1900),0,0,-1
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
    bit_calendar.msec   = rui16_msec;
  };

  /** set the date in local timezone */
  void TimePosGPS_c::setDateLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day)
  {
    i32_lastCalendarSet = System_c::getTime();
    t_cachedLocalSeconds1970AtLastSet = 0;

    // @todo: calender time consists of UTC time and local date?
    struct ::tm testTime = { bit_calendar.second, bit_calendar.minute, bit_calendar.hour,
                            rb_day,(rb_month-1),(ri16_year-1900),0,0,-1
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
  void TimePosGPS_c::setDateUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day)
  {
    i32_lastCalendarSet = System_c::getTime();
    t_cachedLocalSeconds1970AtLastSet = 0;
    bit_calendar.year   = ri16_year;
    bit_calendar.month  = rb_month;
    bit_calendar.day    = rb_day;
  }

  /** set the time in local timezone */
  void TimePosGPS_c::setTimeLocal(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec)
  {
    i32_lastCalendarSet = System_c::getTime();
    t_cachedLocalSeconds1970AtLastSet = 0;

    struct ::tm testTime = { rb_second, int(rb_minute)-int(bit_calendar.timezoneMinuteOffset), (int(rb_hour)-(int(bit_calendar.timezoneHourOffsetMinus24)-24)),
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
    bit_calendar.msec   = rui16_msec;
  }

  /** set the time in UTC timezone */
  void TimePosGPS_c::setTimeUtc(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec)
  {
    i32_lastCalendarSet = System_c::getTime();
    t_cachedLocalSeconds1970AtLastSet = 0;
    bit_calendar.hour   = rb_hour;
    bit_calendar.minute = rb_minute;
    bit_calendar.second = rb_second;
    bit_calendar.msec   = rui16_msec;
  }

  const struct ::tm* TimePosGPS_c::Utc2LocalTime()
  {
    if (0 == t_cachedLocalSeconds1970AtLastSet)
    {
      // set t_cachedLocalSeconds1970AtLastSet
      currentUtcTm();
    }

    const time_t t_secondsSince1970Local = t_cachedLocalSeconds1970AtLastSet + calendarSetAge()/1000
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
    if ( 0 == t_cachedLocalSeconds1970AtLastSet)
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
      t_cachedLocalSeconds1970AtLastSet = mktime( &testTime );
      if (-1 == t_cachedLocalSeconds1970AtLastSet) return NULL;
    }

    const time_t t_secondsSince1970 = t_cachedLocalSeconds1970AtLastSet + calendarSetAge()/1000;

    // compensate system time zone setting (part 2)
    return localtime( &t_secondsSince1970 );
  }
} // namespace __IsoAgLib

