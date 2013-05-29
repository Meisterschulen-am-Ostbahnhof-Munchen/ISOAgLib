/*
  tcclientconnection_c.h: class for managing a connection

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TCCLIENTCONNECTION_H
#define TCCLIENTCONNECTION_H

#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>
#include <IsoAgLib/comm/Part3_DataLink/imultisendstreamer_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatahandler_c.h>


namespace IsoAgLib { 
  class iDevicePool_c;
  class iTcClientConnection_c;
  class iProcDataHandler_c;
}

namespace __IsoAgLib {
class IdentItem_c;
class TcClient_c;
class MultiSendPkg_c;
class ProcessPkg_c;
class ProcData_c;

class TcClientConnection_c : public CanCustomer_c
{
  #define DEF_TimeOut_GetVersion            5000
  #define DEF_TimeOut_OPTransfer            20000
  #define DEF_TimeOut_EndOfDevicePool       10000
  #define DEF_WaitFor_Reupload              5000
  #define DEF_TimeOut_ChangeDesignatorValue 1500
  #define DEF_TimeOut_NormalCommand         1500

public:
  enum DevPropertyStatus_t
  {
    StatusNoError = 0,
    StatusBusy,
    StatusNotInitted,
    StatusCannotProcess
  };

  enum DevPoolState_t 
  {
    PoolStateInit = 0, 
    PoolStatePresetting, 
    PoolStateNotPresent, 
    PoolStateStale, 
    PoolStateInvalid, 
    PoolStateUploaded, 
    PoolStateActive, 
    PoolStateError
  };

  enum UploadPoolState_t
  {
    DDOPRegistered,
    DDOPSuccessfullyUploaded,
    DDOPCannotBeUploaded
  };

  TcClientConnection_c(IdentItem_c&, TcClient_c&, IsoAgLib::iProcDataHandler_c&, IsoAgLib::iDevicePool_c& );
  virtual ~TcClientConnection_c();

  /** explicit conversion to reference of interface class type */
  IsoAgLib::iTcClientConnection_c& toInterfaceReference();
  /** explicit conversion to reference of interface class type */
  IsoAgLib::iTcClientConnection_c* toInterfacePointer();

  IdentItem_c& getIdentItem() const { return *m_identItem; }
  TcClient_c& getTcClient() const { return *m_tcClient; }

  void timeEvent();
  void checkAndHandleTcStateChange( const int32_t currentTime );
  void timeEventDevicePool();
  void resetTimerPeriod();

  bool isTcAlive ( int32_t currentTime );
  void sendWorkingSetTaskMsg( int32_t currentTime );
  void startUpload();

  void processMsgTc( const ProcessPkg_c& );

  void outOfMemory();

  bool sendCommandChangeDesignator( uint16_t /* objID */, const char* /* newString */, uint8_t /* length */ ) { return false; /* @todo: send msg */ }

  int32_t requestVersion() { return doCommand(procCmdPar_RequestVersionMsg, DEF_TimeOut_GetVersion); }
  int32_t requestStructureLabel() { return doCommand(procCmdPar_RequestStructureLabelMsg); }
  int32_t requestLocalizationLabel() { return doCommand(procCmdPar_RequestLocalizationLabelMsg); }
  int32_t requestPoolActivate() { return doCommand(procCmdPar_OPActivateMsg); }
  int32_t requestPoolDelete() { return doCommand(procCmdPar_OPDeleteMsg); }
  int32_t requestPoolTransfer( const STL_NAMESPACE::vector<uint8_t>& pool );

  void updateTcStateReceived( uint8_t lastTcState ) { m_lastTcState = lastTcState; m_lastTcStateReceivedTime = HAL::getTime(); }
  void setTcIsoItem ( IsoItem_c& isoItem ) { m_isoItemTC = &isoItem; }

  // ProcData_c handling
  ProcData_c* procData( uint16_t DDI, uint16_t element, const IsoName_c&, bool& elementFound ) const;
  void registerLocalProcessData( ProcData_c* localClient ) { registerC1( localClient ); }
  void unregisterLocalProcessData( ProcData_c* localClient ) { unregisterC1( localClient ); }
  void setProcDataHandler( IsoAgLib::iProcDataHandler_c *procDataHandler ) { m_procDataHandler = procDataHandler; }
  void processTcConnected( IsoAgLib::ProcData::RemoteType_t ecuType, const IsoAgLib::iIsoName_c& isoName )
    { isoaglib_assert(m_procDataHandler); m_procDataHandler->processTcConnected( ecuType, isoName ); }
  void processTaskStarted( IsoAgLib::ProcData::RemoteType_t ecuType )
    { isoaglib_assert(m_procDataHandler); m_procDataHandler->processTaskStarted( ecuType ); }
  void processTaskStopped( IsoAgLib::ProcData::RemoteType_t ecuType )
    { isoaglib_assert(m_procDataHandler); m_procDataHandler->processTaskStopped( ecuType ); }

  void stopRunningMeasurement( IsoAgLib::ProcData::RemoteType_t ecuType );
  void sendNack( const IsoName_c& da, const IsoName_c& sa, int16_t ddi, int16_t element, IsoAgLib::ProcData::NackResponse_t errorcodes ) const;
  void sendProcMsg( const IsoName_c&, const IsoName_c&, uint16_t ddi, uint16_t element, int32_t pdValue ) const;

  /** device pool handling */
  // Task Controller alive message
  virtual void eventTcAlive( bool isAlive );
  // Technical data message responses
  virtual void eventVersionResponse(uint8_t /* version */) {}
  // Device description message responses
  virtual void eventStructureLabelResponse( uint8_t result, const STL_NAMESPACE::vector<uint8_t>& label );
  virtual void eventLocalizationLabelResponse( uint8_t result, const STL_NAMESPACE::vector<uint8_t>& label );
  virtual void eventPoolUploadResponse( uint8_t result );
  virtual void eventPoolActivateResponse( uint8_t result );
  virtual void eventPoolDeleteResponse( uint8_t result );
  virtual void eventTimeout( uint8_t ) {}

  void setDevPoolState( DevPoolState_t newState ) { m_devPoolState = newState; }

  UploadPoolState_t getUploadPoolState() { return m_uploadPoolState; }

protected:
  int32_t doCommand( int32_t opcode, int32_t timeout = DEF_TimeOut_NormalCommand );

private:
  class MultiSendEventHandlerProxy_c : public MultiSendEventHandler_c 
  {
  public:
    typedef TcClientConnection_c Owner_t;

    MultiSendEventHandlerProxy_c(Owner_t &owner) : m_owner(owner) {}
    ~MultiSendEventHandlerProxy_c() {}

  private:
    void reactOnStateChange( const SendStream_c& sendStream )
      { m_owner.reactOnStateChange( sendStream ); }

    MultiSendEventHandlerProxy_c(MultiSendEventHandlerProxy_c const &); // make class non-copyable
    MultiSendEventHandlerProxy_c &operator=(MultiSendEventHandlerProxy_c const &); // make class non-copyable

    Owner_t &m_owner;
  }; // MultiSendEventHandlerProxy_c
  MultiSendEventHandlerProxy_c m_multiSendEventHandler;

  void reactOnStateChange( const SendStream_c& );

  class MultiSendStreamerProxy_c : public IsoAgLib::iMultiSendStreamer_c 
  {
  public:
    typedef TcClientConnection_c Owner_t;

    MultiSendStreamerProxy_c(Owner_t &owner) : m_owner(owner) {}
    ~MultiSendStreamerProxy_c() {}

  private:
    virtual void setDataNextStreamPart (__IsoAgLib::MultiSendPkg_c* data, uint8_t bytes)
      { m_owner.setDataNextStreamPart( data, bytes ); }

    virtual void resetDataNextStreamPart()
      { m_owner.resetDataNextStreamPart(); }

    virtual void saveDataNextStreamPart()
      { m_owner.saveDataNextStreamPart(); }

    virtual void restoreDataNextStreamPart()
      { m_owner.restoreDataNextStreamPart(); }

    virtual uint32_t getStreamSize()
      { return m_owner.getStreamSize(); }

    virtual uint8_t getFirstByte()
      { return m_owner.getFirstByte(); }

    MultiSendStreamerProxy_c(MultiSendStreamerProxy_c const &); // make class non-copyable
    MultiSendStreamerProxy_c &operator=(MultiSendStreamerProxy_c const &); // make class non-copyable

    Owner_t &m_owner;
  }; // MultiSendStreamerProxy_c
    
  MultiSendStreamerProxy_c m_multiSendStreamer;

  void setDataNextStreamPart(MultiSendPkg_c*, uint8_t );
  void resetDataNextStreamPart();
  void saveDataNextStreamPart();
  void restoreDataNextStreamPart();
  uint32_t getStreamSize();
  uint8_t getFirstByte();

  int getMultitonInst() const { return m_identItem->getMultitonInst(); }

  void sendMsg( const IsoName_c&, const IsoName_c&, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t ) const;

private:
  enum PoolAction_t
  {
    PoolActionIdle = 0,
    PoolActionWaiting,
    PoolActionTimeout
  };

  enum UploadState_t
  {
    StateIdle,
    StateBusy,
    StateTimeout
  };

  enum UploadSteps_t
  {
    UploadNone,
    UploadUploading,
    UploadWaitForRequestOPTransferResponse,
    UploadWaitForOPTransferResponse,
    UploadFailed
  };
  
  enum ProcessDataMsg_t
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

  IdentItem_c* m_identItem;
  int32_t m_timeWsAnnounceKey;
  TcClient_c* m_tcClient; // back ref.
  IsoAgLib::iProcDataHandler_c* m_procDataHandler;

  IsoItem_c* m_isoItemTC; // connected TC
  bool m_tcActive;

  bool m_receiveFilterCreated;

  // MultiSendStreamer_c variables
  uint16_t m_currentSendPosition;
  uint16_t m_storedSendPosition;
  STL_NAMESPACE::vector<uint8_t> m_devicePoolToUpload;
  
  // device pool upload variables
  UploadPoolState_t m_uploadPoolState;
  UploadState_t m_uploadState;
  UploadSteps_t m_uploadStep;
  uint32_t m_uploadTimestamp; // @todo This should be an int32_t.
  uint32_t m_uploadTimeout;
  uint8_t m_commandParameter;

  // timeEvent variables
  bool m_initDone;
  bool m_tcAliveNew;
  int32_t m_timeStartWaitAfterAddrClaim;
  int32_t m_lastTcStateReceivedTime;
  uint8_t m_lastTcState;
  int32_t m_timeWsTaskMsgSent;

  bool m_receivedStructureLabel;
  bool m_receivedLocalizationLabel;

  uint8_t m_versionLabel;
  STL_NAMESPACE::vector<uint8_t> m_structureLabel;
  STL_NAMESPACE::vector<uint8_t> m_localizationLabel;

  SendStream_c::sendSuccess_t m_sendSuccess;

  // device pool
  IsoAgLib::iDevicePool_c& m_pool; // back ref.
  DevPoolState_t m_devPoolState;
  PoolAction_t m_devPoolAction;

  CLASS_SCHEDULER_TASK_PROXY(TcClientConnection_c)
  SchedulerTaskProxy_c m_schedulerTaskProxy;

  CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(ProcData_c)
}; // TcClientConnection_c

} // __IsoAgLib
#endif
