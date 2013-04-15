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

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtserverinstance_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/ddidefinition.h>

#include <map>
#include <vector>
#include <string>

using namespace IsoAgLib::ProcData;

namespace __IsoAgLib {
  class ProcData_c;
  class TcClientConnection_c;
}

//static helper
void format( STL_NAMESPACE::vector<uint8_t>& byteStream, const char* str );
void format( STL_NAMESPACE::vector<uint8_t>& byteStream, const uint8_t* val, size_t len );
void format( STL_NAMESPACE::vector<uint8_t>& byteStream, uint8_t val );
void format( STL_NAMESPACE::vector<uint8_t>& byteStream, uint16_t val );
void format( STL_NAMESPACE::vector<uint8_t>& byteStream, uint32_t val );
void format( STL_NAMESPACE::vector<uint8_t>& byteStream, int32_t val );
void format( STL_NAMESPACE::vector<uint8_t>& byteStream, float val );

namespace IsoAgLib {

typedef struct
{
  uint8_t language[2];
  uint8_t numberFormat;
  uint8_t dateFormat;
  uint8_t unitsFormatLow;
  uint8_t unitsFormatHigh;
  uint8_t reserved;
} Localization_s;

typedef struct
{
  uint8_t Byte1;
  uint8_t Byte2;
  uint8_t Byte3;
  uint8_t Byte4;
  uint8_t Byte5;
  uint8_t Byte6;
  uint8_t Byte7;
} StructureLabel_s;

class iDeviceObject_c
{
public:
  iDeviceObject_c( const DeviceObjectType_t type, const char* desig );
  virtual ~iDeviceObject_c() {}

  uint16_t getObjectId() const { return m_objectId; }
  DeviceObjectType_t getObjectType() const { return m_objectType; }
  const char* getDesignator() const { return m_designator.c_str(); }

  virtual bool formatBytestream( STL_NAMESPACE::vector<uint8_t>& byteStream );
  virtual void setDesignator( const char* );

protected:
  const DeviceObjectType_t m_objectType;
  const uint16_t m_objectId;
  STL_NAMESPACE::string m_designator;

  static uint16_t m_objIdCounter;
};

class iDevicePool_c;
class iDeviceObjectDvc_c : public iDeviceObject_c
{
public:
  iDeviceObjectDvc_c( const char* version, const char* desig );
  virtual ~iDeviceObjectDvc_c() {}

  virtual bool formatBytestream( STL_NAMESPACE::vector<uint8_t>& byteStream );

  const iIsoName_c& getWsmName() const { return m_wsmName; }
  const Localization_s& getLocalization() const { return m_localization; }
  const StructureLabel_s& getStructureLabel() const { return m_structLabel; }
  const char* getSerialNumber() const { return m_serialNumber.c_str(); }

  void setLocalization( const localSettings_s& );
  void setLocalization( const Localization_s& );
  void setStructureLabel( const uint8_t* );
  void setStructureLabel( const char* );
  void setWsmName( const iIsoName_c& name );

private:
  friend class iDevicePool_c;

  void init( const iIdentItem_c& ident ) { m_wsmName = ident.isoName(); }

protected:
  const STL_NAMESPACE::string m_version;
  const STL_NAMESPACE::string m_serialNumber;
  StructureLabel_s m_structLabel;
  iIsoName_c m_wsmName;
  Localization_s m_localization;
};

class iDeviceObjectDet_c : public iDeviceObject_c
{
public:
  iDeviceObjectDet_c( const iDeviceObject_c& , uint16_t element, uint8_t type, const char* desig );
  virtual ~iDeviceObjectDet_c() {}

  virtual bool formatBytestream( STL_NAMESPACE::vector<uint8_t>& byteStream );

  size_t numberOfChildren() const { return m_childList.size(); }
  bool addChild( const iDeviceObject_c& );

protected:
  uint16_t elementNumber() const {
    return m_elementNumber;
  }
  const uint8_t m_type;
  const uint16_t m_elementNumber;
  const uint16_t m_parentId;
  STL_NAMESPACE::vector<uint16_t> m_childList;
  friend class __IsoAgLib::ProcData_c;
};


class iDeviceObjectDvp_c;
class iDeviceObjectDpd_c : public iDeviceObject_c
{
public:
  iDeviceObjectDpd_c( uint16_t ddi, const Properties_t&, const Methods_t&, const char* desig, const iDeviceObjectDvp_c* );
  virtual ~iDeviceObjectDpd_c() {}

  virtual bool formatBytestream( STL_NAMESPACE::vector<uint8_t>& byteStream );

protected:
  uint16_t ddi() const {
    return m_ddi;
  }
  uint8_t method() const {
    return m_method;
  }
  bool propertySetpoint() const {
    return ( m_properties & ( 1 << 1 ) ) != 0;
  }
  const uint16_t m_ddi;
  const uint8_t m_properties;
  const uint8_t m_method;
  const uint16_t m_dvpObjectId;
  friend class __IsoAgLib::ProcData_c;
};


class iDeviceObjectDpt_c : public iDeviceObject_c
{
public:
  iDeviceObjectDpt_c( uint16_t ddi, int32_t value, const char* desig, const iDeviceObjectDvp_c* );
  virtual ~iDeviceObjectDpt_c() {}

  virtual bool formatBytestream( STL_NAMESPACE::vector<uint8_t>& byteStream );

	int32_t getValue() const { return m_value; }

protected:
  const uint16_t m_ddi;
  const int32_t m_value;
  const uint16_t m_dvpObjectId;
};


class iDeviceObjectDvp_c : public iDeviceObject_c
{
public:
  iDeviceObjectDvp_c( float scale, int32_t offset, uint8_t decimals, const char* desig );
  virtual ~iDeviceObjectDvp_c() {}

  virtual bool formatBytestream( STL_NAMESPACE::vector<uint8_t>& byteStream );

	void setOffset( int32_t offset ) { if ( checkDirty( m_offset, offset ) ) m_offset = offset; }
	void setDecimals( uint8_t decimals ) { if ( checkDirty( m_decimals, decimals ) ) m_decimals = decimals; }
	void setScale( float scale ) { if ( checkDirty( m_scale, scale ) ) m_scale = scale; }

  void setUnitDesignator( const char* desig ) { setDesignator( desig ); }

protected:
  template<typename T> inline bool checkDirty(T lhs, T rhs) {	setDirty(lhs != rhs); return isDirty(); }

private:
  void setDirty( bool flag ) { if (flag) m_isDirty = true; }
  bool isDirty() const { return m_isDirty; }
  void clearDirty() { m_isDirty = false; }

protected:
  int32_t m_offset;
  float m_scale;
  uint8_t m_decimals;
  bool m_isDirty;

  friend class iDevicePool_c;
};


class iDevicePool_c
{
  typedef STL_NAMESPACE::map<uint16_t, iDeviceObject_c*> deviceMap_t;
public:
  iDevicePool_c();
  virtual ~iDevicePool_c() {}

  bool isEmpty() const { return m_devicePool.empty(); }

  bool add( iDeviceObject_c& devObj );

  const STL_NAMESPACE::vector<uint8_t>& getBytestream();
  bool getDirtyBytestream( STL_NAMESPACE::vector<uint8_t>& byteStream );

  // Object getter methods
  iDeviceObjectDvc_c* getDvcObject() const { return static_cast<iDeviceObjectDvc_c*>(getObject( 0, ObjectTypeDVC )); }

  // sendCommand ChangeDesignator
  void changeDesignator( iDeviceObject_c&, const char* );

  void setLocalization( const localSettings_s& );

  void updateLocale();

	//bool isDirty() const { return m_isDirty; }
	//void setDirty() { m_isDirty = true; }

  void setUploaded();

protected:
  bool add( iDeviceObject_c* devObj );
  iDeviceObject_c* getObject( const uint16_t objId, const DeviceObjectType_t ) const;

private:
  friend class __IsoAgLib::TcClientConnection_c;

  void init( iIdentItem_c& ident ) { getDvcObject()->init( ident ); m_identItem = &ident; }

protected:
  deviceMap_t m_devicePool;
  STL_NAMESPACE::vector<uint8_t> m_bytestream;
  iIdentItem_c* m_identItem;
  //bool m_isDirty;
};

} // IsoAgLib

#endif // _IDEVICEPOOL_C_H_
