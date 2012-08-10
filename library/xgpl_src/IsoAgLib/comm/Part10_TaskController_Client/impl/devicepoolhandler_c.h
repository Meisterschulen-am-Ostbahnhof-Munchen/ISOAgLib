#ifndef	_DEVICEPOOLHANDLER_C_H_
#define	_DEVICEPOOLHANDLER_C_H_

#include	<string>
#include	<vector>

#ifdef USE_DYNAMIC_PART10

class DevicePoolHandler_c
{
public:
	DevicePoolHandler_c()	{}
	virtual ~DevicePoolHandler_c()	{}

// Task Controller alive message
	virtual void eventTcAlive(bool /*isAlive*/)	{}

// Technical data message responses
	virtual void eventVersionResponse(uint8_t /*rui8_version*/)	{}

// Device description message responses
	virtual void eventStructureLabelResponse(uint8_t /*result*/, const STL_NAMESPACE::string& /*label*/)	{}
	virtual void eventLocalizationLabelResponse(uint8_t /*result*/, const STL_NAMESPACE::vector<uint8_t>& /*label*/)	{}
	virtual void eventPoolUploadResponse(uint8_t /*result*/)	{}
	virtual void eventPoolActivateResponse(uint8_t /*result*/)		{}
	virtual void eventPoolDeleteResponse(uint8_t /*result*/)	{}

	virtual void eventTimeout(uint8_t /*opcode*/)	{}

private:
// Unimplemented copy ctor() and assignment operator
	DevicePoolHandler_c(const DevicePoolHandler_c&);
	DevicePoolHandler_c& operator=(DevicePoolHandler_c&);
};
#endif

#endif	// _DEVICEPOOLHANDLER_C_H_
