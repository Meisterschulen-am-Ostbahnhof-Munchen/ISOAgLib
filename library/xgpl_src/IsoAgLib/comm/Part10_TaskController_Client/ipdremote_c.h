/*
  ipdremote_c.h: interface for managing of remote process data

  (C) Copyright 2014 - 2019 by OSB AG

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
      virtual void processMeasurement( iPdRemote_c&, int32_t _value, bool _change ) = 0;
  
    private:
      virtual void _processMeasurement( __IsoAgLib::PdRemote_c& pdRemote, int32_t _value, bool _change )
      { processMeasurement( static_cast<IsoAgLib::iPdRemote_c&>( pdRemote ), _value, _change ); }
    };

    iPdRemote_c() : PdRemote_c() {}
    iPdRemote_c( uint16_t _ddi, uint16_t _element, iMeasurementHandler_c* _handler = NULL );

    void init( uint16_t _ddi, uint16_t _element, iMeasurementHandler_c* _handler = NULL );

    uint16_t DDI() const;
    uint16_t element() const;

    int32_t value() const;
    void requestValue();
    void sendSetpoint( int32_t _value );
    void startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t _cmd, int32_t _value );

    friend class iPdPool_c;
  };



  inline
  iPdRemote_c::iPdRemote_c( uint16_t _ddi, uint16_t _element, iMeasurementHandler_c* _handler )
    : PdRemote_c( _ddi, _element, _handler )
  {}


  inline void
  iPdRemote_c::init( uint16_t _ddi, uint16_t _element, iMeasurementHandler_c* _handler )
  {
    PdRemote_c::init( _ddi, _element, _handler );
  }


  inline uint16_t
  iPdRemote_c::DDI() const
  {
    return PdRemote_c::DDI();
  }

  inline uint16_t
  iPdRemote_c::element() const
  {
    return PdRemote_c::element();
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
  iPdRemote_c::sendSetpoint( int32_t _value )
  {
    PdRemote_c::sendSetpoint( _value );
  }


  inline void
  iPdRemote_c::startMeasurement( IsoAgLib::ProcData::MeasurementCommand_t _cmd, int32_t _value )
  {
    PdRemote_c::startMeasurement( _cmd, _value );
  }

}

#endif
