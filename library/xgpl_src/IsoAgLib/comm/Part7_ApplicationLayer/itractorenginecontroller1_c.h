/*
  itractorenginecontroller1_c.h: interface for retrieving tractor engine controller data

  (C) Copyright 2018 - 2018 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ITRACTORENGINECONTROLLER1_C_H
#define ITRACTORENGINECONTROLLER1_C_H

#include "impl/tractorenginecontroller1_c.h"

namespace IsoAgLib {

  class iTractorEngineController1_c : private __IsoAgLib::TractorEngineController1_c
  {
  public:

      bool isTorqueModeUsable() const {
          return __IsoAgLib::TractorEngineController1_c::isTorqueModeUsable();
      }
      bool isActualPercentTorqueFractionalUsable() const {
          return __IsoAgLib::TractorEngineController1_c::isActualPercentTorqueFractionalUsable();
      }
      bool isDriverTargetPercentTorqueUsable() const {
          return __IsoAgLib::TractorEngineController1_c::isDriverTargetPercentTorqueUsable();
      }
      bool isActualPercentTorqueUsable() const {
          return __IsoAgLib::TractorEngineController1_c::isActualPercentTorqueUsable();
      }
      bool isRpmUsable() const {
          return __IsoAgLib::TractorEngineController1_c::isRpmUsable();
      }
      bool isStarterModeUsable() const {
          return __IsoAgLib::TractorEngineController1_c::isStarterModeUsable();
      }
      bool isDemandPercentTorqueUsable() const {
          return __IsoAgLib::TractorEngineController1_c::isDemandPercentTorqueUsable();
      }

      /** deliver ISO engine torque mode
      * @return TorqueMode_t [0;15], 0 offset
      */
      uint8_t TorqueMode() const {
          return __IsoAgLib::TractorEngineController1_c::TorqueMode();
      }
      /** an additional torque in percent of the reference engine torque
      * @return [+0.000% to +0.875%];  res: .125%/bit, offset: 0
      */
      uint8_t ActualPercentTorqueFractional() const {
          return __IsoAgLib::TractorEngineController1_c::ActualPercentTorqueFractional();
      }
      /** requested torque output of the engine by the drive
      * @return [-125%;125%], res: 1%/bit, offset: -125%
      */
      int8_t DriverTargetPercentTorque() const {
          return __IsoAgLib::TractorEngineController1_c::DriverTargetPercentTorque();
      }
      /** the calculated output torque of the engine
      * @return [-125%;125%], res: 1%/bit, offset: -125%
      */
      int8_t PercentTorque() const {
          return __IsoAgLib::TractorEngineController1_c::PercentTorque();
      }
      /** Actual engine speed which is calculated over a minimum crankshaft angle of 720 degrees divided by the number of
      cylinders
      * @return [0 rpm;8,031.875 rpm], res: 0.125 rpm/bit, offset: 0 rpm
      */
      uint16_t Rpm() const {
          return __IsoAgLib::TractorEngineController1_c::Rpm();
      }
      /** source address of the SAE J1939 device currently controlling the engine
      * @return [0;255] , offset: 0,  Operational Range: 0 to 253
      */
      uint8_t SourceAddress() const {
          return __IsoAgLib::TractorEngineController1_c::SourceAddress();
      }
      /** starter actions / errors
      * @return StarterMode_t [0;15], 0 offset
      */
      StarterMode_t StarterMode() const {
          return __IsoAgLib::TractorEngineController1_c::StarterMode();
      }
      /** target torque output of the engine
      * @return [-125%;125%], res: 1%/bit, offset: -125%
      */
      int8_t DemandPercentTorque() const {
          return __IsoAgLib::TractorEngineController1_c::DemandPercentTorque();
      }

    /** register an event handler that gets called for any incoming PGN.
        Please look into the implementation to see for which PGNs it is
        actually called.
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    {
        TractorEngineController1_c::registerMsgEventHandler( msgEventHandler ); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &msgEventHandler)
    {
        TractorEngineController1_c::deregisterMsgEventHandler( msgEventHandler ); }

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return TractorEngineController1_c::lastedTimeSinceUpdate(); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return TractorEngineController1_c::lastUpdateTime(); }

private:
#if ( PRT_INSTANCE_CNT == 1 )
    friend iTractorEngineController1_c& getITractorEngineController1_Instance();
#endif
    friend iTractorEngineController1_c& getITractorEngineController1_Instance( unsigned instance );
  };

#if ( PRT_INSTANCE_CNT == 1 )
  inline iTractorEngineController1_c& getITractorEngineController1_Instance()
  { return static_cast<iTractorEngineController1_c&>(__IsoAgLib::getTractorEngineController1_Instance( 0 )); }
#endif
  inline iTractorEngineController1_c& getITractorEngineController1_Instance( unsigned instance )
  { return static_cast<iTractorEngineController1_c&>(__IsoAgLib::getTractorEngineController1_Instance( instance )); }

}
#endif
