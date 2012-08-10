/*
  ddopbase_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepoolhandler_c.h>

#include <IsoAgLib/hal/hal_typedef.h>

class DdopBase_c : public DevicePoolHandler_c
{
public:
	enum PoolState_e { PoolStateInit = 0,  PoolStatePresetting, PoolStateNotPresent, PoolStateStale, PoolStateInvalid, PoolStateUploaded, PoolStateActive, PoolStateError};
	enum PoolAction_e { PoolActionIdle = 0, PoolActionWaiting, PoolActionTimeout };

public:
	DdopBase_c();
	virtual ~DdopBase_c() {}

	void init(IsoAgLib::iIdentItem_c* m_myIdent);

	virtual void eventTcAlive(bool isAlive);
	virtual void eventStructureLabelResponse(uint8_t result, const STL_NAMESPACE::string& label);
	virtual void eventLocalizationLabelResponse(uint8_t result, const STL_NAMESPACE::vector<uint8_t>& label);
	virtual void eventPoolUploadResponse(uint8_t result);
	virtual void eventPoolActivateResponse(uint8_t result);
	virtual void eventPoolDeleteResponse(uint8_t result);

	void setPoolState(PoolState_e newState);
	PoolState_e getPoolState() { return m_PoolState; };

protected:
	virtual void initDevicePool() = 0;	//Derived class must implement it to initilize DDOP
	void timeEventDevicePool();

	// Device description object pool for Task Controller
	DevicePool_c m_DevicePool;
	IsoAgLib::iIdentItem_c* m_myIdent;

private:
	bool b_registerSuccess;
	// DevicePoolHandler_c state variables
	PoolState_e	m_PoolState;
	PoolAction_e	m_PoolAction;
	bool	m_bTcActive;
};


