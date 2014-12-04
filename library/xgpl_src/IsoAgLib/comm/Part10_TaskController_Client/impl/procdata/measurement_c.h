/*
  measurement_c.h

  (C) Copyright 2013 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __MEASUREMENT_C_H
#define __MEASUREMENT_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>

namespace __IsoAgLib {

  class PdBase_c;
  class PdConnection_c;
  class ConnectedPd_c;

  class Measurement_c {
    public:
      Measurement_c();

      int32_t getValue() const { return m_value; }

      void setMeasurementValue( PdBase_c &pdBase, int32_t );
      void startMeasurement( PdBase_c &pdBase, PdConnection_c&, IsoAgLib::ProcData::MeasurementCommand_t, int32_t );

    private:
      int32_t m_value;
  };
}

#endif
