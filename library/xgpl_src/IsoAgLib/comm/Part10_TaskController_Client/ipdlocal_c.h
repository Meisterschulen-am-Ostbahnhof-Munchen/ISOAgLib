/*
  ipdlocal_c.h: interface for managing of local process data

  (C) Copyright 2014 - 2014 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPDLOCAL_C_H
#define IPDLOCAL_C_H

#include "impl/procdata/pdlocal_c.h"
#include "ipdconnection_c.h"


namespace IsoAgLib {


  class iPdLocal_c : private __IsoAgLib::PdLocal_c
  {
  public:

    class iSetpointHandler_c : private __IsoAgLib::SetpointHandler_c
    {
    public:
      virtual ~iSetpointHandler_c() {}
      virtual void processSetpointSet( iPdLocal_c& procdata, int32_t value, bool change ) = 0;

    private:
      virtual void _processSetpointSet( __IsoAgLib::PdLocal_c& procdata, int32_t value, bool change )
      { processSetpointSet( static_cast<iPdLocal_c&>( procdata ), value, change ); }

      friend class IsoAgLib::iPdLocal_c;
    };

    iPdLocal_c();
    iPdLocal_c( uint16_t ddi, uint16_t element, uint8_t triggerMethod, bool settable, iSetpointHandler_c * );

    void init( uint16_t ddi, uint16_t element, uint8_t triggerMethod, bool settable, iSetpointHandler_c * );

    uint16_t DDI() const;
    uint16_t element() const;
    bool isMethodSet( ProcData::TriggerMethod_t method ) const;

    int32_t measurementValue() const;
    int32_t setpointValue() const;

    /* sets and send value (according to running programs) */
    void setMeasurementValue( int32_t ai32_val );
    void startMeasurement( iPdConnection_c&, ProcData::MeasurementCommand_t, int32_t increment );

    friend class iPdPool_c;
  };


  inline
  iPdLocal_c::iPdLocal_c()
    : PdLocal_c()
  {
  }

  inline
  iPdLocal_c::iPdLocal_c( uint16_t ddi, uint16_t element, uint8_t triggerMethod, bool settable, iSetpointHandler_c *handler )
    : PdLocal_c( ddi, element, triggerMethod, settable, handler )
  {
  }

  inline void
  iPdLocal_c::init( uint16_t ddi, uint16_t element, uint8_t triggerMethod, bool settable, iSetpointHandler_c *handler )
  {
    PdLocal_c::init( ddi, element, triggerMethod, settable, handler );
  }

  inline uint16_t
  iPdLocal_c::DDI() const
  {
    return PdLocal_c::DDI();
  }

  inline uint16_t
  iPdLocal_c::element() const
  {
    return PdLocal_c::element();
  }

  inline bool
  iPdLocal_c::isMethodSet( ProcData::TriggerMethod_t method ) const
  {
    return PdLocal_c::isMethodSet( method );
  }

  inline int32_t
  iPdLocal_c::measurementValue() const
  {
    return PdLocal_c::getMeasurement().getValue();
  }

  inline void
  iPdLocal_c::setMeasurementValue( int32_t ai32_val )
  {
    PdLocal_c::getMeasurement().setMeasurementValue( *this, ai32_val );
  }

  inline int32_t
  iPdLocal_c::setpointValue() const
  {
    return PdLocal_c::getSetpoint().setpointValue();
  }

  inline void
  iPdLocal_c::startMeasurement( IsoAgLib::iPdConnection_c& pdNode, ProcData::MeasurementCommand_t type, int32_t increment )
  {
    PdLocal_c::getMeasurement().startMeasurement( *this, static_cast<__IsoAgLib::PdConnection_c&>( pdNode ), type, increment );
  }

}

#endif
