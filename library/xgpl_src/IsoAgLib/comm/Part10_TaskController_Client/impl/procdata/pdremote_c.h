/*
  pdremote_c.h: Class for handling Remote Process Data

  (C) Copyright 2014 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PDREMOTE_C_H
#define PDREMOTE_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/pdbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/remotevalue_c.h>


namespace __IsoAgLib {

  class PdRemote_c : public PdBase_c
  {
  public:
    PdRemote_c();
    PdRemote_c( uint16_t ddi, uint16_t element, MeasurementHandler_c* = NULL );

    void init( uint16_t ddi, uint16_t element, MeasurementHandler_c* = NULL );

    void requestValue();
    void sendSetpoint( int32_t );
    void startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t, int32_t value );

    virtual ConnectedPd_c &createConnectedPd( PdConnection_c &connection );

    RemoteValue_c &getRemoteValue() { return m_remoteValue; }

  protected:
    RemoteValue_c m_remoteValue;
  };


  inline
  PdRemote_c::PdRemote_c( )
    : PdBase_c()
  {
  }


  inline
  PdRemote_c::PdRemote_c( uint16_t ddi, uint16_t element, MeasurementHandler_c* handler )
    : PdBase_c()
  {
    init( ddi, element, handler );
  }

}

#endif
