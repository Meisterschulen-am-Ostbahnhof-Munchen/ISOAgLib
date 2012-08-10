////////////////////////////////////////////////////////////////////////////////
//  ddopbase_c.h -
//  Base class working together with devpropertyhandler_c to handle DDOP
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

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepoolhandler_c.h>

#include <IsoAgLib/hal/hal_typedef.h>

class DdopBase_c : public DevicePoolHandler_c
{
public:
	enum PoolState_e { PoolStateInit = 0,  PoolStatePresetting, PoolStateNotPresent, PoolStateStale, PoolStateInvalid, PoolStateUploaded, PoolStateActive, PoolStateError};
	enum PoolAction_e { PoolActionIdle = 0, PoolActionWaiting, PoolActionTimeout };

	void init(IsoAgLib::iIdentItem_c* m_myIdent);
	DdopBase_c();
	virtual ~DdopBase_c(){};

// DevicePoolHandler_c overrides
	virtual void eventTcAlive(bool isAlive);
	virtual void eventPoolDeleteResponse(uint8_t result);
	virtual void eventLocalizationLabelResponse(uint8_t result, const STL_NAMESPACE::vector<uint8_t>& label);
	virtual void eventStructureLabelResponse(uint8_t result, const STL_NAMESPACE::string& label);
	virtual void eventPoolActivateResponse(uint8_t result);
	virtual void eventPoolUploadResponse(uint8_t result);
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


