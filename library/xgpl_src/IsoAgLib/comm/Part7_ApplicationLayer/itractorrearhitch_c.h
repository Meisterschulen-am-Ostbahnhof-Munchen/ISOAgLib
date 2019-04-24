/*
  itractorrearhitch_c.h: interface for retrieving rear hitch status.

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
#ifndef ITRACTORREARHITCH_C_H
#define ITRACTORREARHITCH_C_H

#include "impl/tractorrearhitch_c.h"

namespace IsoAgLib {

  class iTractorRearHitch_c : private __IsoAgLib::TractorRearHitch_c
  {
  public:
    /** @return hitch up position [0,0%;100.0%], res: 0,4%/bit, offset: 0 */
    uint8_t getPosition() const { return TractorRearHitch_c::getPosition(); }
    
    /** @return nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
    uint8_t getNominalLowerLinkForce() const { return TractorRearHitch_c::getNominalLowerLinkForce(); }

    /** @return hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
    uint16_t getDraft() const { return TractorRearHitch_c::getDraft(); }

    /** @return code Note this is 6 bit only! */
    uint8_t getExitReasonCode() const { return TractorRearHitch_c::getExitReasonCode(); }

    IsoAgLib::IsoHitchInWorkIndicationFlag_t getInWorkIndication() const { return TractorRearHitch_c::getInWorkIndication(); }

    IsoAgLib::IsoLimitFlag_t getPositionLimit() const { return TractorRearHitch_c::getPositionLimit(); }


    /** register an event handler that gets called for any incoming PGN.
        Please look into the implementation to see for which PGNs it is
        actually called.
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorRearHitch_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorRearHitch_c::deregisterMsgEventHandler( msgEventHandler ); }

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return TractorRearHitch_c::lastedTimeSinceUpdate(); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return TractorRearHitch_c::lastUpdateTime(); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractorRearHitch_c& getITractorRearHitchInstance();
#endif
    friend iTractorRearHitch_c& getITractorRearHitchInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractorRearHitch_c& getITractorRearHitchInstance()
  { return static_cast<iTractorRearHitch_c&>(__IsoAgLib::getTractorRearHitchInstance( 0 )); }
#endif
  inline iTractorRearHitch_c& getITractorRearHitchInstance( unsigned instance )
  { return static_cast<iTractorRearHitch_c&>(__IsoAgLib::getTractorRearHitchInstance( instance )); }

}
#endif
