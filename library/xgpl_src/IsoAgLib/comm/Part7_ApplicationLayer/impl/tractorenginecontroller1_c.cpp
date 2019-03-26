/*
  tractorenginecontroller1_c.cpp: class for retrieving tractor engine controller data

  (C) Copyright 2018 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tractorenginecontroller1_c.h"

namespace __IsoAgLib {

  TractorEngineController1_c &getTractorEngineController1Instance( unsigned instance )
  { // if > 1 singleton instance is used, no static reference can be used
    MACRO_MULTITON_GET_INSTANCE_BODY(TractorEngineController1_c, PRT_INSTANCE_CNT, instance);
  }

  void
  TractorEngineController1_c::resetValues()
  {
      mt_torqueMode = torque_mode_not_available;
      mui8_actualPercentTorqueFractional = NO_VAL_8;
      mui8_driverTargetPercentTorque = NO_VAL_8;
      mui8_actualPercentTorque = NO_VAL_8;
      mui16_rpm = NO_VAL_16;
      mui8_sourceAddressOfEngineControl = NO_VAL_8;
      mt_starterMode = starter_mode_not_available;
      mui8_demandPercentTorque = NO_VAL_8;
  };


  void
  TractorEngineController1_c::setValues( const CanPkgExt_c& pkg )
  {
      mt_torqueMode = (TorqueMode_t)(pkg.getUint8Data( 0 ) & 0xF);
      mui8_actualPercentTorqueFractional = pkg.getUint8Data( 0 ) >> 4;
      mui8_driverTargetPercentTorque = pkg.getUint8Data( 1 );
      mui8_actualPercentTorque = pkg.getUint8Data( 2 );
      mui16_rpm = pkg.getUint16Data( 3 );
      mui8_sourceAddressOfEngineControl = pkg.getUint8Data( 5 );
      mt_starterMode = ( StarterMode_t)(pkg.getUint8Data( 6 ) & 0xF);
      mui8_demandPercentTorque = pkg.getUint8Data(7);
  }


} // __IsoAglib
