/*
  canutils.cpp:

  (C) Copyright 2012 - 2019 by OSB AG

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

#include "canutils.h"

namespace HAL {

#ifdef USE_CAN_MEASURE_BUSLOAD
  canBusLoad_c canBusLoads[ HAL_CAN_MAX_BUS_NR + 1 ];

  void canBusLoad_c::init() {
    m_busLoadCurrentSlice = 0;
    for( int i = 0; i < mc_numSlices; ++i ) {
      m_busLoadBytes[ i ] = 0;
    }
  }
#endif

}
