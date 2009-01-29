/***************************************************************************
                itimeposgps_c.h  - working on Base Data Msg Type NMEA 200 GPS
                                  and Calendar; stores, updates  and
                                  delivers all base data informations
                                  from CanCustomer_c derived for CAN
                                  sending and receiving interaction;
                                  from Scheduler_Task_c derived for
                                  interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
#ifndef ITIMEPOSGPS_C_H
#define ITIMEPOSGPS_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement//iisoname_c.h>
#include "ibasetypes.h"
#include "impl/timeposgps_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  working on Base Data Msg Type 1, 2 and Calendar;
  stores, updates  and delivers all base data informations;
  Derive from Scheduler_Task_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
   per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  *@author Dipl.-Inform. Achim Spangler
*/
class iTimePosGps_c : private __IsoAgLib::TimePosGps_c {
public:
  // Public methods
  /**
      config the Base_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode set mode to either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config (const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0)
  { return TimePosGps_c::config_base( apc_isoName, at_identMode, aui16_suppressMask ); }

  /** config the Base_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identModeGps either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool configGps(const iIsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identModeGps)
  {return TimePosGps_c::configGps( apc_isoName, at_identModeGps);}

  /** Retrieve the last update time of the specified information type
   */
  int32_t lastedTimeSinceUpdateGps() const {return TimePosGps_c::lastedTimeSinceUpdateGps();}

  /** Retrieve the time of last update */
  int32_t lastUpdateTimeGps() const {return TimePosGps_c::lastUpdateTimeGps();}

  /** Retrieve the last update time of the specified information type
   */
  int32_t lastedTimeSinceUpdateDirection() const {return TimePosGps_c::lastedTimeSinceUpdateDirection();}

  /** Retrieve the time of last update */
  int32_t lastUpdateTimeDirection() const {return TimePosGps_c::lastUpdateTimeDirection();}

  /** return a sender which sends GPS commands as a tractor */
  iIsoName_c& getSenderISONameGps() {return TimePosGps_c::getSenderISONameGps().toIisoName_c();}

  /** return a sender which sends GPS commands as a tractor */
  const iIsoName_c& getSenderISONameGpsConst() const {return TimePosGps_c::getSenderISONameGpsConst().toConstIisoName_c();}

  /** force a request for pgn for time/date information */
  bool sendRequestUpdateTimeDate() { return TimePosGps_c::sendRequestUpdateTimeDate(); }

  /** @return TRUE => data source dev key is specified */
  bool checkDataSourceSpecified() { return TimePosGps_c::getSelectedDataSourceISONameConst().isSpecified();}

  /** get Devkey of data source (e.g. tractor, terminal) from which commands are send exclusively */
  const iIsoName_c& getSelectedDataSourceISONameConst() const {return TimePosGps_c::getSelectedDataSourceISONameConst().toConstIisoName_c();}

  /* ******************************************* */
  /** \name Get Values */
  /*@{*/
  /** check if a calendar information was received since init */
  bool isCalendarReceived() const { return TimePosGps_c::isCalendarReceived(); }

  /** check if a calendar information was received since init */
  bool isCalendarDateValid() const { return TimePosGps_c::isCalendarDateValid(); }

  /** get the calendar year value */
  int16_t yearLocal() { return TimePosGps_c::yearLocal();}

  /** get the calendar year value */
  int16_t yearUtc(){ return TimePosGps_c::yearUtc();}

  /** get the calendar month value */
  uint8_t monthLocal() { return TimePosGps_c::monthLocal();}

  /** get the calendar month value */
  uint8_t monthUtc() { return TimePosGps_c::monthUtc();}

  /** get the calendar day value */
  uint8_t dayLocal() { return TimePosGps_c::dayLocal();}

  /** get the calendar day value */
  uint8_t dayUtc() { return TimePosGps_c::dayUtc();}

  /** get the calendar hour value */
  uint8_t hourLocal() { return TimePosGps_c::hourLocal();}

  /** get the calendar hour value */
  uint8_t hourUtc() { return TimePosGps_c::hourUtc();}

  /** get the calendar minute value */
  uint8_t minuteLocal() { return TimePosGps_c::minuteLocal();}

  /** get the calendar minute value */
  uint8_t minuteUtc() { return TimePosGps_c::minuteUtc();}

  /** get the calendar second value */
  uint8_t second() { return TimePosGps_c::second();}

  /** get the calendar millisecond value */
  uint16_t millisecond() const { return TimePosGps_c::millisecond();}

  /** deliver UTC time in struct tm instead of separate calls to minuteUtc(), hourUtc(), ... to reduce load
    @return struct tm
  */
  struct CNAMESPACE::tm* currentUtcTm() { return TimePosGps_c::currentUtcTm();}

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLatitudeDegree10Minus7(void) const { return TimePosGps_c::getGpsLatitudeDegree10Minus7();}

  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLongitudeDegree10Minus7(void) const { return TimePosGps_c::getGpsLongitudeDegree10Minus7();}

  /** check if an NMEA2000 position signal was received */
  bool isPositionReceived() const { return TimePosGps_c::isPositionReceived(); }

  /** check if an NMEA2000 direction signal was received */
  bool isDirectionReceived() const { return TimePosGps_c::isDirectionReceived(); }

#if defined(USE_FLOAT_DATA_TYPE)
  /** deliver Minute GPS Latitude */
  float getGpsLatitudeMinute(void) const { return TimePosGps_c::getGpsLatitudeMinute();}

  /** deliver Minute GPS Longitude */
  float getGpsLongitudeMinute(void) const { return TimePosGps_c::getGpsLongitudeMinute();}

  /** deliver Degree GPS Latitude */
  float getGpsLatitudeDegree(void) const { return TimePosGps_c::getGpsLatitudeDegree();}

  /** deliver Degree GPS Longitude */
  float getGpsLongitudeDegree(void) const { return TimePosGps_c::getGpsLongitudeDegree();}
  #endif // END of USE_FLOAT_DATA_TYPE

  /** deliver GPS Speed Over Ground as [cm/s] */
  uint16_t getGpsSpeedCmSec(void) const { return TimePosGps_c::getGpsSpeedCmSec(); }
  /** deliver GPS Course Over Ground [1x10E-4rad] */
  uint16_t getGpsCourseRad10Minus4(void) const { return TimePosGps_c::getGpsCourseRad10Minus4(); }

  /** deliver GPS receive qualitiy */
  IsoAgLib::IsoGnssMethod_t getGnssMode(void) const { return TimePosGps_c::getGnssMode();}

#ifdef ENABLE_NMEA_2000_MULTI_PACKET
  /** get the GPS UTC hour value
      @return actual GPS UTC hour value
   */
  uint8_t hourUtcGps() const {return TimePosGps_c::hourUtcGps();}

  /** get the GPS UTC minute value
      @return actual GPS UTC minute value
   */
  uint8_t minuteUtcGps() const {return TimePosGps_c::minuteUtcGps();}

  /** get the GPS UTC second value
      @return actual GPS UTC second value
   */
  uint8_t secondUtcGps() const {return TimePosGps_c::secondUtcGps();}

  /** get the GPS UTC millisecond value
      @return actual GPS UTC millisecond value
   */
  uint16_t millisecondUtcGps() const {return TimePosGps_c::millisecondUtcGps();}

  /** deliver GPS altitude - [cm] */
  int32_t getGpsAltitudeCm(void) const { return TimePosGps_c::getGpsAltitudeCm();}

  /** simply check for some sort of Differential signal */
  bool hasDifferentialPosition() const{ return TimePosGps_c::hasDifferentialPosition();}

  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  IsoAgLib::IsoGnssType_t getGnssType(void) const { return TimePosGps_c::getGnssType();}

  /** deliver number of received satellites */
  uint8_t satelliteCnt() const { return TimePosGps_c::satelliteCnt();}

  /** deliver HDOP with scaling [1x10E-2] */
  int16_t hdop10Minus2() const { return TimePosGps_c::hdop10Minus2();}

  /** PDOP with scaling [1x10E-2] */
  int16_t pdop10Minus2() const { return TimePosGps_c::pdop10Minus2();}
  #endif // END ENABLE_NMEA_2000_MULTI_PACKET

  /** deliver age of last gps-position-update in milliseconds */
  uint16_t getGpsPositionUpdateAge(void) const { return TimePosGps_c::getGpsPositionUpdateAge(); }

  /** deliver age of last gps-direction-update in milliseconds */
  uint16_t getGpsDirectionUpdateAge(void) const { return TimePosGps_c::getGpsDirectionUpdateAge(); }

  /* ******************************************* */
  /** \name Set Values */
  /*@{*/

  /** set the calendar value */
  void setCalendarUtc(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day, uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec = 0)
  { return TimePosGps_c::setCalendarUtc(ai16_year, ab_month, ab_day, ab_hour, ab_minute, ab_second, aui16_msec);}

  /** set the calendar value as local time ( take local time offsets into account ) */
  void setCalendarLocal(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day, uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec = 0)
  { return TimePosGps_c::setCalendarLocal(ai16_year, ab_month, ab_day, ab_hour, ab_minute, ab_second, aui16_msec);}

  /** set the date in local timezone */
  void setDateLocal(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day)
  { return TimePosGps_c::setDateLocal(ai16_year, ab_month, ab_day);}

  /** set the date in UTC timezone */
  void setDateUtc(int16_t ai16_year, uint8_t ab_month, uint8_t ab_day)
  { return TimePosGps_c::setDateUtc(ai16_year, ab_month, ab_day);}

  /** set the time in local timezone */
  void setTimeLocal(uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec = 0)
  { return TimePosGps_c::setTimeLocal(ab_hour, ab_minute, ab_second, aui16_msec);}

  /** set the time in UTC timezone */
  void setTimeUtc(uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec = 0, bool b_updateDate = FALSE)
  { return TimePosGps_c::setTimeUtc(ab_hour, ab_minute, ab_second, aui16_msec, b_updateDate);};
  /** set the actual calendar year value */
  void setYearUtc(uint16_t aui16_year)
  { return TimePosGps_c::setYearUtc(aui16_year);}

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  void setGpsLatitudeDegree10Minus7( int32_t ai32_newVal )
  { return TimePosGps_c::setGpsLatitudeDegree10Minus7(ai32_newVal);}

  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  void setGpsLongitudeDegree10Minus7(int32_t ai32_newVal)
  { return TimePosGps_c::setGpsLongitudeDegree10Minus7(ai32_newVal);}

  /** deliver GPS Speed Over Ground as [cm/s] */
  void setGpsSpeedCmSec(uint16_t aui16_newVal)
  { return TimePosGps_c::setGpsSpeedCmSec(aui16_newVal); }

  /** deliver GPS Course Over Ground [1x10E-4rad] */
  void setGpsCourseRad10Minus4(uint16_t aui16_newVal)
  { return TimePosGps_c::setGpsCourseRad10Minus4(aui16_newVal); }


#if defined(ENABLE_NMEA_2000_MULTI_PACKET)
  /** set the GPS time in UTC timezone.
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   */
  void setTimeUtcGps(uint8_t ab_hour, uint8_t ab_minute, uint8_t ab_second, uint16_t aui16_msec = 0)
    {TimePosGps_c::setTimeUtcGps( ab_hour, ab_minute, ab_second, aui16_msec );}

  /** set the calendar hour value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param ab_hour actual calendar hour value
   */
  void setHourUtcGps(uint8_t ab_hour) {TimePosGps_c::setHourUtcGps( ab_hour );}

  /** set the calendar minute value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param ab_minute actual calendar minute value
   */
  void setMinuteUtcGps(uint8_t ab_minute) {TimePosGps_c::setMinuteUtcGps( ab_minute );}

  /** set the calendar second value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param ab_second actual calendar second value
   */
  void setSecondUtcGps(uint8_t ab_second) {TimePosGps_c::setSecondUtcGps( ab_second );}

  /** set the calendar millisecond value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param ab_millisecond actual calendar second value
   */
  void setMillisecondUtcGps(uint16_t aui16_millisecond) {TimePosGps_c::setMillisecondUtcGps( aui16_millisecond );}

  /** deliver GPS altitude - [cm] */
  void setGpsAltitudeCm( uint32_t aui32_newVal )
  { return TimePosGps_c::setGpsAltitudeCm(aui32_newVal);}

  /** deliver GPS receive qualitiy */
  void setGnssMode(IsoAgLib::IsoGnssMethod_t at_newVal)
  { return TimePosGps_c::setGnssMode(at_newVal);}

  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  void setGnssType( IsoAgLib::IsoGnssType_t at_newVal )
  { return TimePosGps_c::setGnssType(at_newVal);}

  /** deliver number of received satellites */
  void setSatelliteCnt(uint8_t aui8_newVal)
  { return TimePosGps_c::setSatelliteCnt(aui8_newVal);}

  /** deliver HDOP with scaling [1x10E-2] */
  void setHdop10Minus2(uint16_t aui16_newVal)
  { return TimePosGps_c::setHdop10Minus2(aui16_newVal);}

  /** PDOP with scaling [1x10E-2] */
  void setPdop10Minus2(uint16_t aui16_newVal)
  { return TimePosGps_c::setPdop10Minus2(aui16_newVal);}
  #endif // END of ENABLE_NMEA_2000_MULTI_PACKET

  /** set the calendar month value */
  void setMonthUtc(uint8_t ab_month)
  { return TimePosGps_c::setMonthUtc(ab_month);}

  /** set the calendar day value */
  void setDayUtc(uint8_t ab_day)
  { return TimePosGps_c::setDayUtc(ab_day);}

  /** set the calendar hour value */
  void setHourUtc(uint8_t ab_hour)
  { return TimePosGps_c::setHourUtc(ab_hour);}

  /** set the calendar minute value */
  void setMinuteUtc(uint8_t ab_minute)
  { return TimePosGps_c::setMinuteUtc(ab_minute);}

  /** set the calendar second value */
  void setSecond(uint8_t ab_second)
  { return TimePosGps_c::setSecond(ab_second);}

  /** set the calendar millisecond value */
  void setMillisecond(uint16_t aui16_millisecond)
  { return TimePosGps_c::setMillisecond(aui16_millisecond);}

  /** set the local time to UTC time offsets */
  void setLocalTimeOffsets(int16_t ai16_hourOffset, uint16_t aui16_minuteOffset)
  { return TimePosGps_c::setLocalTimeOffsets(ai16_hourOffset, aui16_minuteOffset);}


  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
 private:
  /** allow getITimePosGpsInstance() access to shielded TimePosGps_c class.
    * otherwise __IsoAgLib::getTimePosGpsInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iTimePosGps_c& getITimePosGpsInstance( uint8_t aui8_instance );
  #else
  friend iTimePosGps_c& getITimePosGpsInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTimePosGps_c& getITimePosGpsInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iTimePosGps_c&>(__IsoAgLib::getTimePosGpsInstance(aui8_instance));}
#else
  /** C-style function, to get access to the unique Base_c singleton instance */
  inline iTimePosGps_c& getITimePosGpsInstance( void )
  { return static_cast<iTimePosGps_c&>(__IsoAgLib::getTimePosGpsInstance());}
#endif

}
#endif
