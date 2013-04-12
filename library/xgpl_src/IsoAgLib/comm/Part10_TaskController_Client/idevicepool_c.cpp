/*
  idevicepool_c.cpp

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "idevicepool_c.h"
#include <IsoAgLib/util/iutil_funcs.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclientconnection_c.h>

using namespace IsoAgLib::ProcData;

// The ISO standard object labels
const char* deviceLabels[] = { "DVC", "DET", "DPD", "DPT", "DVP" };

void format( STL_NAMESPACE::vector<uint8_t>& byteStream, uint8_t val )
{
  byteStream.push_back(val);
}

void format( STL_NAMESPACE::vector<uint8_t>& byteStream, uint16_t val )
{
  byteStream.push_back((uint8_t)(val & 0xff));
  byteStream.push_back((uint8_t)((val >> 8) & 0xff));
}

void format(STL_NAMESPACE::vector<uint8_t>& byteStream, uint32_t val)
{
  format( byteStream, (uint16_t)(val & 0xffff) );
  format( byteStream, (uint16_t)((val >> 16) & 0xffff) );
}

void format( STL_NAMESPACE::vector<uint8_t>& byteStream, const char* str )
{
  const size_t l = CNAMESPACE::strlen( str );
  byteStream.push_back( uint8_t( l ) );
  format( byteStream, (const uint8_t*)str, l );
}

void format( STL_NAMESPACE::vector<uint8_t>& byteStream, const uint8_t* bp, size_t len )
{
  while ( len-- )
    byteStream.push_back(*bp++);
}

void format( STL_NAMESPACE::vector<uint8_t>& byteStream, int32_t val )
{
  format( byteStream, (uint32_t)val );
}

void format( STL_NAMESPACE::vector<uint8_t>& byteStream, float val )
{
  uint32_t iVal = 0;
  CNAMESPACE::memcpy( &iVal, &val, sizeof( float ) );
#if defined(__TSW_CPP__) // Tasking uses mixed endian
  uint16_t lo = iVal >> 16;
  iVal = (iVal << 16) | lo;
#endif
  format( byteStream, iVal );
}

namespace IsoAgLib {

uint16_t iDeviceObject_c::m_objIdCounter = 1;

iDevicePool_c::iDevicePool_c() 
  //: m_isDirty(false)
{}


void 
iDevicePool_c::changeDesignator( iDeviceObject_c& object, const char* newDesignator )
{
  object.setDesignator( newDesignator );
  getItcClientInstance().processChangeDesignator( *m_identItem, object.getObjectId(), newDesignator );
}


void 
iDevicePool_c::setLocalization( const localSettings_s& settings )
{
  getDvcObject()->setLocalization( settings );
  // @todo: Aufruf von Funktion im TcClient_c (partieller Pool-Upload wird getriggert)
}


// the localization label of the pool is changing. Set all DVP
// entries as changed (scale or designator changed)
void
iDevicePool_c::updateLocale()
{
  deviceMap_t::iterator it;
  for ( it = m_devicePool.begin(); it != m_devicePool.end(); it++ )
  {
    if ( it->second && it->second->getObjectType() == ObjectTypeDVP )
      static_cast<iDeviceObjectDvp_c*>(it->second)->setDirty(true);
  }
}

// the pool has been successfully uploaded to the TC. Mark
// all objects in the pool as synchronized with the TC (not dirty)
void
iDevicePool_c::setUploaded()
{
  deviceMap_t::iterator it;
  for ( it = m_devicePool.begin(); it != m_devicePool.end(); it++ )
  {
    if ( it->second && it->second->getObjectType() == ObjectTypeDVP )
      static_cast<iDeviceObjectDvp_c*>(it->second)->clearDirty();
  }
  //m_isDirty = false;
}


iDeviceObject_c*
iDevicePool_c::getObject( const uint16_t objId, const DeviceObjectType_t objectType ) const
{
  deviceMap_t::const_iterator it = m_devicePool.find(objId);
  if ( it != m_devicePool.end() )
  {
    if ( it->second && it->second->getObjectType() == objectType )
      return it->second;
  }

  return NULL;
}


bool
iDevicePool_c::add( iDeviceObject_c& devObj )
{
  if ( ! m_devicePool.insert(std::pair<uint16_t, iDeviceObject_c*>(devObj.getObjectId(), &devObj)).second )
  {
    return false;
  }
  return true;
}


// retrieve the device pool as a stream of bytes suitable for upload to the TC
const std::vector<uint8_t>&
iDevicePool_c::getBytestream()
{
  m_bytestream.clear();

  iDeviceObject_c* devObject;

  deviceMap_t::iterator it;
  for ( it = m_devicePool.begin(); it != m_devicePool.end(); it++ )
  {
    devObject = it->second;
    if ( ! devObject->formatBytestream( m_bytestream ) )
    {
      m_bytestream.clear();
      break;
    }
  }

  return m_bytestream;
}


// retrieve the bytes of changed (dirty) objects
bool
iDevicePool_c::getDirtyBytestream( std::vector<uint8_t>& byteStream )
{
  /*if ( isDirty() )
  {
    byteStream = getBytestream();
    return true;
  }*/

  iDeviceObject_c* devObject;
  bool retval = false;

  byteStream.clear();
  deviceMap_t::iterator it;
  for ( it = m_devicePool.begin(); it != m_devicePool.end(); it++ )
  {
    devObject = it->second;
    if ( devObject && ( devObject->getObjectType() == ObjectTypeDVP ) )
    { bool isDirty = static_cast<iDeviceObjectDvp_c*>(devObject)->isDirty();
      if ( isDirty )
      {
        retval = true;
        devObject->formatBytestream( byteStream );
      }
    }
  }
  return retval;
}


/////////////////////////////////////////////////////////////////////
// iDeviceObject_c
/////////////////////////////////////////////////////////////////////
iDeviceObject_c::iDeviceObject_c( const DeviceObjectType_t type, const char* desig )
  : m_objectType(type)
  , m_objectId((type == ObjectTypeDVC)? 0 : m_objIdCounter)
{
  isoaglib_assert( desig != NULL );

  m_designator = STL_NAMESPACE::string(desig);

  isoaglib_assert( m_objectId != 0xFFFF );

  if ( m_objectType != ObjectTypeDVC )
    m_objIdCounter++;
}


void
iDeviceObject_c::setDesignator( const char* desig )
{
  isoaglib_assert( desig != NULL );

  m_designator = STL_NAMESPACE::string(desig);
}


bool
iDeviceObject_c::formatBytestream( std::vector<uint8_t>& byteStream )
{
  format( byteStream, (const uint8_t*)deviceLabels[m_objectType], 3 );
  format( byteStream, m_objectId );

  return true;
}


/////////////////////////////////////////////////////////////////////
// iDeviceObjectDvc_c
/////////////////////////////////////////////////////////////////////
iDeviceObjectDvc_c::iDeviceObjectDvc_c( const char* version, const char* desig )
  : iDeviceObject_c( ObjectTypeDVC, desig )
  , m_version(version)
{
  std::memset( (void*)&m_localization, 0, sizeof(m_localization) );
  m_localization.reserved = 0xff; // Reserved field
}

void
iDeviceObjectDvc_c::setLocalization( const localSettings_s& currentSettings )
{
  Localization_s localSettings;

  localSettings.language[0] = currentSettings.languageCode >> 8;
  localSettings.language[1] = currentSettings.languageCode & 0xff;

  localSettings.dateFormat = currentSettings.dFormat;

  localSettings.numberFormat =  ((currentSettings.nDecimalPoint & 3) << 6);
  localSettings.numberFormat |= ((currentSettings.nTimeFormat & 3) << 4);

  localSettings.unitsFormatLow  = ((currentSettings.uDistance & 3) << 6);
  localSettings.unitsFormatLow |= ((currentSettings.uArea     & 3) << 4);
  localSettings.unitsFormatLow |= ((currentSettings.uVolume   & 3) << 2);
  localSettings.unitsFormatLow |= ((currentSettings.uMass     & 3));

  localSettings.unitsFormatHigh  = ((currentSettings.uTemperature & 3) << 6);
  localSettings.unitsFormatHigh |= ((currentSettings.uPressure & 3)    << 4);
  localSettings.unitsFormatHigh |= ((currentSettings.uForce & 3)       << 2);
  localSettings.unitsFormatHigh |= ((currentSettings.uUnitsSystem & 3));

  localSettings.reserved = 0xff;

  setLocalization( localSettings );
}

void
iDeviceObjectDvc_c::setLocalization( const Localization_s& local )
{
  //if (std::memcmp((void*)&local, (void*)&m_Localization, sizeof(Localization_s)) != 0)
  // setDirty(true);

  m_localization = local;
}

void
iDeviceObjectDvc_c::setStructureLabel( const uint8_t* label )
{
  m_structLabel.Byte1 = label[0];
  m_structLabel.Byte2 = label[1];
  m_structLabel.Byte3 = label[2];
  m_structLabel.Byte4 = label[3];
  m_structLabel.Byte5 = label[4];
  m_structLabel.Byte6 = label[5];
  m_structLabel.Byte7 = label[6];
}


void
iDeviceObjectDvc_c::setStructureLabel( const char* label )
{
  if( CNAMESPACE::strlen( label ) > 6)
  {
    setStructureLabel( (const uint8_t*) label );
  }
  else
  {
    uint8_t tmpBuf[7] = {0, 0, 0, 0, 0, 0, 0};
    for( unsigned int i = 0; i < CNAMESPACE::strlen( label ); i++ )
    {
      tmpBuf[i] = label[i];
    }
    setStructureLabel( tmpBuf );
  }
}


bool
iDeviceObjectDvc_c::formatBytestream( std::vector<uint8_t>& byteStream )
{
  if ( ! iDeviceObject_c::formatBytestream( byteStream ) )
    return false;

  format( byteStream, m_designator.c_str() );
  format( byteStream, m_version.c_str() );
  format( byteStream, m_wsmName.outputString(), 8 );
  format( byteStream, m_serialNumber.c_str() );

  format( byteStream, (uint8_t*)&m_structLabel, 7 );
  format( byteStream, (uint8_t*)&m_localization, 7 );

  return true;
}


/////////////////////////////////////////////////////////////////////
// iDeviceObjectDet_c
/////////////////////////////////////////////////////////////////////
iDeviceObjectDet_c::iDeviceObjectDet_c( const iDeviceObject_c& parentObj, uint16_t element, uint8_t type, const char* desig )
  : iDeviceObject_c( ObjectTypeDET, desig )
  , m_type(type)
  , m_elementNumber(element)
  , m_parentId(parentObj.getObjectId())
{}


bool
iDeviceObjectDet_c::formatBytestream( std::vector<uint8_t>& byteStream )
{
  if ( ! iDeviceObject_c::formatBytestream( byteStream ) )
    return false;

  byteStream.push_back( m_type );
  format( byteStream, m_designator.c_str() );
  format( byteStream, m_elementNumber );
  format( byteStream, m_parentId );
  format( byteStream, (uint16_t)m_childList.size() );

  std::vector<uint16_t>::iterator it;
  for ( it = m_childList.begin(); it != m_childList.end(); it++ )
    format( byteStream, *it );

  return true;
}

bool
iDeviceObjectDet_c::addChild( const iDeviceObject_c& child )
{
  std::vector<uint16_t>::iterator it;
  for ( it = m_childList.begin(); it != m_childList.end(); it++ )
  {
    if ( *it == child.getObjectId() )
      return true;
  }
  
  size_t oldSize = m_childList.size();
  m_childList.push_back(child.getObjectId());
  return m_childList.size() > oldSize;
}


/////////////////////////////////////////////////////////////////////
// iDeviceObjectDpd_c
/////////////////////////////////////////////////////////////////////
iDeviceObjectDpd_c::iDeviceObjectDpd_c( uint16_t ddi, const Properties_t& bitmaskProps, const Methods_t& bitmaskMethods, const char* desig, const iDeviceObjectDvp_c* dvpRef )
  : iDeviceObject_c( ObjectTypeDPD, desig )
  , m_ddi(ddi)
  , m_properties(bitmaskProps.getByte(0))
  , m_method(bitmaskMethods.getByte(0))
  , m_dvpObjectId((dvpRef) ? dvpRef->getObjectId() : 0xFFFF)
{
}


bool
iDeviceObjectDpd_c::formatBytestream( std::vector<uint8_t>& byteStream )
{
  if ( ! iDeviceObject_c::formatBytestream( byteStream ) )
    return false;

  format( byteStream, m_ddi );
  format( byteStream, m_properties );
  format( byteStream, m_method );
  format( byteStream, m_designator.c_str() );
  format( byteStream, m_dvpObjectId );

  return true;
}


/////////////////////////////////////////////////////////////////////
// iDeviceObjectDpt_c
/////////////////////////////////////////////////////////////////////
iDeviceObjectDpt_c::iDeviceObjectDpt_c( uint16_t ddi, int32_t value, const char* desig, const iDeviceObjectDvp_c* dvpRef )
  : iDeviceObject_c( ObjectTypeDPT, desig )
  , m_ddi(ddi)
  , m_value(value)
  , m_dvpObjectId((dvpRef) ? dvpRef->getObjectId() : 0xFFFF)
{}


bool
iDeviceObjectDpt_c::formatBytestream( std::vector<uint8_t>& byteStream )
{
  if ( ! iDeviceObject_c::formatBytestream( byteStream ) )
    return false;

  format( byteStream, m_ddi );
  format( byteStream, m_value );
  format( byteStream, m_designator.c_str() );
  format( byteStream, m_dvpObjectId );

  return true;
}


/////////////////////////////////////////////////////////////////////
// iDeviceObjectDvp_c
/////////////////////////////////////////////////////////////////////
iDeviceObjectDvp_c::iDeviceObjectDvp_c( float scale, int32_t offset, uint8_t decimals, const char* desig )
  : iDeviceObject_c( ObjectTypeDVP, desig )
  , m_offset(offset)
  , m_scale(scale)
  , m_decimals(decimals)
  , m_isDirty(false)
{}


bool
iDeviceObjectDvp_c::formatBytestream( std::vector<uint8_t>& byteStream )
{
  if ( ! iDeviceObject_c::formatBytestream( byteStream ) )
    return false;

  format( byteStream, m_offset );
  format( byteStream, m_scale );
  format( byteStream, m_decimals );
  format( byteStream, m_designator.c_str() );

  return true;
}

} // IsoAgLib
