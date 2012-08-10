/*
  processpkg_c.cpp: data object for Process-Data messages

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "processpkg_c.h"

namespace __IsoAgLib {

ProcessPkg_c::ProcessPkg_c( const CanPkg_c& arc_src, int ai_multitonInst )
  : CanPkgExt_c( arc_src, ai_multitonInst ),
    mi32_pdValue( mc_data.getInt32Data(4) ),
    men_command( CommandType_t(mc_data[0] & 0xf) ),
    mui16_element( 0 ),
    mui16_DDI( 0 )
{
  mui16_element = static_cast<uint16_t>(mc_data[1]) << 4;
  mui16_element |= (mc_data[0] & 0xF0) >> 4;

  mui16_DDI = static_cast<uint16_t>(mc_data[3]) << 8;
  mui16_DDI |= mc_data[2];
}

ProcessPkg_c::ProcessPkg_c()
  : CanPkgExt_c(),
    mi32_pdValue( 0 ),
    men_command( CommandUndefined ),
    mui16_element( 0 ),
    mui16_DDI( 0 )
{
}


}
