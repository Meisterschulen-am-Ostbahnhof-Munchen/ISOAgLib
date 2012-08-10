/*
  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>

#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/stream_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendpkg_c.h>

#include <IsoAgLib/util/impl/singleton.h>

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>

#include <fstream>

#ifdef USE_DYNAMIC_PART10

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/dynamic_devpropertyhandler_c.h>

#ifdef USE_ISO_TERMINAL
  #include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminal_c.h>
  #include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtclientservercommunication_c.h>
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

#define ISO_VT_UPLOAD_BUFFER_SIZE 16384
uint8_t marr_uploadBuffer [ISO_VT_UPLOAD_BUFFER_SIZE];

//define length of every attribute in deviceObject
#define DEF_Transfer_Code 1
#define DEF_TableID 3
#define DEF_ObjectID 2
#define DEF_Designator_Length 1
#define DEF_Software_Version_Length 1
#define DEF_Workingset_MasterNAME 8
#define DEF_Serialnumber_Length 1
#define DEF_Structurelabel 7


namespace __IsoAgLib {
DevPropertyHandler_c::DevPropertyHandler_c() :  MultiSendEventHandler_c( *this ),
												ui16_currentSendPosition(0), 
												ui16_storedSendPosition(0), 
												pc_wsMasterIdentItem(0),
                        m_DevicePoolToUploadSize(0)
{
	i32_tcStateLastReceived = i32_timeStartWaitAfterAddrClaim = i32_timeWsTaskMsgSent = -1;
	ui8_lastTcState = 0;
	b_initDone = FALSE;
	tcSourceAddress = 0xfe;

	en_poolState = OPNotRegistered;
	en_uploadState = StateIdle;
	en_uploadStep = UploadNone;

	b_tcAliveNew = false;
}

bool
DevPropertyHandler_c::registerPoolHandler(DevicePoolHandler_c* rc_handler)
{
	STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it;
	for (it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
	{
		if (*it == rc_handler)
			return false;
	}
	m_PoolHandlerList.push_back(rc_handler);
	return true;
}


/** process received can messages
    @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
  */
bool
DevPropertyHandler_c::processMsg( ProcessPkg_c& arc_data )
{
    arc_data.string2Flags();

//should never be the case
    if ((arc_data.isoPgn() & 0x3FF00LU) != PROCESS_DATA_PGN)
		return false;

// set false if "default" is selected in switch case
	bool b_rc = true;

// handling of NACK
//	means that no device description is uploaded before
//	so extract all necessary information from the byte-stream (structure and localization label)
//	add the stream to the map
	if ((arc_data.getUint8Data(0) & 0xF) == 0xD)
	{
		//here starts nack handling
		// these two cmds could be answered with a NACK
		switch ((arc_data.getUint8Data(0) >> 4) & 0xF)
		{
		case 0x0:	//NACK Request StructureLabel
			en_uploadState = StateIdle;
			pch_structureLabel = "";
			for (STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
				(*it)->eventStructureLabelResponse(1, pch_structureLabel);
#ifdef DEBUG
			INTERNAL_DEBUG_DEVICE << "Received NACK for structure label..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        requestVersion();
			break;
		case 0x2:	//NACK Request LocalizationLabel
			en_uploadState = StateIdle;
			pch_localizationLabel.clear();
			for (STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
				(*it)->eventLocalizationLabelResponse(1, pch_localizationLabel);
#ifdef DEBUG
			INTERNAL_DEBUG_DEVICE << "Received NACK for localization label..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        requestVersion();
			break;
		default:
			b_rc = false;
			break;
		}
	}

	switch (arc_data.getUint8Data (0))
	{
	case procCmdPar_VersionMsg:
		ui8_versionLabel = arc_data.getUint8Data(1);
		en_uploadState = StateIdle;
#ifdef DEBUG
		INTERNAL_DEBUG_DEVICE << "Received version response..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
		break;

	case procCmdPar_StructureLabelMsg:
	// command is always successful if not NACK'd above
		pch_structureLabel = "";
		for (int i = 1; i < 8; i++)
			pch_structureLabel.push_back((char)arc_data.getUint8Data(i));

		en_uploadState = StateIdle;
		en_uploadStep = UploadNone;

		for (STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
			(*it)->eventStructureLabelResponse(0, pch_structureLabel);
		break;

	case procCmdPar_LocalizationLabelMsg:
	// command is always successful if not NACK'd above
		pch_localizationLabel.clear();
		for (int i = 1; i < 8; i++)
			pch_localizationLabel.push_back(arc_data.getUint8Data(i));

		en_uploadState = StateIdle;
		en_uploadStep = UploadNone;
		for (STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
			(*it)->eventLocalizationLabelResponse(0, pch_localizationLabel);
#ifdef DEBUG
		INTERNAL_DEBUG_DEVICE << "Received localization response..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
		break;

	case procCmdPar_RequestOPTransferRespMsg:
		if (arc_data.getUint8Data(1) == 0) // on success, send the object pool
		{
#if !defined(PROPRIETARY_DEVPROPERTYHANDLER_H)
			getMultiSendInstance4Comm().sendIsoTarget(pc_wsMasterIdentItem->isoName(),
#else
			getMultiSendInstance4Comm().sendIsoTarget(pc_wsMasterIdentItem->isoName(),
#endif
				getIsoMonitorInstance4Comm().isoMemberNr(tcSourceAddress).isoName(),
				&marr_uploadBuffer[0],(uint32_t)m_DevicePoolToUploadSize, PROCESS_DATA_PGN, this);

			en_uploadState = StateBusy;
			en_uploadStep = UploadUploading;
		}
		else
			outOfMemory();
		break;

	case procCmdPar_OPTransferRespMsg:
		en_uploadState = StateIdle;
		en_uploadStep = UploadNone;
		for (STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
			(*it)->eventPoolUploadResponse(arc_data.getUint8Data(1));
		break;

	case procCmdPar_OPActivateRespMsg:
		en_uploadState = StateIdle;
		en_uploadStep = UploadNone;
		for (STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
			(*it)->eventPoolActivateResponse(arc_data.getUint8Data(1));
		break;

	case procCmdPar_OPDeleteRespMsg:
		en_uploadState = StateIdle;
		en_uploadStep = UploadNone;
		for (STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
			(*it)->eventPoolDeleteResponse(arc_data.getUint8Data(1));
		break;

	case procCmdPar_ChangeDesignatorRespMsg:
		en_uploadState = StateIdle;
		en_uploadStep = UploadNone;
		break;
	default:
		b_rc = false;
		break;
	}

	return b_rc;
}


/** initialise element which can't be done during construct
  */
void
DevPropertyHandler_c::init()
{
}

int32_t
DevPropertyHandler_c::doCommand(int32_t opcode, int32_t timeout)
{
	if (pc_wsMasterIdentItem == 0 || pc_wsMasterIdentItem->getIsoItem() == 0)
		return StatusNotInitted;

	if (en_uploadState == StateBusy)
		return StatusBusy;
	if (en_uploadState == StateTimeout)
		return StatusCannotProcess;

	mc_data.setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
		opcode, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

	getIsoBusInstance4Comm() << mc_data;

	ui32_uploadTimestamp = HAL::getTime();
	ui32_uploadTimeout = timeout;
	en_uploadStep = UploadNone;
	ui8_commandParameter = opcode;
	en_uploadState = StateBusy;

	return StatusNoError;
}

int32_t
DevPropertyHandler_c::requestPoolTransfer(const DevicePool_c& devicePool)
{
  // get check length
  m_DevicePoolToUploadSize = devicePool.getByteStreamSize() + 1; 
  isoaglib_assert(m_DevicePoolToUploadSize < ISO_VT_UPLOAD_BUFFER_SIZE);

  // The pool will be sent in a multipacket.  Insert the upload command byte (061) at the head of buffer
  marr_uploadBuffer[0] = (uint8_t)procCmdPar_OPTransferMsg;

	if (!devicePool.getByteStream(marr_uploadBuffer + 1))
		return StatusCannotProcess;

#ifdef SYSTEM_PC
  // save DDD in file for DEBUG
  std::fstream ofs("devicedescription.bin", std::ios::out | std::ios::binary);
  ofs.write((char*)marr_uploadBuffer,m_DevicePoolToUploadSize);
#endif

	startUpload();

	return StatusNoError;
}

int32_t
DevPropertyHandler_c::requestPoolPartielTransfer(const DevicePool_c& devicePool)
{
  // get check length
  m_DevicePoolToUploadSize = devicePool.getDirtyByteStreamSize(); 
  isoaglib_assert(m_DevicePoolToUploadSize < ISO_VT_UPLOAD_BUFFER_SIZE);

  // The pool will be sent in a multipacket.  Insert the upload command byte (061) at the head of buffer
  marr_uploadBuffer[0] = (uint8_t)procCmdPar_OPTransferMsg;

	if (!devicePool.getDirtyByteStream(marr_uploadBuffer + 1))
		return StatusCannotProcess;

	startUpload();

	return StatusNoError;
}

// timeEvent()
//	periodic event handling
//		return value true -> all processing complete
bool
DevPropertyHandler_c::timeEvent( void )
{
// Wait until we are properly initialized before doing anything else
	if (!b_initDone)
	{
		checkInitState();
		return true;
	}

	const int32_t i32_currentTime = HAL::getTime();

// Check the state of the TC via its status message
	bool tcAliveOld = b_tcAliveNew;
	b_tcAliveNew = isTcAlive(i32_currentTime);

  static int32_t mi32_timeWsAnnounceKey = -1;

// If there was a state change in TC alive
	if (tcAliveOld != b_tcAliveNew)
	{
	// react on tc alive change "false->true"
		if (b_tcAliveNew == true)
		{
		// OPNotRegistered -> no device pool has been set by the application (don't change state)
		// otherwise, set the state back to registered which will trigger an upload of the
		// device pool
			if (en_poolState != OPNotRegistered)
				en_poolState = OPRegistered;
			en_uploadState = StateIdle;

      #ifndef USE_WORKING_SET
      #error "Need to define USE_WORKING_SET when utilizing a TaskController-Client."
      #endif
      mi32_timeWsAnnounceKey = pc_wsMasterIdentItem->getIsoItem()->startWsAnnounce();
    }

	// Notify all pool handlers of the change in TC alive state
		//STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it;
		//for (it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
		//	(*it)->eventTcAlive(b_tcAliveNew);
	}

// ### Do nothing if there's no TC alive ###
	if (!isTcAlive(i32_currentTime)) 
		return true;

// If our address claim is not complete, do nothing
	if (!getIsoMonitorInstance4Comm().existActiveLocalIsoMember())
		return true;

  /// Wait until 6.4.2.d) has finished (WS is completely announced)
  if (!pc_wsMasterIdentItem->getIsoItem()->isWsAnnounced (mi32_timeWsAnnounceKey)) return true;

  // @TODO improve this part ...
  static bool temp = false;
  if (!temp && isTcAlive(i32_currentTime))
  {
	  // Notify all pool handlers of the change in TC alive state
		STL_NAMESPACE::vector<DevicePoolHandler_c*>::iterator it;
		for (it = m_PoolHandlerList.begin(); it != m_PoolHandlerList.end(); it++)
			(*it)->eventTcAlive(b_tcAliveNew);
    temp = true;
  }

// Send the WS task message to maintain connection with the TC
	sendWorkingSetTaskMsg(i32_currentTime);

// If there is no device pool or the pool cannot be uploaded, do nothing
	if ((en_poolState == OPNotRegistered) || (en_poolState == OPCannotBeUploaded))
		return true;

//now TC is active, so send your msgs to TC
	switch(en_uploadStep)
	{
	case UploadUploading:
		if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
		{
			startUpload();
			break;
		}
		break;

// Waiting for request to transfer response
	case UploadWaitForOPTransferResponse:
		if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
			startUpload();
		break;

	case UploadFailed:
		ui8_commandParameter = procCmdPar_OPTransferMsg;
		ui32_uploadTimestamp = HAL::getTime();
		ui32_uploadTimeout = DEF_WaitFor_Reupload;
		break;

	case UploadNone:
	default:
		break;
	}
	return true;
}


bool
DevPropertyHandler_c::registerDevicePool(const IdentItem_c* rpc_wsMasterIdentItem, const HUGE_MEM uint8_t* /*rpc_devicePoolByteArray*/, const uint32_t /*rui32_bytestreamlength*/, bool /*rb_setToDefault*/)
{
	pc_wsMasterIdentItem = rpc_wsMasterIdentItem;

	en_poolState = OPRegistered;      // try to upload until state == UploadedSuccessfully || CannotBeUploaded
	return true;
}


//
// isTcAlive()
//	@return true if a TC status message has been received in the last six seconds
//
bool
DevPropertyHandler_c::isTcAlive (int32_t i32_currentTime)
{
	if ((i32_tcStateLastReceived != -1) && ( i32_currentTime - i32_tcStateLastReceived <= 6000))
		return true;

	return false;
}

void
DevPropertyHandler_c::sendWorkingSetTaskMsg(int32_t i32_currentTime)
{
// Ensure that we are on the bus...
	if (pc_wsMasterIdentItem == 0 || !pc_wsMasterIdentItem->isClaimedAddress())
		return;

// Don't send the working set meesage if the TC is gone
	if (!isTcAlive(HAL::getTime()))
		return;

// Send the WS task message every two seconds
	if (i32_currentTime - i32_timeWsTaskMsgSent >= 2000)
	{
		i32_timeWsTaskMsgSent = i32_currentTime;
		mc_data.setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
								0xFF, 0xFF, 0xFF, 0xFF, ui8_lastTcState, 0x00, 0x00, 0x00);

		getIsoBusInstance4Comm() << mc_data;
	}
}

/**
  local instances: call to check
  - address claim completed at least 6sec in the past
  - TC status message received
*/
void
DevPropertyHandler_c::checkInitState()
{
// Ensure that we are on the bus...
	if (pc_wsMasterIdentItem == 0 || !pc_wsMasterIdentItem->isClaimedAddress())
		return;

// Address claim is complete.  Set the time at which our address was claimed if it is
// still in an initialized state
	if (i32_timeStartWaitAfterAddrClaim == -1)
	{
		i32_timeStartWaitAfterAddrClaim = HAL::getTime();
		return;
	}

// init is finished when more then 6sec after addr claim and at least one TC status message was received
	if ( (HAL::getTime() - i32_timeStartWaitAfterAddrClaim >= 6000) && (i32_tcStateLastReceived != -1))
		b_initDone = true;
}

//
// send the request for object pool transfer
void
DevPropertyHandler_c::startUpload()
{
//estimate size of bytestream
	uint32_t ui32_byteStreamLength = (uint32_t)marr_uploadBuffer - 1;		// -1 to remove 0x61 opcode

	mc_data.setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
		procCmdPar_RequestOPTransferMsg,
		(uint8_t)((ui32_byteStreamLength      ) & 0xff),
		(uint8_t)((ui32_byteStreamLength >>  8) & 0xff),
		(uint8_t)((ui32_byteStreamLength >> 16) & 0xff),
		(uint8_t)((ui32_byteStreamLength >> 24) & 0xff),
		0xff, 0xff, 0xff);

	getIsoBusInstance4Comm() << mc_data;

	ui32_uploadTimestamp = HAL::getTime();
	ui32_uploadTimeout = DEF_TimeOut_OPTransfer;

	en_uploadState = StateBusy;
	en_uploadStep = UploadNone;
#ifdef DEBUG
	INTERNAL_DEBUG_DEVICE << "Wait for response for request OP transfer..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
}


/** if a pool couldn't be uploaded, register error in IsoTerminal and set the necessary states
  */
void
DevPropertyHandler_c::outOfMemory()
{
// can't (up)load the pool.
	getILibErrInstance().registerError( iLibErr_c::RemoteServiceOutOfMemory, iLibErr_c::TaskController );
	en_uploadStep = UploadFailed; // no timeout needed
	en_poolState = OPCannotBeUploaded;
#ifdef DEBUG
	INTERNAL_DEBUG_DEVICE << "upload failed, no retry" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  //men_uploadStep = UploadFailed; // no timeout needed
  //men_poolState = OPCannotBeUploaded;
}


//  NOT IMPLEMENTED
// put a new change designator command in the send-queue   
//    @return true, if command was queued
//
bool
DevPropertyHandler_c::sendCommandChangeDesignator(uint16_t /*rui16_objectID*/, const char* /*rpc_newString*/, uint8_t /*stringLength*/)
{
	return false;
}


// MultiSend_c implementation
void
DevPropertyHandler_c::setDataNextStreamPart(MultiSendPkg_c* mspData, uint8_t bytes)
{
	mspData->setDataPart (marr_uploadBuffer, ui16_currentSendPosition, bytes);
	ui16_currentSendPosition += bytes;
}


void
DevPropertyHandler_c::resetDataNextStreamPart()
{
  ui16_currentSendPosition = 0;
}


void
DevPropertyHandler_c::saveDataNextStreamPart()
{
  ui16_storedSendPosition = ui16_currentSendPosition;
}


void
DevPropertyHandler_c::restoreDataNextStreamPart()
{
  ui16_currentSendPosition = ui16_storedSendPosition;
}


uint32_t
DevPropertyHandler_c::getStreamSize()
{
	return m_DevicePoolToUploadSize;
}


uint8_t
DevPropertyHandler_c::getFirstByte()
{
  return marr_uploadBuffer[0];
}

void DevPropertyHandler_c::reactOnStateChange(const SendStream_c& sendStream)
{
	switch (sendStream.getSendSuccess())
	{
		case __IsoAgLib::SendStream_c::Running:
			break;
		case __IsoAgLib::SendStream_c::SendAborted:
		// aborted sending
			en_uploadState = StateIdle;
			en_uploadStep = UploadFailed;
#ifdef DEBUG
INTERNAL_DEBUG_DEVICE << "Upload failed, send aborted..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
			break;

	// when the pool has been sent successfully, wait on the transfer response
		case __IsoAgLib::SendStream_c::SendSuccess:
			en_uploadStep = UploadWaitForOPTransferResponse;
			ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
			break;
	}
}

};	// namespace __IsoAgLib

#endif
