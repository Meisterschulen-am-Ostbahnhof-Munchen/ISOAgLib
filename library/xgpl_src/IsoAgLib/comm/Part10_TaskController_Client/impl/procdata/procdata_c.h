/*
  procdata_c.h

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCDATA_C_H
#define PROCDATA_C_H

#include <IsoAgLib/isoaglib_config.h>

#include <list>

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measurement_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/setpoint_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclientconnection_c.h>

namespace IsoAgLib {
  class iProcDataSetpointHandler_c;
}


namespace __IsoAgLib {
  class TcClientConnection_c;
  class DeviceObjectDpd_c;
  class DeviceObjectDet_c;

  class ProcData_c {
    public:
      ProcData_c();
      void init( IdentItem_c& ident, const DeviceObjectDpd_c& dpd, const DeviceObjectDet_c& det, IsoAgLib::iProcDataSetpointHandler_c* setpointhandler = NULL );

      void close();

      int32_t setpointValue() const {
        return m_setpoint.setpointValue();
      }

      IsoAgLib::iProcDataSetpointHandler_c* getSetpointHandler() {
        return m_setpointhandler;
      }

      void setMeasurementValue( int32_t ai32_val );
      void incrMeasurementValue( int32_t ai32_val );


      int32_t measurementValue() const {
        return m_measurement.getValue();
      }
      void sendMeasurementVal() const;
      void startDataLogging( IsoAgLib::ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment );
      void stopRunningMeasurement();

      void addMeasureProg( MeasureProg_c* m ) {
        m_measurement.addMeasureProg( m );
      }

      void removeMeasureProg( MeasureProg_c* m ) {
        m_measurement.removeMeasureProg( m );
      }


      const IsoName_c& isoName() const {
        return identItem().isoName();
      }
      const IdentItem_c& identItem() const {
        isoaglib_assert( m_ident );
        return *m_ident;
      }

      const DeviceObjectDpd_c* getDpd() const {
        return m_dpd;
      }
      const DeviceObjectDet_c* getDet() const {
        return m_det;
      }

      uint16_t DDI() const;
      uint16_t element() const;
      uint8_t triggerMethod() const;

      void processMsg( const ProcessPkg_c& pkg );
    private:
      IdentItem_c* m_ident;
      const DeviceObjectDpd_c* m_dpd;
      const DeviceObjectDet_c* m_det;
      IsoAgLib::iProcDataSetpointHandler_c* m_setpointhandler;

      Setpoint_c m_setpoint;
      Measurement_c m_measurement;

    private:
      /** not copyable : copy constructor/operator only declared, not defined */
      ProcData_c( const ProcData_c& );
      ProcData_c& operator=( const ProcData_c& );
  };

}

#endif
