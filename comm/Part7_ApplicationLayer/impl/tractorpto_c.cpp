/*
  tractorpto_c.cpp: common class for front and rear PTO.

  (C) Copyright 2017 - 2017 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tractorpto_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>


namespace __IsoAgLib {


  void
  TractorPto_c::resetValues()
  {
    mui16_speed8DigitPerRpm         = NO_VAL_16;
    mui16_speedSetPoint8DigitPerRpm = NO_VAL_16;

    mt_ptoEngaged               = IsoAgLib::IsoNotAvailable;
    mt_pto1000                  = IsoAgLib::IsoNotAvailable;
    mt_ptoEconomy               = IsoAgLib::IsoNotAvailable;

    mt_ptoEngagementReqStatus   = IsoAgLib::IsoNotAvailableReq;
    mt_ptoModeReqStatus         = IsoAgLib::IsoNotAvailableReq;
    mt_ptoEconomyModeReqStatus  = IsoAgLib::IsoNotAvailableReq;

    mt_ptoShaftSpeedLimitStatus = IsoAgLib::IsoNotAvailableLimit;
  };


  void
  TractorPto_c::setValues( const CanPkgExt_c& pkg )
  {
    mui16_speed8DigitPerRpm         = pkg.getUint16Data( 0 );
    mui16_speedSetPoint8DigitPerRpm = pkg.getUint16Data( 2 );

    mt_ptoEngaged = IsoAgLib::IsoActiveFlag_t( ( pkg.getUint8Data( 4 ) >> 6 ) & 3 );
    mt_pto1000    = IsoAgLib::IsoActiveFlag_t( ( pkg.getUint8Data( 4 ) >> 4 ) & 3 );
    mt_ptoEconomy = IsoAgLib::IsoActiveFlag_t( ( pkg.getUint8Data( 4 ) >> 2 ) & 3 );

    mt_ptoEngagementReqStatus   = IsoAgLib::IsoReqFlag_t( ( pkg.getUint8Data( 4 )      ) & 3 );
    mt_ptoModeReqStatus         = IsoAgLib::IsoReqFlag_t( ( pkg.getUint8Data( 5 ) >> 6 ) & 3 );
    mt_ptoEconomyModeReqStatus  = IsoAgLib::IsoReqFlag_t( ( pkg.getUint8Data( 5 ) >> 4 ) & 3 );

    mt_ptoShaftSpeedLimitStatus = IsoAgLib::IsoLimitFlag_t( ( pkg.getUint8Data( 5 ) >> 1 ) & 0x7 );
  }


} // __IsoAglib
