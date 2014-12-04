/*
  remotevalue_c.cpp

  (C) Copyright 2014 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
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