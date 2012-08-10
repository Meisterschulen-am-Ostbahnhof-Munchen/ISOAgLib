////////////////////////////////////////////////////////////////////////////////
//  devicepool_c.h -
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
#ifndef	_DEVICEPOOL_C_H_
#define	_DEVICEPOOL_C_H_

#include	<IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include	<IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtserverinstance_c.h>
#include	<map>
#include	<vector>
#include	<string>

#ifdef USE_DYNAMIC_PART10

#include	"ddidefinition.h"

typedef struct
{
	uint8_t language[2];
	uint8_t numberFormat;
	uint8_t dateFormat;
	uint8_t unitsFormatLow;
	uint8_t unitsFormatHigh;
	uint8_t reserved;
} Localization_s;

class DeviceObject_c
{
public:
	DeviceObject_c(const std::string& type, uint16_t objId = 0xffff) : m_ObjectType(type), m_ObjectId(objId), m_isDirty(true) {}
	virtual ~DeviceObject_c()	{}

	virtual DeviceObject_c*	clone() const = 0;		// virtual copy constructor
	virtual DeviceObject_c* create() const = 0;		// virtual constructor

	uint16_t			getObjectId() const		{ return m_ObjectId; }
	const std::string&	getObjectType() const	{ return m_ObjectType; }

	void Designator(const char* str, uint8_t encoding = 0)	{ Designator(std::string(str), encoding); }
	void Designator(const std::string& str, uint8_t encoding = 0);
	const std::string& Designator() const { return m_Designator; }

	virtual bool isValid() const			{ return !m_ObjectType.empty() && m_ObjectId != 65535; }

	virtual size_t init(const HUGE_MEM uint8_t* bytestream, size_t cnt) = 0;
	virtual bool formatBytestream(std::vector<uint8_t>& byteStream);

	virtual bool isDirty() const	{ return m_isDirty; }

protected:
	template<typename T> inline bool checkDirty(T lhs, T rhs) {	 setDirty(lhs != rhs); return isDirty(); }

	void setObjectId(uint16_t objId)		{ m_ObjectId = objId; }
	void validateSize(std::string& str, size_t len);

	virtual void format(std::vector<uint8_t>& byteStream, const std::string& str);
	virtual void format(std::vector<uint8_t>& byteStream, const char* str)			
		{ format(byteStream, (const uint8_t*)str, std::strlen(str)); }
	virtual void format(std::vector<uint8_t>& byteStream, const uint8_t* val, size_t len);
	virtual void format(std::vector<uint8_t>& byteStream, uint8_t val);
	virtual void format(std::vector<uint8_t>& byteStream, uint16_t val);
	virtual void format(std::vector<uint8_t>& byteStream, uint32_t val);
	virtual void format(std::vector<uint8_t>& byteStream, int32_t val)	
		{ format(byteStream, (uint32_t)val); }
	virtual void format(std::vector<uint8_t>& byteStream, float val);

	void setDirty(bool flag)		{ if (flag) m_isDirty = true; }
	void clearDirty()				{ m_isDirty = false; }

	void storeString(std::string& toString, const std::string& fromString, uint16_t encoding);

protected:
	std::string		m_Designator;
	const std::string	m_ObjectType;
	uint16_t		m_ObjectId;
	bool			m_isDirty;

	friend class DevicePool_c;

private:
	DeviceObject_c& operator=(const DeviceObject_c& rhs);
};

class DeviceObjectDvc_c : public DeviceObject_c
{
public:
	DeviceObjectDvc_c(uint16_t objId = 65535);
	virtual ~DeviceObjectDvc_c()	{}

	DeviceObjectDvc_c*	clone() const	{ return new DeviceObjectDvc_c(*this); }
	DeviceObjectDvc_c* create() const	{ return new DeviceObjectDvc_c(); }

	virtual bool isValid() const	{ return true; }
	virtual size_t init(const HUGE_MEM uint8_t* bytestream, size_t cnt);
	virtual bool formatBytestream(std::vector<uint8_t>& byteStream);

	void Version(const char* str);
	void Version(const std::string& str);
	void SerialNumber(const char* str);
	void SerialNumber(const std::string& str);
	void WsmName(const IsoAgLib::iIsoName_c& name);
	
	void Localization(const Localization_s& localization);
	void Localization(const localSettings_s& currentSettings);
	
	void StructureLabel(const std::string& label)
		{ checkDirty(label, m_StructLabel); m_StructLabel = label; }

	const IsoAgLib::iIsoName_c& WsmName() const		{ return m_WsmName; }
	const Localization_s&	Localization() const	{ return m_Localization; }
	const std::string&	StructureLabel() const		{ return m_StructLabel; }
	const std::string&	SerialNumber() const		{ return m_SerialNumber; }

protected:
	std::string				m_Version;
	std::string				m_SerialNumber;
	IsoAgLib::iIsoName_c	m_WsmName;
	std::string				m_StructLabel;
	Localization_s			m_Localization;
};

class DeviceObjectDet_c : public DeviceObject_c
{
public:
	DeviceObjectDet_c(uint16_t objId = 65535);
	DeviceObjectDet_c(uint16_t objId, uint16_t parentObjId, uint16_t element, uint8_t type, const std::string& desig = "");
	virtual ~DeviceObjectDet_c()	{}

	DeviceObjectDet_c*	clone() const	{ return new DeviceObjectDet_c(*this); }
	DeviceObjectDet_c* create() const	{ return new DeviceObjectDet_c(); }

	virtual bool isValid() const;
	virtual size_t init(const HUGE_MEM uint8_t* bytestream, size_t cnt);
	virtual bool formatBytestream(std::vector<uint8_t>& byteStream);

	size_t	numberOfChildren() const	{ return m_ChildList.size(); }
	bool	AddChild(uint16_t objId);

	void Type(uint8_t type)					{ if (checkDirty(m_Type, type)) m_Type = type; }
	void ElementNumber(uint16_t element)	{ if (checkDirty(m_ElementNumber, element)) m_ElementNumber = element; }
	void Parent(uint16_t parent)			{ if (checkDirty(m_ParentId, parent)) m_ParentId = parent; }

protected:
	uint8_t					m_Type;
	uint16_t				m_ElementNumber;
	uint16_t				m_ParentId;
	std::vector<uint16_t>	m_ChildList;
};

class DeviceObjectDpd_c : public DeviceObject_c
{
public:
	DeviceObjectDpd_c(uint16_t objId = 65535);
	DeviceObjectDpd_c(uint16_t objId, uint16_t ddi, uint8_t props, uint8_t method, const std::string& desig = "", uint16_t dvpRef = 65535);
	virtual ~DeviceObjectDpd_c()	{}

	DeviceObjectDpd_c*	clone() const	{ return new DeviceObjectDpd_c(*this); }
	DeviceObjectDpd_c* create() const	{ return new DeviceObjectDpd_c(); }

	virtual bool isValid() const	{ return true; }
	virtual size_t init(const HUGE_MEM uint8_t* bytestream, size_t cnt);
	virtual bool formatBytestream(std::vector<uint8_t>& byteStream);

	void Ddi(uint16_t ddi)				{ if (checkDirty(m_Ddi, ddi)) m_Ddi = ddi; }
	void Properties(uint8_t properties)	{ if (checkDirty(m_Properties, properties)) m_Properties = properties; }
	void Method(uint8_t method)			{ if (checkDirty(m_Method, method)) m_Method = method; }
	void DvpObjectId(uint16_t objId)	{ if (checkDirty(m_DvpObjectId, objId)) m_DvpObjectId = objId; }

protected:
	uint16_t				m_Ddi;
	uint8_t					m_Properties;
	uint8_t					m_Method;
	uint16_t				m_DvpObjectId;
};

class DeviceObjectDpt_c : public DeviceObject_c
{
public:
	DeviceObjectDpt_c(uint16_t objId = 65535);
	DeviceObjectDpt_c(uint16_t objId, uint16_t ddi, int32_t value, const std::string& desig = "", uint16_t dvpRef = 65535);
	virtual ~DeviceObjectDpt_c()	{}

	DeviceObjectDpt_c*	clone() const	{ return new DeviceObjectDpt_c(*this); }
	DeviceObjectDpt_c* create() const	{ return new DeviceObjectDpt_c(); }

	virtual bool isValid() const	{ return true; }
	virtual size_t init(const HUGE_MEM uint8_t* bytestream, size_t cnt);
	virtual bool formatBytestream(std::vector<uint8_t>& byteStream);

	void Value(int32_t val)		{ if (checkDirty(m_Value, val)) m_Value = val; }
	int32_t Value() const		{ return m_Value; }

protected:
	uint16_t				m_Ddi;
	int32_t					m_Value;
	uint16_t				m_DvpObjectId;
};

class DeviceObjectDvp_c : public DeviceObject_c
{
public:
	DeviceObjectDvp_c(uint16_t objId = 65535);
	DeviceObjectDvp_c(uint16_t objId, float scale, int32_t offset, uint8_t decimals, const std::string& desig = "");
	virtual ~DeviceObjectDvp_c()	{}

	DeviceObjectDvp_c*	clone() const	{ return new DeviceObjectDvp_c(*this); }
	DeviceObjectDvp_c* create() const	{ return new DeviceObjectDvp_c(); }

	virtual bool isValid() const	{ return true; }
	virtual size_t init(const HUGE_MEM uint8_t* bytestream, size_t cnt);
	virtual bool formatBytestream(std::vector<uint8_t>& byteStream);

	void Offset(int32_t offset)			{ if (checkDirty(m_Offset, offset)) m_Offset = offset; }
	void Decimals(uint8_t decimals)		{ if (checkDirty(m_Decimals, decimals)) m_Decimals = decimals; }
	void Scale(float scale)				{ if (checkDirty(m_Scale, scale)) m_Scale = scale; }

protected:
	int32_t					m_Offset;
	float					m_Scale;
	uint8_t					m_Decimals;
};

class DevicePool_c
{
	typedef std::map<uint16_t, DeviceObject_c*> deviceMap_t;
public:
	DevicePool_c();
	DevicePool_c(const DevicePool_c& rhs);
	virtual ~DevicePool_c()	{ clear(); }

	void	clear();
	bool	isEmpty() const	{ return m_DevicePool.empty(); }

	bool	init(const HUGE_MEM uint8_t* bytestream, size_t count);

	bool	Add(const DeviceObject_c& devObj);

	const std::vector<uint8_t>&	getBytestream();
	bool getDirtyBytestream(std::vector<uint8_t>& byteStream);

// Object getter methods
	DeviceObjectDet_c* getDetObject(uint16_t objId) const		{ return (DeviceObjectDet_c*)getObject(objId, "DET"); }
	DeviceObjectDpd_c* getDpdObject(uint16_t objId) const		{ return (DeviceObjectDpd_c*)getObject(objId, "DPD"); }
	DeviceObjectDpt_c* getDptObject(uint16_t objId) const		{ return (DeviceObjectDpt_c*)getObject(objId, "DPT"); }
	DeviceObjectDvc_c* getDvcObject(uint16_t objId) const		{ return (DeviceObjectDvc_c*)getObject(objId, "DVC"); }
	DeviceObjectDvp_c* getDvpObject(uint16_t objId) const		{ return (DeviceObjectDvp_c*)getObject(objId, "DVP"); }

	uint16_t updateDvpObject(uint16_t objId, float scale, int32_t offset, uint8_t decimals, const std::string& desig);

	void updateLocale();

	bool isDirty() const;
	void setDirty()	{ m_isDirty = true; }

	void setUploaded();

protected:
	bool Add(DeviceObject_c* devObj);
	DeviceObject_c*	createDeviceObject(const HUGE_MEM uint8_t* bp, size_t cnt);
	DeviceObject_c*	getObject(uint16_t objId, const std::string& objectType) const;

protected:
	deviceMap_t		m_DevicePool;
	std::vector<uint8_t>	m_Bytestream;
	bool	m_isDirty;

private:
	DevicePool_c& operator=(const DevicePool_c&);

};
#endif
#endif	// _DEVICEPOOL_C_H_
