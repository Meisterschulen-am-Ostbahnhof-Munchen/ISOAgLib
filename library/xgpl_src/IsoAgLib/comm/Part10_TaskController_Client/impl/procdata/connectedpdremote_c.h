/*
  connectedpdremote_c.h: object for managing connected remote measure

  (C) Copyright 2014 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef CONNECTEDPDREMOTE_C_H
#define CONNECTEDPDREMOTE_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/connectedpd_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/pdremote_c.h>

namespace __IsoAgLib {

  class PdConnection_c;


  class ConnectedPdRemote_c : public ConnectedPd_c
  {
    public:
      ConnectedPdRemote_c( PdConnection_c&, PdRemote_c& );
      virtual ~ConnectedPdRemote_c();

      virtual void handleRequest();
      virtual void handleIncoming( int32_t, bool wasBroadcast );
      virtual bool startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t, int32_t );

      const PdRemote_c &pdRemote() const { return static_cast<const PdRemote_c &>( m_pdBase ); }
      PdRemote_c &pdRemote() { return static_cast<PdRemote_c &>( m_pdBase ); }
  };

}

#endif
