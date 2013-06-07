/*
  idevicepool_c.h - Implementation of a device description object pool using modifiable
    C++ objects. The pool is represented of a collection of objects
    derived from a common base class. The device pool synchronization
    with the Task Controller is accomplished by uploading a partial pool
    when objects change.

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _IDEVICEPOOL_C_H_
#define _IDEVICEPOOL_C_H_

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/ddidefinition.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>

namespace IsoAgLib {
  class iProcData_c;

  typedef __IsoAgLib::Localization_s Localization_s;
  typedef __IsoAgLib::StructureLabel_s StructureLabel_s;

  /* Dvc */

  class iDeviceObjectDvc_c : public __IsoAgLib::DeviceObjectDvc_c {
    public:
      iDeviceObjectDvc_c( const char* version, const char* desig ) : __IsoAgLib::DeviceObjectDvc_c( version, desig ) {}
      void setLocalization( const localSettings_s& s ) {
        __IsoAgLib::DeviceObjectDvc_c::setLocalSettings( s );
      }
      void setLocalization( const Localization_s& s ) {
        __IsoAgLib::DeviceObjectDvc_c::setLocalization( s );
      }
      void setStructureLabel( const char* s ) {
        __IsoAgLib::DeviceObjectDvc_c::setStructureLabel( s );
      }
  };


  /* Dvp */

  class iDeviceObjectDvp_c : public __IsoAgLib::DeviceObjectDvp_c {
    public:
      iDeviceObjectDvp_c( float scale, int32_t offset, uint8_t decimals, const char* desig ) : __IsoAgLib::DeviceObjectDvp_c( scale, offset, decimals, desig ) {}
      void setOffset( int32_t offset ) {
        __IsoAgLib::DeviceObjectDvp_c::setOffset( offset );
      }
      void setDecimals( uint8_t decimals ) {
        __IsoAgLib::DeviceObjectDvp_c::setDecimals( decimals );
      }
      void setScale( float scale ) {
        __IsoAgLib::DeviceObjectDvp_c::setScale( scale );
      }
      void setUnitDesignator( const char* desig ) {
        __IsoAgLib::DeviceObjectDvp_c::setDesignator( desig );
      }
  };


  /* Dpd */

  class iDeviceObjectDpd_c : public __IsoAgLib::DeviceObjectDpd_c {
    public:
      iDeviceObjectDpd_c( uint16_t ddi, const ProcData::Properties_t& p, const ProcData::Methods_t& m, const char* desig, const iDeviceObjectDvp_c* dvp ) :
        DeviceObjectDpd_c( ddi, p, m, desig, dvp ) {}
  };


  /* Dpt */

  class iDeviceObjectDpt_c : public __IsoAgLib::DeviceObjectDpt_c {
    public:
      iDeviceObjectDpt_c( uint16_t ddi, int32_t value, const char* desig, const iDeviceObjectDvp_c* dvp ) : __IsoAgLib::DeviceObjectDpt_c( ddi, value, desig, dvp ) {}
      int32_t getValue() const {
        return __IsoAgLib::DeviceObjectDpt_c::getValue();
      }
  };

  /* Det */

  class iDeviceObjectDet_c : public __IsoAgLib::DeviceObjectDet_c {
    public:
      iDeviceObjectDet_c( const iDeviceObjectDet_c& obj, uint16_t element, uint8_t type, const char* desig ) : DeviceObjectDet_c( obj.getObjectId(), element, type, desig ) {}
      iDeviceObjectDet_c( const iDeviceObjectDvc_c& obj, uint16_t element, uint8_t type, const char* desig ) : DeviceObjectDet_c( obj.getObjectId(), element, type, desig ) {}
      bool addChild( const iDeviceObjectDpt_c& c ) {
        return __IsoAgLib::DeviceObjectDet_c::addChild( c.getObjectId() );
      }
      bool addChild( const iDeviceObjectDpd_c& c ) {
        return __IsoAgLib::DeviceObjectDet_c::addChild( c.getObjectId() );
      }
      uint16_t elementNumber() const {
        return __IsoAgLib::DeviceObjectDet_c::elementNumber();
      };
  };



  /* iDevicePool_c */

  class iDevicePool_c : public __IsoAgLib::DevicePool_c {
    public:
      virtual ~iDevicePool_c() {}

      virtual uint8_t* allocByteStreamBuffer( uint32_t size ) {
        return static_cast<uint8_t*>( CNAMESPACE::malloc( size ) );
      }
      virtual void freeByteStreamBuffer( uint8_t* buffer ) {
        CNAMESPACE::free( buffer );
      }

      template<class T>
      bool add( T& devObj ) {
        return __IsoAgLib::DevicePool_c::add( devObj );
      }
      bool add( iProcData_c& pd );

      template<class T>
      void changeDesignator( T& obj, const char* str ) {
        __IsoAgLib::DevicePool_c::changeDesignator( obj, str );
      }

      void setLocalSettings( const localSettings_s& l ) {
        __IsoAgLib::DevicePool_c::setLocalSettings( l );
      }
      void updateLocale() {
        __IsoAgLib::DevicePool_c::updateLocale();
      }
    private:
      friend class iTcClient_c;
  };

} // IsoAgLib

#endif // _IDEVICEPOOL_C_H_
