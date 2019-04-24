/*
  measurement_c.h - class to provide local measurement-value

  (C) Copyright 2013 - 2019 by OSB AG

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
#ifndef MEASUREMENT_C_H
#define MEASUREMENT_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>


namespace __IsoAgLib
{

  class PdBase_c;
  class PdConnection_c;
  class ConnectedPd_c;

  class Measurement_c
  {
    public:
      Measurement_c();

      int32_t getValue() const { return m_value; }

      void setMeasurementValue( PdBase_c &, int32_t );
      void startMeasurement( PdBase_c &, PdConnection_c&, IsoAgLib::ProcData::MeasurementCommand_t, int32_t );

    private:
      int32_t m_value;
  };

}

#endif
