/*
  iprocdata_c.h: managing of local process data (with DDOP)

  (C) Copyright 2009 - 2019 by OSB AG

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
#ifndef IPROCDATA_C_H
#define IPROCDATA_C_H

#include "impl/procdata/procdata_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/idevicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclientconnection_c.h>


namespace IsoAgLib {

  class iProcData_c : private __IsoAgLib::ProcData_c {
    public:

      iProcData_c() : ProcData_c() {}

      class iSetpointHandler_c : private __IsoAgLib::SetpointHandler_c {
        public:
          virtual ~iSetpointHandler_c() {}
          virtual void processSetpointSet( iProcData_c& procdata, int32_t value, bool change ) = 0;

        private:
          virtual void _processSetpointSet( __IsoAgLib::PdLocal_c& procdata, int32_t value, bool change ) {
            processSetpointSet( static_cast<iProcData_c&>( procdata ), value, change );
          }
          friend class IsoAgLib::iProcData_c;
      };

      void init( const iDeviceObjectDpd_c& dpd, const iDeviceObjectDet_c& det, iSetpointHandler_c* setpointhandler = NULL ) {
        ProcData_c::init( dpd, det, static_cast<__IsoAgLib::SetpointHandler_c*>( setpointhandler ) );
      }

      uint16_t DDI() const {
        return ProcData_c::DDI();
      }
      uint16_t element() const  {
        return ProcData_c::element();
      }
      bool isMethodSet( ProcData::TriggerMethod_t method ) const {
        return ProcData_c::isMethodSet( method );
      }

      int32_t measurementValue() const {
        return ProcData_c::getMeasurement().getValue();
      }

      /* sets and send value (according to running programs) */
      void setMeasurementValue( int32_t ai32_val ) {
        ProcData_c::getMeasurement().setMeasurementValue( *this, ai32_val );
      }

      int32_t setpointValue() const {
        return ProcData_c::getSetpoint().setpointValue();
      }

      void startMeasurement( IsoAgLib::iTcClientConnection_c& ecu, ProcData::MeasurementCommand_t ren_type, int32_t ai32_increment ) {
        ProcData_c::getMeasurement().startMeasurement( *this, static_cast<__IsoAgLib::PdConnection_c&>( ecu ), ren_type, ai32_increment );
      }

      friend class iDevicePool_c;
  };

}

#endif
