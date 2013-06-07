/*
  devicepool_c.h:

  (C) Copyright 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _DEVICEPOOL_C_H_
#define _DEVICEPOOL_C_H_

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtserverinstance_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/tcclientconnection_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdata/procdata_c.h>


namespace __IsoAgLib {

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


  class DeviceObject_c {
    public:
      DeviceObject_c( const IsoAgLib::ProcData::DeviceObjectType_t type, const char* desig );
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


      friend class __IsoAgLib::DevicePool_c;
      virtual uint32_t getSize() const;

      virtual void format( ByteStreamBuffer_c& byteStream ) const;
      virtual void formatBytestream( ByteStreamBuffer_c& byteStream ) const = 0;

    private:
      static uint16_t m_objIdCounter;
  };

  /* Dvc */
  class DeviceObjectDvc_c : public DeviceObject_c {
    public:
      DeviceObjectDvc_c( const char* version, const char* desig );
      const IsoName_c& getWsmName() const {
        return m_wsmName;
      }
      const Localization_s& getLocalization() const {
        return m_localization;
      }
      const StructureLabel_s& getStructureLabel() const {
        return m_structLabel;
      }
      const char* getVersion() const {
        return m_version;
      }
      const char* getSerialNumber() const {
        return m_serialNumber;
      }
    protected:
      void setLocalSettings( const localSettings_s& s );
      void setLocalization( const Localization_s& s );
      void setStructureLabel( const char* s );
      void setStructureLabel( const uint8_t* label );

    private:
      friend class __IsoAgLib::DevicePool_c;
      void init( const IdentItem_c& ident ) {
        m_wsmName = ident.isoName();
      }
      const char* m_version;
      const char* m_serialNumber;
      StructureLabel_s m_structLabel;
      IsoName_c m_wsmName;
      Localization_s m_localization;

      uint32_t getSize() const;
      void formatBytestream( ByteStreamBuffer_c& byteStream ) const;
  };


  /* Det */

  class DeviceObjectDet_c : public DeviceObject_c {
    public:
      DeviceObjectDet_c( uint16_t pid, uint16_t element, uint8_t type, const char* desig );

    protected:
      uint16_t elementNumber() const {
        return m_elementNumber;
      }

    private:
      uint32_t getSize() const;
      void formatBytestream( ByteStreamBuffer_c& byteStream ) const;

      size_t numberOfChildren() const {
        return m_childList.size();
      }
      const uint8_t m_type;
      const uint16_t m_elementNumber;
      const uint16_t m_parentId;
      STL_NAMESPACE::vector<uint16_t> m_childList;
      friend class __IsoAgLib::ProcData_c;

    protected:
      bool addChild( uint16_t id );
  };

  /* Dpd */

  class DeviceObjectDvp_c;
  class DeviceObjectDpd_c : public DeviceObject_c {
    public:
      DeviceObjectDpd_c( uint16_t ddi, const IsoAgLib::ProcData::Properties_t&, const IsoAgLib::ProcData::Methods_t&, const char* desig, const DeviceObjectDvp_c* );

      uint16_t ddi() const {
        return m_ddi;
      }
      uint8_t method() const {
        return m_method;
      }
      bool propertySetpoint() const {
        return ( m_properties & ( 1 << 1 ) ) != 0;
      }

    private:
      uint32_t getSize() const;
      void formatBytestream( ByteStreamBuffer_c& byteStream ) const;

      const uint16_t m_ddi;
      const uint8_t m_properties;
      const uint8_t m_method;
      const uint16_t m_dvpObjectId;
      friend class __IsoAgLib::ProcData_c;
  };



  /* Dpt */
  class DeviceObjectDpt_c : public DeviceObject_c {
    public:
      DeviceObjectDpt_c( uint16_t ddi, int32_t value, const char* desig, const DeviceObjectDvp_c* );
      int32_t getValue() const {
        return m_value;
      }

    private:
      uint32_t getSize() const;
      void formatBytestream( ByteStreamBuffer_c& byteStream ) const;

      const uint16_t m_ddi;
      const int32_t m_value;
      const uint16_t m_dvpObjectId;
  };

  /* Dvp */

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

    private:
      uint32_t getSize() const;
      void formatBytestream( ByteStreamBuffer_c& byteStream ) const;

      int32_t m_offset;
      float m_scale;
      uint8_t m_decimals;
      bool m_isDirty;
  };



  /*  Pool  */

  class DevicePool_c {
    public:
      DevicePool_c();

      virtual uint8_t* allocByteStreamBuffer( uint32_t size ) = 0;
      virtual void freeByteStreamBuffer( uint8_t* buffer ) = 0;

      bool isEmpty() const {
        return m_devicePool.empty();
      }

    protected:
      bool add( DeviceObject_c& devObj );
      bool add( ProcData_c& pd );
      void changeDesignator( DeviceObject_c&, const char* );
      void setLocalSettings( const localSettings_s& );
      void updateLocale();

    private:
      friend class TcClientConnection_c;
      void init( IdentItem_c& ident );

      DeviceObjectDvc_c* getDvcObject() const;
      DeviceObject_c* getObject( const uint16_t objId, const IsoAgLib::ProcData::DeviceObjectType_t ) const;

      ByteStreamBuffer_c getBytestream();
      uint32_t getBytestreamSize() const;

      typedef STL_NAMESPACE::map<uint16_t, DeviceObject_c*> deviceMap_t;
      typedef STL_NAMESPACE::list<ProcData_c*> procDataList_t;
      deviceMap_t m_devicePool;
      procDataList_t m_procDatas;
      IdentItem_c* m_identItem;
      TcClientConnection_c* m_connection;

  };

}
#endif
