/*
  itractorgroundbased_c.h: interface for retrieving GB speed and distance
    from a tractor ecu.

  (C) Copyright 2016 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITRACTORGROUNDBASED_C_H
#define ITRACTORGROUNDBASED_C_H

#include "impl/tractorgroundbased_c.h"

namespace IsoAgLib {

  class iTractorGroundBased_c : private __IsoAgLib::TractorGroundBased_c
  {
  public:
    /** is looking for a valid speed value
        @return if speed is valid
      */
    bool isSpeedUsable() const { return TractorGroundBased_c::isSpeedUsable(); }

    /** only use this value if "isSpeedUsable()" returns true!
        @return speed value [mm/s]
      */
    uint16_t speed() const { return TractorGroundBased_c::speed(); }

    /** is looking for a valid distance value
        @return if distance is valid
      */
    bool isDistanceUsable() const { return TractorGroundBased_c::isDistanceUsable(); }

    /** @return distance value [mm]
      */
    uint32_t distance() const { return TractorGroundBased_c::distance(); }

    /** @return direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t direction() { return TractorGroundBased_c::direction(); }

    /** register an event handler that gets called for any incoming PGN.
        Please look into the implementation to see for which PGNs it is
        actually called.
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorGroundBased_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorGroundBased_c::deregisterMsgEventHandler( msgEventHandler ); }

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return TractorGroundBased_c::lastedTimeSinceUpdate(); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return TractorGroundBased_c::lastUpdateTime(); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractorGroundBased_c& getITractorGroundBasedInstance();
#endif
    friend iTractorGroundBased_c& getITractorGroundBasedInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractorGroundBased_c& getITractorGroundBasedInstance()
  { return static_cast<iTractorGroundBased_c&>(__IsoAgLib::getTractorGroundBasedInstance( 0 )); }
#endif
  inline iTractorGroundBased_c& getITractorGroundBasedInstance( unsigned instance )
  { return static_cast<iTractorGroundBased_c&>(__IsoAgLib::getTractorGroundBasedInstance( instance )); }

}
#endif
