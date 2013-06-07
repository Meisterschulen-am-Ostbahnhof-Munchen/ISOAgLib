/*
  iprocdata_c.h: managing of local process data

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCDATA_C_H
#define IPROCDATA_C_H

#include "impl/procdata/procdata_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclientconnection_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>

namespace __IsoAgLib {
  class Setpoint_c;
}

namespace IsoAgLib {

  class iProcData_c : public __IsoAgLib::ProcData_c {
    public:
      iProcData_c() : ProcData_c() {}

      void init( iIdentItem_c& ident, const iDeviceObjectDpd_c& dpd, const iDeviceObjectDet_c& det, iProcDataSetpointHandler_c* setpointhandler = NULL ) {
        ProcData_c::init( ident, dpd, det, setpointhandler );
      }

      const iIsoName_c& isoName() const {
        return ProcData_c::isoName().toConstIisoName_c();
      }

      uint16_t DDI() const {
        return ProcData_c::DDI();
      }
      uint16_t element() const  {
        return ProcData_c::element();
      }

      void sendMeasurementVal() const {
        ProcData_c::sendMeasurementVal();
      }

      int32_t measurementValue() const {
        return ProcData_c::measurementValue();
      }

      void setMeasurementValue( int32_t ai32_val ) {
        ProcData_c::setMeasurementValue( ai32_val );
      }

      void incrMeasurementValue( int32_t ai32_val ) {
        ProcData_c::incrMeasurementValue( ai32_val );
      }

      int32_t setpointValue() const {
        return ProcData_c::setpointValue();
      }

      void startDataLogging( ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment ) {
        ProcData_c::startDataLogging( ren_type, ai32_increment );
      }

      friend class __IsoAgLib::Setpoint_c;
      friend class iDevicePool_c;
  };

}

#endif
