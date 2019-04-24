/*
  pdremote_c.h: Class for handling Remote Process Data

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
#ifndef PDREMOTE_C_H
#define PDREMOTE_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/pdbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/remotevalue_c.h>


namespace __IsoAgLib {

  class PdRemote_c : public PdBase_c
  {
  public:
    PdRemote_c();
    PdRemote_c( uint16_t _ddi, uint16_t _element, MeasurementHandler_c* = NULL );

    void init( uint16_t _ddi, uint16_t _element, MeasurementHandler_c* = NULL );

    void requestValue();
    void sendSetpoint( int32_t );
    void startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t, int32_t _value );

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
  PdRemote_c::PdRemote_c( uint16_t _ddi, uint16_t _element, MeasurementHandler_c* _handler )
    : PdBase_c()
  {
    init( _ddi, _element, _handler );
  }

}

#endif
