/*
  connectedpd_c.h: object for connection-associated Pd-instance

  (C) Copyright 2014 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef CONNECTEDPD_C_H
#define CONNECTEDPD_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>


namespace __IsoAgLib {

  class PdBase_c;
  class PdConnection_c;

  class ConnectedPd_c
  {
  private:
    /** not copyable */
    ConnectedPd_c( const ConnectedPd_c& );
    ConnectedPd_c& operator=( const ConnectedPd_c& );

  public:
    virtual ~ConnectedPd_c();

    const PdConnection_c& connection() const { return m_connection; }
    const PdBase_c &pdBase() const { return m_pdBase; }
    PdBase_c &pdBase() { return m_pdBase; }

    virtual void handleRequest() = 0;
    virtual void handleIncoming( int32_t, bool wasBroadcast ) = 0;
    virtual bool startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment ) = 0;

    void sendMsg( IsoAgLib::ProcData::CommandType_t cmd, int32_t value );

  protected:
    ConnectedPd_c( PdConnection_c&, PdBase_c& );

    PdConnection_c& m_connection;
    PdBase_c& m_pdBase;
  };

}

#endif
