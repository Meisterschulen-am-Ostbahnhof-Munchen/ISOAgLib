/***************************************************************************
                    timeposgps_c.h  - working on GPS data
                                      and Calendar; stores, updates  and
                                      delivers all information
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
#ifndef TIMEPOSGPS_C_H
#define TIMEPOSGPS_C_H

#include <ctime>
#include <IsoAgLib/comm/Base/impl/basecommon_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgnhandler_c.h>

#if defined(NMEA_2000_FAST_PACKET)
#include <IsoAgLib/comm/Multipacket/imultisendstreamer_c.h>
#include <IsoAgLib/comm/Multipacket/multireceiveclient_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#endif

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

#if defined(NMEA_2000_FAST_PACKET)

class MultiSendPkg_c;
class Nmea2000SendStreamer_c : public IsoAgLib::iMultiSendStreamer_c
{
  public:
    Nmea2000SendStreamer_c() : ui16_currentSendPosition(0), ui16_storedSendPosition(0) {};
    virtual ~Nmea2000SendStreamer_c()  {};
    /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards
    */
    virtual void setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes);
    /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards
   */
    virtual void setDataNextFastPacketStreamPart (MultiSendPkg_c* mspData, uint8_t bytes, uint8_t rui8_offset = 0 );

    /** set cache for data source to stream start */
    virtual void resetDataNextStreamPart();

    /** save current send position in data source - neeed for resend on send problem */
    virtual void saveDataNextStreamPart ();

    /** reactivate previously stored data source position - used for resend on problem */
    virtual void restoreDataNextStreamPart ();

    /** calculate the size of the data source */
    virtual uint32_t getStreamSize ();

    /** get the first byte, which is the COMMAND-byte and should be given back CONST! */
    virtual uint8_t getFirstByte ();

    void reset() { ui16_currentSendPosition = ui16_storedSendPosition = 0; vec_data.clear();};

    /** public send buffer */
    std::vector<uint8_t> vec_data;
  private:
    uint16_t ui16_currentSendPosition;
    uint16_t ui16_storedSendPosition;
};
#endif // END of NMEA_2000_FAST_PACKET

class TimePosGPS_c;
typedef SINGLETON_DERIVED(TimePosGPS_c,BaseCommon_c) SingletonTimePosGps_c;

/** working on GPS data and Calendar;
  stores, updates  and delivers all base data informations;
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
  per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  */

class TimePosGPS_c : public SingletonTimePosGps_c
                   , public ISORequestPGNHandler_c
              #if defined(NMEA_2000_FAST_PACKET)
                   , public IsoAgLib::MultiReceiveClient_c
              #endif // END of NMEA_2000_FAST_PACKET
{
 public:
  // Public methods
  /* ********************************************* */
  /** \name Management Functions for class TimePosGPS_c  */
  /*@{*/
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
  virtual bool timeEvent(  );

  /** config the Base_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool config(const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode);

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  virtual void init(const ISOName_c*, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement);

  /** destructor for Base_c which has nothing to do */
  virtual ~TimePosGPS_c() { BaseCommon_c::close();}

  /** process a message request for PGN
      @param  rui32_pgn  PGN
      @param  rui8_sa    source address
      @param  rui8_da    destination address
    */
  bool processMsgRequestPGN (uint32_t rui32_pgn, uint8_t rui8_sa, uint8_t rui8_da);

  /** force a request for pgn for time/date information */
  bool sendRequestUpdateTimeDate();

   /** config the Base_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identModeGps either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool configGps(const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identModeGps);

  /** return if you currently are in gps mode*/
  bool checkModeGps(IsoAgLib::IdentMode_t rt_identModeGps) const {return (t_identModeGps == rt_identModeGps);}

  /** send ISO11783 calendar PGN
    possible errors:
      * dependant error in CANIO_c on CAN send problems
    @see CANPkg_c::getData
    @see CANPkgExt_c::getData
    @see CANIO_c::operator<<
    */
  void sendCalendar(const ISOName_c& rpc_isoName);

  /** Retrieve the last update time of the specified information type*/
  int32_t lastedTimeSinceUpdateGps() const;

  /** Retrieve the time of last update */
  int32_t lastUpdateTimeGps() const;

  /** return a sender which sends commands as a tractor */
  ISOName_c& getSenderISONameGps() {return c_sendGpsISOName;}

  /** return a sender which sends commands as a tractor */
  const ISOName_c& getSenderISONameGpsConst() const {return c_sendGpsISOName;}

  /* ********************************************* */
  /** \name MultiReceive functions for TimePosGPS_c  */
  /*@{*/
  #if defined(NMEA_2000_FAST_PACKET)
  //! @param rc_ident:
  //! @param rui32_totalLen:
  virtual bool reactOnStreamStart(IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                                  uint32_t rui32_totalLen);

  //! @param rpc_stream:
  //! @param rb_isFirstChunk:
  //! @param rb_isLastChunkAndACKd:
  virtual bool processPartStreamDataChunk(IsoAgLib::iStream_c* rpc_stream,
                                          bool rb_isFirstChunk,
                                          bool rb_isLastChunkAndACKd);

  virtual void reactOnAbort(IsoAgLib::ReceiveStreamIdentifier_c rc_ident);
  #endif // END of NMEA_2000_FAST_PACKET


  /// Setter functions /////////////////////////////////////////////////////////////////////////////////
  /** set the calendar value
    @param ri16_year value to store as year
    @param rb_month value to store as month
    @param rb_day value to store as day
    @param rb_hour value to store as hour
    @param rb_minute value to store as minute
    @param rb_second value to store as second
   */
  void setCalendarUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0);

  /** set the calendar value as local time ( take local time offsets into account )
    @param ri16_year value to store as year
    @param rb_month value to store as month
    @param rb_day value to store as day
    @param rb_hour value to store as hour
    @param rb_minute value to store as minute
    @param rb_second value to store as second
    */
  void setCalendarLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0);

  /** set the date in local timezone */
  void setDateLocal(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day);

  /** set the date in UTC timezone */
  void setDateUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day);

  /** set the time in local timezone */
  void setTimeLocal(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0);

  /** set the time in UTC timezone */
  void setTimeUtc(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0, bool b_updateDate = FALSE);

  /** set the actual calendar year value */
  void setYearUtc(uint16_t rui16_year){i32_lastCalendarSet = System_c::getTime();bit_calendar.year = rui16_year; t_cachedLocalSeconds1970AtLastSet = 0;};

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  void setGpsLatitudeDegree10Minus7( int32_t ri32_newVal ) { i32_latitudeDegree10Minus7 = ri32_newVal; }

  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  void setGpsLongitudeDegree10Minus7( int32_t ri32_newVal ) { i32_longitudeDegree10Minus7 = ri32_newVal; }


  #if defined(NMEA_2000_FAST_PACKET)
  /** set the GPS time in UTC timezone.
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   */
  void setTimeUtcGps(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0);

  /** set the calendar hour value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param rb_hour actual calendar hour value
   */
  void setHourUtcGps(uint8_t rb_hour);

  /** set the calendar minute value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param rb_minute actual calendar minute value
   */
  void setMinuteUtcGps(uint8_t rb_minute);

  /** set the calendar second value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param rb_second actual calendar second value
   */
  void setSecondUtcGps(uint8_t rb_second);

  /** set the calendar millisecond value
   *  When no remote system is sending the 11783-7 PGN with date & time, the new UTC time is also set with
   *  setTimeUtc().
   *  In case another system is sending TIME_DATE_PGN, this time could be out-of-sync with GPS time.
   *  To avoid a jumping back and forth Non-GPS UTC time, those two UTC time sources are then not to be synced.
   *  @param rb_millisecond actual calendar second value
   */
  void setMillisecondUtcGps(uint16_t rui16_millisecond);

  /** deliver GPS altitude - [cm] */
  void setGpsAltitudeCm( uint32_t rui32_newVal ) { ui32_altitudeCm = rui32_newVal; }

  /** deliver GPS receive qualitiy */
  void setGnssMode( IsoAgLib::IsoGnssMethod_t rt_newVal );

  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  void setGnssType( IsoAgLib::IsoGnssType_t rt_newVal )   { if(rt_newVal<=IsoAgLib::IsoGnssTypeMAX) t_gnssType = rt_newVal;}

  /** deliver GPS speed as [cm/s] */
  void setGpsSpeedCmSec( uint16_t rui16_newVal ) { ui16_speedCmSec = rui16_newVal;}

  /** deliver GPS Heading [1x10E-4rad] */
  void setGpsHeadingRad10Minus4( uint16_t rui16_newVal ) { ui16_headingRad10Minus4 = rui16_newVal; }

  /** deliver number of received satellites */
  void setSatelliteCnt( uint8_t rui8_newVal ) { ui8_satelliteCnt = rui8_newVal;}

  /** deliver HDOP with scaling [1x10E-2] */
  void setHdop10Minus2( uint16_t rui16_newVal ) { i16_hdop = rui16_newVal;}

  /** PDOP with scaling [1x10E-2] */
  void setPdop10Minus2( uint16_t rui16_newVal ) { i16_pdop = rui16_newVal;}
  #endif // END of NMEA_2000_FAST_PACKET

  /** set the calendar month value
    @param rb_month actual calendar month value
    */
  void setMonthUtc(uint8_t rb_month){i32_lastCalendarSet = System_c::getTime();bit_calendar.month = rb_month; t_cachedLocalSeconds1970AtLastSet = 0;}

  /** set the calendar day value
    @param rb_day actual calendar day value
    */
  void setDayUtc(uint8_t rb_day){i32_lastCalendarSet = System_c::getTime();bit_calendar.day = rb_day; t_cachedLocalSeconds1970AtLastSet = 0;}

  /** set the calendar hour value
    @param rb_hour actual calendar hour value
    */
  void setHourUtc(uint8_t rb_hour){i32_lastCalendarSet = System_c::getTime();bit_calendar.hour = rb_hour; t_cachedLocalSeconds1970AtLastSet = 0;}

  /** set the calendar minute value
    @param rb_minute actual calendar minute value
    */
  void setMinuteUtc(uint8_t rb_minute){i32_lastCalendarSet = System_c::getTime();bit_calendar.minute = rb_minute; t_cachedLocalSeconds1970AtLastSet = 0;}

  /** set the calendar second value
    @param rb_second actual calendar second value
    */
  void setSecond(uint8_t rb_second){i32_lastCalendarSet = System_c::getTime();bit_calendar.second = rb_second; t_cachedLocalSeconds1970AtLastSet = 0;}

  /** set the calendar millisecond value
    @param rb_millisecond actual calendar second value
    */
  void setMillisecond(uint16_t rui16_millisecond){i32_lastCalendarSet = System_c::getTime();bit_calendar.msec = rui16_millisecond; t_cachedLocalSeconds1970AtLastSet = 0;}

  /** set the local time to UTC time offsets */
  void setLocalTimeOffsets( int16_t ri16_hourOffset, uint16_t rui16_minuteOffset )
  {bit_calendar.timezoneHourOffsetMinus24 = (ri16_hourOffset+24); bit_calendar.timezoneMinuteOffset = rui16_minuteOffset;}


  /// Getter functions
  /** check if a calendar information was received since init */
  bool isCalendarReceived() const;

  /** get the calendar year value
      @return actual calendar year value
    */
  int16_t yearLocal();

  /** get the calendar year value
    @return actual calendar year value
    */
  int16_t yearUtc();

  /** get the calendar month value
    @return actual calendar month value
    */
  uint8_t monthLocal();

  /** get the calendar month value
    @return actual calendar month value
    */
  uint8_t monthUtc();

  /** get the calendar day value
    @return actual calendar day value
    */
  uint8_t dayLocal();

  /** get the calendar day value
    @return actual calendar day value
    */
  uint8_t dayUtc();

  /** get the calendar hour value
    @return actual calendar hour value
    */
  uint8_t hourLocal();

  /** get the calendar hour value
    @return actual calendar hour value
    */
  uint8_t hourUtc();

  /** get the calendar minute value
    @return actual calendar minute value
    */
  uint8_t minuteLocal();

  /** get the calendar minute value
    @return actual calendar minute value
    */
  uint8_t minuteUtc();

  /** get the calendar second value
    @return actual calendar second value
    */
  uint8_t second();

  /** get the calendar millisecond value
      @return actual calendar second value
    */
  uint16_t millisecond() const {return ((bit_calendar.msec + calendarSetAge())%1000);}

  /** deliver UTC time in struct tm instead of separate calls to minuteUtc(), hourUtc(), ... to reduce load
    @return struct tm
  */
  struct CNAMESPACE::tm* currentUtcTm();

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLatitudeDegree10Minus7( void ) const { return i32_latitudeDegree10Minus7; }

  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLongitudeDegree10Minus7( void ) const { return i32_longitudeDegree10Minus7; }

  #if defined(USE_FLOAT_DATA_TYPE)
  /** check if an NMEA2000 position signal was received */
  bool isPositionReceived() const;

  /** deliver Minute GPS Latitude */
  float getGpsLatitudeMinute( void ) const { return ( i32_latitudeDegree10Minus7 * 6.0e-4  ); }

  /** deliver Minute GPS Longitude */
  float getGpsLongitudeMinute( void ) const { return ( i32_longitudeDegree10Minus7 * 6.0e-4 ); }

  /** deliver Degree GPS Latitude */
  float getGpsLatitudeDegree( void ) const { return ( float(i32_latitudeDegree10Minus7) * 1.0e-7  ); }

  /** deliver Degree GPS Longitude */
  float getGpsLongitudeDegree( void ) const { return ( float(i32_longitudeDegree10Minus7) * 1.0e-7 ); }
  #endif // END of USE_FLOAT_DATA_TYPE

  #ifdef NMEA_2000_FAST_PACKET
  /** get the GPS UTC hour value
    @return actual GPS UTC hour value
   */
  uint8_t hourUtcGps() const {return bit_gpsTime.hour;}

  /** get the GPS UTC minute value
      @return actual GPS UTC minute value
   */
  uint8_t minuteUtcGps() const {return bit_gpsTime.minute;}

  /** get the GPS UTC second value
      @return actual GPS UTC second value
   */
  uint8_t secondUtcGps() const {return bit_gpsTime.second;}

  /** get the GPS UTC millisecond value
      @return actual GPS UTC millisecond value
   */
  uint16_t millisecondUtcGps() const {return bit_gpsTime.msec;}

  /** deliver GPS altitude - [cm] */
  uint32_t getGpsAltitudeCm( void ) const { return ui32_altitudeCm; }

  /** deliver GPS receive qualitiy */
  IsoAgLib::IsoGnssMethod_t getGnssMode( void ) const { return t_gnssMethod;}

  /** simply check for some sort of Differential signal */
  bool hasDifferentialPosition() const { return ( ( t_gnssMethod > IsoAgLib::IsoGnssFix ) && ( t_gnssMethod < IsoAgLib::IsoDrEstimated ) )?true:false;}

  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  IsoAgLib::IsoGnssType_t getGnssType(void) const { return t_gnssType;}

  /** deliver GPS speed as [cm/s] */
  uint16_t getGpsSpeedCmSec( void ) const { return ui16_speedCmSec;}

  /** deliver GPS Heading [1x10E-4rad] */
  uint16_t getGpsHeadingRad10Minus4( void ) const { return ui16_headingRad10Minus4; }

  /** deliver number of received satellites */
  uint8_t satelliteCnt() const { return ui8_satelliteCnt;}

  /** deliver HDOP with scaling [1x10E-2] */
  int16_t hdop10Minus2() const { return i16_hdop;}

  /** PDOP with scaling [1x10E-2] */
  int16_t pdop10Minus2() const { return i16_pdop;}
  #endif // END NMEA_2000_FAST_PACKET

#if defined (NMEA_2000_FAST_PACKET)
  /** deliver age of last gps-update in milliseconds */
  uint16_t getGpsUpdateAge( void ) const
  { if ( i32_lastIsoPositionStream > i32_lastIsoPositionSimple) return (System_c::getTime() - i32_lastIsoPositionStream);
    else                                                        return (System_c::getTime() - i32_lastIsoPositionSimple);}
#else
  /** deliver age of last gps-update in milliseconds */
  uint16_t getGpsUpdateAge( void ) const
  { return (System_c::getTime() - i32_lastIsoPositionSimple);}
#endif

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;


private:
  // Private methods
  friend class SINGLETON_DERIVED(TimePosGPS_c,BaseCommon_c);

  /** HIDDEN constructor for a TimePosGPS_c object instance which can optional
    set the configuration for send/receive for base msg type NMEA 2000 GPS
    and calendar
    NEVER instantiate a variable of type TimePosGPS_c within application
    only access TimePosGPS_c via getTimePosGpsInstance() or getTimePosGpsInstance( int riLbsBusNr ) in case more than one BUS is used for IsoAgLib
    */
  TimePosGPS_c(): c_sendGpsISOName(),
                  pc_isoNameGps(NULL),
                  t_identModeGps( IsoAgLib::IdentModeImplement )
  {}

  /** deliver time between now and last calendar set in [msec]
    @return msec since last calendar set
    */
  int32_t calendarSetAge() const {return (System_c::getTime() - i32_lastCalendarSet);};

  const struct CNAMESPACE::tm* Utc2LocalTime();

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  virtual void checkCreateReceiveFilter( );

  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  virtual bool timeEventTracMode();

  /** send position rapid update message */
  void sendPositionRapidUpdate( void );

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CANPkgExt_c::resolveSendingInformation()
    */
  bool processMsg();

  #if defined(NMEA_2000_FAST_PACKET)
  /** send position as detailed stream */
  void isoSendPositionStream( void );

  /** send direction as detailed stream */
  void isoSendDirectionStream( void );

  //! @param rc_ident:
  //! @param rpc_stream:
  bool reactOnLastChunk(IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                        IsoAgLib::iStream_c& rpc_stream);

  virtual void reactOnAbort(IsoAgLib::iStream_c* rpc_stream);
  #endif // END NMEA_2000_FAST_PACKET

private:
  // Private attributes
    /** last timestamp where calendar was set
    -> use this to calculate new time
    */
  int32_t i32_lastCalendarSet;

      /** last timestamp where calendar was set
    -> use this to calculate new time
    */
  CNAMESPACE::time_t t_cachedLocalSeconds1970AtLastSet;

  CNAMESPACE::time_t t_tzOffset;

  /** bit_calendar */
  struct {
    uint16_t year : 12;
    uint16_t month : 4;
    uint16_t day : 6;
    uint16_t hour : 6;
    uint16_t minute : 6;
    uint16_t second : 6;
    uint16_t msec   : 10;
    uint16_t timezoneMinuteOffset : 6;
    uint16_t timezoneHourOffsetMinus24 : 6;
  } bit_calendar;

  /** raw GPS latitude [degree] ; Lat_Min < 0 --> South */
  int32_t i32_latitudeDegree10Minus7;

  /** raw GPS longitude [degree]; Long_Min < 0 --> West */
  int32_t i32_longitudeDegree10Minus7;

  /// General
  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoPositionSimple;

  #ifdef NMEA_2000_FAST_PACKET
  /** GPS time in UTC */
  struct {
    uint16_t hour : 6;
    uint16_t minute : 6;
    uint16_t second : 6;
    uint16_t msec   : 10;
  } bit_gpsTime;

  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoPositionStream;

  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoDirectionStream;

  /** GPS altitude - [cm] */
  uint32_t ui32_altitudeCm;

  /** GNSS Method and Quality */
  IsoAgLib::IsoGnssMethod_t t_gnssMethod;

  /** GNSS Type */
  IsoAgLib::IsoGnssType_t t_gnssType;

  /** number of received satellites */
  uint8_t ui8_satelliteCnt;

  /** HDOP with scaling [1x10E-2] */
  int16_t i16_hdop;

  /** PDOP with scaling [1x10E-2] */
  int16_t i16_pdop;

  /** integrity of GPS signal */
  uint8_t ui8_integrity;

  /** sequence ID of GPS string */
  uint8_t ui8_positionSequenceID;
  int32_t i32_geoidalSeparation;
  uint8_t ui8_noRefStations;
  std::vector<uint16_t> vec_refStationTypeAndStation;
  std::vector<uint16_t> vec_refStationDifferentialAge10Msec;

  /** data mode and Set/COG/Heading Ref. of Direction Data PGN 130577 */
  uint8_t ui8_dataModeAndHeadingReference;

  /** sequence ID of direction data */
  uint8_t ui8_directionSequenceID;

  /** course over ground */
  uint16_t ui16_courseOverGroundRad10Minus4;

  /** speed over ground */
  uint16_t ui16_speedOverGroundCmSec;

  /** GPS heading [1x10E-4rad] */
  uint16_t ui16_headingRad10Minus4;

  /** GPS speed - [cm/s] */
  uint16_t ui16_speedCmSec;

  /** flow direction */
  uint16_t ui16_flowDirectionRad10Minus4;

  /** drift speed */
  uint16_t ui16_driftSpeedCmSec;

  /** buffer class for sending data streams */
  Nmea2000SendStreamer_c c_nmea2000Streamer;

  /** sending success state */
  MultiSend_c::sendSuccess_t t_multiSendSuccessState;

  #endif // END of NMEA_2000_FAST_PACKET
  /** ISOName of GPS data sender */
  ISOName_c c_sendGpsISOName;

  /** isoName which act as sender of a msg: either responses on behalf of implement or commands as tractor.
      This pointer is set in config function
    */
  const ISOName_c* pc_isoNameGps;
  IsoAgLib::IdentMode_t  t_identModeGps;
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TimePosGPS_c& getTimePosGpsInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TimePosGPS_c& getTimePosGpsInstance( void );
#endif // END of PRT_INSTANCE_CNT

} // END of Namespace __IsoAgLib

#endif // END of TIMEPOSGPS_C_H
