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

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measurement_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/setpoint_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclientconnection_c.h>

#include <list>

namespace __IsoAgLib {
  class TcClientConnection_c;
  class DeviceObjectDpd_c;
  class DeviceObjectDet_c;

  class SetpointHandler_c {
    public:
      virtual ~SetpointHandler_c() {}
      virtual void _processSetpointSet( ProcData_c& procdata, int32_t value, bool change ) = 0;
  };

  class ProcData_c {
    public:
      ProcData_c();
      void init( IdentItem_c& ident, const DeviceObjectDpd_c& dpd, const DeviceObjectDet_c& det, SetpointHandler_c* setpointhandler = NULL );

      void close();

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

      const Setpoint_c& getSetpoint() const { return m_setpoint; }
      const Measurement_c& getMeasurement() const { return m_measurement; }
      Setpoint_c& getSetpoint() { return m_setpoint; }
      Measurement_c& getMeasurement() { return m_measurement; }

    private:
      IdentItem_c* m_ident;
      const DeviceObjectDpd_c* m_dpd;
      const DeviceObjectDet_c* m_det;

      Setpoint_c m_setpoint;
      Measurement_c m_measurement;

    private:
      /** not copyable : copy constructor/operator only declared, not defined */
      ProcData_c( const ProcData_c& );
      ProcData_c& operator=( const ProcData_c& );
  };

}

#endif
