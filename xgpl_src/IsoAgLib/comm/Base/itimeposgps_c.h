/***************************************************************************
                itimeposgps_c.h  - working on Base Data Msg Type NMEA 200 GPS
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
#ifndef ITIMEPOSGPS_C_H
#define ITIMEPOSGPS_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/igetypos_c.h>
#include "ibasetypes.h"
#include "impl/timeposgps_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  working on Base Data Msg Type 1, 2 and Calendar;
  stores, updates  and delivers all base data informations;
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
   per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  *@author Dipl.-Inform. Achim Spangler
*/
class iTimePosGPS_c : private __IsoAgLib::TimePosGPS_c {
public:
  // Public methods
  /**
    config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rb_implementMode implement mode (true) or tractor mode (false)!!!
  */
  void config(const iDevKey_c* rpc_devKey, bool rb_implementMode = true)
  {TimePosGPS_c::config(rpc_devKey, rb_implementMode );};

  #ifdef USE_ISO_11783
  /** config the Base_c object after init -> set pointer to devKey and
      config send/receive of different base msg types
      @param rpc_devKey pointer to the DEV_KEY variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rb_implementMode implement mode (true) or tractor mode (false)!!!
    */
  void configGps(const iDevKey_c* rpc_devKey, bool rb_implementMode)
  {TimePosGPS_c::configGps(rpc_devKey, rb_implementMode );};
  /** Retrieve the last update time of the specified information type
     @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
   */
  int32_t lastedTimeSinceUpdateGps() const
    {return TimePosGPS_c::lastedTimeSinceUpdateGps();};
  /** Retrieve the time of last update */
  int32_t lastUpdateTimeGps() const
  {return TimePosGPS_c::lastUpdateTimeGps();};
  /** return a sender which sends commands as a tractor */
  iDevKey_c& getSenderDevKeyGps() {return static_cast<iDevKey_c&>(TimePosGPS_c::getSenderDevKeyGps());};
  /** return a sender which sends commands as a tractor */
  const iDevKey_c& getSenderDevKeyGpsConst() const {return static_cast<const iDevKey_c&>(TimePosGPS_c::getSenderDevKeyGpsConst());};
  #endif
  /* ******************************************* */
  /** \name Get Values */
  /*@{*/
  /** check if a calendar information was received since init */
  bool isCalendarReceived() const { return TimePosGPS_c::isCalendarReceived();};
  /** get the calendar year value */
  int16_t yearLocal() const { return TimePosGPS_c::yearLocal();};
  /** get the calendar year value */
  int16_t yearUtc() const { return TimePosGPS_c::yearUtc();};
  /** get the calendar month value */
  uint8_t monthLocal() const { return TimePosGPS_c::monthLocal();};
  /** get the calendar month value */
  uint8_t monthUtc() const { return TimePosGPS_c::monthUtc();};
  /** get the calendar day value */
  uint8_t dayLocal() const { return TimePosGPS_c::dayLocal();};
  /** get the calendar day value */
  uint8_t dayUtc() const { return TimePosGPS_c::dayUtc();};
  /** get the calendar hour value */
  uint8_t hourLocal() const { return TimePosGPS_c::hourLocal();};
  /** get the calendar hour value */
  uint8_t hourUtc() const { return TimePosGPS_c::hourUtc();};
  /** get the calendar minute value */
  uint8_t minuteLocal() const { return TimePosGPS_c::minuteLocal();};
  /** get the calendar minute value */
  uint8_t minuteUtc() const { return TimePosGPS_c::minuteUtc();};
  /** get the calendar second value */
  uint8_t second() const { return TimePosGPS_c::second();};
  /** get the calendar millisecond value */
  uint16_t millisecond() const { return TimePosGPS_c::millisecond();};

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLatitudeDegree10Minus7(void) const { return TimePosGPS_c:: getGpsLatitudeDegree10Minus7();};
  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLongitudeDegree10Minus7(void) const { return TimePosGPS_c::getGpsLongitudeDegree10Minus7();};

    #if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
  /** check if an NMEA2000 position signal was received */
  bool isPositionReceived() const { return TimePosGPS_c::isPositionReceived();};
  /** deliver Minute GPS Latitude */
  float getGpsLatitudeMinute(void) const { return TimePosGPS_c::getGpsLatitudeMinute();};
  /** deliver Minute GPS Longitude */
  float getGpsLongitudeMinute(void) const { return TimePosGPS_c::getGpsLongitudeMinute();};
  /** deliver Degree GPS Latitude */
  float getGpsLatitudeDegree(void) const { return TimePosGPS_c::getGpsLatitudeDegree();};
  /** deliver Degree GPS Longitude */
  float getGpsLongitudeDegree(void) const { return TimePosGPS_c::getGpsLongitudeDegree();};
  #endif // END of USE_FLOAT_DATA_TYPE and USE_DIN_GPS

  #ifdef NMEA_2000_FAST_PACKET
  /** deliver GPS altitude - [cm] */
  uint32_t getGpsAltitudeCm(void) const { return TimePosGPS_c::getGpsAltitudeCm();};
  /** deliver GPS receive qualitiy */
  IsoAgLib::IsoGnssMethod_t getGnssMode(void) const { return TimePosGPS_c::getGnssMode();};
  /** simply check for some sort of Differential signal */
  bool hasDifferentialPosition() const{ return TimePosGPS_c::hasDifferentialPosition();};
  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  IsoAgLib::IsoGnssType_t getGnssType(void) const { return TimePosGPS_c::getGnssType();};
  /** deliver GPS speed as [cm/s] */
  uint16_t getGpsSpeedCmSec(void) const { return TimePosGPS_c::getGpsSpeedCmSec();};
  /** deliver GPS Heading [1x10E-4rad] */
  uint16_t getGpsHeadingRad10Minus4(void) const { return TimePosGPS_c::getGpsHeadingRad10Minus4();};
  /** deliver number of received satellites */
  uint8_t satelliteCnt() const { return TimePosGPS_c::satelliteCnt();};
  /** deliver HDOP with scaling [1x10E-2] */
  int16_t hdop10Minus2() const { return TimePosGPS_c::hdop10Minus2();};
  /** PDOP with scaling [1x10E-2] */
  int16_t pdop10Minus2() const { return TimePosGPS_c::pdop10Minus2();};
  #endif // END NMEA_2000_FAST_PACKET

  /** deliver age of last gps-update in milliseconds */
  uint16_t getGpsUpdateAge(void) const { return TimePosGPS_c::getGpsUpdateAge();};

  /* ******************************************* */
  /** \name Set Values */
  /*@{*/

  /** set the calendar value */
  void setCalendarUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0)
  { return TimePosGPS_c::setCalendarUtc(ri16_year, rb_month, rb_day, rb_hour, rb_minute, rb_second, rui16_msec);};
  /** set the calendar value as local time ( take local time offsets into account ) */
  void setCalendarLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0)
  { return TimePosGPS_c::setCalendarLocal(ri16_year, rb_month, rb_day, rb_hour, rb_minute, rb_second, rui16_msec);};
  /** set the date in local timezone */
  void setDateLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day)
  { return TimePosGPS_c::setDateLocal(ri16_year, rb_month, rb_day);};
  /** set the date in UTC timezone */
  void setDateUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day)
  { return TimePosGPS_c::setDateUtc(ri16_year, rb_month, rb_day);};
  /** set the time in local timezone */
  void setTimeLocal(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0)
  { return TimePosGPS_c::setTimeLocal(rb_hour, rb_minute, rb_second, rui16_msec);};
  /** set the time in UTC timezone */
  void setTimeUtc(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0)
  { return TimePosGPS_c::setTimeUtc(rb_hour, rb_minute, rb_second, rui16_msec);};
  /** set the actual calendar year value */
  void setYearUtc(uint16_t rui16_year)
  { return TimePosGPS_c::setYearUtc(rui16_year);};

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  void setGpsLatitudeDegree10Minus7( int32_t ri32_newVal )
  { return TimePosGPS_c::setGpsLatitudeDegree10Minus7(ri32_newVal);};
  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  void setGpsLongitudeDegree10Minus7(int32_t ri32_newVal)
  { return TimePosGPS_c::setGpsLongitudeDegree10Minus7(ri32_newVal);};

  #if defined(NMEA_2000_FAST_PACKET)
  /** deliver GPS altitude - [cm] */
  void setGpsAltitudeCm( uint32_t rui32_newVal )
  { return TimePosGPS_c::setGpsAltitudeCm(rui32_newVal);};
  /** deliver GPS receive qualitiy */
  void setGnssMode(IsoAgLib::IsoGnssMethod_t rt_newVal)
  { return TimePosGPS_c::setGnssMode(rt_newVal);};
  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  void setGnssType( IsoAgLib::IsoGnssType_t rt_newVal )
  { return TimePosGPS_c::setGnssType(rt_newVal);};
  /** deliver GPS speed as [cm/s] */
  void setGpsSpeedCmSec(uint16_t rui16_newVal)
  { return TimePosGPS_c::setGpsSpeedCmSec(rui16_newVal);};
  /** deliver GPS Heading [1x10E-4rad] */
  void setGpsHeadingRad10Minus4(uint16_t rui16_newVal)
  { return TimePosGPS_c::setGpsHeadingRad10Minus4(rui16_newVal);};
  /** deliver number of received satellites */
  void setSatelliteCnt(uint8_t rui8_newVal)
  { return TimePosGPS_c::setSatelliteCnt(rui8_newVal);};
  /** deliver HDOP with scaling [1x10E-2] */
  void setHdop10Minus2(uint16_t rui16_newVal)
  { return TimePosGPS_c::setHdop10Minus2(rui16_newVal);};
  /** PDOP with scaling [1x10E-2] */
  void setPdop10Minus2(uint16_t rui16_newVal)
  { return TimePosGPS_c::setPdop10Minus2(rui16_newVal);};
  #endif // END of NMEA_2000_FAST_PACKET

  /** set the calendar month value */
  void setMonthUtc(uint8_t rb_month)
  { return TimePosGPS_c::setMonthUtc(rb_month);};
  /** set the calendar day value */
  void setDayUtc(uint8_t rb_day)
  { return TimePosGPS_c::setDayUtc(rb_day);};
  /** set the calendar hour value */
  void setHourUtc(uint8_t rb_hour)
  { return TimePosGPS_c::setHourUtc(rb_hour);};
  /** set the calendar minute value */
  void setMinuteUtc(uint8_t rb_minute)
  { return TimePosGPS_c::setMinuteUtc(rb_minute);};
  /** set the calendar second value */
  void setSecond(uint8_t rb_second)
  { return TimePosGPS_c::setSecond(rb_second);};
  /** set the calendar millisecond value */
  void setMillisecond(uint16_t rui16_millisecond)
  { return TimePosGPS_c::setMillisecond(rui16_millisecond);};
  /** set the local time to UTC time offsets */
  void setLocalTimeOffsets(int16_t ri16_hourOffset, uint16_t rui16_minuteOffset)
  { return TimePosGPS_c::setLocalTimeOffsets(ri16_hourOffset, rui16_minuteOffset);};


  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
 private:
  /** allow getITimePosGpsInstance() access to shielded TimePosGPS_c class.
    * otherwise __IsoAgLib::getTimePosGpsInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iTimePosGPS_c& getITimePosGpsInstance( uint8_t rui8_instance );
  #else
  friend iTimePosGPS_c& getITimePosGpsInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iTimePosGPS_c& getITimePosGpsInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iTimePosGPS_c&>(__IsoAgLib::getTimePosGpsInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique Base_c singleton instance */
  inline iTimePosGPS_c& getITimePosGpsInstance( void )
  { return static_cast<iTimePosGPS_c&>(__IsoAgLib::getTimePosGpsInstance());};
#endif

}
#endif
