/*
  gnss_c.h: Receiving GNSS Position, Direction and Date/Time.

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
#ifndef GNSS_C_H
#define GNSS_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/basecommon_c.h>


namespace __IsoAgLib {


class Gnss_c : public BaseCommon_c
{
  MACRO_MULTITON_CONTRIBUTION();
public:
  virtual void timeEvent();

  // from BaseCommon_c
  virtual void init_specialized();
  virtual void close_specialized();

  virtual ~Gnss_c() {}

  /** Retrieve the time of last position update (-1 if no position available (anymore, i.e. after timeout!)) */
  ecutime_t lastPositionUpdate() const;

  /** Retrieve the time of last date/time update (-1 if no date/time available (anymore, i.e. after timeout!)) */
  ecutime_t lastDateTimeUpdate() const;

  /** Retrieve the time of last direction update (-1 if no direction available (anymore, i.e. after timeout!)) */
  ecutime_t lastDirectionUpdate() const;

  const IsoAgLib::iDateTime_s & getDateTime() const { return m_dateTime; }

  void reactOnIsoItemModification(ControlFunctionStateHandler_c::iIsoItemAction_e at_action, IsoItem_c const& acrc_isoItem);

public:
  virtual bool reactOnStreamStart(const ReceiveStreamIdentifier_c& ac_ident,
                                  uint32_t aui32_totalLen);

  virtual bool processPartStreamDataChunk(Stream_c& apc_stream,
                                          bool ab_isFirstChunk,
                                          bool ab_isLastChunkAndACKd);

  virtual void reactOnAbort (Stream_c&) {}

private:
  void setDateTimeUtcGps(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day, uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec );

  virtual bool processMsgRequestPGN(uint32_t , IsoItem_c *, IsoItem_c *, ecutime_t ) { return false; } // not used in here!

  virtual void checkCreateReceiveFilter() {}

public:
  /** *** Position ** **/

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLatitudeDegree10Minus7( void ) const { return mi32_latitudeDegree10Minus7; }

  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLongitudeDegree10Minus7( void ) const { return mi32_longitudeDegree10Minus7; }

  /** deliver Minute GPS Latitude */
  float getGpsLatitudeMinute( void ) const { return ( mi32_latitudeDegree10Minus7 * 6.0e-4f  ); }

  /** deliver Minute GPS Longitude */
  float getGpsLongitudeMinute( void ) const { return ( mi32_longitudeDegree10Minus7 * 6.0e-4f ); }

  /** deliver Degree GPS Latitude */
  float getGpsLatitudeDegree( void ) const { return ( float(mi32_latitudeDegree10Minus7) * 1.0e-7f  ); }

  /** deliver Degree GPS Longitude */
  float getGpsLongitudeDegree( void ) const { return ( float(mi32_longitudeDegree10Minus7) * 1.0e-7f ); }


  /** *** Direction *** **/

  /** deliver GPS Speed Over Ground as [cm/s] */
  uint16_t getGpsSpeedCmSec( void ) const { return mui16_speedOverGroundCmSec; }
  /** deliver GPS Course Over Ground [1x10E-4rad] */
  uint16_t getGpsCourseRad10Minus4( void ) const { return mui16_courseOverGroundRad10Minus4; }

  /** deliver GPS receive qualitiy - also needed to see if we have valid GPS-positioning!!! */
  IsoAgLib::IsoGnssMethod_t getGnssMode( void ) const { return mt_gnssMethod; }

  /** deliver GPS altitude - [cm] */
  int32_t getGpsAltitudeCm( void ) const { return mi32_altitudeCm; }

  /** deliver GPS Geodial Separation - [cm] */
  int32_t getGpsGeoidalSeparationCm ( void ) const { return mi32_geoidalSeparationCm; }

  /** simply check for some sort of Differential signal */
  bool hasDifferentialPosition() const { return ( ( mt_gnssMethod > IsoAgLib::IsoGnssFix ) && ( mt_gnssMethod < IsoAgLib::IsoDrEstimated ) );}

  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  IsoAgLib::IsoGnssType_t getGnssType(void) const { return mt_gnssType; }

  /** deliver number of received satellites */
  uint8_t satelliteCnt() const { return mui8_satelliteCnt; }

  /** deliver HDOP with scaling [1x10E-2] */
  int16_t hdop10Minus2() const { return mi16_hdop; }

  /** PDOP with scaling [1x10E-2] */
  int16_t pdop10Minus2() const { return mi16_pdop; }

private:
  Gnss_c();

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  void processMsg( const CanPkg_c& arc_data );

private:
  /** raw GPS latitude [degree] ; Lat_Min < 0 --> South */
  int32_t mi32_latitudeDegree10Minus7;

  /** raw GPS longitude [degree]; Long_Min < 0 --> West */
  int32_t mi32_longitudeDegree10Minus7;

  /// General
  /** last time of GPS msg [msec] Position Strean*/
  ecutime_t mi32_lastPositionStream;

  /** last time of GPS msg [msec] Position Simple */
  ecutime_t mi32_lastPositionSimple;

  /** last time of GPS msg [msec] Direction */
  ecutime_t mi32_lastDirection;

  /** course over ground reference */
  uint8_t mui8_courseOverGroundReference;

  /** sequence ID of direction data */
  uint8_t mui8_directionSequenceID;

  /** course over ground */
  uint16_t mui16_courseOverGroundRad10Minus4;

  /** speed over ground */
  uint16_t mui16_speedOverGroundCmSec;

  /** GNSS Method and Quality - not only in ENABLE_NMEA_2000_MULTI_PACKET as we need to know if GPS inf. is valid! */
  IsoAgLib::IsoGnssMethod_t mt_gnssMethod;

  /** GPS altitude - [cm] */
  int32_t mi32_altitudeCm;

  /** GPS date/time in UTC */
  IsoAgLib::iDateTime_s m_dateTime;

  /** GNSS Type */
  IsoAgLib::IsoGnssType_t mt_gnssType;

  /** number of received satellites */
  uint8_t mui8_satelliteCnt;

  /** HDOP with scaling [1x10E-2] */
  int16_t mi16_hdop;

  /** PDOP with scaling [1x10E-2] */
  int16_t mi16_pdop;

  /** integrity of GPS signal */
  uint8_t mui8_integrity;

  /** sequence ID of GPS string */
  uint8_t mui8_positionSequenceID;
  int32_t mi32_geoidalSeparationCm;
  uint8_t mui8_noRefStations;
  STL_NAMESPACE::vector<uint16_t> mvec_refStationTypeAndStation;
  STL_NAMESPACE::vector<uint16_t> mvec_refStationDifferentialAge10Msec;

  ControlFunctionStateHandlerProxy_c<Gnss_c> m_gnssCFSHproxy;

  friend Gnss_c &getGnssInstance( unsigned instance );
};


inline
ecutime_t
Gnss_c::lastDateTimeUpdate() const
{
  return m_dateTime.timestamp;
}


inline
ecutime_t
Gnss_c::lastPositionUpdate() const
{
  if ( mi32_lastPositionStream > mi32_lastPositionSimple ) return mi32_lastPositionStream;
  else return mi32_lastPositionSimple;
}


inline
ecutime_t
Gnss_c::lastDirectionUpdate() const
{
  return mi32_lastDirection;
}



Gnss_c &getGnssInstance( unsigned instance );

} // __IsoAgLib

#endif
