/*
  gnss_c.cpp: Receiving GNSS Position, Direction and Date/Time.

  (C) Copyright 2015 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "gnss_c.h"

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/stream_c.h>
#include <IsoAgLib/util/iutil_funcs.h>
#include <IsoAgLib/util/iliberr_c.h>

#include <ctime>

#ifndef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
#  error "Fast Packet must be activated in order to properly receive GNSS information from the ISOBUS."
#endif

#if DEBUG_NMEA
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/irs232io_c.h>
  #endif
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#pragma warning( disable : 4355 )
#endif


// Off-class/namespace c-style helper functions
static void getDegree10Minus7FromStream( __IsoAgLib::Stream_c& rc_stream, int32_t& ri32_result )
{
  #if HAL_SIZEOF_INT == 4
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

static void getAltitude10Minus2FromStream( __IsoAgLib::Stream_c& rc_stream, int32_t& ri32_result )
{
  #if HAL_SIZEOF_INT == 4
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

namespace __IsoAgLib {

  Gnss_c::Gnss_c()
    : BaseCommon_c()
    , mi32_latitudeDegree10Minus7( 0x7FFFFFFF )
    , mi32_longitudeDegree10Minus7( 0x7FFFFFFF )
    , mi32_lastPositionStream( -1 )
    , mi32_lastPositionSimple( -1 )
    , mi32_lastDirection( -1 )
    , mui8_courseOverGroundReference( 0 )
    , mui8_directionSequenceID( 0 )
    , mui16_courseOverGroundRad10Minus4( 0xFFFF )
    , mui16_speedOverGroundCmSec( 0xFFFF )
    , mt_gnssMethod( IsoAgLib::IsoNoGps )
    , mi32_altitudeCm( 0x7FFFFFFF )
  //, m_dateTime
    , mt_gnssType( IsoAgLib::IsoGnssGps )
    , mui8_satelliteCnt( 0 )
    , mi16_hdop( 0 )
    , mi16_pdop( 0 )
    , mui8_integrity( 0 )
    , mui8_positionSequenceID( 0 )
    , mi32_geoidalSeparationCm( 0 )
    , mui8_noRefStations( 0 )
    , mvec_refStationTypeAndStation()
    , mvec_refStationDifferentialAge10Msec()
    , m_gnssCFSHproxy( *this )
  {
    mt_task.setPeriod( 1000, false );
  }


  void Gnss_c::timeEvent()
  {
    isoaglib_assert( checkMode(IsoAgLib::IdentModeImplement) );

    const ecutime_t ci32_now = System_c::getTime();

    if( getSelectedDataSourceISOName().isSpecified() )
    {
      if( ( mi32_lastPositionStream >= 0 ) && (ci32_now - mi32_lastPositionStream) >= getTimeOut( ) )
      {
        m_dateTime.timestamp = -1;
        mi32_lastPositionStream = -1;
      }

      if( ( mi32_lastPositionSimple >= 0 ) && (ci32_now - mi32_lastPositionSimple) >= getTimeOut( ) )
      {
        mi32_lastPositionSimple = -1;
      }

      if( ( mi32_lastPositionStream == -1 )
          &&
          ( mi32_lastPositionSimple == -1 ) )
      { // the previously sending node didn't send ANY POSITION information for X seconds
        mi32_latitudeDegree10Minus7 = mi32_longitudeDegree10Minus7 = 0x7FFFFFFF;
        mt_gnssMethod = IsoAgLib::IsoNoGps;
        mt_gnssType = IsoAgLib::IsoGnssGps;
        mui8_satelliteCnt = 0;
        mi32_altitudeCm = 0x7FFFFFFF;
      }

      if( ( mi32_lastDirection >= 0 ) && (ci32_now - mi32_lastDirection) >= getTimeOut( ) )
      { // the previously sending node didn't send the information for 3 seconds -> give other items a chance
        mui16_speedOverGroundCmSec = mui16_courseOverGroundRad10Minus4 = 0xFFFF;
        mi32_lastDirection = -1;
      }

      if( ( mi32_lastPositionStream == -1 )
          &&
          ( mi32_lastPositionSimple == -1 )
          &&
          ( mi32_lastDirection == -1 ) )
      { // neither Simple/Stream Position nor Direction are specified, so kick the sender!
        getSelectedDataSourceISOName().setUnspecified();
      }
    }
  }


  void
  Gnss_c::init_specialized()
  {
    IsoBus_c &isobus = getIsoBusInstance4Comm();

    isobus.insertFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (NMEA_GPS_POSITION_RAPID_UPDATE_PGN<<8) ), 8 );
    isobus.insertFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN<<8) ), 8 );
#ifdef USE_J1939_VEHICLE_PGNS
    isobus.insertFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (VEHICLE_POSITION_PGN<<8) ), 8 );
    isobus.insertFilter( *this, IsoAgLib::iMaskFilter_c( 0x3FFFF00UL, (VEHICLE_DIRECTION_SPEED_PGN<<8) ), 8 );
#endif

    getMultiReceiveInstance4Comm().registerClientNmea( *this, __IsoAgLib::IsoName_c::IsoNameUnspecified(), NMEA_GPS_POSITION_DATA_PGN,  0x3FFFFLU, true, false );
    getMultiReceiveInstance4Comm().registerClientNmea( *this, __IsoAgLib::IsoName_c::IsoNameUnspecified(), NMEA_GPS_DIRECTION_DATA_PGN, 0x3FFFFLU, true, false );

    __IsoAgLib::getIsoMonitorInstance4Comm().registerControlFunctionStateHandler( m_gnssCFSHproxy );

    mi32_lastPositionStream = -1;
    mi32_lastPositionSimple = -1;
    mi32_lastDirection = -1;
    mt_gnssMethod = IsoAgLib::IsoNoGps;
    m_dateTime.timestamp = -1;
    mt_gnssType = IsoAgLib::IsoGnssGps;
  }


  void
  Gnss_c::close_specialized()
  {
    __IsoAgLib::getIsoMonitorInstance4Comm().deregisterControlFunctionStateHandler( m_gnssCFSHproxy );

    getMultiReceiveInstance4Comm().deregisterClient( *this );

    getIsoBusInstance4Comm().deleteAllFiltersForCustomer( *this );
  }


  Gnss_c &getGnssInstance( unsigned instance )
  {
    MACRO_MULTITON_GET_INSTANCE_BODY(Gnss_c, PRT_INSTANCE_CNT, instance);
  }


  void Gnss_c::processMsg( const CanPkg_c& arc_data )
  {
    IsoName_c senderName;

    CanPkgExt_c pkg( arc_data, getMultitonInst() );
    if (!pkg.isValid() || (pkg.getMonitorItemForSA() == NULL))
    {
      uint8_t SA = arc_data.ident() & 0xFF;
      senderName.setOsbSpecialName(SA);
    }
    else
    {
      senderName = pkg.getISONameForSA();
    }


    const ecutime_t ci32_now = pkg.time();

    switch( pkg.isoPgn() )
    {
      case NMEA_GPS_POSITION_RAPID_UPDATE_PGN:
#ifdef USE_J1939_VEHICLE_PGNS
      case VEHICLE_POSITION_PGN:
#endif
        if ( checkParseReceived( senderName ) )
        { // sender is allowed to send
#ifdef USE_J1939_VEHICLE_PGNS
          if (pkg.isoPgn() == VEHICLE_POSITION_PGN)
          {
            const int32_t latitudeDegree10Minus7  = pkg.getUint32Data( 0 ) - 2100000000; // 210 / 0.0000001
            const int32_t longitudeDegree10Minus7 = pkg.getUint32Data( 4 ) - 2100000000; // 210 / 0.0000001

            if ( (latitudeDegree10Minus7  < ( -90*10000000)) || (latitudeDegree10Minus7  > ( 90*10000000))
              || (longitudeDegree10Minus7 < (-180*10000000)) || (longitudeDegree10Minus7 > (180*10000000)) )
              return;

            mi32_latitudeDegree10Minus7  = latitudeDegree10Minus7;
            mi32_longitudeDegree10Minus7 = longitudeDegree10Minus7;

            setTimeOut( TIMEOUT_SENDING_NODE_J1939 );
          }
          else
#endif
          {
            const int32_t latitudeDegree10Minus7  = pkg.getInt32Data( 0 );
            const int32_t longitudeDegree10Minus7 = pkg.getInt32Data( 4 );

            if ( (latitudeDegree10Minus7  < ( -90*10000000)) || (latitudeDegree10Minus7  > ( 90*10000000))
              || (longitudeDegree10Minus7 < (-180*10000000)) || (longitudeDegree10Minus7 > (180*10000000)) )
              return;

            mi32_latitudeDegree10Minus7  = latitudeDegree10Minus7;
            mi32_longitudeDegree10Minus7 = longitudeDegree10Minus7;

            setTimeOut( TIMEOUT_SENDING_NODE_NMEA );
          }
          mi32_lastPositionSimple = ci32_now;
          setSelectedDataSourceISOName( senderName );

          if (getGnssMode() == IsoAgLib::IsoNoGps)
          { /// @todo ON REQUEST-259: Allow Rapid Update without Complete Position TP/FP before? Is is just an update or can it be standalone?
            /// for now, allow it as standalone and set GpsMethod simply to IsoGnssNull as we don't have reception info...
            mt_gnssMethod = IsoAgLib::IsoGnssNull; // was IsoGnssFix before. Actually, noone knows what to set here ;-)
            mt_gnssType = IsoAgLib::IsoGnssGps;
          }
        }
        else
        { // there is a sender conflict
          IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::TracMultipleSender, getMultitonInst() );
          return;
        }
        break;

#ifdef USE_J1939_VEHICLE_PGNS
      case VEHICLE_DIRECTION_SPEED_PGN:
        if ( checkParseReceived( senderName ) )
        { // sender is allowed to send
          // Here we get degrees as fraction of 128, and have to change to rad 10^-4
          // -> / 128 * MATH_PI / 180 * 10000
#define MATH_PI 3.14159265
          const uint16_t courseOverGroundRad10Minus4 = static_cast<uint16_t>( static_cast<double>(pkg.getUint16Data( 0 ) ) * MATH_PI * 125.0 / 288.0 );
          // [256 one bit is 1/256 km/h] [* 1000 * 100 / 60 / 60 -> we get km/h but want cm/sec]
          const uint16_t speedOverGroundCmSec        = static_cast<uint16_t>( static_cast<double>( pkg.getUint16Data( 2 ) * 125 ) / static_cast<double>( 128 * 9 ) );

          /// @todo ON REQUEST-259: check for the REAL max, 62855 is a little bigger than 62831 or alike that could be calculated. but anyway...
          if ( (courseOverGroundRad10Minus4 > (62855))
            || (speedOverGroundCmSec        > (65532)) )
            return;

          mui16_courseOverGroundRad10Minus4 = courseOverGroundRad10Minus4;
          mui16_speedOverGroundCmSec        = speedOverGroundCmSec;

          mi32_altitudeCm = static_cast<int32_t>( ( static_cast<double>( pkg.getUint16Data( 6 ) ) * 0.125 - 2500.0 ) * 100.0 );

          mi32_lastDirection = ci32_now;
          setSelectedDataSourceISOName( senderName );

          setTimeOut(TIMEOUT_SENDING_NODE_J1939);
        }
        else
        { // there is a sender conflict
          IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::TracMultipleSender, getMultitonInst() );
          return;
        }
        break;
#endif

      case NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN:
        if ( checkParseReceived( senderName ) )
        {
          /// @todo ON REQUEST-259: check for the REAL max, 62855 is a little bigger than 62831 or alike that could be calculated. but anyway...
          if ( (pkg.getUint16Data( 2 ) > 62855)
            || (pkg.getUint16Data( 4 ) > 65532) )
          return;

          mui8_directionSequenceID          = pkg.getUint8Data ( 0 );
          mui8_courseOverGroundReference    = pkg.getUint8Data ( 1 ) & 0x03;
          mui16_courseOverGroundRad10Minus4 = pkg.getUint16Data( 2 );
          mui16_speedOverGroundCmSec        = pkg.getUint16Data( 4 );

          setTimeOut(TIMEOUT_SENDING_NODE_NMEA);

          mi32_lastDirection = ci32_now;
          setSelectedDataSourceISOName( senderName );
        }
        else
        { // there is a sender conflict
          IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::TracMultipleSender, getMultitonInst() );
          return;
        }
        break;
    }

    notifyOnEvent (pkg.isoPgn());
  }


  void Gnss_c::setDateTimeUtcGps(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day, uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec )
  {
    m_dateTime.timestamp = HAL::getTime();

    m_dateTime.date.year = ai16_year;
    m_dateTime.date.month = ab_month;
    m_dateTime.date.day = ab_day;

    m_dateTime.time.hour = ab_hour;
    m_dateTime.time.minute = ab_minute;
    m_dateTime.time.second = ab_second;
    m_dateTime.time.msec = aui16_msec;
  }

  void
  Gnss_c::reactOnIsoItemModification(ControlFunctionStateHandler_c::iIsoItemAction_e at_action, IsoItem_c const& acrc_isoItem)
  {
    if (getSelectedDataSourceISONameConst().isSpecified()
     && getSelectedDataSourceISONameConst().isOsbSpecialName())
    {

      switch (at_action)
      {
      case __IsoAgLib::ControlFunctionStateHandler_c::AddToMonitorList:
      case __IsoAgLib::ControlFunctionStateHandler_c::ChangedAddress:
      case __IsoAgLib::ControlFunctionStateHandler_c::ReclaimedAddress:
        {
          const uint8_t selectedDataSourceSA = getSelectedDataSourceISOName().getOsbSpecialSA();
          const uint8_t isoItemSA = acrc_isoItem.nr();

          if (selectedDataSourceSA == isoItemSA)
          {
            setSelectedDataSourceISOName(acrc_isoItem.isoName());
          }
        }
        break;
      case __IsoAgLib::ControlFunctionStateHandler_c::LostAddress:
        break;
      case __IsoAgLib::ControlFunctionStateHandler_c::RemoveFromMonitorList:
        break;
      default:
        break;
      }
    }
  }


  bool
  Gnss_c::reactOnStreamStart (const ReceiveStreamIdentifier_c& ident, uint32_t totalLen)
  {
    switch( ident.getPgn() )
    {
    case NMEA_GPS_POSITION_DATA_PGN:
      return ( totalLen >= 43 ) && ( ( ( totalLen - 43 ) & 0x03 ) == 0 );

    case NMEA_GPS_DIRECTION_DATA_PGN:
      return totalLen == 14;

    default:
      isoaglib_assert( !"Shouldn't happen, not registered for this PGN!" );
    }
    return false;
  }


  bool Gnss_c::processPartStreamDataChunk (Stream_c& stream,
                                           bool /*ab_isFirstChunk*/,
                                           bool ab_isLastChunkAndACKd)
  {
    if (!ab_isLastChunkAndACKd)
      return false;

    const ReceiveStreamIdentifier_c& ident = stream.getIdent();
    IsoName_c const& senderName = ident.getSaIsoName();

    if( !checkParseReceived( senderName ) )
      return false;

    setSelectedDataSourceISOName( senderName );

    switch( ident.getPgn() )
    {
      case NMEA_GPS_POSITION_DATA_PGN: // 0x01F805LU -> 129029
      {
        setTimeOut( TIMEOUT_SENDING_NODE_NMEA );
        mi32_lastPositionStream = stream.getStartTime();
        // fetch sequence number from Byte1
        IsoAgLib::convertIstream( stream, mui8_positionSequenceID );
        // --> continue with Byte2 ...
        uint16_t ui16_daysSince1970;
        uint32_t ui32_milliseconds;
        // read 2-bytes of Generic date as days since 1-1-1970 ( UNIX date )
        IsoAgLib::convertIstream( stream, ui16_daysSince1970 );
        IsoAgLib::convertIstream( stream, ui32_milliseconds );
        // NMEA NMEA_GPS_POSITON_DATA_PGN sends with 0.1 msec
        ui32_milliseconds /= 10;

        const CNAMESPACE::time_t t_tempUnixTime = ( CNAMESPACE::time_t(ui16_daysSince1970) * CNAMESPACE::time_t(60L * 60L * 24L) ) + (ui32_milliseconds/1000);
#ifdef WINCE
        time_t_ce ceTime = (time_t_ce)t_tempUnixTime;
        tm* UtcNow = gmtime_ce(&ceTime);
#else
        CNAMESPACE::tm* UtcNow = CNAMESPACE::gmtime( &t_tempUnixTime );
#endif
        if ( UtcNow != NULL )
        {
          setDateTimeUtcGps((UtcNow->tm_year+1900), UtcNow->tm_mon + 1, UtcNow->tm_mday,
                             UtcNow->tm_hour, UtcNow->tm_min, UtcNow->tm_sec, (ui32_milliseconds%1000));
        }
        // now read Latitude --> convert into double [degree]
        getDegree10Minus7FromStream( stream, mi32_latitudeDegree10Minus7 );
        // now read Longitude --> convert into double [degree]
        getDegree10Minus7FromStream( stream, mi32_longitudeDegree10Minus7 );
        // now read Altitude --> convert into double [meter]
        getAltitude10Minus2FromStream( stream, mi32_altitudeCm );
        // now fetch Quality - gps-mode
        uint8_t ui8_tempValue;
        stream >> ui8_tempValue;
        if ( ( ui8_tempValue >> 4  ) <= IsoAgLib::IsoGnssMethodMAX ) mt_gnssMethod = IsoAgLib::IsoGnssMethod_t(ui8_tempValue >> 4 );
        if ( ( ui8_tempValue & 0xF ) <= IsoAgLib::IsoGnssTypeMAX   ) mt_gnssType   = IsoAgLib::IsoGnssType_t(ui8_tempValue & 0xF );
        // GNSS Integrity
        stream >> mui8_integrity;
        mui8_integrity &= 0x3; // mask reserved bits out
        // now fetch the number of satelites
        stream >> mui8_satelliteCnt;
        // now fetch HDOP from raw uint16_t to float [1.0*1.0e-2)
        IsoAgLib::convertIstream( stream, mi16_hdop );
        // now fetch PDOP from raw uint16_t to float [1.0*1.0e-2)
        IsoAgLib::convertIstream( stream, mi16_pdop );
        // Geodial Separation
        IsoAgLib::convertIstream( stream, mi32_geoidalSeparationCm );
        // number of reference stations
        IsoAgLib::convertIstream( stream, mui8_noRefStations );
        // now read the type and age of all following reference stations
        for ( unsigned int ind = 0; ((ind < mui8_noRefStations) &&(!stream.eof())); ind++ )
        { // push new items at the end or simply update the corresponding value
          if ( mvec_refStationTypeAndStation.size() < (ind+1) )
            mvec_refStationTypeAndStation.push_back(__IsoAgLib::convertIstreamUi16( stream ) );
          else
            IsoAgLib::convertIstream( stream, mvec_refStationTypeAndStation[ind] );
          if ( mvec_refStationDifferentialAge10Msec.size() < (ind+1) )
            mvec_refStationDifferentialAge10Msec.push_back(__IsoAgLib::convertIstreamUi16( stream ) );
          else
            IsoAgLib::convertIstream( stream, mvec_refStationDifferentialAge10Msec[ind] );
        }
        #if DEBUG_NMEA
        INTERNAL_DEBUG_DEVICE << "process NMEA_GPS_POSITON_DATA_PGN Lat: " << mi32_latitudeDegree10Minus7
          << ", Lon: " << mi32_longitudeDegree10Minus7 << ", Alt: " << mi32_altitudeCm
          << ", TotalSize: " << rc_stream.getByteTotalSize() << ", resceived: " << rc_stream.getByteAlreadyReceived()
          << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;

      case NMEA_GPS_DIRECTION_DATA_PGN: // 0x01FE11LU - 130577 with Heading and Speed
        setTimeOut( TIMEOUT_SENDING_NODE_NMEA );
        uint8_t ui8_dummy;
        uint16_t ui16_newCOG, ui16_newSOG;
        IsoAgLib::convertIstream( stream, ui8_dummy ); //ui8_dataModeAndHeadingReference ); //ui8_dataModeAndHeadingReferenceDUMMY &= 0x3F;
        IsoAgLib::convertIstream( stream, mui8_directionSequenceID );
        IsoAgLib::convertIstream( stream, ui16_newCOG );
        IsoAgLib::convertIstream( stream, ui16_newSOG );
      //IsoAgLib::convertIstream( stream, ui16_dummy ); //ui16_headingRad10Minus4 );
      //IsoAgLib::convertIstream( stream, ui16_dummy ); //ui16_speedCmSec );
      //IsoAgLib::convertIstream( stream, ui16_dummy ); //ui16_flowDirectionRad10Minus4 );
      //IsoAgLib::convertIstream( stream, ui16_dummy ); //ui16_driftSpeedCmSec );
        /** @todo ON REQUEST-259: Do we need those valus above?? */

        /// @todo ON REQUEST-259: check for the REAL max, 62855 is a little bigger than 62831 or alike that could be calculated. but anyway...
        if ( (ui16_newCOG > 62855)
          || (ui16_newSOG > 65532) )
          return false;

        mui16_courseOverGroundRad10Minus4 = ui16_newCOG;
        mui16_speedOverGroundCmSec = ui16_newSOG;
        mi32_lastDirection = stream.getStartTime();

        #if DEBUG_NMEA
        INTERNAL_DEBUG_DEVICE << "process NMEA_GPS_DIRECTION_DATA_PGN: CourseOverGround: " << mui16_courseOverGroundRad10Minus4
          << ", SpeedOverGround [cm/sec]: " << mui16_speedOverGroundCmSec
          << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        break;
    }
    notifyOnEvent( ident.getPgn() );
    return false;
  }


} // __IsoAgLib

