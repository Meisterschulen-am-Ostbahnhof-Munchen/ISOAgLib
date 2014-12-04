/*
  remotevalue_c.h

  (C) Copyright 2014 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef REMOTEVALUE_C_H
#define REMOTEVALUE_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/pdbase_c.h>

namespace __IsoAgLib {

  class PdRemote_c;
  
  
  class MeasurementHandler_c {
    public:
      virtual ~MeasurementHandler_c() {}
      virtual void _processMeasurement( PdRemote_c& procdata, int32_t value, bool change ) = 0;
  };


  class RemoteValue_c
  {
  public:
    RemoteValue_c();
    ~RemoteValue_c() {}

    void init( MeasurementHandler_c* );

    void processMsg( PdRemote_c &, int32_t pdValue );

    int32_t value() const {
      return m_value;
    }

  private:
    int32_t m_value;

    MeasurementHandler_c* m_handler;

    /** not copyable : copy constructor/operator only declared, not defined */
    RemoteValue_c( const RemoteValue_c& );
    RemoteValue_c& operator=( const RemoteValue_c& );  };

}

#endif
