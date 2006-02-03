/***************************************************************************
               tracgeneral_c.h  - working on Base Data Msg Type 1, 2
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
#ifndef TRACGENERAL_C_H
#define TRACGENERAL_C_H

#include <IsoAgLib/typedef.h>
#include "../ibasetypes.h"
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/util/impl/getypos_c.h>

#include "basepkg_c.h"
#include <ctime>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class TracGeneral_c;
typedef SINGLETON_DERIVED(TracGeneral_c,ElementBase_c) SingletonTracGeneral_c;
/**
  working on Base Data Msg Type 1 and 2;
  stores, updates  and delivers all base data informations;
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
  per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
*/

class TracGeneral_c  : public SingletonTracGeneral_c {
public: // Public methods
  /* ********************************************* */
  /** \name Management Functions for class TracGeneral_c  */
  /*@{*/

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances, to receive
      the needed CAN msg with base msg type 1 and 2
      possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rb_sendState optional setting to express TECU sending state
   */
  void init(const DevKey_c* rpc_devKey = NULL, bool rb_sendState = false );

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close(void);

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
  bool timeEvent(void);
  /** check if filter boxes shall be created - create only ISO or DIN filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void checkCreateReceiveFilter(void);

  /** config the Base_c object after init -> set pointer to devKey and
      config send/receive of different base msg types
      @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rb_sendState optional setting to express TECU sending state
   */
  void config(const DevKey_c* rpc_devKey, bool rb_sendState);
  /** Retrieve the last update time */
  int32_t lastedTimeSinceUpdate() const { return (System_c::getTime() - i32_lastEngineHitch);};
  /** Retrieve the time of last update */
  int32_t lastUpdateTime() const { return i32_lastEngineHitch;};
    /** deliver the devKey of the sender of the base data
  @return DEV_KEY code of member who is sending the intereested base msg type
     */
  const DevKey_c& senderDevKey() const { return c_sendDevKey;};

  /** destructor for Base_c which has nothing to do */
  virtual ~TracGeneral_c() { close();};
  /** process received base msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a msg
      possible errors:
          * Err_c::lbsBaseSenderConflict base msg recevied from different member than before
      @see CANIO_c::processMsg
      @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
    */
  virtual bool processMsg();
  #ifdef USE_DIN_9684
  /** helper function to do the parsing of the flag data of a
   * received DIN9684 base message with Pto,Hitch,Engine information */
  void dinParseHitchEngineFlags(const BasePkg_c& rrefc_pkg);
  /** helper function to set the Hitch and Engine flags of a DIN base data message */
  void dinSetHitchEngineFlags(BasePkg_c& rrefc_pkg);

  /** config the Base_c object after init -> set pointer to devKey and
    config send/receive of different base msg types
    @param rpc_devKey pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rb_sendState optional setting to express TECU sending state
   */
  void configFuel(const DevKey_c* rpc_devKey, bool rb_sendState);
  #endif
  /*@}*/

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
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
  void setHitchFrontDraft(int32_t ri32_val) { i16_frontDraft = ((ri32_val + 320000) / 10);};
  /** set rear hitch draft
    * @return rear hitch draft [-320.000N;322.550N]; 1N/bit
    */
  void setHitchRearDraft(int32_t ri32_val) { i16_rearDraft = ((ri32_val + 320000) / 10);};
  /** set front hitch nominal link force
    * @return front nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchFrontLowerLinkForce(int16_t ri16_val) { ui8_frontLinkForce = ((ri16_val + 1000) / 8);};
  /** set rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchRearLowerLinkForce(int16_t ri16_val) { ui8_rearLinkForce = ((ri16_val + 1000) / 8);};
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
  #endif
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get engine speed
      @return actual engine rpm speed value
    */
  int16_t engine() const { return i16_engine;};

  /** get rear hitch
      @return actual position of rear hitch
    */
  uint8_t hitchRear() const {return b_hitchRear;};
  /** get front hitch
      @return actual position of front hitch
    */
  uint8_t hitchFront() const {return b_hitchFront;};

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
  int16_t hitchFrontLowerLinkForce() const { return ((static_cast<int16_t>(ui8_frontLinkForce) * 8) - 1000);};
  /** deliver rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  int16_t hitchRearLowerLinkForce() const { return ((static_cast<int16_t>(ui8_rearLinkForce) * 8) - 1000);};
  /** deliver the ISO key switch state of the tractor
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
  int32_t lastMaintainPowerRequest() const { return ui32_lastMaintainPowerRequest;};
  /** check whether maintenance of ECU power was requested */
  bool maintainEcuPower() const { return b_maintainEcuPower;};
  /** check whether maintenance of actuator power was requested */
  bool maintainActuatorPower() const { return b_maintainActuatorPower;};
  /** check whether maintenance of power
    * for implement in transport state was requested */
  bool maintainPowerForImplInTransport() const { return b_maintainPowerForImplInTransport;};
  /** check whether maintenance of power
    * for implement in park state was requested */
  bool maintainPowerForImplInPark() const { return b_maintainPowerForImplInPark;};
  /** check whether maintenance of power
    * for implement in work state was requested */
  bool maintainPowerForImplInWork() const { return b_maintainPowerForImplInWork;};

  bool isVtLanguageReceived()   const { return b_languageVtReceived; };
  bool isTecuLanguageReceived() const { return b_languageTecuReceived; };

  const uint8_t* getVtLanguage()   const { return p8ui8_languageVt; };
  const uint8_t* getTecuLanguage() const { return p8ui8_languageTecu; };
  #endif
  /*@}*/

private:
  // Private methods
  friend class SINGLETON_DERIVED(TracGeneral_c,ElementBase_c);
  /** HIDDEN constructor for a TracGeneral_c object instance which can optional
      set the configuration for send/receive for base msg type 1 and 2
      NEVER instantiate a variable of type TracGeneral_c within application
      only access TracGeneral_c via getTracGeneralInstance() or getTracGeneralInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
      @param rpc_devKey optional pointer to the DEV_KEY variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
    */
  TracGeneral_c() {};
  /** initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead.
    */
  void singletonInit();
  /** deliver reference to data pkg
      @return reference to the member BasePkg_c, which encapsulates the CAN send structure
    */
  BasePkg_c& data() {return c_data;};
  /** deliver reference to data pkg as reference to CANPkgExt_c
      to implement the base virtual function correct
    */
  CANPkgExt_c& dataBase();

  /** check if a received message should be parsed */
  bool checkParseReceived(const DevKey_c& rrefc_currentSender) const
  {
    return ( ( !b_sendState ) // I'm not the sender
          && ( // one of the following conditions must be true
               (c_sendDevKey == rrefc_currentSender) // actual sender equivalent to last
            || (c_sendDevKey.isUnspecified() ) // last sender has not correctly claimed address member
             )
           )?true:false;
  };

  #ifdef USE_DIN_9684
  /** check if a received message should be parsed */
  bool checkParseReceivedFuel(const DevKey_c& rrefc_currentSender) const
  {
    return ( ( !b_sendStateFuel ) // I'm not the sender
          && ( // one of the following conditions must be true
               (c_sendFuelDevKey == rrefc_currentSender) // actual sender equivalent to last
            || (c_sendFuelDevKey.isUnspecified() ) // last sender has not correctly claimed address member
             )
           )?true:false;
  };

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
  bool isoTimeEvent(void);
  /** process a ISO11783 base information PGN */
  bool isoProcessMsg();
  #endif

private:
  // Private attributes
  /** temp data where received data is put */
  BasePkg_c c_data;
  /** last time of base_2 msg [msec] */
  int32_t i32_lastEngineHitch;
  /** flag to store sending state of engine and hitch */
  bool b_sendState;
  /** DEVKEY of engine and hitch sender */
  DevKey_c c_sendDevKey;
  /** devKey which act as sender of base msg */
  const DevKey_c* pc_devKey;

  /** engine speed */
  int16_t i16_engine;
  /** front hitch data */
  uint8_t b_hitchFront;
  /** rear hitch data */
  uint8_t b_hitchRear;

  #ifdef USE_DIN_9684
  /** last time of fuel msg [msec] */
  int32_t i32_lastFuel;
  /** send state for fuel */
  bool b_sendStateFuel;
  /** DevKey_c for fuel sender */
  DevKey_c c_sendFuelDevKey;


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
  /// General
  /** VT language information */
  uint8_t p8ui8_languageVt[8];
  /** TECU language information */
  uint8_t p8ui8_languageTecu[8];
  /** VT language reception information */
  bool b_languageVtReceived;
  /** TECU language reception information */
  bool b_languageTecuReceived;

  /// General
  /** flag to detect, if receive filters for ISO are created */
  bool b_isoFilterCreated;
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
  #endif
};

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracGeneral_c& getTracGeneralInstance(uint8_t rui8_instance = 0);
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracGeneral_c& getTracGeneralInstance(void);
  #endif

} // End Namespace __IsoAgLib
#endif
