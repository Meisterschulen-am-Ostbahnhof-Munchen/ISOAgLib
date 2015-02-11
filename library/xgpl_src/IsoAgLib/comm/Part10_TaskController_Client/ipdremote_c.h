/*
  ipdremote_c.h: interface for managing of remote process data

  (C) Copyright 2014 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPDREMOTE_C_H
#define IPDREMOTE_C_H

#include "impl/procdata/pdremote_c.h"


namespace IsoAgLib {

  class iMeasurementHandler_c;


  class iPdRemote_c : private __IsoAgLib::PdRemote_c
  {
  public:

    class iMeasurementHandler_c : public __IsoAgLib::MeasurementHandler_c
    {
    public:
      virtual ~iMeasurementHandler_c() {}
      virtual void processMeasurement( iPdRemote_c&, int32_t value, bool change ) = 0;
  
    private:
      virtual void _processMeasurement( __IsoAgLib::PdRemote_c& pdRemote, int32_t value, bool change )
      { processMeasurement( static_cast<IsoAgLib::iPdRemote_c&>( pdRemote ), value, change ); }
    };

    iPdRemote_c() : PdRemote_c() {}
    iPdRemote_c( uint16_t ddi, uint16_t element, iMeasurementHandler_c* handler = NULL );

    void init( uint16_t ddi, uint16_t element, iMeasurementHandler_c* handler = NULL );

    int32_t value() const;
    void requestValue();
    void sendSetpoint( int32_t value );
    void startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t cmd, int32_t value );

    friend class iPdPool_c;
  };



  inline
  iPdRemote_c::iPdRemote_c( uint16_t ddi, uint16_t element, iMeasurementHandler_c* handler )
    : PdRemote_c( ddi, element, handler )
  {}


  inline void
  iPdRemote_c::init( uint16_t ddi, uint16_t element, iMeasurementHandler_c* handler )
  {
    PdRemote_c::init( ddi, element, handler );
  }


  inline int32_t
  iPdRemote_c::value() const
  {
    return m_remoteValue.value();
  }


  inline void
  iPdRemote_c::requestValue()
  {
    PdRemote_c::requestValue();
  }


  inline void
  iPdRemote_c::sendSetpoint( int32_t value )
  {
    PdRemote_c::sendSetpoint( value );
  }


  inline void
  iPdRemote_c::startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t cmd, int32_t value )
  {
    PdRemote_c::startMeasurement( cmd, value );
  }

}

#endif
