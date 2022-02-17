/*
  idevicepool_c.h - Implementation of a device description object pool using modifiable
    C++ objects. The pool is represented of a collection of objects
    derived from a common base class. The device pool synchronization
    with the Task Controller is accomplished by uploading a partial pool
    when objects change.

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
#ifndef _IDEVICEPOOL_C_H_
#define _IDEVICEPOOL_C_H_

#include <IsoAgLib/comm/Part10_TaskController_Client/iddidefinition.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>

namespace IsoAgLib {
  class iProcData_c;
  class iTcClient_c;

  typedef __IsoAgLib::Localization_s Localization_s;
  typedef __IsoAgLib::StructureLabel_s StructureLabel_s;

  /* Dvc */

  class iDeviceObjectDvc_c : public __IsoAgLib::DeviceObjectDvc_c {
    public:
      iDeviceObjectDvc_c( const char* version, const char* desig ) : __IsoAgLib::DeviceObjectDvc_c( version, desig ) {}
      // TODO currently not updated during run-time.
      void setLocalization( const localSettings_s& s ) {
        __IsoAgLib::DeviceObjectDvc_c::setLocalSettings( s );
      }
      void setLocalization( const Localization_s& s ) {
        __IsoAgLib::DeviceObjectDvc_c::setLocalization( s );
      }
      void setStructureLabel( const uint8_t s[7] ) {
        __IsoAgLib::DeviceObjectDvc_c::setStructureLabel( s );
      }
      void setStructureLabel( const char* s ) {
        __IsoAgLib::DeviceObjectDvc_c::setStructureLabel( s );
      }
      void setSerialNumber( const char* s ) {
        __IsoAgLib::DeviceObjectDvc_c::setSerialNumber( s );
      }
  };


  /* Dvp */

  class iDeviceObjectDvp_c : public __IsoAgLib::DeviceObjectDvp_c {
    public:
      iDeviceObjectDvp_c( float scale, int32_t offset, uint8_t decimals, const char* desig ) : __IsoAgLib::DeviceObjectDvp_c( scale, offset, decimals, desig ) {}
      iDeviceObjectDvp_c( const iDeviceObjectDvp_c& obj ) : __IsoAgLib::DeviceObjectDvp_c( obj.getScale(), obj.getOffset(), obj.getDecimals(), obj.getDesignator() ) {}
      const iDeviceObjectDvp_c& operator=(const iDeviceObjectDvp_c& obj) 
      {
        setScale( obj.getScale() );
        setOffset( obj.getOffset() );
        setDecimals( obj.getDecimals() );
        setDesignator( obj.getDesignator() );

        return *this;
      }

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

      int32_t getOffset() const {
        return __IsoAgLib::DeviceObjectDvp_c::getOffset();
      }
      uint8_t getDecimals() const {
        return __IsoAgLib::DeviceObjectDvp_c::getDecimals();
      }
      float getScale() const {
        return __IsoAgLib::DeviceObjectDvp_c::getScale();
      }
  };


  /* Dpd */

  class iDeviceObjectDpd_c : public __IsoAgLib::DeviceObjectDpd_c {
    public:
      iDeviceObjectDpd_c( uint16_t dpd_ddi, const ProcData::Properties_t& p, const ProcData::Methods_t& m, const char* desig, const iDeviceObjectDvp_c* dvp )
        : DeviceObjectDpd_c( dpd_ddi, p, m, desig, dvp ) {}

      //! Caution: Only use this c'tor for porting legacy-DDOPs.
      //! For normal, new applications use the type-safe version above!
      iDeviceObjectDpd_c( uint16_t dpd_ddi, uint8_t properties, uint8_t triggerMethods, const char* desig, const iDeviceObjectDvp_c* dvp )
        : DeviceObjectDpd_c( dpd_ddi, properties, triggerMethods, desig, dvp ) {}

      // Caution: Only use this c'tor if you use the init() function exactly once before activating this object!
      iDeviceObjectDpd_c() : DeviceObjectDpd_c() {}

      // Caution: be sure to not call this init twice, only use it in conjunction with the default c'tor!
      void init( uint16_t dpd_ddi, const ProcData::Properties_t& p, const ProcData::Methods_t& m, const char* desig, const iDeviceObjectDvp_c* dvp ) {
        return __IsoAgLib::DeviceObjectDpd_c::init( dpd_ddi, p, m, desig, dvp );
      }

      bool propertyDefaultSet() const {
        return __IsoAgLib::DeviceObjectDpd_c::hasProperty( ProcData::Defaultset );
      }
  };


  /* Dpt */

  class iDeviceObjectDpt_c : public __IsoAgLib::DeviceObjectDpt_c {
    public:
      iDeviceObjectDpt_c( uint16_t dpt_ddi, int32_t value, const char* desig, const iDeviceObjectDvp_c* dvp ) : __IsoAgLib::DeviceObjectDpt_c( dpt_ddi, value, desig, dvp ) {}
      iDeviceObjectDpt_c() : __IsoAgLib::DeviceObjectDpt_c() {}
      void init( uint16_t dpt_ddi, int32_t value, const char* desig, const iDeviceObjectDvp_c* dvp ) { __IsoAgLib::DeviceObjectDpt_c::init( dpt_ddi, value, desig, dvp ); }

      int32_t getValue() const {
        return __IsoAgLib::DeviceObjectDpt_c::getValue();
      }
  };

  /* Det */

  class iDeviceObjectDet_c : public __IsoAgLib::DeviceObjectDet_c {
    public:
      iDeviceObjectDet_c( const iDeviceObjectDet_c& parent, uint16_t element, uint8_t type, const char* desig ) : DeviceObjectDet_c( parent.getObjectId(), element, type, desig ) {}
      iDeviceObjectDet_c( const iDeviceObjectDvc_c& parent, uint16_t element, uint8_t type, const char* desig ) : DeviceObjectDet_c( parent.getObjectId(), element, type, desig ) {}
      iDeviceObjectDet_c() : DeviceObjectDet_c() {}
      void init( const iDeviceObjectDet_c& parent, uint16_t element, uint8_t type, const char* desig ) { __IsoAgLib::DeviceObjectDet_c::init( parent.getObjectId(), element, type, desig ); }
      void init( const iDeviceObjectDvc_c& parent, uint16_t element, uint8_t type, const char* desig ) { __IsoAgLib::DeviceObjectDet_c::init( parent.getObjectId(), element, type, desig ); }

      bool addChild( const iDeviceObjectDpt_c& c ) {
        return __IsoAgLib::DeviceObjectDet_c::addChild( c.getObjectId() );
      }
      bool addChild( const iDeviceObjectDpd_c& c ) {
        return __IsoAgLib::DeviceObjectDet_c::addChild( c.getObjectId() );
      }
      uint16_t elementNumber() const {
        return __IsoAgLib::DeviceObjectDet_c::elementNumber();
      }
      void clearChildren() {
        return __IsoAgLib::DeviceObjectDet_c::clearChildren();
      }
};



  /* iDevicePool_c */

  class iDevicePool_c : private __IsoAgLib::DevicePool_c {
    public:
      iDevicePool_c( unsigned int reserveSize ) : DevicePool_c( reserveSize ) {}
      virtual ~iDevicePool_c() {}

      virtual uint8_t* allocByteStreamBuffer( uint32_t size ) {
        return static_cast<uint8_t*>( std::malloc( size ) );
      }
      virtual void freeByteStreamBuffer( uint8_t* buffer ) {
        std::free( buffer );
      }

      virtual void calcChecksumStart() { /* no default implementation */ }
      virtual void calcChecksumAdd( uint8_t ) { /* no default implementation */ }
      virtual void calcChecksumEnd() { /* no default implementation */ }

      void calcChecksum() {
        __IsoAgLib::DevicePool_c::calcChecksum();
      }

      //! not pedantic: it is safe to insert object multiple time
      //!   object is then present only once in pool
      template<class T>
      void add( T& devObj ) {
        __IsoAgLib::DevicePool_c::add( devObj );
      }
      //! Caution: Be sure one iProcData_c is only added once
      //!   Not safe to insert same iProcData_c multiple time
      void add( iProcData_c& pd );

      //! Caution: Be sure to only clear if not connected to any server!
      void clear() {
        __IsoAgLib::DevicePool_c::clear();
      }

      /* changing designators is not yet supported */
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

      // gets the DET by ElementNumber. This is just a convenience function
      // as the DeviceObjects are inserted by the application itself before!
      iDeviceObjectDet_c* getDetObject( uint16_t elementNumber ) {
        return (iDeviceObjectDet_c*) __IsoAgLib::DevicePool_c::getDetObject( elementNumber );
      }

      iDeviceObjectDvc_c* getDvcObject() const {
        return static_cast<iDeviceObjectDvc_c*>(DevicePool_c::getDvcObject());
      }
    private:
      friend class iTcClient_c;
      friend class iTcClientConnection_c;
  };

} // IsoAgLib

#endif // _IDEVICEPOOL_C_H_
