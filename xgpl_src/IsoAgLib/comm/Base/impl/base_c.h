/***************************************************************************
                          base_c.h  - working on Base Data Msg Type 1, 2
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
#ifndef BASE_H
#define BASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
//#include <Application_Config/isoaglib_config.h>
#include <IsoAgLib/typedef.h>
#include "../ibasetypes.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/getypos_c.h>

#include "basepkg_c.h"

#if defined(NMEA_2000_FAST_PACKET) && defined(USE_ISO_11783)
#include <IsoAgLib/comm/Multipacket/multireceiveclient_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisendstreamer_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#endif
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
    virtual void setDataNextStreamPart (__IsoAgLib::MultiSendPkg_c* mspData, uint8_t bytes);
    /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards
   */
    virtual void setDataNextFastPacketStreamPart (__IsoAgLib::MultiSendPkg_c* mspData, uint8_t bytes, uint8_t rui8_offset = 0 );

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
#endif

class Base_c;
typedef SINGLETON_DERIVED(Base_c,ElementBase_c) SingletonBase_c;
/**
  working on Base Data Msg Type 1, 2 and Calendar;
  stores, updates  and delivers all base data informations;
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
   per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  *@author Dipl.-Inform. Achim Spangler
*/
class Base_c : public SingletonBase_c
#if defined(NMEA_2000_FAST_PACKET) && defined(USE_ISO_11783)
    , public IsoAgLib::MultiReceiveClient_c
#endif
{
public: // Public methods
  /* ********************************************* */
  /** \name Management Functions for class Base_c  */
  /*@{*/

  /**
    initialise element which can't be done during construct;
    above all create the needed FilterBox_c instances, to receive
    the needed CAN msg with base msg type 1,2 and calendar

    possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
    @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
  */
  void init(const DevKey_c* rpc_devKey = NULL, IsoAgLib::BaseDataGroup_t rt_mySendSelection = IsoAgLib::BaseDataNothing );

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );

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
  bool timeEvent( void );
  /** check if filter boxes shall be created - create only ISO or DIN filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void checkCreateReceiveFilter( void );

  /**
    config the Base_c object after init -> set pointer to devKey and
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
  virtual ~Base_c() { close();};
  /**
    process received base msg and store updated value for later reading access;
    called by FilterBox_c::processMsg after receiving a msg

    possible errors:
        * Err_c::lbsBaseSenderConflict base msg recevied from different member than before
    @see CANIO_c::processMsg
    @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
  */
  virtual bool processMsg();

  #ifdef USE_ISO_11783
  /**
  send ISO11783 calendar PGN
  @param rc_devKey DEV_KEY code off calling item which wants to send
  @parm ri32_time timestamp where calendar was last sent (default autodetect)

    possible errors:
   * dependant error in CANIO_c on CAN send problems
  @see CANPkg_c::getData
  @see CANPkgExt_c::getData
  @see CANIO_c::operator<<
   */
  void isoSendCalendar(const DevKey_c& rc_devKey);
  #endif
  /*@}*/

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/

  /**
    set the value of real speed (measured by radar)
    @param ri16_val value to store as real radar measured speed
  */
  void setSpeedReal(int16_t ri16_val) {i16_speedReal = ri16_val;};
  /**
    set the value of theoretical speed (calculated from gear)
    @param ri16_val value to store as theoretical gear calculated speed
  */
  void setSpeedTheor(int16_t ri16_val) {i16_speedTheor = ri16_val;};

  /**
    set the real (radar measured) driven distance with int16_t val
    @param ri16_val value to store as real radar measured distance
  */
  void setDistReal(int16_t ri16_val)
  { // use function to detect and handle 16bit integer overflows
    setOverflowSecure(i32_distReal, i16_lastDistReal, ri16_val);
  };

  /**
    set the real (radar measured) driven distance with int32_t val
    @param rreflVal value to store as real radar measured distance
  */
  void setDistReal(const int32_t& rreflVal);

  /**
    set the theoretical (gear calculated) driven distance with int16_t val
    @param ri16_val value to store as theoretical (gear calculated) driven distance
  */
  void setDistTheor(int16_t ri16_val)
  { // use function to detect and handle 16bit integer overflows
    setOverflowSecure(i32_distTheor, i16_lastDistTheor, ri16_val);
  };
  /**
    set the theoretical (gear calculated) driven distance with int32_t val
    @param rreflVal value to store as theoretical (gear calculated) driven distance
  */
  void setDistTheor(const int32_t& rreflVal);

  /**
    set rear PTO
    @param ri16_val value to store as the speed of the rear PTO
  */
  void setPtoRear(int16_t ri16_val){i16_ptoRear = ri16_val;};
  /**
    set front PTO
    @param ri16_val value to store as the speed of the front PTO
  */
  void setPtoFront(int16_t ri16_val){i16_ptoFront = ri16_val;};
  /**
  set engine speed
  @param ri16_val value to store as engine rpm value
   */
  void setEngine(int16_t ri16_val){i16_engine = ri16_val;};

  /**
  set rear hitch
  @param rb_val uint8_t value to store as position of rear hitch
   */
  void setHitchRear(uint8_t rb_val){ b_hitchRear = rb_val;};
  /**
  set front hitch
  @param rb_val uint8_t value to store as position of front hitch
   */
  void setHitchFront(uint8_t rb_val){ b_hitchFront = rb_val;};

  /**
  set the calendar value
  @param ri16_year value to store as year
  @param rb_month value to store as month
  @param rb_day value to store as day
  @param rb_hour value to store as hour
  @param rb_minute value to store as minute
  @param rb_second value to store as second
   */
  void setCalendarUtc(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second, uint16_t rui16_msec = 0);
  /**
  set the calendar value as local time ( take local time offsets into account )
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

  #ifdef USE_DIN_9684
  /** deliver rear left draft */
  void setRearLeftDraft( int16_t ri16_val ) { i16_rearLeftDraft = ri16_val;};
  /** deliver rear right draft */
  void setRearRightDraft( int16_t ri16_val ) { i16_rearRightDraft = ri16_val;};
  /** deliver rear total draft Newton */
  void setRearDraftNewton( int16_t ri16_val ) { i16_rearDraftNewton = ri16_val;};
  /** deliver rear total draft Percent */
  void setRearDraftNominal( uint8_t rui8_val ) { ui8_rearDraftNominal = rui8_val;};
  /** deliver fuel consumption L/h */
  void setFuelRate( int16_t ri16_val ) { i16_fuelRate = ri16_val;};
  /** deliver fuel temperature °C */
  void setFuelTemperature( uint8_t rui8_val ) { ui8_fuelTemperature = rui8_val;};
  #endif
  #ifdef USE_ISO_11783
  /** set front hitch draft
    * @return front hitch draft [-320.000N;322.550N]; 1N/bit
    */
  void setHitchFrontDraft(int32_t ri32_val)
    { i16_frontDraft = ((ri32_val + 320000) / 10);};
  /** set rear hitch draft
    * @return rear hitch draft [-320.000N;322.550N]; 1N/bit
    */
  void setHitchRearDraft(int32_t ri32_val)
    { i16_rearDraft = ((ri32_val + 320000) / 10);};
  /** set front hitch nominal link force
    * @return front nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchFrontLowerLinkForce(int16_t ri16_val)
    { ui8_frontLinkForce = ((ri16_val + 1000) / 8);};
  /** set rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchRearLowerLinkForce(int16_t ri16_val)
    { ui8_rearLinkForce = ((ri16_val + 1000) / 8);};
  /** set explicit information whether front PTO is engaged
    * @param rt_val IsoActive -> PTO is engaged
    */
  void setPtoFrontEngaged(IsoAgLib::IsoActiveFlag_t rt_val) { t_frontPtoEngaged = rt_val;};
  /** set explicit information whether rear PTO is engaged
    * @param rt_val IsoActive -> PTO is engaged
    */
  void setPtoRearEngaged(IsoAgLib::IsoActiveFlag_t rt_val) { t_rearPtoEngaged = rt_val;};
  /** set std RPM of front PTO (1000 n/min; 540 n/min)
    * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
    */
  void setPtoFront1000(IsoAgLib::IsoActiveFlag_t rt_val) { t_frontPto1000 = rt_val;};
  /** set std RPM of rear PTO (1000 n/min; 540 n/min)
    * @param rt_val IsoActive == 1000 n/min; false == 540 n/min
    */
  void setPtoRear1000(IsoAgLib::IsoActiveFlag_t rt_val) { t_rearPto1000 = rt_val;};
  /** set economy mode of front PTO
    * @param rt_val IsoActive -> front PTO is in economy mode
    */
  void setPtoFrontEconomy(IsoAgLib::IsoActiveFlag_t rt_val) { t_frontPtoEconomy = rt_val;};
  /** set economy mode of rear PTO
    * @param rt_val IsoActive -> rear PTO is in economy mode
    */
  void setPtoRearEconomy(IsoAgLib::IsoActiveFlag_t rt_val) { t_rearPtoEconomy = rt_val;};

  /**
    * set the ISO key switch state of the tractor
    * @param rt_val IsoActive -> key switch ON
    */
  void setKeySwitch(IsoAgLib::IsoActiveFlag_t rt_val) { t_keySwitch = rt_val;};
  /** set the maximum power time of the tractor in [min]
    * @return maximum power time of the tractor in [min]
    */
  void setMaxPowerTime(uint8_t rui8_val) { ui8_maxPowerTime = rui8_val;};
  /** force maintain power from tractor
    * @param rb_ecuPower true -> maintain ECU power
    * @param rb_actuatorPower -> maintain actuator power
    * @param rt_implTransport IsoActive -> implement is in transport state
    * @param rt_implPark IsoActive -> implement is in park state
    * @param rt_implWork IsoActive -> implement is in work state
    */
  void forceMaintainPower( bool rb_ecuPower, bool rb_actuatorPower, IsoAgLib::IsoActiveFlag_t rt_implTransport,
    IsoAgLib::IsoActiveFlag_t rt_implPark, IsoAgLib::IsoActiveFlag_t rt_implWork);

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  void setGpsLatitudeDegree10Minus7( int32_t ri32_newVal ) { i32_latitudeDegree10Minus7 = ri32_newVal; };
  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  void setGpsLongitudeDegree10Minus7( int32_t ri32_newVal ) { i32_longitudeDegree10Minus7 = ri32_newVal; };

  #if defined(NMEA_2000_FAST_PACKET)
  /** deliver GPS altitude - [cm] */
  void setGpsAltitudeCm( uint32_t rui32_newVal ) { ui32_altitudeCm = rui32_newVal; };
  /** deliver GPS receive qualitiy */
  void setGnssMode( IsoAgLib::IsoGnssMethod_t rt_newVal ) { t_gnssMethod = rt_newVal;};
  /** deliver GNSS type ( e.g. GPS, GLONASS or SBAS ) */
  void setGnssType( IsoAgLib::IsoGnssType_t rt_newVal ) { t_gnssType = rt_newVal;};
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

  //  Operation: reactOnStreamStart
  //! Parameter:
  //! @param rc_ident:
  //! @param rui32_totalLen:
  virtual bool reactOnStreamStart(IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                                  uint32_t rui32_totalLen);

  //  Operation: processPartStreamDataChunk
  //! Parameter:
  //! @param rpc_stream:
  //! @param rb_isFirstChunk:
  //! @param rb_isLastChunkAndACKd:
  virtual bool processPartStreamDataChunk(IsoAgLib::iStream_c* rpc_stream,
                                          bool rb_isFirstChunk,
                                          bool rb_isLastChunkAndACKd);

  //  Operation: reactOnAbort
  virtual void reactOnAbort(IsoAgLib::ReceiveStreamIdentifier_c rc_ident);
  #endif
#endif
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/

  /**
    get the value of real speed (measured by radar)
    @return actual radar measured speed value
  */
  int16_t speedReal() const { return i16_speedReal;};
  /**
    get the value of theoretical speed (calculated from gear)
    @return theoretical gear calculated speed value
  */
  int16_t speedTheor() const { return i16_speedTheor;};

  /**
    get the real driven distance with int16_t val
    @return actual radar measured driven distance value
  */
  int32_t distReal() const { return i32_distReal;};
  /**
    get the real driven distance with int16_t val
    @return actual gear calculated driven distance value
  */
  int32_t distTheor() const { return i32_distTheor;};

  /**
    get rear pto
    @return actual rpm speed of rear PTO
  */
  int16_t ptoRear() const { return i16_ptoRear;};
  /**
    get front pto
    @return actual rpm speed of front PTO
  */
  int16_t ptoFront() const { return i16_ptoFront;};

  /**
    get engine speed
    @return actual engine rpm speed value
  */
  int16_t engine() const { return i16_engine;};

  /**
    get rear hitch
    @return actual position of rear hitch
  */
  uint8_t hitchRear() const {return  b_hitchRear;};
  /**
    get front hitch
    @return actual position of front hitch
  */
  uint8_t hitchFront() const {return  b_hitchFront;};

  /** check if a calendar information was received since init */
  bool isCalendarReceived() const;
  /**
      get the calendar year value
      @return actual calendar year value
  */
    int16_t yearLocal() const;
  /**
    get the calendar year value
    @return actual calendar year value
  */
  int16_t yearUtc() const {return bit_calendar.year;};
  /**
    set the calendar year value
    @param rui16_year actual calendar year value
  */
  void setYearUtc(uint16_t rui16_year){i32_lastCalendarSet = System_c::getTime();bit_calendar.year = rui16_year;};
  /**
    get the calendar month value
    @return actual calendar month value
  */
  uint8_t monthLocal() const;
  /**
    get the calendar month value
    @return actual calendar month value
  */
  uint8_t monthUtc() const {return bit_calendar.month;};
  /**
    set the calendar month value
    @param rb_month actual calendar month value
  */
  void setMonthUtc(uint8_t rb_month){i32_lastCalendarSet = System_c::getTime();bit_calendar.month = rb_month;};
  /**
    get the calendar day value
    @return actual calendar day value
  */
  uint8_t dayLocal() const;
  /**
    get the calendar day value
    @return actual calendar day value
  */
  uint8_t dayUtc() const {return bit_calendar.day + (calendarSetAge() / 86400000);};
  /**
    set the calendar day value
    @param rb_day actual calendar day value
  */
  void setDayUtc(uint8_t rb_day){i32_lastCalendarSet = System_c::getTime();bit_calendar.day = rb_day;};
  /**
    get the calendar hour value
    @return actual calendar hour value
  */
  uint8_t hourLocal() const;
  /**
    get the calendar hour value
    @return actual calendar hour value
  */
  uint8_t hourUtc() const {return ((bit_calendar.hour + (calendarSetAge() / 3600000))%24);};
  /**
    set the calendar hour value
    @param rb_hour actual calendar hour value
  */
  void setHourUtc(uint8_t rb_hour){i32_lastCalendarSet = System_c::getTime();bit_calendar.hour = rb_hour;};
  /**
    get the calendar minute value
    @return actual calendar minute value
  */
  uint8_t minuteLocal() const;
  /**
    get the calendar minute value
    @return actual calendar minute value
  */
  uint8_t minuteUtc() const {return ((bit_calendar.minute + (calendarSetAge() / 60000))%60);};
  /**
    set the calendar minute value
    @param rb_minute actual calendar minute value
  */
  void setMinuteUtc(uint8_t rb_minute){i32_lastCalendarSet = System_c::getTime();bit_calendar.minute = rb_minute;};
  /**
    get the calendar second value
    @return actual calendar second value
  */
  uint8_t second() const {return ((bit_calendar.second + (calendarSetAge() / 1000))%60);};
  /**
    set the calendar second value
    @param rb_second actual calendar second value
  */
  void setSecond(uint8_t rb_second){i32_lastCalendarSet = System_c::getTime();bit_calendar.second = rb_second;};
  /**
  set the calendar millisecond value
    @param rb_millisecond actual calendar second value
   */
  void setMillisecond(uint16_t rui16_millisecond){i32_lastCalendarSet = System_c::getTime();bit_calendar.msec = rui16_millisecond;};
  /**
      get the calendar millisecond value
      @return actual calendar second value
   */
  uint16_t millisecond() const {return ((bit_calendar.msec + calendarSetAge())%1000);};

  /** set the local time to UTC time offsets */
  void setLocalTimeOffsets( int16_t ri16_hourOffset, uint16_t rui16_minuteOffset )
  {bit_calendar.timezoneHourOffsetMinus24 = (ri16_hourOffset+24); bit_calendar.timezoneMinuteOffset = rui16_minuteOffset;};

  /**
    deliver the devKey of the sender of the base data

    possible errors:
        * Err_c::range rui8_typeNr doesn't match valid base msg type number
    @param rt_typeGrp base msg type no of interest: BaseDataGroup1 | BaseDataGroup2 | BaseDataCalendar
    @return DEV_KEY code of member who is sending the intereested base msg type
  */
  const DevKey_c& senderDevKey(IsoAgLib::BaseDataGroup_t rt_typeGrp);

  #ifdef USE_DIN_9684
  /** deliver rear left draft */
  int rearLeftDraft() const { return i16_rearLeftDraft;};
  /** deliver rear right draft */
  int rearRightDraft() const { return i16_rearRightDraft;};
  /** deliver rear total draft Newton */
  int rearDraftNewton() const { return i16_rearDraftNewton;};
  /** deliver rear total draft Percent */
  int rearDraftNominal() const { return ui8_rearDraftNominal;};
  /** deliver fuel consumption L/h */
  int fuelRate() const { return i16_fuelRate;};
  /** deliver fuel temperature °C */
  int fuelTemperature() const { return ui8_fuelTemperature;};
  #endif
  #ifdef USE_ISO_11783
  /** deliver front hitch draft
    * @return front hitch draft [-320.000N;322.550N]; 1N/bit
    */
  int32_t hitchFrontDraft() const { return ((i16_frontDraft * 10) - 320000);};
  /** deliver rear hitch draft
    * @return rear hitch draft [-320.000N;322.550N]; 1N/bit
    */
  int32_t hitchRearDraft() const { return ((i16_rearDraft * 10) - 320000);};
  /** deliver front hitch nominal link force
    * @return front nominal link force [-100%;100%]; 1 promille per bit
    */
  int16_t hitchFrontLowerLinkForce() const
    { return ((static_cast<int16_t>(ui8_frontLinkForce) * 8) - 1000);};
  /** deliver rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  int16_t hitchRearLowerLinkForce() const
    { return ((static_cast<int16_t>(ui8_rearLinkForce) * 8) - 1000);};
  /** deliver explicit information whether front PTO is engaged
    * @return IsoActive -> PTO is engaged
    */
  IsoAgLib::IsoActiveFlag_t ptoFrontEngaged() const { return t_frontPtoEngaged;};
  /** deliver explicit information whether rear PTO is engaged
    * @return IsoActive -> PTO is engaged
    */
  IsoAgLib::IsoActiveFlag_t ptoRearEngaged() const { return t_rearPtoEngaged;};
  /** deliver std RPM of front PTO (1000 n/min; 540 n/min)
    * @return IsoActive == 1000 n/min; false == 540 n/min
    */
  IsoAgLib::IsoActiveFlag_t ptoFront1000() const { return t_frontPto1000;};
  /** deliver std RPM of rear PTO (1000 n/min; 540 n/min)
    * @return IsoActive == 1000 n/min; false == 540 n/min
    */
  IsoAgLib::IsoActiveFlag_t ptoRear1000() const { return t_rearPto1000;};
  /** deliver economy mode of front PTO
    * @return IsoActive -> front PTO is in economy mode
    */
  IsoAgLib::IsoActiveFlag_t ptoFrontEconomy() const { return t_frontPtoEconomy;};
  /** deliver economy mode of rear PTO
    * @return IsoActive -> rear PTO is in economy mode
    */
  IsoAgLib::IsoActiveFlag_t ptoRearEconomy() const { return t_rearPtoEconomy;};

  /**
    * deliver the ISO key switch state of the tractor
    * @return IsoActive -> key switch ON
    */
  IsoAgLib::IsoActiveFlag_t keySwitch() const { return t_keySwitch;};
  /** deliver the maximum power time of the tractor in [min]
    * @return maximum power time of the tractor in [min]
    */
  uint8_t maxPowerTime() const { return ui8_maxPowerTime;};

  /** deliver last receive time of maintain power request
    * @return time in [ms] since system start -> comparable to system time
    */
  int32_t lastMaintainPowerRequest() const
    { return ui32_lastMaintainPowerRequest;};

  /** check whether maintenance of ECU power was requested */
  bool maintainEcuPower() const { return b_maintainEcuPower;};

  /** check whether maintenance of actuator power was requested */
  bool maintainActuatorPower() const { return b_maintainActuatorPower;};
  /** check whether maintenance of power
    * for implement in transport state was requested */
  bool maintainPowerForImplInTransport() const
    { return b_maintainPowerForImplInTransport;};
  /** check whether maintenance of power
    * for implement in park state was requested */
  bool maintainPowerForImplInPark() const
    { return b_maintainPowerForImplInPark;};
  /** check whether maintenance of power
    * for implement in work state was requested */
  bool maintainPowerForImplInWork() const
    { return b_maintainPowerForImplInWork;};


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
#endif
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
  // END NMEA_2000_FAST_PACKET
#endif
  /** deliver age of last gps-update in milliseconds */
  uint16_t getGpsUpdateAge( void ) const { return 2000; /** @todo ACHIM - Implement this dummy function */ };

  bool isVtLanguageReceived()   const { return b_languageVtReceived; };
  bool isTecuLanguageReceived() const { return b_languageTecuReceived; };

  const uint8_t* getVtLanguage()   const { return p8ui8_languageVt; };
  const uint8_t* getTecuLanguage() const { return p8ui8_languageTecu; };

#endif

  /*@}*/


private:
  // Private methods
  friend class SINGLETON_DERIVED(Base_c,ElementBase_c);
  /**
    HIDDEN constructor for a Base_c object instance which can optional
    set the configuration for send/receive for base msg type 1,2
    and calendar
    NEVER instantiate a variable of type Base_c within application
    only access Base_c via getBaseInstance() or getBaseInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
  */
  Base_c() {};
  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();
  /**
    deliver reference to data pkg
    @return reference to the member BasePkg_c, which encapsulates the CAN send structure
  */
  BasePkg_c& data(){return c_data;};
  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  CANPkgExt_c& dataBase();

  /**
    update distance values ; react on int16_t overflow
    @param reflVal to be updated value as int32_t variable
    @param refiVal to be updated value as 16bit int16_t variable
    @param rrefiNewVal new value given as reference to 16bit int
  */
  static void setOverflowSecure(int32_t& reflVal, int16_t& refiVal, const int16_t& rrefiNewVal);

  /**
    get int16_t overflowed val from long
    @param rreflVal value as int32_t (32bit) variable
    @return 16bit int16_t calculated with counting overflow from 32767 to (-32766)
  */
  static int16_t long2int(const int32_t& rreflVal);

  /**
    deliver time between now and last calendar set in [msec]
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
#endif
  #if defined(USE_ISO_11783)
  /** send a ISO11783 base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
    */
  bool isoTimeEvent( void );
  /** send Base1 Group data with ground&theor speed&dist */
  void isoSendBase1Update( void );
  /** send Base2 Group data with hitch and PTO state */
  void isoSendBase2Update( void );
  /** send position rapid update message */
  void isoSendPositionRapidUpdate( void );
  /** process a ISO11783 base information PGN */
  bool isoProcessMsg();

  #if defined(NMEA_2000_FAST_PACKET)
  /** send position as detailed stream */
  void isoSendPositionStream( void );
  /** send direction as detailed stream */
  void isoSendDirectionStream( void );

  //  Operation: reactOnLastChunk
  //! Parameter:
  //! @param rc_ident:
  //! @param rpc_stream:
  bool reactOnLastChunk(IsoAgLib::ReceiveStreamIdentifier_c rc_ident,
                        IsoAgLib::iStream_c& rpc_stream);

  //  Operation: reactOnAbort
  virtual void reactOnAbort(IsoAgLib::iStream_c* rpc_stream);
  // END NMEA_2000_FAST_PACKET
  #endif
  // END USE_ISO_11783
  #endif
private:
  // Private attributes
  /** temp data where received data is put */
  BasePkg_c c_data;
  /** last time of base_1 msg [msec] */
  int32_t i32_lastBase1;
  /** last time of base_2 msg [msec] */
  int32_t i32_lastBase2;
  /** last time of base_3 msg [msec] */
  int32_t i32_lastBase3;
  /** last time of fuel msg [msec] */
  int32_t i32_lastFuel;
  /** last time of calendar msg [msec] */
  int32_t i32_lastCalendar;
  /**
    last timestamp where calendar was set
    -> use this to calculate new time
  */
  int32_t i32_lastCalendarSet;

  /** distance */
  /** real distance as int32_t value (cumulates 16bit overflows) */
  int32_t i32_distReal;
  /** theoretical distance as int32_t value (cumulates 16bit overflows)*/
  int32_t i32_distTheor;
  /** last 16bit real distance to cope with 16bit overflows */
  int16_t i16_lastDistReal;
  /** last 16bit theoretical distance to cope with 16bit overflows */
  int16_t i16_lastDistTheor;

  /** speed */
  /** real speed */
  int16_t i16_speedReal;
  /** theoretical speed */
  int16_t i16_speedTheor;

  /** pto rear */
  int16_t i16_ptoRear;
  /** pto front */
  int16_t i16_ptoFront;
  /** engine speed */
  int16_t i16_engine;

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
  const struct tm* Utc2LocalTime() const;

  /** bitmask with selection of all base data types to send */
  IsoAgLib::BaseDataGroup_t t_mySendSelection;
  /** DEVKEY of base1 sender */
  DevKey_c c_sendBase1DevKey;
  /** DEVKEY of base2 sender */
  DevKey_c c_sendBase2DevKey;
  /** DEVKEY of base3 sender */
  DevKey_c c_sendBase3DevKey;
  /** DEVKEY of fuel data sender */
  DevKey_c c_sendFuelDevKey;
  /** DEVKEY of calendar sender */
  DevKey_c c_sendCalendarDevKey;
  /** front hitch data */
  uint8_t b_hitchFront;
  /** rear hitch data */
  uint8_t b_hitchRear;

#ifdef USE_DIN_9684
  /** flag to detect, if receive filters for DIN are created */
  bool b_dinFilterCreated;
  /** NEW from AGCO Fendt Vario: rear left draft */
  int16_t i16_rearLeftDraft;
  /** NEW from AGCO Fendt Vario: rear right draft */
  int16_t i16_rearRightDraft;
  /** NEW from AGCO Fendt Vario: rear total draft Newton */
  int16_t i16_rearDraftNewton;
  /** NEW from AGCO Fendt Vario: rear total draft Percent */
  uint8_t ui8_rearDraftNominal;
  /** NEW: fuel consumption L/h */
  int16_t i16_fuelRate;
  /** NEW from AGCO Fendt Vario: fuel temperature C */
  uint8_t ui8_fuelTemperature;
  #endif
  #ifdef USE_ISO_11783
  /** VT language information */
  uint8_t p8ui8_languageVt[8];
  /** TECU language information */
  uint8_t p8ui8_languageTecu[8];
  /** VT language reception information */
  bool b_languageVtReceived;
  /** TECU language reception information */
  bool b_languageTecuReceived;

  /** flag to detect, if receive filters for ISO are created */
  bool b_isoFilterCreated;

#ifdef NMEA_2000_FAST_PACKET
  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoPositionStream;
  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoDirectionStream;
#endif
  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoPositionSimple;

  /** key switch state */
  IsoAgLib::IsoActiveFlag_t t_keySwitch;
  /** maximum time of tractor power in [min] */
  uint8_t ui8_maxPowerTime;
  /** front hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
  int16_t i16_frontDraft;
  /** rear hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
  int16_t i16_rearDraft;
  /** front nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
  uint8_t ui8_frontLinkForce;
  /** rear nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
  uint8_t ui8_rearLinkForce;
  /** front PTO engaged */
  IsoAgLib::IsoActiveFlag_t t_frontPtoEngaged;
  /** rear PTO engaged */
  IsoAgLib::IsoActiveFlag_t t_rearPtoEngaged;
  /** mode of front PTO: true == 1000 n/min; false == 540 n/min */
  IsoAgLib::IsoActiveFlag_t t_frontPto1000;
  /** mode of rear PTO: true == 1000 n/min; false == 540 n/min */
  IsoAgLib::IsoActiveFlag_t t_rearPto1000;
  /** economy mode of front PTO */
  IsoAgLib::IsoActiveFlag_t t_frontPtoEconomy;
  /** economy mode of rear PTO */
  IsoAgLib::IsoActiveFlag_t t_rearPtoEconomy;

  /** last time of maintain power request [ms] */
  uint32_t ui32_lastMaintainPowerRequest;
  /** state whether maintenance of ECU power was requested */
  bool b_maintainEcuPower;
  /** state whether maintenance of actuator power was requested */
  bool b_maintainActuatorPower;
  /** state whether maintenance of power
    * for implement in transport state was requested */
  bool b_maintainPowerForImplInTransport;
  /** state whether maintenance of power
    * for implement in park state was requested */
  bool b_maintainPowerForImplInPark;
  /** state whether maintenance of power
    * for implement in work state was requested */
  bool b_maintainPowerForImplInWork;

  /** raw GPS latitude [degree] ; Lat_Min < 0 --> South */
  int32_t i32_latitudeDegree10Minus7;
  /** raw GPS longitude [degree]; Long_Min < 0 --> West */
  int32_t i32_longitudeDegree10Minus7;
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
#endif
  /** DEVKEY of GPS data sender */
  DevKey_c c_sendGpsDevKey;
#endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  Base_c& getBaseInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Base_c singleton instance */
  Base_c& getBaseInstance( void );
#endif


}
#endif
