/*
  itractormachineselected_c.h: interface for retrieving machine selected
    speed and distance from a tractor ecu.

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
#ifndef ITRACTORMACHINESELECTED_C_H
#define ITRACTORMACHINESELECTED_C_H

#include "impl/tractormachineselected_c.h"

namespace IsoAgLib {

  class iTractorMachineSelected_c : private __IsoAgLib::TractorMachineSelected_c
  {
  public:
    /** is looking for a valid speed value
        @return if speed is valid
      */
    bool isSpeedUsable() const { return TractorMachineSelected_c::isSpeedUsable(); }

    /** only use this value if "isSpeedUsable()" returns true!
        @return speed value [mm/s]
      */
    uint16_t speed() const { return TractorMachineSelected_c::speed(); }

    /** is looking for a valid distance value
        @return if distance is valid
      */
    bool isDistanceUsable() const { return TractorMachineSelected_c::isDistanceUsable(); }

    /** @return distance value [mm]
      */
    uint32_t distance() const { return TractorMachineSelected_c::distance(); }

    /** @return direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t direction() { return TractorMachineSelected_c::direction(); }

    /** @return Tractor ECU's present limit status */
    IsoAgLib::IsoLimitFlag_t speedLimitStatus() const { return TractorMachineSelected_c::speedLimitStatus(); }

    /** @return An indication of the speed source that is currently being reported */
    IsoAgLib::IsoSpeedSourceFlag_t speedSource() const { return TractorMachineSelected_c::speedSource(); }

    /** register an event handler that gets called for any incoming PGN.
        Please look into the implementation to see for which PGNs it is
        actually called.
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorMachineSelected_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorMachineSelected_c::deregisterMsgEventHandler( msgEventHandler ); }

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return TractorMachineSelected_c::lastedTimeSinceUpdate(); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return TractorMachineSelected_c::lastUpdateTime(); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractorMachineSelected_c& getITractorMachineSelectedInstance();
#endif
    friend iTractorMachineSelected_c& getITractorMachineSelectedInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractorMachineSelected_c& getITractorMachineSelectedInstance()
  { return static_cast<iTractorMachineSelected_c&>(__IsoAgLib::getTractorMachineSelectedInstance( 0 )); }
#endif
  inline iTractorMachineSelected_c& getITractorMachineSelectedInstance( unsigned instance )
  { return static_cast<iTractorMachineSelected_c&>(__IsoAgLib::getTractorMachineSelectedInstance( instance )); }

}
#endif
