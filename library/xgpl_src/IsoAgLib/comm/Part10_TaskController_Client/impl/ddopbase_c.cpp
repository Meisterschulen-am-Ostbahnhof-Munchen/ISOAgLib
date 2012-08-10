#include "ddopbase_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocess_c.h>

#ifdef USE_DYNAMIC_PART10

DdopBase_c::DdopBase_c()
: b_registerSuccess(false)
, m_PoolState(PoolStateInit)
, m_PoolAction(PoolActionIdle)
, m_bTcActive(false)
, m_myIdent(NULL)
{
}


void DdopBase_c::init(IsoAgLib::iIdentItem_c* myIdent)
{
	m_myIdent = myIdent;
	IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().registerPoolHandler(this);
}

void
DdopBase_c::timeEventDevicePool()
{
	if (m_bTcActive && m_PoolAction == PoolActionIdle)
	{
		if (m_PoolState == PoolStateActive)
		{
			if (m_DevicePool.isDirty())
				m_PoolState = PoolStateStale;
		}
		else
		{
			switch (m_PoolState)
			{
			case PoolStateInit:
			// Initialization state
			//	Retrieve the structure label from the current pool
				if (m_DevicePool.isEmpty())
					break;
				m_PoolState = PoolStatePresetting;
				IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().requestStructureLabel();
				m_PoolAction = PoolActionWaiting;
				break;

//			case  PoolStateError:
//				m_PoolState = PoolStateInit;
//				m_PoolAction = PoolActionIdle;
//				break;

			case PoolStateInvalid:
			// Pool is present, but it is the wrong version -- delete it
				IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().requestPoolDelete();
				m_PoolAction = PoolActionWaiting;
				break;

			case PoolStateNotPresent:
			// No device pool present - upload it
				if (m_DevicePool.isEmpty())
					break;
				IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().requestPoolTransfer(m_DevicePool.getBytestream());
				m_PoolAction = PoolActionWaiting;
				break;

			case PoolStateStale:	// Upload changed descriptions
				{
					std::vector<uint8_t> newBytes;
					if (m_DevicePool.getDirtyBytestream(newBytes))
					{
						IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().requestPoolTransfer(newBytes);
						m_PoolAction = PoolActionWaiting;
					}
					else
						setPoolState(PoolStateActive);
				}
				break;

			case PoolStateUploaded:
			// Pool has been uploaded - activate it
				IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().requestPoolActivate();
				m_PoolAction = PoolActionWaiting;
				break;

			default:
				break;
			}
		}
	}
}

void
DdopBase_c::setPoolState(PoolState_e newState)
{
	m_PoolState = newState;
}

void
DdopBase_c::eventPoolDeleteResponse(uint8_t /*result*/)
{
	setPoolState(PoolStateNotPresent);
	m_PoolAction = PoolActionIdle;
}

//
// eventLocalizationLabelResponse()
//	Handle the receipt of the DDOP localization label from the Task Controller.
//	The default implementation checks for a label mismatch.  A mismatch will mark 
//	all DVP entries as needing upload
void
DdopBase_c::eventLocalizationLabelResponse(uint8_t result, const std::vector<uint8_t>& label)
{
	if (result == 0 && !label.empty())
	{
		DeviceObjectDvc_c* dvc = m_DevicePool.getDvcObject(0);	// DVC entry is always 0
		if (dvc)
		{
			const Localization_s& locale = dvc->Localization();
			if (std::memcmp((void*)&locale, (void *)&label[0], 7) != 0)
			{
				m_DevicePool.updateLocale();
			}
			setPoolState(PoolStateUploaded);
		}
	}
	else
	{
		m_DevicePool.setDirty();
		setPoolState(PoolStateNotPresent);
	}

	m_PoolAction = PoolActionIdle;
}

//
// eventStructureLabelResponse()
//	Handle the receipt of the DDOP structure label from the Task Controller.
//	The default implementation will set the pool state to invalid, which will
//	result in an upload of a new device pool
void
DdopBase_c::eventStructureLabelResponse(uint8_t result, const std::string& label)
{
	if (result == 0 && !label.empty())
	{
		DeviceObjectDvc_c* dvc = m_DevicePool.getDvcObject(0);	// DVC entry is always 0
		if (dvc)
		{
			if (label != dvc->StructureLabel())
			{
				m_DevicePool.setDirty();
				setPoolState(PoolStateInvalid);
			}
			else
				m_DevicePool.setUploaded();
			setPoolState(PoolStateInvalid);
		}
	}
	else
	{
		m_DevicePool.setDirty();
		setPoolState(PoolStateNotPresent);
	}

	if (m_PoolState == PoolStatePresetting)
	{
		IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().requestLocalizationLabel();
		m_PoolAction = PoolActionWaiting;
	}
	else
		m_PoolAction = PoolActionIdle;

}

void
DdopBase_c::eventPoolUploadResponse(uint8_t result)
{
	if (result == 0)
	{
		setPoolState(PoolStateUploaded);
		m_DevicePool.setUploaded();
	}
	else
		setPoolState(PoolStateError);
	m_PoolAction = PoolActionIdle;
}

void
DdopBase_c::eventPoolActivateResponse(uint8_t result)
{
	if (result == 0)
		setPoolState(PoolStateActive);
	else
		setPoolState(PoolStateError);

	m_PoolAction = PoolActionIdle;
}
//
// eventTcAlive()
//	Called to process a change in the Task Controller alive state.
void
DdopBase_c::eventTcAlive(bool isAlive)
{
	m_bTcActive = isAlive;
// If the TC has come online, prepare the device pool for upload
	if (isAlive)
		initDevicePool();

	setPoolState(PoolStateInit);
	m_PoolAction = PoolActionIdle;
}
#endif
