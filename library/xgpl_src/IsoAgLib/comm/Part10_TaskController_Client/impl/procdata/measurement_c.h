/*
  measurement_c.h

  (C) Copyright 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef __MEASUREMENT_C_H
#define __MEASUREMENT_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measureprog_c.h>

namespace __IsoAgLib {

  class ProcData_c;

  class Measurement_c {
    public:
      Measurement_c();

      int32_t getValue() const {
        return m_value;
      }

      void setMeasurementValue( ProcData_c& pd, int32_t v );

      void addMeasureProg( MeasureProg_c* m );

      void processMsg( ProcData_c& pd, const ProcessPkg_c& pkg );
      void startDataLogging( ProcData_c& pd, IsoAgLib::ProcData::MeasurementCommand_t type, int32_t inc );
      void stopRunningMeasurement();

    private:
      int32_t m_value;
      STL_NAMESPACE::list<MeasureProg_c*> m_measureProgs;
  };
}

#endif
