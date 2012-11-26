/*
  procdata_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

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
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/managemeasureprog_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/setpoint_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include "../processpkg_c.h"

namespace IsoAgLib {
  class iProcDataHandler_c;
}


namespace __IsoAgLib {

class ProcData_c
{
public:
  ProcData_c();
  ~ProcData_c();

  void init(
    IdentItem_c& acrc_identItem,
    uint16_t aui16_ddi,
    uint16_t aui16_element,
    bool ab_isSetpoint,
    uint8_t aui8_triggerMethod,
    IsoAgLib::iProcDataHandler_c *apc_procDataHandler = NULL );

  IsoAgLib::iProcDataHandler_c* getProcDataHandler( ) const { return mpc_procDataHandler; }
  void setProcDataHandler( IsoAgLib::iProcDataHandler_c *apc_procDataHandler )
  { mpc_procDataHandler = apc_procDataHandler; }

  int32_t measurementVal() const { return mc_measureprog.measurementValue(); }
  int32_t setpointVal() const { return mc_setpoint.setpointValue(); }

  void setMeasurementVal(int32_t ai32_val);
  void incrMeasurementVal(int32_t ai32_val);

  void processMsg( const ProcessPkg_c& pkg, IsoAgLib::ProcData::remoteType_t a_ecuType );
  void timeEvent( uint16_t& rui16_nextTimePeriod );

  void sendMeasurementVal( const IsoName_c& ac_targetISOName ) const;

  void startDataLogging(
    IsoAgLib::ProcData::measurementCommand_t ren_type,
    int32_t ai32_increment,
	IsoAgLib::ProcData::remoteType_t a_ecuType );

  void stopRunningMeasurement( IsoAgLib::ProcData::remoteType_t a_ecuType );

  uint16_t DDI() const{ return mui16_ddi; }
  uint16_t element() const{ return mui16_element; }

  bool isSetPoint() const { return configuration.mb_isSetpoint; }
  uint8_t triggerMethod() const { return configuration.mui8_triggerMethod; } 

  const IsoName_c& isoName() const
  { isoaglib_assert(mc_myIdentItem != NULL); return mc_myIdentItem->isoName(); }

  int getMultitonInst() const { isoaglib_assert(mc_myIdentItem != NULL); return mc_myIdentItem->getMultitonInst(); }

private:
  void sendValue( IsoAgLib::ProcData::remoteType_t a_remoteType, int32_t ai32_val ) const;

private:
  IdentItem_c* mc_myIdentItem;
  
  uint16_t mui16_ddi;
  uint16_t mui16_element;

  struct {
    bool mb_isSetpoint : 1;
    uint8_t mui8_triggerMethod : 7;
  } configuration;

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
