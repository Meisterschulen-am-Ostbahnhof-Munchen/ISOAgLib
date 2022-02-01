/*
  devicepool_c.h: Device Description Object Pool with its Objects

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
#ifndef _DEVICEPOOL_C_H_
#define _DEVICEPOOL_C_H_

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtserverinstance_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/pdpool_c.h>


namespace __IsoAgLib {

  class DevicePool_c;

  typedef struct {
    uint8_t language[2];
    uint8_t numberFormat;
    uint8_t dateFormat;
    uint8_t unitsFormatLow;
    uint8_t unitsFormatHigh;
    uint8_t reserved;
  } Localization_s;


  typedef struct {
    uint8_t Byte1;
    uint8_t Byte2;
    uint8_t Byte3;
    uint8_t Byte4;
    uint8_t Byte5;
    uint8_t Byte6;
    uint8_t Byte7;
  } StructureLabel_s;


  typedef struct {
      uint8_t length;
      uint8_t byteString[32];
  } ExtendedStructureLabel_s;


  class DeviceObject_c {
    public:
      DeviceObject_c( const IsoAgLib::ProcData::DeviceObjectType_t type, const char* desig );
      DeviceObject_c( const IsoAgLib::ProcData::DeviceObjectType_t type );
      void init( const char* desig );

      virtual ~DeviceObject_c() {}

      uint16_t getObjectId() const {
        return m_objectId;
      }
      IsoAgLib::ProcData::DeviceObjectType_t getObjectType() const {
        return m_objectType;
      }

      void setDesignator( const char* );
      const char* getDesignator() const {
        return m_designator;
      };


    protected:
      const IsoAgLib::ProcData::DeviceObjectType_t m_objectType;
      const uint16_t m_objectId;
      const char* m_designator;

      virtual uint32_t getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const;

      void formatHeader( ByteStreamBuffer_c& byteStream ) const;
      virtual void formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const = 0;
      virtual void calcChecksumAdd( DevicePool_c & ) const = 0;
      void calcChecksumAddHeader( DevicePool_c & ) const;

    private:
      static uint16_t m_objIdCounter;
      friend class DevicePool_c;
  };


  /* DVC */
  class DeviceObjectDvc_c : public DeviceObject_c {
    public:
      DeviceObjectDvc_c( const char* version, const char* desig );

      const IdentItem_c& getIdentItem()           const { return *m_identItem; }
      const IsoName_c& getWsmName()               const { return m_identItem->isoName(); }
      const Localization_s& getLocalization()     const { return m_localization; }
      const StructureLabel_s& getStructureLabel() const { return m_structLabel; }
      const ExtendedStructureLabel_s& getExtendedStructureLabel()
                                                  const { return m_extendedStructureLabel; }
      const char* getVersion()                    const { return m_version; }
      const char* getSerialNumber()               const { return m_serialNumber; }

    protected:
      void setLocalSettings( const localSettings_s& s );
      void setLocalization( const Localization_s& s );
      void setStructureLabel( const char* s );
      void setStructureLabel( const uint8_t s[7] );
      void setExtendedStructureLabel( const char* s );                 // len 0..32. input 0-terminated string
      void setExtendedStructureLabel( const char* s, uint8_t length ); // len 0..32. input can have 0-bytes!
      void setSerialNumber( const char* s );

    private:
      friend class DevicePool_c;
      void init( const IdentItem_c& ident ) {
        m_identItem = &ident;
      }
      const char* m_version;
      char m_serialNumber[ 32+1 ];
      StructureLabel_s m_structLabel;
      ExtendedStructureLabel_s m_extendedStructureLabel;
      const IdentItem_c *m_identItem;
      Localization_s m_localization;

      uint32_t getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const;
      void formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const;
      void calcChecksumAdd( DevicePool_c & ) const;
  };


  /* DET */

  class DeviceObjectDet_c : public DeviceObject_c {
    public:
      DeviceObjectDet_c( uint16_t pid, uint16_t element, uint8_t type, const char* desig );
      DeviceObjectDet_c();
      void init( uint16_t pid, uint16_t element, uint8_t type, const char* desig );

    protected:
      uint16_t elementNumber() const {
        return m_elementNumber;
      }

    private:
      uint32_t getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const;
      void formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const;
      void calcChecksumAdd( DevicePool_c & ) const;

      size_t numberOfChildren() const {
        return m_childList.size();
      }
      uint8_t m_type;
      uint16_t m_elementNumber;
      uint16_t m_parentId;
      std::vector<uint16_t> m_childList;
      friend class __IsoAgLib::ProcData_c;
      friend class __IsoAgLib::DevicePool_c;

    protected:
      bool addChild( uint16_t id );
      void clearChildren();
  };


  /* DPD */

  class DeviceObjectDvp_c;
  class DeviceObjectDpd_c : public DeviceObject_c {
    public:
      DeviceObjectDpd_c( uint16_t dpd_ddi, const IsoAgLib::ProcData::Properties_t&, const IsoAgLib::ProcData::Methods_t&, const char* desig, const DeviceObjectDvp_c* );
      DeviceObjectDpd_c( uint16_t dpd_ddi, uint8_t properties, uint8_t triggerMethods, const char* desig, const DeviceObjectDvp_c* dvp );
      DeviceObjectDpd_c();
      // be sure to not call this init twice, only use it in conjunction with the default c'tor!
      void init( uint16_t dpd_ddi, const IsoAgLib::ProcData::Properties_t&, const IsoAgLib::ProcData::Methods_t&, const char* desig, const DeviceObjectDvp_c* );

      uint16_t ddi() const {
        return m_ddi;
      }
      uint8_t method() const {
        return m_method;
      }
      uint8_t properties( bool includeControlSource ) const {
        return( includeControlSource ? m_properties : ( m_properties & ~( 1 << IsoAgLib::ProcData::ControlSource ) ) );
      }
      bool hasProperty( IsoAgLib::ProcData::Property_t prop ) const {
        return( m_properties & ( 1 << prop ) ) != 0;
      }

    private:
      uint32_t getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const;
      void formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const;
      void calcChecksumAdd( DevicePool_c & ) const;

      uint16_t m_ddi;
      uint8_t m_properties;
      uint8_t m_method;
      uint16_t m_dvpObjectId;
      friend class __IsoAgLib::ProcData_c;
  };



  /* DPT */
  class DeviceObjectDpt_c : public DeviceObject_c {
    public:
      DeviceObjectDpt_c( uint16_t dpt_ddi, int32_t value, const char* desig, const DeviceObjectDvp_c* );
      DeviceObjectDpt_c();
      void init( uint16_t dpt_ddi, int32_t value, const char* desig, const DeviceObjectDvp_c* );

      int32_t getValue() const {
        return m_value;
      }

    private:
      uint32_t getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const;
      void formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const;
      void calcChecksumAdd( DevicePool_c & ) const;

      uint16_t m_ddi;
      int32_t m_value;
      uint16_t m_dvpObjectId;
  };


  /* DVP */

  class DeviceObjectDvp_c : public DeviceObject_c {
    public:
      DeviceObjectDvp_c( float scale, int32_t offset, uint8_t decimals, const char* desig );

      void setOffset( int32_t offset ) {
        m_offset = offset;
      }
      void setDecimals( uint8_t decimals ) {
        m_decimals = decimals;
      }
      void setScale( float scale ) {
        m_scale = scale;
      }

      void setUnitDesignator( const char* desig ) {
        setDesignator( desig );
      }

      int32_t getOffset() const {
        return m_offset;
      }
      uint8_t getDecimals() const {
        return m_decimals;
      }
      float getScale() const {
        return m_scale;
      }

    private:
      uint32_t getSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const;
      void formatBytestream( ByteStreamBuffer_c& byteStream, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps ) const;
      void calcChecksumAdd( DevicePool_c & ) const;

      int32_t m_offset;
      float m_scale;
      uint8_t m_decimals;
      bool m_isDirty;
  };



  /*  DDOP  */

  class DevicePool_c : public PdPool_c
  {
    public:
      DevicePool_c( unsigned int reserveSize );
      virtual ~DevicePool_c() {}

      virtual uint8_t* allocByteStreamBuffer( uint32_t size ) = 0;
      virtual void freeByteStreamBuffer( uint8_t* buffer ) = 0;

      virtual void calcChecksumStart() = 0;
      virtual void calcChecksumAdd( uint8_t ) = 0;
      virtual void calcChecksumEnd() = 0;

      void calcChecksum();

      void calcChecksumAdd( uint16_t val );
      void calcChecksumAdd( uint32_t val );
      void calcChecksumAdd( const uint8_t* bp, size_t len );
      void calcChecksumAdd( const char* str );
      void calcChecksumAdd( int32_t val );
      void calcChecksumAdd( float val );

      bool isEmpty() const {
        return m_devicePool.empty();
      }

    protected:
      void add( DeviceObject_c& devObj );
      void add( ProcData_c& pd );

      void clear();

      void changeDesignator( DeviceObject_c&, const char* );
      void setLocalSettings( const localSettings_s& );
      void updateLocale();

      DeviceObjectDvc_c* getDvcObject() const;
      DeviceObjectDet_c* getDetObject( uint16_t elementNumber );

    private:
      friend class TcClientConnection_c;
      void init( const IdentItem_c& ident );
      void close();

      DeviceObject_c* getObject( const uint16_t objId, const IsoAgLib::ProcData::DeviceObjectType_t ) const;

      ByteStreamBuffer_c getBytestream( uint8_t cmdByte, const IsoAgLib::ProcData::ConnectionCapabilities_s& caps );
      uint32_t getBytestreamSize( const IsoAgLib::ProcData::ConnectionCapabilities_s& ) const;

      typedef std::list<ProcData_c*> ProcDataList_t;
      ProcDataList_t &getProcDataList() { return *reinterpret_cast<ProcDataList_t*>( &m_procDatas ); }

      typedef std::map<uint16_t, DeviceObject_c*> deviceMap_t;
      deviceMap_t m_devicePool;
  };

}

#endif
