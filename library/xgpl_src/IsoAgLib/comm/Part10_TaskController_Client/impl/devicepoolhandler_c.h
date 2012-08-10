/*
  devicepoolhandler_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

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
	virtual void eventTcAlive(bool /*isAlive*/)	= 0;

// Technical data message responses
	//virtual void eventVersionResponse(uint8_t /*rui8_version*/)	= 0;

// Device description message responses
	virtual void eventStructureLabelResponse(uint8_t /*result*/, const STL_NAMESPACE::string& /*label*/) = 0;
	virtual void eventLocalizationLabelResponse(uint8_t /*result*/, const STL_NAMESPACE::vector<uint8_t>& /*label*/) = 0;
	virtual void eventPoolUploadResponse(uint8_t /*result*/) = 0;
	virtual void eventPoolActivateResponse(uint8_t /*result*/) = 0;
	virtual void eventPoolDeleteResponse(uint8_t /*result*/) = 0;

	//virtual void eventTimeout(uint8_t /*opcode*/) = 0;

private:
  /** not copyable : copy constructor is only declared, never defined */
  DevicePoolHandler_c(const DevicePoolHandler_c&);
  /** not copyable : copy operator is only declared, never defined */
  DevicePoolHandler_c& operator=(const DevicePoolHandler_c&); 
};
#endif

#endif	// _DEVICEPOOLHANDLER_C_H_
