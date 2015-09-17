/*
  pdlocal_c.h: Class for providing Process Data

  (C) Copyright 2014 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PDLOCAL_C_H
#define PDLOCAL_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/pdbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/measurement_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/setpoint_c.h>


namespace __IsoAgLib {

  class PdConnection_c;


  class PdLocal_c : public PdBase_c
  {
  public:
    PdLocal_c();
    PdLocal_c( uint16_t _ddi, uint16_t _element, uint8_t _triggerMethod, bool _settable, SetpointHandler_c * );

    void init( uint16_t _ddi, uint16_t _element, uint8_t _triggerMethod, bool _settable, SetpointHandler_c * );

    virtual ConnectedPd_c &createConnectedPd( PdConnection_c & );

    const Setpoint_c &getSetpoint() const { return m_setpoint; }
    Setpoint_c &getSetpoint() { return m_setpoint; }

    const Measurement_c& getMeasurement() const { return m_measurement; }
    Measurement_c& getMeasurement() { return m_measurement; }

    uint8_t triggerMethod() const { return m_triggerMethod; }
    inline bool isMethodSet( IsoAgLib::ProcData::TriggerMethod_t _method ) const;

  protected:
    Setpoint_c m_setpoint;
    Measurement_c m_measurement;

    uint8_t m_triggerMethod;

    /** not copyable */
    PdLocal_c( const PdLocal_c& );
    PdLocal_c& operator=( const PdLocal_c& );
  };


  inline bool
  PdLocal_c::isMethodSet( IsoAgLib::ProcData::TriggerMethod_t _method ) const
  {
    return( (triggerMethod() & (1 << _method)) != 0 );
  }

}

#endif
