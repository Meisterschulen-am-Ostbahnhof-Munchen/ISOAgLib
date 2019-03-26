/*
  tractorhitch_c.cpp: common class for front and rear hitch.

  (C) Copyright 2016 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tractorhitch_c.h"
#include <IsoAgLib/comm/impl/isobus_c.h>


namespace __IsoAgLib {


  void
  TractorHitch_c::resetValues()
  {
    mui8_position       = NO_VAL_8;
    mui8_linkForce      = NO_VAL_8;
    mui16_draft         = NO_VAL_16;
    mui8_exitReasonCode = NO_VAL_8;

    mt_inWorkIndication = IsoAgLib::IsoNotAvailableHitchInWorkIndication;
    mt_posLimitStatus   = IsoAgLib::IsoNotAvailableLimit;
  };


  void
  TractorHitch_c::setValues( const CanPkgExt_c& pkg )
  {
    mui8_position       = pkg.getUint8Data( 0 );
    mui8_linkForce      = pkg.getUint8Data( 2 );
    mui16_draft         = static_cast<uint16_t>(pkg.getUint8Data( 3 ) ) + (static_cast<uint16_t>(pkg.getUint8Data( 4 ) ) << 8);
    mui8_exitReasonCode = pkg.getUint8Data( 5 ) & 0x3F;

    mt_inWorkIndication = IsoAgLib::IsoHitchInWorkIndicationFlag_t( pkg.getUint8Data( 1 ) >> 6 );
    mt_posLimitStatus   = IsoAgLib::IsoLimitFlag_t( ( pkg.getUint8Data( 1 ) >> 3 ) & 0x07 );
  }


} // __IsoAglib
