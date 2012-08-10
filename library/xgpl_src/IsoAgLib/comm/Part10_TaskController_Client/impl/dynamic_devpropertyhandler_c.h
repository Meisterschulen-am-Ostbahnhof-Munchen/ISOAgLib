
/*
  devpropertyhandler_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef DYNAMIC_DEVPROPERTYHANDLER_C_H
#define DYNAMIC_DEVPROPERTYHANDLER_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part3_DataLink/imultisendstreamer_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>

#ifdef USE_DYNAMIC_PART10

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepoolhandler_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/devicepool_c.h>

#include <map>
#include <list>
#include <vector>
#include <string>

namespace IsoAgLib { class iProcess_c;};

namespace __IsoAgLib {

class MultiSendPkg_c;
class DevPropertyHandler_c : public IsoAgLib::iMultiSendStreamer_c, public __IsoAgLib::MultiSendEventHandler_c
{
#define DEF_TimeOut_GetVersion	5000
#define DEF_TimeOut_OPTransfer 20000
#define DEF_TimeOut_EndOfDevicePool 10000
#define DEF_WaitFor_Reupload 5000
#define DEF_TimeOut_ChangeDesignatorValue 1500
#define DEF_TimeOut_NormalCommand 1500

	enum en_processDataMsg
	{
		procCmdPar_RequestVersionMsg = 0x0,
		procCmdPar_VersionMsg = 0x10,
		procCmdPar_RequestStructureLabelMsg = 0x1,
		procCmdPar_StructureLabelMsg = 0x11,
		procCmdPar_RequestLocalizationLabelMsg = 0x21,
		procCmdPar_LocalizationLabelMsg = 0x31,
		procCmdPar_RequestOPTransferMsg = 0x41,
		procCmdPar_RequestOPTransferRespMsg = 0x51,
		procCmdPar_OPTransferMsg = 0x61,
		procCmdPar_OPTransferRespMsg = 0x71,
		procCmdPar_OPActivateMsg = 0x81,
		procCmdPar_OPActivateRespMsg = 0x91,
		procCmdPar_OPDeleteMsg = 0xA1,
		procCmdPar_OPDeleteRespMsg = 0xB1,
		procCmdPar_ChangeDesignatorMsg = 0xC1,
		procCmdPar_ChangeDesignatorRespMsg = 0xD1
	};

public:
	enum en_devPropertyStatus
	{
		StatusNoError = 0,
		StatusBusy,
		StatusNotInitted,
		StatusCannotProcess
	};

public:
	DevPropertyHandler_c();
	virtual ~DevPropertyHandler_c()  {};

// initialisation for DevPropertyHandler_c
	void init();

	bool registerPoolHandler(DevicePoolHandler_c* handler);
	bool sendCommandChangeDesignator(uint16_t rpui16_objectID, const char* rpc_newString, uint8_t stringLength);

// Device pool request methods
	int32_t requestVersion()			{ return doCommand(procCmdPar_RequestVersionMsg, DEF_TimeOut_GetVersion); }
	int32_t requestStructureLabel()		{ return doCommand(procCmdPar_RequestStructureLabelMsg); }
	int32_t requestLocalizationLabel()	{ return doCommand(procCmdPar_RequestLocalizationLabelMsg); }
	int32_t requestPoolActivate()		{ return doCommand(procCmdPar_OPActivateMsg); }
	int32_t requestPoolDelete()			{ return doCommand(procCmdPar_OPDeleteMsg); }
	int32_t requestPoolTransfer(const DevicePool_c& devicePool);
	int32_t requestPoolPartielTransfer(const DevicePool_c& devicePool);
  

// start processing of a process msg
    bool processMsg( ProcessPkg_c& arc_data );

	bool timeEvent( void );

	bool registerDevicePool (const IdentItem_c* rpc_wsMasterIdentItem, const HUGE_MEM uint8_t* rpc_devicePoolByteArray, const uint32_t rui32_bytestreamLength, bool b_setToDefault);
	
// place next data to send direct into send buffer of pointed
//	stream send package - MultiSend_c will send this
//	buffer afterwards
	virtual void setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes);

// set cache for data source to stream start
	virtual void resetDataNextStreamPart();

// save current send position in data source - neeed for resend on send problem
	virtual void saveDataNextStreamPart ();

// reactivate previously stored data source position - used for resend on problem
	virtual void restoreDataNextStreamPart ();

// calculate the size of the data source
	virtual uint32_t getStreamSize ();

// get the first byte, which is the COMMAND-byte and should be given back CONST!
	virtual uint8_t getFirstByte ();

	void reset() { ui16_currentSendPosition = ui16_storedSendPosition = 0; };

	void updateTcStateReceived(uint8_t rui8_lastTcState) { ui8_lastTcState = rui8_lastTcState; i32_tcStateLastReceived = HAL::getTime();};
	void setTcSourceAddress(uint8_t rtcSourceAddress) { tcSourceAddress = rtcSourceAddress;}

// React to changes in the state of a multi-packet stream
	void reactOnStateChange(const SendStream_c& sendStream);

protected:
	int32_t	doCommand(int32_t opcode, int32_t timeout = DEF_TimeOut_NormalCommand);

private:
	uint16_t ui16_currentSendPosition;
	uint16_t ui16_storedSendPosition;
	enum PoolState_t
	{
		OPNotRegistered,		// Initial state, no device pool registered from application
		OPRegistered,			// Idle state, pool is registered but not uploaded and active
		OPSuccessfullyUploaded,	// Run state, pool is uploaded and activated
		OPCannotBeUploaded		// Error state, the pool cannot be uploaded (TC out of memory condition)
	};
	enum UploadState_t  { StateIdle, StateBusy, StateTimeout};
	enum UploadSteps_t  
	{
		UploadNone,                             //state when nothing is to do
		UploadUploading,
		UploadWaitForRequestOPTransferResponse,
		UploadWaitForOPTransferResponse,		// wait for transfer response after sending pool to TC
		UploadFailed                            //Upload failed
	};

	friend class IsoAgLib::iProcess_c;

// check if the TC is active (has sent a status message in the past six seconds)
	bool isTcAlive (int32_t i32_currentTime);

// send our WS task message to the TC indicating that we are alive and functioning
	void sendWorkingSetTaskMsg(int32_t i32_currentTime);

	/** local instances: call to check
	- address claim completed at least 6sec in the past
	- TC status message received
	*/
	void checkInitState();

	uint8_t getTcSourceAddress () { return tcSourceAddress; };

	void outOfMemory();
	void startUpload();

//
	int32_t i32_tcStateLastReceived;			// time of the last TC status message
	uint8_t ui8_lastTcState;					// last TC status byte value
	int32_t i32_timeStartWaitAfterAddrClaim;	// 
	bool b_initDone;
	int32_t i32_timeWsTaskMsgSent;				// time the last WS status message was sent

	bool b_tcAliveNew;

	bool b_receivedStructureLabel;
	bool b_receivedLocalizationLabel;

	ProcessPkg_c mc_data;

	uint8_t tcSourceAddress;		// SA of the Task Controller (usually 0xF7)

	uint8_t ui8_versionLabel;					// Version reported by the TC
	STL_NAMESPACE::string pch_structureLabel;	// Pool structure label received from the TC in response to a request
	STL_NAMESPACE::vector<uint8_t> pch_localizationLabel;	// Pool version label from TC

// has to be set using registerDevicePool (...) so that DevPropertyHandler_c can interact in the name of the wsMaster
	const IdentItem_c* pc_wsMasterIdentItem;

// Upload-State & Variables
	PoolState_t en_poolState;
	UploadState_t en_uploadState;
	UploadSteps_t en_uploadStep;

	uint32_t ui32_uploadTimestamp;
	uint32_t ui32_uploadTimeout;

	uint8_t ui8_uploadRetry;
	uint8_t ui8_commandParameter;

// list of all device pool handlers registered to receive events
	STL_NAMESPACE::vector<DevicePoolHandler_c*> m_PoolHandlerList;
	//STL_NAMESPACE::vector<uint8_t> m_DevicePoolToUpload;

  uint16_t m_DevicePoolToUploadSize;
};

}
#endif
#endif
