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
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/managemeasureprog_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/setpoint_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclientconnection_c.h>
#include "../processpkg_c.h"

namespace IsoAgLib {
  class iProcDataHandler_c;
}


namespace __IsoAgLib {
  class TcClientConnection_c;

class ProcData_c
{
public:
  ProcData_c();
  void init( TcClientConnection_c& tcCC, const IsoAgLib::iDeviceObjectDpd_c& dpd, const IsoAgLib::iDeviceObjectDet_c& det, IsoAgLib::iProcDataHandler_c *apc_procDataHandler = NULL );

  /** @todo call close() from generated proc2iso code (closeAll())!!! */
  void close();

  IsoAgLib::iProcDataHandler_c* getProcDataHandler( ) const { return mpc_procDataHandler; }
  void setProcDataHandler( IsoAgLib::iProcDataHandler_c *apc_procDataHandler )
  { mpc_procDataHandler = apc_procDataHandler; }

  int32_t measurementVal() const { return mc_measureprog.measurementValue(); }
  int32_t setpointVal() const { return mc_setpoint.setpointValue(); }

  void setMeasurementVal(int32_t ai32_val);
  void incrMeasurementVal(int32_t ai32_val);

  void processMsg( const ProcessPkg_c& pkg, IsoAgLib::ProcData::RemoteType_t a_ecuType );
  void timeEvent( uint16_t& rui16_nextTimePeriod );

  void sendMeasurementVal( const IsoName_c& ac_targetISOName ) const;

  void startDataLogging( IsoAgLib::ProcData::MeasurementCommand_t, int32_t increment, IsoAgLib::ProcData::RemoteType_t );

  void stopRunningMeasurement( IsoAgLib::ProcData::RemoteType_t a_ecuType );

  uint16_t DDI() const{ return m_dpd->ddi(); }
  uint16_t element() const{ return m_det->elementNumber(); }

  bool isSetPoint() const { return m_dpd->propertySetpoint(); }
  uint8_t triggerMethod() const { return m_dpd->method(); } 

  const IsoName_c& isoName() const
  { isoaglib_assert(m_tcCC != NULL); return m_tcCC->getIdentItem().isoName(); }

  const TcClientConnection_c& getTcClientConnection() const {
    return *m_tcCC;
  }

private:
  void sendValue( IsoAgLib::ProcData::RemoteType_t a_remoteType, int32_t ai32_val ) const;

private:
  TcClientConnection_c* m_tcCC; // back ref.
  const IsoAgLib::iDeviceObjectDpd_c* m_dpd;
  const IsoAgLib::iDeviceObjectDet_c* m_det;
  IsoAgLib::iProcDataHandler_c* mpc_procDataHandler;

  ManageMeasureProg_c mc_measureprog;
  Setpoint_c mc_setpoint;

private:
  friend class MeasureProg_c;

private:
  /** not copyable : copy constructor/operator only declared, not defined */
  ProcData_c( const ProcData_c& );
  ProcData_c& operator=( const ProcData_c& );
};

}

#endif
