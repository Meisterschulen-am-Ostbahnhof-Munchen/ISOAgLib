/*
  itractorfronthitch_c.h: interface for retrieving front hitch status.

  (C) Copyright 2016 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITRACTORFRONTHITCH_C_H
#define ITRACTORFRONTHITCH_C_H

#include "impl/tractorfronthitch_c.h"

namespace IsoAgLib {

  class iTractorFrontHitch_c : private __IsoAgLib::TractorFrontHitch_c
  {
  public:
    /** @return hitch up position [0,0%;100.0%], res: 0,4%/bit, offset: 0 */
    uint8_t getPosition() const { return TractorFrontHitch_c::getPosition(); }
    
    /** @return nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
    uint8_t getNominalLowerLinkForce() const { return TractorFrontHitch_c::getNominalLowerLinkForce(); }

    /** @return hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
    uint16_t getDraft() const { return TractorFrontHitch_c::getDraft(); }

    /** @return code Note this is 6 bit only! */
    uint8_t getExitReasonCode() const { return TractorFrontHitch_c::getExitReasonCode(); }

    IsoAgLib::IsoHitchInWorkIndicationFlag_t getInWorkIndication() const { return TractorFrontHitch_c::getInWorkIndication(); }

    IsoAgLib::IsoLimitFlag_t getPositionLimit() const { return TractorFrontHitch_c::getPositionLimit(); }


    /** register an event handler that gets called for any incoming PGN.
        Please look into the implementation to see for which PGNs it is
        actually called.
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorFrontHitch_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorFrontHitch_c::deregisterMsgEventHandler( msgEventHandler ); }

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return TractorFrontHitch_c::lastedTimeSinceUpdate(); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return TractorFrontHitch_c::lastUpdateTime(); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractorFrontHitch_c& getITractorFrontHitchInstance();
#endif
    friend iTractorFrontHitch_c& getITractorFrontHitchInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractorFrontHitch_c& getITractorFrontHitchInstance()
  { return static_cast<iTractorFrontHitch_c&>(__IsoAgLib::getTractorFrontHitchInstance( 0 )); }
#endif
  inline iTractorFrontHitch_c& getITractorFrontHitchInstance( unsigned instance )
  { return static_cast<iTractorFrontHitch_c&>(__IsoAgLib::getTractorFrontHitchInstance( instance )); }

}
#endif
