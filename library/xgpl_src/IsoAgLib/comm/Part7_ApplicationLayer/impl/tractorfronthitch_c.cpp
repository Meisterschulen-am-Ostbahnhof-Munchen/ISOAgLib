/*
  tractorfronthitch_c.cpp: 

  (C) Copyright 2016 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "tractorfronthitch_c.h"


namespace __IsoAgLib {

  TractorFrontHitch_c &getTractorFrontHitchInstance( unsigned instance )
  { // if > 1 singleton instance is used, no static reference can be used
    MACRO_MULTITON_GET_INSTANCE_BODY(TractorFrontHitch_c, PRT_INSTANCE_CNT, instance);
  }


} // __IsoAglib
