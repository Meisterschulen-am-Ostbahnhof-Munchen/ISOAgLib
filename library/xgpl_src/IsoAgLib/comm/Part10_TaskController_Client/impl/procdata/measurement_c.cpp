/*
  measurement_c.cpp

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "measurement_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measureprog_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/pdbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclientconnection_c.h>


namespace __IsoAgLib {

  Measurement_c::Measurement_c()
    : m_value( 0 )
  {
  }


  void Measurement_c::setMeasurementValue( PdBase_c &pdBase, int32_t v )
  {
    m_value = v;

    for( PdBase_c::ConnectedPds_t::iterator iter = pdBase.connectedPds().begin(); iter != pdBase.connectedPds().end(); ++iter )
      static_cast< MeasureProg_c *>( *iter )->valueUpdated();
  }


  void Measurement_c::startMeasurement( PdBase_c &pdBase, PdConnection_c& connection, IsoAgLib::ProcData::MeasurementCommand_t type, int32_t inc )
  {
    for( PdBase_c::ConnectedPds_t::iterator iter = pdBase.connectedPds().begin(); iter != pdBase.connectedPds().end(); ++iter ) 
    {
      if ( &( *iter )->connection() == &connection )
      {
        ( *iter )->startMeasurement( type, inc );
        break;
      }
    }
  }

}
