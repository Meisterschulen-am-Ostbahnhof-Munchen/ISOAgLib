/*
  itractor_c.h: interface for retrieving WB speed and distance
    and OperatorDirection/Master/Key switch state from a tractor ecu.
    Also Maintain Power can be commanded to the TECU.

  (C) Copyright 2016 - 2019 by OSB AG

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
#ifndef ITRACTOR_C_H
#define ITRACTOR_C_H

#include "impl/tractor_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>


namespace IsoAgLib
{

  class iTractor_c : private __IsoAgLib::Tractor_c
  {
  public:
    //! Register implement for MAINTAIN_POWER messages,
    //! including Park, Work, Transport states
    //! @return success - only one implement can be registered!
    bool registerImplement( iIdentItem_c& ident ) { return Tractor_c::registerImplement( static_cast<__IsoAgLib::IdentItem_c&>( ident ) ); }

    /** register an event handler that gets called for any incoming message
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { Tractor_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { Tractor_c::deregisterMsgEventHandler( msgEventHandler ); }

    
    /// SPEED & DISTANCE
    ///////////////////

    /** is looking for a valid speed value
        @return if speed is valid
      */
    bool isSpeedUsable() const { return Tractor_c::isSpeedUsable(); }

    /** only use this value if "isSpeedUsable()" returns true!
        @return speed value [mm/s]
      */
    uint16_t speed() const { return Tractor_c::speed(); }

    /** @return direction of travel */
    IsoAgLib::IsoDirectionFlag_t direction() { return Tractor_c::direction(); }

    /** is looking for a valid distance value
        @return if distance is valid
      */
    bool isDistanceUsable() const { return Tractor_c::isDistanceUsable(); }

    /** @return distance value [mm]
      */
    uint32_t distance() const { return Tractor_c::distance(); }

    /** return Operator Direction */
    IsoAgLib::IsoOperatorDirectionFlag_t operatorDirection() const { return Tractor_c::operatorDirection(); }
    
    /** return Implement Start/Stop Switch State, also called Master Switch */
    IsoAgLib::IsoActiveFlag_t masterSwitch() const { return Tractor_c::masterSwitch(); }

    /** return Ignition Key Switch State. */
    IsoAgLib::IsoActiveFlag_t keySwitch() const { return Tractor_c::keySwitch(); }

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return Tractor_c::lastedTimeSinceUpdate(); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return Tractor_c::lastUpdateTime(); }


    // POWER MANAGEMENT & STATES
    ///////////////////////////

    /** Client function: force maintain power from tractor (sends once per call) and
      * send current Implement Transport/Park/Work States
      * Be sure to call faster than 2s during shut-down phase as long as more power is required
      * or call when the Implement States have been changed.
      * @pre An Implement must have been registered using \ref registerImplement
      * @param ecuPower IsoActive -> maintain ECU power
      * @param actuatorPower IsoActive-> maintain actuator power
      */
    void maintainPowerAndSendImplStates( IsoAgLib::IsoActiveFlag_t ecuPower, IsoAgLib::IsoActiveFlag_t actuatorPower ) { Tractor_c::maintainPowerAndSendImplStates( ecuPower, actuatorPower ); }

    /** set state of implement in transport state
      * Information is only sent out in a subsequent "maintainPowerAndSendImplStates(..)"-call!
      * @param val state of implement in transport state
      */
    void setImplTransportState( IsoAgLib::IsoImplTransportFlag_t val ) { Tractor_c::setImplTransportState( val ); }

    /** set state of implement in park state
      * Information is only sent out in a subsequent "maintainPowerAndSendImplStates(..)"-call!
      * @param val state of implement in park state
      */
    void setImplParkState( IsoAgLib::IsoImplParkFlag_t val ) { Tractor_c::setImplParkState( val ); }

    /** set state of implement ready to work state
      * Information is only sent out in a subsequent "maintainPowerAndSendImplStates(..)"-call!
      * @param val state of implement ready to work state
      */
    void setImplReadyToWorkState( IsoAgLib::IsoImplReadyToWorkFlag_t val ) { Tractor_c::setImplReadyToWorkState( val ); }

    /** set state of implement in work state
      * Information is only sent out in a subsequent "maintainPowerAndSendImplStates(..)"-call!
      * @param val state of implement in work state
      */
    void setImplInWorkState( IsoAgLib::IsoImplInWorkFlag_t val ) { Tractor_c::setImplInWorkState( val ); }

    /** @return state of implement in transport state
      */
    IsoAgLib::IsoImplTransportFlag_t getImplTransportState() const { return Tractor_c::getImplTransportState(); }

    /** @return state of implement in park state
      */
    IsoAgLib::IsoImplParkFlag_t getImplParkState() const { return Tractor_c::getImplParkState(); }

    /** @return state of implement ready to work state
      */
    IsoAgLib::IsoImplReadyToWorkFlag_t getImplReadyToWorkState() const { return Tractor_c::getImplReadyToWorkState(); }

    /** @return state of implement in work state
      */
    IsoAgLib::IsoImplInWorkFlag_t getImplInWorkState() const { return Tractor_c::getImplInWorkState(); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractor_c& getITractorInstance();
#endif
    friend iTractor_c& getITractorInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractor_c& getITractorInstance()
  { return static_cast<iTractor_c&>(__IsoAgLib::getTractorInstance( 0 )); }
#endif
  inline iTractor_c& getITractorInstance( unsigned instance )
  { return static_cast<iTractor_c&>(__IsoAgLib::getTractorInstance( instance )); }

}
#endif
