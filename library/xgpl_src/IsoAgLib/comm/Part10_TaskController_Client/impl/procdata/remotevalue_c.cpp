/*
  remotevalue_c.cpp

  (C) Copyright 2014 - 2019 by OSB AG

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
#include "remotevalue_c.h"

namespace __IsoAgLib
{

  RemoteValue_c::RemoteValue_c()
    : m_value( 0 )
    , m_handler( NULL )
  {
  }


  void
  RemoteValue_c::init( MeasurementHandler_c *handler )
  {
    m_handler = handler;
  }


  void
  RemoteValue_c::processMsg( PdRemote_c &pd, int32_t value )
  {
    const bool b_change = ( m_value != value );
    m_value = value;

    if( m_handler )
      m_handler->_processMeasurement( pd, value, b_change );
  }

}
