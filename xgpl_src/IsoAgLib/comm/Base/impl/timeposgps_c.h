/***************************************************************************
                    timeposgps_c.h  - working on GPS data
                                      and Calendar; stores, updates  and
                                      delivers all information
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
#ifndef TIMEPOSGPS_C_H
#define TIMEPOSGPS_C_H

#include <IsoAgLib/typedef.h>
#include "../ibasetypes.h"
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/getypos_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>

#include "basepkg_c.h"
#include <ctime>

#if defined(NMEA_2000_FAST_PACKET) && defined(USE_ISO_11783)
  #include <IsoAgLib/comm/Multipacket/multireceiveclient_c.h>
  #include <IsoAgLib/comm/Multipacket/impl/multisendstreamer_c.h>
  #include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#endif // END of NMEA_2000_FAST_PACKET and USE_ISO_11783

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

#if defined(NMEA_2000_FAST_PACKET) && defined(USE_ISO_11783)

class MultiSendPkg_c;
class Nmea2000SendStreamer_c : public MultiSendStreamer_c
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
#endif // END of NMEA_2000_FAST_PACKET and USE_ISO_11783

class TimePosGPS_c;
typedef SINGLETON_DERIVED(TimePosGPS_c,ElementBase_c) SingletonTimePosGps_c;

/** working on GPS data and Calendar;
  stores, updates  and delivers all base data informations;
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
  per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  */

class TimePosGPS_c : public SingletonTimePosGps_c
#if defined(NMEA_2000_FAST_PACKET) && defined(USE_ISO_11783)
    , public IsoAgLib::MultiReceiveClient_c
#endif // END of NMEA_2000_FAST_PACKET and USE_ISO_11783
{
public:
  // Public methods
  /* ********************************************* */
  /** \name Management Functions for class TimePosGPS_c  */
  /*@{*/

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances, to receive
      the needed CAN msg with base msg type NMEA 2000 GPS and calendar
      possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
  void init(const DevKey_c* rpc_devKey = NULL, IsoAgLib::BaseDataGroup_t rt_mySendSelection = IsoAgLib::BaseDataNothing );

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

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
  bool timeEvent( void );

  /** check if filter boxes shall be created - create only ISO or DIN filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void checkCreateReceiveFilter( void );

  /** config the Base_c object after init -> set pointer to devKey and
      config send/receive of different base msg types
      @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
  void config(const DevKey_c* rpc_devKey, IsoAgLib::BaseDataGroup_t rt_mySendSelection);
  /** Retrieve the last update time of the specified information type
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
   */
  int32_t lastUpdate( IsoAgLib::BaseDataGroup_t rt_mySendSelection ) const;

  /** destructor for Base_c which has nothing to do */
  virtual ~TimePosGPS_c() { close();};

  /** process received base msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a msg
      possible errors:
        * Err_c::lbsBaseSenderConflict base msg recevied from different member than before
      @see CANIO_c::processMsg
      @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
    */
  virtual bool processMsg();

  #ifdef USE_ISO_11783
  /** send ISO11783 calendar PGN
    @param rc_devKey DEV_KEY code off calling item which wants to send
    @param ri32_time timestamp where calendar was last sent (default autodetect)
    possible errors:
      * dependant error in CANIO_c on CAN send problems
    @see CANPkg_c::getData
    @see CANPkgExt_c::getData
    @see CANIO_c::operator<<
    */
  void isoSendCalendar(const DevKey_c& rc_devKey);
  #endif // END of USE_ISO_11783

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
  void setTimeUtc(uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0);
  /** set the actual calendar year value */
  void setYearUtc(uint16_t rui16_year){i32_lastCalendarSet = System_c::getTime();bit_calendar.year = rui16_year;};

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  void setGpsLatitudeDegree10Minus7( int32_t ri32_newVal ) { i32_latitudeDegree10Minus7 = ri32_newVal; };
  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  void setGpsLongitudeDegree10Minus7( int32_t ri32_newVal ) { i32_longitudeDegree10Minus7 = ri32_newVal; };

  #if defined(NMEA_2000_FAST_PACKET)
  /** deliver GPS altitude - [cm] */
  void setGpsAltitudeCm( uint32_t rui32_newVal ) { ui32_altitudeCm = rui32_newVal; };
  /** deliver GPS receive qualitiy */
  void setGnssMode( IsoAgLib::IsoGnssMethod_t rt_newVal );
  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  void setGnssType( IsoAgLib::IsoGnssType_t rt_newVal )   { if(rt_newVal<=IsoAgLib::IsoGnssTypeMAX) t_gnssType = rt_newVal;};
  /** deliver GPS speed as [cm/s] */
  void setGpsSpeedCmSec( uint16_t rui16_newVal ) { ui16_speedCmSec = rui16_newVal;};
  /** deliver GPS Heading [1x10E-4rad] */
  void setGpsHeadingRad10Minus4( uint16_t rui16_newVal ) { ui16_headingRad10Minus4 = rui16_newVal; };
  /** deliver number of received satellites */
  void setSatelliteCnt( uint8_t rui8_newVal ) { ui8_satelliteCnt = rui8_newVal;};
  /** deliver HDOP with scaling [1x10E-2] */
  void setHdop10Minus2( uint16_t rui16_newVal ) { i16_hdop = rui16_newVal;};
  /** PDOP with scaling [1x10E-2] */
  void setPdop10Minus2( uint16_t rui16_newVal ) { i16_pdop = rui16_newVal;};
  #endif // END of NMEA_2000_FAST_PACKET

  /** set the calendar month value
    @param rb_month actual calendar month value
    */
  void setMonthUtc(uint8_t rb_month){i32_lastCalendarSet = System_c::getTime();bit_calendar.month = rb_month;};
  /** set the calendar day value
    @param rb_day actual calendar day value
    */
  void setDayUtc(uint8_t rb_day){i32_lastCalendarSet = System_c::getTime();bit_calendar.day = rb_day;};
  /** set the calendar hour value
    @param rb_hour actual calendar hour value
    */
  void setHourUtc(uint8_t rb_hour){i32_lastCalendarSet = System_c::getTime();bit_calendar.hour = rb_hour;};
  /** set the calendar minute value
    @param rb_minute actual calendar minute value
    */
  void setMinuteUtc(uint8_t rb_minute){i32_lastCalendarSet = System_c::getTime();bit_calendar.minute = rb_minute;};
  /** set the calendar second value
    @param rb_second actual calendar second value
    */
  void setSecond(uint8_t rb_second){i32_lastCalendarSet = System_c::getTime();bit_calendar.second = rb_second;};
  /** set the calendar millisecond value
    @param rb_millisecond actual calendar second value
    */
  void setMillisecond(uint16_t rui16_millisecond){i32_lastCalendarSet = System_c::getTime();bit_calendar.msec = rui16_millisecond;};
  /** set the local time to UTC time offsets */
  void setLocalTimeOffsets( int16_t ri16_hourOffset, uint16_t rui16_minuteOffset )
  {bit_calendar.timezoneHourOffsetMinus24 = (ri16_hourOffset+24); bit_calendar.timezoneMinuteOffset = rui16_minuteOffset;};


  /// Getter functions
  /** check if a calendar information was received since init */
  bool isCalendarReceived() const;
  /** get the calendar year value
      @return actual calendar year value
    */
  int16_t yearLocal() const;
  /** get the calendar year value
    @return actual calendar year value
    */
  int16_t yearUtc() const {return bit_calendar.year;};
  /** get the calendar month value
    @return actual calendar month value
    */
  uint8_t monthLocal() const;
  /** get the calendar month value
    @return actual calendar month value
    */
  uint8_t monthUtc() const {return bit_calendar.month;};
  /** get the calendar day value
    @return actual calendar day value
    */
  uint8_t dayLocal() const;
  /** get the calendar day value
    @return actual calendar day value
    */
  uint8_t dayUtc() const {return bit_calendar.day + (calendarSetAge() / 86400000);};
  /** get the calendar hour value
    @return actual calendar hour value
    */
  uint8_t hourLocal() const;
  /** get the calendar hour value
    @return actual calendar hour value
    */
  uint8_t hourUtc() const {return ((bit_calendar.hour + (calendarSetAge() / 3600000))%24);};
  /** get the calendar minute value
    @return actual calendar minute value
    */
  uint8_t minuteLocal() const;
  /** get the calendar minute value
    @return actual calendar minute value
    */
  uint8_t minuteUtc() const {return ((bit_calendar.minute + (calendarSetAge() / 60000))%60);};
  /** get the calendar second value
    @return actual calendar second value
    */
  uint8_t second() const {return ((bit_calendar.second + (calendarSetAge() / 1000))%60);};
  /** get the calendar millisecond value
      @return actual calendar second value
    */
  uint16_t millisecond() const {return ((bit_calendar.msec + calendarSetAge())%1000);};

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLatitudeDegree10Minus7( void ) const { return i32_latitudeDegree10Minus7; };
  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLongitudeDegree10Minus7( void ) const { return i32_longitudeDegree10Minus7; };

    #if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
  /** check if an NMEA2000 position signal was received */
  bool isPositionReceived() const;
  /** deliver Minute GPS Latitude */
  float getGpsLatitudeMinute( void ) const { return ( i32_latitudeDegree10Minus7 * 6.0e-4  ); };
  /** deliver Minute GPS Longitude */
  float getGpsLongitudeMinute( void ) const { return ( i32_longitudeDegree10Minus7 * 6.0e-4 ); };
  /** deliver Degree GPS Latitude */
  float getGpsLatitudeDegree( void ) const { return ( float(i32_latitudeDegree10Minus7) * 1.0e-7  ); };
  /** deliver Degree GPS Longitude */
  float getGpsLongitudeDegree( void ) const { return ( float(i32_longitudeDegree10Minus7) * 1.0e-7 ); };
  #endif // END of USE_FLOAT_DATA_TYPE and USE_DIN_GPS

  #ifdef NMEA_2000_FAST_PACKET
  /** deliver GPS altitude - [cm] */
  uint32_t getGpsAltitudeCm( void ) const { return ui32_altitudeCm; };
  /** deliver GPS receive qualitiy */
  IsoAgLib::IsoGnssMethod_t getGnssMode( void ) const { return t_gnssMethod;};
  /** simply check for some sort of Differential signal */
  bool hasDifferentialPosition() const { return ( ( t_gnssMethod > IsoAgLib::IsoGnssFix ) && ( t_gnssMethod < IsoAgLib::IsoDrEstimated ) )?true:false;};
  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  IsoAgLib::IsoGnssType_t getGnssType(void) const { return t_gnssType;};
  /** deliver GPS speed as [cm/s] */
  uint16_t getGpsSpeedCmSec( void ) const { return ui16_speedCmSec;};
  /** deliver GPS Heading [1x10E-4rad] */
  uint16_t getGpsHeadingRad10Minus4( void ) const { return ui16_headingRad10Minus4; };
  /** deliver number of received satellites */
  uint8_t satelliteCnt() const { return ui8_satelliteCnt;};
  /** deliver HDOP with scaling [1x10E-2] */
  int16_t hdop10Minus2() const { return i16_hdop;};
  /** PDOP with scaling [1x10E-2] */
  int16_t pdop10Minus2() const { return i16_pdop;};
  #endif // END NMEA_2000_FAST_PACKET

  /** deliver age of last gps-update in milliseconds */
  uint16_t getGpsUpdateAge( void ) const { return 2000; /** @todo ACHIM - Implement this dummy function */ };

  /** deliver the devKey of the sender of the base data
    possible errors:
      * Err_c::range rui8_typeNr doesn't match valid base msg type number
    @param rt_typeGrp base msg type no of interest: BaseDataGroup1 | BaseDataGroup2
    @return DEV_KEY code of member who is sending the interested base msg type
    */
  const DevKey_c& senderDevKey(IsoAgLib::BaseDataGroup_t rt_typeGrp);

private:
  // Private methods
  friend class SINGLETON_DERIVED(TimePosGPS_c,ElementBase_c);

  /** HIDDEN constructor for a TimePosGPS_c object instance which can optional
    set the configuration for send/receive for base msg type NMEA 2000 GPS
    and calendar
    NEVER instantiate a variable of type TimePosGPS_c within application
    only access TimePosGPS_c via getTimePosGpsInstance() or getTimePosGpsInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
  TimePosGPS_c() {};
  /** initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
    */
  void singletonInit();
  /** deliver reference to data pkg
    @return reference to the member BasePkg_c, which encapsulates the CAN send structure
    */
  BasePkg_c& data(){return c_data;};
  /** deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
    */
  CANPkgExt_c& dataBase();

  /** deliver time between now and last calendar set in [msec]
    @return msec since last calendar set
    */
  int32_t calendarSetAge() const {return (System_c::getTime() - i32_lastCalendarSet);};

  /** check if a received message should be parsed */
  bool checkParseReceived(const DevKey_c& rrefc_currentSender, const DevKey_c& rrefc_activeSender, IsoAgLib::BaseDataGroup_t rt_selfSend ) const
  {
    return ( ( ( t_mySendSelection & rt_selfSend   ) == 0 ) // I'm not the sender
          && ( // one of the following conditions must be true
               (rrefc_activeSender == rrefc_currentSender) // actual sender equivalent to last
            || (rrefc_activeSender.isUnspecified() ) // last sender has not correctly claimed address member
             )
           )?true:false;
  };

  #ifdef USE_DIN_9684
  /** send a DIN9684 base information PGN.
   * this is only called when sending ident is configured and it has already claimed an address
   */
  bool dinTimeEvent( void );

  /** process a DIN9684 base information PGN */
  bool dinProcessMsg();
  #endif // END of USE_DIN_9684

  #if defined(USE_ISO_11783)
  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool isoTimeEvent( void );

  /** send position rapid update message */
  void isoSendPositionRapidUpdate( void );

  /** process a ISO11783 base information PGN */
  bool isoProcessMsg();

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
  #endif // END USE_ISO_11783

private:
  // Private attributes
  /** temp data where received data is put */
  BasePkg_c c_data;
    /** last time of calendar msg [msec] */
  int32_t i32_lastCalendar;
    /** last timestamp where calendar was set
    -> use this to calculate new time
    */
  int32_t i32_lastCalendarSet;
  /** devKey which act as sender of base msg */
  const DevKey_c* pc_devKey;

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
  const struct CNAMESPACE::tm* Utc2LocalTime() const;

  /** bitmask with selection of all base data types to send */
  IsoAgLib::BaseDataGroup_t t_mySendSelection;
  /** DEVKEY of calendar sender */
  DevKey_c c_sendCalendarDevKey;

  #ifdef USE_DIN_9684
  /** flag to detect, if receive filters for DIN are created */
  bool b_dinFilterCreated;
  #endif // END of  USE_DIN_9684

  /** raw GPS latitude [degree] ; Lat_Min < 0 --> South */
  int32_t i32_latitudeDegree10Minus7;
  /** raw GPS longitude [degree]; Long_Min < 0 --> West */
  int32_t i32_longitudeDegree10Minus7;

  #ifdef USE_ISO_11783
  /// General
  /** flag to detect, if receive filters for ISO are created */
  bool b_isoFilterCreated;

  #ifdef NMEA_2000_FAST_PACKET
  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoPositionStream;
  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoDirectionStream;
  #endif // END of NMEA_2000_FAST_PACKET

  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoPositionSimple;

  #ifdef NMEA_2000_FAST_PACKET
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
  /** DEVKEY of GPS data sender */
  DevKey_c c_sendGpsDevKey;
  #endif // END of USE_ISO_11783
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
