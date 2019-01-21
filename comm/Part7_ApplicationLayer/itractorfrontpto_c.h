/*
  itractorfrontpto_c.h: interface for retrieving front PTO information.

  (C) Copyright 2017 - 2017 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITRACTORFRONTPTO_C_H
#define ITRACTORFRONTPTO_C_H

#include "impl/tractorfrontpto_c.h"

namespace IsoAgLib {

  class iTractorFrontPto_c : private __IsoAgLib::TractorFrontPto_c
  {
  public:
    /** get output shaft speed
      @return actual rpm speed of PTO [1/8RPM]
     */
    uint16_t getSpeed() const { return TractorFrontPto_c::getSpeed(); }

    /** get measured value of the set point of the rotational speed of the power take-off (PTO) output shaft
        @return measured value of the set point [1/8RPM]
      */
    uint16_t getSpeedSetPoint() const { return TractorFrontPto_c::getSpeedSetPoint(); }

    /** deliver explicit information whether PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t getEngagement() const { return TractorFrontPto_c::getEngagement(); }

    /** deliver std RPM of PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t getMode1000() const { return TractorFrontPto_c::getMode1000(); }

    /** deliver economy mode of PTO
      * @return IsoActive -> PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t getEconomyMode() const { return TractorFrontPto_c::getEconomyMode(); }

    /** get reported tractor ECU's status of engagement
        @return reported status
      */
    IsoAgLib::IsoReqFlag_t getEngagementReqStatus() const { return TractorFrontPto_c::getEngagementReqStatus(); }

    /** get reported tractor ECU's status of mode
         @return reported status
      */
    IsoAgLib::IsoReqFlag_t getModeReqStatus() const { return TractorFrontPto_c::getModeReqStatus(); }

    /** get reported tractor ECU's status of economy mode
        @return reported status
      */
    IsoAgLib::IsoReqFlag_t getEconomyModeReqStatus() const { return TractorFrontPto_c::getEconomyModeReqStatus(); }

    /** get present limit status of pto shaft speed
        @return present limit status
      */
    IsoAgLib::IsoLimitFlag_t getShaftSpeedLimitStatus() const { return TractorFrontPto_c::getShaftSpeedLimitStatus(); }


    /** register an event handler that gets called for any incoming PGN.
        Please look into the implementation to see for which PGNs it is
        actually called.
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorFrontPto_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    { TractorFrontPto_c::deregisterMsgEventHandler( msgEventHandler ); }

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return TractorFrontPto_c::lastedTimeSinceUpdate(); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return TractorFrontPto_c::lastUpdateTime(); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractorFrontPto_c& getITractorFrontPtoInstance();
#endif
    friend iTractorFrontPto_c& getITractorFrontPtoInstance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractorFrontPto_c& getITractorFrontPtoInstance()
  { return static_cast<iTractorFrontPto_c&>(__IsoAgLib::getTractorFrontPtoInstance( 0 )); }
#endif
  inline iTractorFrontPto_c& getITractorFrontPtoInstance( unsigned instance )
  { return static_cast<iTractorFrontPto_c&>(__IsoAgLib::getTractorFrontPtoInstance( instance )); }

}
#endif
