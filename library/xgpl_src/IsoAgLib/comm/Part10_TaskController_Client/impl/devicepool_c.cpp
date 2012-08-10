////////////////////////////////////////////////////////////////////////////////
//  devicepool_c.cpp -
//		Implementation of a device description object pool using modifiable
//		C++ objects.  The pool is represented of a collection of objects
//		derived from a common base class.  The device pool synchronization
//		with the Task Controller is accomplished by uploading a partial pool
//		when objects change.
//
//
//   Copyright (C) 2008,2009 AGCO Corporation, as an unpublished work.
//   THIS SOFTWARE AND/OR MATERIAL IS THE PROPERTY OF AGCO
//   Corporation. ALL USE, DISCLOSURE, AND/OR REPRODUCTION NOT
//   SPECIFICALLY AUTHORIZED BY AGCO CORPORATION IS PROHIBITED.
//
// $Workfile: $
// $Header: $
//
////////////////////////////////////////////////////////////////////////////////
#include	"devicepool_c.h"
#include	<IsoAgLib/util/iutil_funcs.h>

#ifdef USE_DYNAMIC_PART10

// The ISO standard object labels
const char* const deviceLabels[] = { "DVC", "DET", "DPD", "DPT", "DVP" };

const char* getObjectTypeString(DeviceObject_c::ObjectType_e objecttype)
{
  switch (objecttype)
  {
  case DeviceObject_c::DeviceObject: return "DVC";
  case DeviceObject_c::DeviceElementObject: return "DET";
  case DeviceObject_c::DeviceProcessDataObject: return "DPD";
  case DeviceObject_c::DevicePropertyObject: return "DPT";
  case DeviceObject_c::DeviceValuePresentationObject: return "DVP";
  case DeviceObject_c::ObjectTypeUndefined: break;
  }
  isoaglib_assert(false);
  return "NOT"; 
}

//
// DevicePool_c class
//	Container class that is the device description object pool
DevicePool_c::DevicePool_c() : m_isDirty(false)
{
}

// copy ctor
//	removed any existing device objects and copy from the rhs
DevicePool_c::DevicePool_c(const DevicePool_c& rhs) : m_isDirty(false)
{
	clear();
	deviceMap_t::const_iterator rhsIt;
	for (rhsIt = rhs.m_DevicePool.begin(); rhsIt != rhs.m_DevicePool.end(); rhsIt++)
	{
		DeviceObject_c* devObj = rhsIt->second;
	// Create and insert a clone of the object
		if (devObj)
			m_DevicePool.insert(std::pair<uint16_t, DeviceObject_c*>(devObj->getObjectId(), devObj->clone()));
	}
	setDirty();
}

// clear()
//	delete all objects from the device pool.  The map of objects was
//	created using either new() or clone() and must be deleted to prevent
//	memory leaks
void
DevicePool_c::clear()
{
	deviceMap_t::iterator it;
	for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
		delete it->second;

	m_DevicePool.clear();
}

// isDirty()
//	check the object map for any changed object
bool
DevicePool_c::isDirty() const
{
	deviceMap_t::const_iterator it;
	for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
	{
		if (it->second && it->second->isDirty())
			return true;
	}
	return false;
}

//
// updateLocale()
//	the localization label of the pool is changing.  Set all DVP
//	entries as changed (scale or designator changed)
void
DevicePool_c::updateLocale()
{
	deviceMap_t::iterator it;
	for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
	{
    if (it->second && it->second->getObjectType() == DeviceObject_c::DeviceValuePresentationObject)
			it->second->setDirty(true);
	}

	if (getObject(0, DeviceObject_c::DeviceObject))
		getObject(0, DeviceObject_c::DeviceObject)->setDirty(true);
}

//
// setUploaded()
//	the pool has been successfully uploaded to the TC.  Mark
//	all objects in the pool as synchronized with the TC (not dirty)
void
DevicePool_c::setUploaded()
{
	deviceMap_t::iterator it;
	for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
	{
		if (it->second)
			it->second->clearDirty();
	}
	m_isDirty = false;
}

//
// getObject(objId, objectType)
//	retrieve an object from the pool based on object ID and type ("DET", "DVC"...)
DeviceObject_c*
DevicePool_c::getObject(uint16_t objId, DeviceObject_c::ObjectType_e objectType) const
{
// retreive the object based on ID
	deviceMap_t::const_iterator it = m_DevicePool.find(objId);
	if (it != m_DevicePool.end())
	{
	// Verify that the object type is correct
		if (it->second && it->second->getObjectType() == objectType)
			return it->second;
		return 0;
	}
	
	return 0;
}

// Add()
//	insert a new object into the pool
uint16_t
DevicePool_c::Add(const DeviceObject_c& devObj)
{
	return Add(devObj.clone());
}

uint16_t
DevicePool_c::Add(DeviceObject_c* devObj)
{
	if (devObj == 0)
		return 0xFFFF;

	if (!m_DevicePool.insert(std::pair<uint16_t, DeviceObject_c*>(devObj->getObjectId(), devObj)).second)
	{
	// must delete the object since it was created using clone() above.
		delete devObj;
		return 0xFFFF;
	}
	return devObj->getObjectId();
}

uint16_t
DevicePool_c::getByteStreamSize() const
{
	DeviceObject_c* devObject;
	deviceMap_t::const_iterator it;
  uint16_t size_total = 0;
	for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
	{
		devObject = it->second;
    size_total += devObject->getSizeInBytes();
	}

	return size_total;
}

//
// getByteStream()
//	retrieve the device pool as a stream of bytes suitable for upload to the TC
bool
DevicePool_c::getByteStream(uint8_t* destMemory) const
{
	DeviceObject_c* devObject;
	
	deviceMap_t::const_iterator it;
  uint16_t current_position = 0;
	for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
	{
		devObject = it->second;
		if (!devObject->formatBytestream(destMemory, current_position))
		{
      return false;
		}
	}

	return true;
}

uint16_t
DevicePool_c::getDirtyByteStreamSize() const
{
	if (m_isDirty)
	{
		return getByteStreamSize();
	}

	DeviceObject_c* devObject;
	deviceMap_t::const_iterator it;
  uint16_t size_total = 0;
  for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
	{
		devObject = it->second;
		if (devObject->isDirty())
		{
      size_total += devObject->getSizeInBytes();
		}
	}
	return size_total;

}

//
// getDirtyBytestream()
//	retrieve the bytes of changed (dirty) objects
bool
DevicePool_c::getDirtyByteStream(uint8_t* destMemory) const
{
	if (m_isDirty)
	{
		return getByteStream(destMemory);
	}

	DeviceObject_c* devObject;
	bool retval = false;

	deviceMap_t::const_iterator it;
  uint16_t current_position = 0;
	for (it = m_DevicePool.begin(); it != m_DevicePool.end(); it++)
	{
		devObject = it->second;
		if (devObject->isDirty())
		{
			retval = true;
			devObject->formatBytestream(destMemory, current_position);
		}
	}
	return retval;
}

//
// updateDvpObject()
//	set the attributes of a device value presentation object referenced by objId
//	If the object does not exist, it will be created.
uint16_t
DevicePool_c::updateDvpObject(uint16_t objId, float scale, int32_t offset, uint8_t decimals, const std::string& desig)
{
	DeviceObjectDvp_c* dvpObj = getDvpObject(objId);
	if (dvpObj == 0)
	{
		Add(DeviceObjectDvp_c(objId, scale, offset, decimals, desig));
		dvpObj = getDvpObject(objId);
		if (dvpObj == 0)
			return 65535;
	}

	dvpObj->Scale(scale);
	dvpObj->Offset(offset);
	dvpObj->Decimals(decimals);
	dvpObj->setDesignator(desig.c_str());

	return dvpObj->getObjectId();
}

/////////////////////////////////////////////////////////////////////
// DeviceObject_c
/////////////////////////////////////////////////////////////////////

uint16_t
DeviceObject_c::getSizeInBytes() const
{
	if (m_ObjectId == 65535)	// NULL object ID
		return 0;

  // object type (3 bytes) + object id (2 bytes)
  return 5;
}

bool
DeviceObject_c::formatBytestream(uint8_t* destMemory, uint16_t& position) const
{
  std::cout << (uint32_t)destMemory << " ID = " << m_ObjectId << std::endl;

	if (m_ObjectId == 65535)	// NULL object ID
  {
    std::cout << "m_ObjectId = " << m_ObjectId << std::endl;
		return false;
  }

	format(destMemory, getObjectTypeString(m_ObjectType), position);
	format(destMemory, m_ObjectId, position);

	return true;
}

void
DeviceObject_c::validateSize(std::string& str, size_t len) const
{
	if (str.size() > len)
		str.resize(len);
}

void
DeviceObject_c::format(uint8_t* byteStream, uint8_t val, uint16_t& position) const
{
  byteStream[position] = val;
  ++position;
}

void
DeviceObject_c::format(uint8_t* byteStream, uint16_t val, uint16_t& position) const
{
  byteStream[position] = (uint8_t)(val & 0xff);
  byteStream[position + 1] = (uint8_t)((val >> 8) & 0xff);
  position += 2;
}

void
DeviceObject_c::format(uint8_t* byteStream, uint32_t val, uint16_t& position) const
{
	format(byteStream, (uint16_t)(val & 0xffff), position);
	format(byteStream, (uint16_t)((val >> 16) & 0xffff), position);
}

void
DeviceObject_c::format(uint8_t* byteStream, const std::string& str, uint16_t& position) const
{
	format(byteStream, (uint8_t)str.size(), position);
	format(byteStream, str.c_str(), position);
}

void
DeviceObject_c::format(uint8_t* byteStream, const uint8_t* bp, size_t len, uint16_t& position) const
{
  memcpy(byteStream + position,bp,len);
  position += len;
}

void
DeviceObject_c::format(uint8_t* byteStream, float val, uint16_t& position) const
{
	uint32_t iVal = *((uint32_t*)&val);
#if defined(__TSW_CPP__)	// Tasking uses mixed endian
	uint16_t lo = iVal >> 16;
	iVal = (iVal << 16) | lo;
#endif
	format(byteStream, iVal, position);
}

void
DeviceObject_c::setDesignator(const std::string& str, uint8_t /*encoding*/)
{
	if (m_Designator != str) 
	{
		m_Designator = str;
		validateSize(m_Designator, 32);
		setDirty(true);
	}
}

/////////////////////////////////////////////////////////////////////
// DeviceObjectDvc_c
/////////////////////////////////////////////////////////////////////
DeviceObjectDvc_c::DeviceObjectDvc_c() : DeviceObject_c(DeviceObject_c::DeviceObject, 0), m_WsmName(IsoAgLib::iIsoName_c::iIsoNameUnspecified())
{
	std::memset((void*)&m_Localization, 0, sizeof(m_Localization));
	m_Localization.reserved = 0xff;	// Reserved field
}

void
DeviceObjectDvc_c::setLocalization(const localSettings_s& currentSettings)
{
	Localization_s	localSettings;

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

	setLocalization(localSettings);
}

void
DeviceObjectDvc_c::setLocalization(const Localization_s& local)
{
	if (std::memcmp((void*)&local, (void*)&m_Localization, sizeof(Localization_s)) != 0)
		setDirty(true);
	m_Localization = local;
}

void
DeviceObjectDvc_c::setVersion(const char* str)
{
	setVersion(std::string(str));
}

void
DeviceObjectDvc_c::setVersion(const std::string& str)
{
	checkDirty(m_Version, str);
	m_Version = str;
  isoaglib_assert(m_Version.size() <= 32);
}

void
DeviceObjectDvc_c::setSerialNumber(const char* str)
{
	setSerialNumber(std::string(str));
}

void
DeviceObjectDvc_c::setSerialNumber(const std::string& str)
{
	checkDirty(m_SerialNumber.c_str(), str.c_str());
	m_SerialNumber = str;
  isoaglib_assert(m_SerialNumber.size() <= 32);
}

void
DeviceObjectDvc_c::setWsmName(const IsoAgLib::iIsoName_c& name)
{
	checkDirty(name, m_WsmName);
	m_WsmName = name;
}

uint16_t
DeviceObjectDvc_c::getSizeInBytes() const
{
  return (30 + m_Designator.size() + m_Version.size() + m_SerialNumber.size());
}

bool
DeviceObjectDvc_c::formatBytestream(uint8_t* byteStream, uint16_t& position) const
{
	if (!DeviceObject_c::formatBytestream(byteStream, position))
		return false;

	format(byteStream, m_Designator, position);
	format(byteStream, m_Version, position);
	format(byteStream, m_WsmName.outputString(), 8, position);
	format(byteStream, m_SerialNumber, position);

// blank pad the stucture label to 7 characters
  isoaglib_assert(m_StructLabel.size() == 7);
	format(byteStream, (uint8_t*)m_StructLabel.c_str(), 7, position);
	format(byteStream, (uint8_t*)&m_Localization, 7, position);

	return true;
}
/////////////////////////////////////////////////////////////////////
// DeviceObjectDet_c
/////////////////////////////////////////////////////////////////////
DeviceObjectDet_c::DeviceObjectDet_c(uint16_t objId) : DeviceObject_c(DeviceObject_c::DeviceElementObject, objId)
{
	m_Type = 255;
	m_ElementNumber = 4095;
	m_ParentId = 65535;
}

DeviceObjectDet_c::DeviceObjectDet_c(uint16_t objId, uint16_t parentObjId, 
				  uint16_t element, uint8_t type, const std::string& desig) : DeviceObject_c(DeviceObject_c::DeviceElementObject, objId)
{
	m_Type = type;
	m_ElementNumber = element;
	m_ParentId = parentObjId;
	m_Designator = desig;
}

bool
DeviceObjectDet_c::isValid() const
{
	if (!DeviceObject_c::isValid())
		return false;

	return m_ElementNumber != 4095 && m_ParentId != 65535;
}

uint16_t
DeviceObjectDet_c::getSizeInBytes() const
{
  return 13 + m_Designator.size() + m_ChildList.size() * 2;
}

bool
DeviceObjectDet_c::formatBytestream(uint8_t* byteStream, uint16_t& position) const
{
	if (!DeviceObject_c::formatBytestream(byteStream, position))
		return false;

	byteStream[position] = m_Type;
  ++position;
	format(byteStream, m_Designator, position);
	format(byteStream, m_ElementNumber, position);
	format(byteStream, m_ParentId, position);
	format(byteStream, (uint16_t)m_ChildList.size(), position);

	std::vector<uint16_t>::const_iterator it;
	for (it = m_ChildList.begin(); it != m_ChildList.end(); it++)
		format(byteStream, *it, position);

	return true;
}

bool
DeviceObjectDet_c::AddChild(uint16_t objId)
{
	std::vector<uint16_t>::iterator it;
	for (it = m_ChildList.begin(); it != m_ChildList.end(); it++)
	{
		if (*it == objId)
			return true;
	}
	
	size_t oldSize = m_ChildList.size();
	m_ChildList.push_back(objId);
	return m_ChildList.size() > oldSize;
}

/////////////////////////////////////////////////////////////////////
//
// DeviceObjectDpd_c class
//	wrapper for Device Process Data (DPD) object
/////////////////////////////////////////////////////////////////////
DeviceObjectDpd_c::DeviceObjectDpd_c(uint16_t objId)
	: DeviceObject_c(DeviceObject_c::DeviceProcessDataObject, objId)
	, m_Ddi(65535)
	, m_Properties(0)
	, m_Method(0)
	, m_DvpObjectId(65535)
{
}

DeviceObjectDpd_c::DeviceObjectDpd_c(uint16_t objId, uint16_t ddi, uint8_t props, uint8_t method, 
									 const std::string& desig, uint16_t dvpRef)
                   : DeviceObject_c(DeviceObject_c::DeviceProcessDataObject, objId)
	, m_Ddi(ddi)
	, m_Properties(props)
	, m_Method(method)
	, m_DvpObjectId(dvpRef)
{
	setDesignator(desig);
}

uint16_t
DeviceObjectDpd_c::getSizeInBytes() const
{
  return 12 + m_Designator.size();
}

bool
DeviceObjectDpd_c::formatBytestream(uint8_t* byteStream, uint16_t& position) const
{
	if (!DeviceObject_c::formatBytestream(byteStream, position))
		return false;

	format(byteStream, m_Ddi, position);
	format(byteStream, m_Properties, position);
	format(byteStream, m_Method, position);
	format(byteStream, m_Designator, position);
	format(byteStream, m_DvpObjectId, position);

	return true;
}

/////////////////////////////////////////////////////////////////////
//
// DeviceObjectDpt_c
//	wrapper for Device Property (DPT) object
/////////////////////////////////////////////////////////////////////
DeviceObjectDpt_c::DeviceObjectDpt_c(uint16_t objId) : DeviceObject_c(DeviceObject_c::DevicePropertyObject, objId)
{
	m_Ddi = 65535;
	m_Value = 0;
	m_DvpObjectId = 65535;
}

DeviceObjectDpt_c::DeviceObjectDpt_c(uint16_t objId, uint16_t ddi, int32_t value, 
									 const std::string& desig, uint16_t dvpRef) : DeviceObject_c(DeviceObject_c::DevicePropertyObject, objId)
{
	m_Ddi = ddi;
	m_Value = value;
	m_DvpObjectId = dvpRef;
	m_Designator = desig;
}

uint16_t
DeviceObjectDpt_c::getSizeInBytes() const
{
  return 14 + m_Designator.size();
}

bool
DeviceObjectDpt_c::formatBytestream(uint8_t* byteStream, uint16_t& position) const
{
	if (!DeviceObject_c::formatBytestream(byteStream, position))
		return false;

	format(byteStream, m_Ddi, position);
	format(byteStream, m_Value, position);
	format(byteStream, m_Designator, position);
	format(byteStream, m_DvpObjectId, position);

	return true;
}

/////////////////////////////////////////////////////////////////////
// DeviceObjectDvp_c
//	wrapper for Device Value Presentation (DVP) object
/////////////////////////////////////////////////////////////////////
DeviceObjectDvp_c::DeviceObjectDvp_c(uint16_t objId)
	: DeviceObject_c(DeviceObject_c::DeviceValuePresentationObject, objId)
	, m_Offset(0)
	, m_Scale(1.0)
	, m_Decimals(0)
{
}

DeviceObjectDvp_c::DeviceObjectDvp_c(uint16_t objId,
									 float scale,
									 int32_t offset,
									 uint8_t decimals,
									 const std::string& desig)
	: DeviceObject_c(DeviceObject_c::DeviceValuePresentationObject, objId)
	, m_Offset(offset)
	, m_Scale(scale)
	, m_Decimals(decimals)
{
	setDesignator(desig);
}

uint16_t
DeviceObjectDvp_c::getSizeInBytes() const
{
  return 15 + m_Designator.size();
}

bool
DeviceObjectDvp_c::formatBytestream(uint8_t* byteStream, uint16_t& position) const
{
	if (!DeviceObject_c::formatBytestream(byteStream, position))
		return false;

	format(byteStream, m_Offset, position);
	format(byteStream, m_Scale, position);
	format(byteStream, m_Decimals, position);
	format(byteStream, m_Designator, position);

	return true;
}
#endif
