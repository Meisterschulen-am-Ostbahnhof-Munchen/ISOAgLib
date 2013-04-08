/*
  vtclientconnection_c.h: class for managing a connection

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTCLIENTCONNECTION_H
#define VTCLIENTCONNECTION_H

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientobjectpool_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobject_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>

#include "../ivtobjectauxiliaryfunction2_c.h"
#include "vtobjectauxiliaryinput2_c.h"
#include "aux2inputs_c.h"
#include "aux2functions_c.h"
#include "objectpoolstreamer_c.h"
#include "sendupload_c.h"

#define USE_LIST_FOR_FIFO

#ifdef USE_LIST_FOR_FIFO
  #include <list>
#else
  #include <queue>
#endif


namespace IsoAgLib {
  class iVtObjectString_c;
  class iVtClientConnection_c;
  class iVtClientDataStorage_c;
}


namespace __IsoAgLib {
class VtServerInstance_c;
class VtClient_c;


class VtClientConnection_c : public CanCustomer_c
{
private:
  struct AuxAssignment_s
  {
    IsoName_c mc_inputIsoName;
    uint8_t mui8_inputNumber;
    uint16_t mui16_functionUid;
  };

  struct UploadPhase_s
  {
    UploadPhase_s() : pc_streamer (NULL), ui32_size (0) {}
    UploadPhase_s (IsoAgLib::iMultiSendStreamer_c* apc_streamer, uint32_t aui32_size) : pc_streamer (apc_streamer), ui32_size(aui32_size) {}

    IsoAgLib::iMultiSendStreamer_c* pc_streamer;
    uint32_t ui32_size;
  };

  enum UploadPhase_t {
    UploadPhaseFIRSTfix = 0,
    UploadPhaseFIRSTlang = 1,
    UploadPhaseIVtObjectsFix = 0,
    UploadPhaseIVtObjectsLang = 1,
    UploadPhaseAppSpecificFix = 2,
    UploadPhaseAppSpecificLang = 3,
    UploadPhaseLAST = 3
  };

public:
  enum objectPoolState_t {
    OPInitial,
    OPUploadedSuccessfully,
    OPCannotBeUploaded
  };

  enum uploadType_t {
    UploadIdle,
    UploadPool,
    UploadCommand
  };

  enum uploadPoolState_t {
    UploadPoolInit,
    UploadPoolWaitingForGetVersionsResponse,
    UploadPoolWaitingForLoadVersionResponse,
    UploadPoolWaitingForMemoryResponse,
    UploadPoolUploading,
    UploadPoolWaitingForEOOResponse,
    UploadPoolWaitingForStoreVersionResponse,
    UploadPoolFailed
  }; /* completely uploaded is now detected by "OPUploadedSuccessfully" */

  enum uploadCommandState_t {
    UploadCommandWithAwaitingResponse,
    UploadCommandPartialPoolUpdate // for e.g. user/language reasons
  };

  enum uploadPoolType_t {
    UploadPoolTypeCompleteInitially,
    UploadPoolTypeLanguageUpdate,
    UploadPoolTypeUserPoolUpdate
  };

  enum vtClientDisplayState_t {
    VtClientDisplayStateHidden,   // not on the display at all!
  //VtClientDisplayStateInactive, // on the display, but not the Active Working Set (Version 4)
    VtClientDisplayStateActive    // on the display and the Active Working Set
  };

  // UploadCommandFailed is obsolete, as we're not retrying and error-responses any more.
  // UploadCommandResponseless is used for cmd:0x11 Object Pool Transfer, as there's NO response sent from the VT and it's a "special" upload...

  virtual void reactOnAbort (Stream_c& apc_stream);
  virtual bool reactOnStreamStart (const ReceiveStreamIdentifier_c& ac_ident, uint32_t aui32_totalLen);
  virtual bool processPartStreamDataChunk (Stream_c& apc_stream, bool ab_isFirstChunk, bool ab_isLastChunk);

  /** constructor of VtClientConnection_c
   */
  VtClientConnection_c(
    IdentItem_c& wsMasterIdentItem,
    VtClient_c& isoTerminal,
    IsoAgLib::iVtClientObjectPool_c& pool,
    const char* versionLabel,
    IsoAgLib::iVtClientDataStorage_c& dataStorage,
    uint8_t clientId,
    IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_en mode);

  virtual ~VtClientConnection_c();

  /** explicit conversion to reference of interface class type */
  IsoAgLib::iVtClientConnection_c& toInterfaceReference();
  /** explicit conversion to reference of interface class type */
  IsoAgLib::iVtClientConnection_c* toInterfacePointer();

  void timeEvent();
  void timeEventUploadPoolTimeoutCheck();
  void timeEventPrePoolUpload();
  bool timeEventPoolUpload();

  void processMsgVtToEcu( const CanPkgExt_c& c_data );
  void processMsgAck( const CanPkgExt_c& arc_data );

  void notifyOnVtsLanguagePgn();
  void notifyOnVtStatusMessage();
  void notifyOnAuxInputStatus( const CanPkgExt_c& arc_data );
  void notifyOnAux2InputStatus( const CanPkgExt_c& arc_data );
  void notifyOnAux2InputMaintenance( const CanPkgExt_c& arc_data);

#ifdef USE_VTOBJECT_auxiliaryinput2
  void triggerAux2InputStatusMsg(vtObjectAuxiliaryInput2_c* a_aux2InputObj) { m_aux2Inputs.timeEventInputStateMsg(a_aux2InputObj); }
#endif

  uint16_t getVtObjectPoolDimension() const;
  uint16_t getVtObjectPoolSoftKeyWidth() const;
  uint16_t getVtObjectPoolSoftKeyHeight() const;
  unsigned getUploadBufferSize() const;
  uint8_t  getUserClippedColor (uint8_t colorValue, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour);
  uint8_t  getClientId() const { return mui8_clientId; }

  void sendCommandsToBus( bool commandsToBus ) { mb_commandsToBus = commandsToBus; }

  bool connectedToVtServer() const               { return (mpc_vtServerInstance != NULL); }
  /** ATTENTION: Please assure "connectedToVtServer()" before getting this reference */
  VtServerInstance_c& getVtServerInst() const    { return *mpc_vtServerInstance; }
  VtServerInstance_c* getVtServerInstPtr() const { return mpc_vtServerInstance; }
  IdentItem_c& getIdentItem() const              { return mrc_wsMasterIdentItem; }

  void notifyOnVtServerInstanceLoss (VtServerInstance_c& r_oldVtServerInst);

  /** sendCommand... methods */
  bool sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, bool b_enableReplaceOfCmd=true);
  bool sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, bool b_enableReplaceOfCmd=true, IsoAgLib::iVtObject_c** rppc_vtObjects=NULL, uint16_t aui16_numObjects=0);
  bool sendCommand (uint8_t* apui8_buffer, uint32_t ui32_size);

  /** sendCommand... methods with iVtObject_c parameters */
  bool sendCommandHideShow (IsoAgLib::iVtObject_c* apc_object, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd);
  bool sendCommandChangeNumericValue (IsoAgLib::iVtObject_c* apc_object, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeAttribute (IsoAgLib::iVtObject_c* apc_object, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSoftKeyMask (IsoAgLib::iVtObject_c* apc_object, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeStringValue (IsoAgLib::iVtObject_c* apc_object, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeActiveMask (IsoAgLib::iVtObject_c* apc_object, IsoAgLib::iVtObject_c* apc_mask, bool b_enableReplaceOfCmd=true );
  bool sendCommandChangeStringValue (IsoAgLib::iVtObjectString_c* apc_objectstring, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildPosition (IsoAgLib::iVtObject_c* apc_object, IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildLocation (IsoAgLib::iVtObject_c* apc_object, IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeBackgroundColour (IsoAgLib::iVtObject_c* apc_object, uint8_t newColour,  bool b_enableReplaceOfCmd=true);
  bool sendCommandChangePriority (IsoAgLib::iVtObject_c* apc_object, int8_t newPriority, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSize (IsoAgLib::iVtObject_c* apc_object, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeEndPoint (IsoAgLib::iVtObject_c* apc_object, uint16_t newWidth, uint16_t newHeight, uint8_t newLineDirection, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeFillAttributes (IsoAgLib::iVtObject_c* apc_object, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeFontAttributes (IsoAgLib::iVtObject_c* apc_object, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeLineAttributes (IsoAgLib::iVtObject_c* apc_object, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true);
  bool sendCommandLockUnlockMask( IsoAgLib::iVtObject_c* apc_object, bool b_lockMask, uint16_t ui16_lockTimeOut, bool b_enableReplaceOfCmd = true);

  /** version of the change command functions with UID value as parameter - works independent from iVtObject_c* pointer */
  bool sendCommandHideShow (uint16_t aui16_objectUid, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeNumericValue (uint16_t aui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeAttribute    (uint16_t aui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSoftKeyMask  (uint16_t aui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeActiveMask   (uint16_t aui16_objectUid, uint16_t maskId, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeStringValue  (uint16_t aui16_objectUid, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildPosition (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildLocation (uint16_t aui16_objectUid, uint16_t aui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeBackgroundColour (uint16_t aui16_objectUid, uint8_t newColour,  bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSize             (uint16_t aui16_objectUid, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeFillAttributes (uint16_t aui16_objectUid, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeFontAttributes (uint16_t aui16_objectUid, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeLineAttributes (uint16_t aui16_objectUid, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true);

#ifdef USE_ISO_TERMINAL_GRAPHICCONTEXT
  bool sendCommandSetGraphicsCursor (IsoAgLib::iVtObject_c* apc_object, int16_t ai16_x, int16_t ai16_y, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetForegroundColour (IsoAgLib::iVtObject_c* apc_object, uint8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetBackgroundColour (IsoAgLib::iVtObject_c* apc_object, uint8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetGCLineAttributes (IsoAgLib::iVtObject_c* apc_object, const IsoAgLib::iVtObjectLineAttributes_c* const newLineAttributes, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetGCFillAttributes (IsoAgLib::iVtObject_c* apc_object, const IsoAgLib::iVtObjectFillAttributes_c* const newFillAttributes, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetGCFontAttributes (IsoAgLib::iVtObject_c* apc_object, const IsoAgLib::iVtObjectFontAttributes_c* const newFontAttributes, bool b_enableReplaceOfCmd=true);
  bool sendCommandEraseRectangle (IsoAgLib::iVtObject_c* apc_object, int16_t ai16_x, int16_t ai16_y, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawPoint (IsoAgLib::iVtObject_c* apc_object, bool  b_enableReplaceOfCmd=true);
  bool sendCommandDrawLine (IsoAgLib::iVtObject_c* apc_object, int16_t ai16_x, int16_t ai16_y, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawRectangle (IsoAgLib::iVtObject_c* apc_object, int16_t ai16_x, int16_t ai16_y, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawClosedEllipse (IsoAgLib::iVtObject_c* apc_object, int16_t ai16_x, int16_t ai16_y, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawPolygon (IsoAgLib::iVtObject_c* apc_object, uint16_t ui16_numOfPoints, const int16_t* api16_x, const int16_t* api16_y, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawText (IsoAgLib::iVtObject_c* apc_object, uint8_t ui8_textType, uint8_t ui8_numOfCharacters, const char *apc_newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandPanViewport (IsoAgLib::iVtObject_c* apc_object, int16_t ai16_x, int16_t ai16_y, bool b_enableReplaceOfCmd=true);
  bool sendCommandZoomViewport (IsoAgLib::iVtObject_c* apc_object, int8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandPanAndZoomViewport (IsoAgLib::iVtObject_c* apc_object, int16_t ai16_x, int16_t ai16_y, int8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeViewportSize (IsoAgLib::iVtObject_c* apc_object, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawVtObject (IsoAgLib::iVtObject_c* apc_object, const IsoAgLib::iVtObject_c* const pc_VtObject, bool b_enableReplaceOfCmd=true);
  bool sendCommandCopyCanvas2PictureGraphic (IsoAgLib::iVtObject_c* apc_object, const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic, bool b_enableReplaceOfCmd=true);
  bool sendCommandCopyViewport2PictureGraphic (IsoAgLib::iVtObject_c* apc_object, const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic, bool b_enableReplaceOfCmd=true);
#endif
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  bool sendCommandGetAttributeValue (IsoAgLib::iVtObject_c* apc_object, const uint8_t cui8_attrID, bool b_enableReplaceOfCmd=true);
#endif

  bool sendCommandControlAudioDevice (uint8_t aui8_repetitions, uint16_t aui16_frequency, uint16_t aui16_onTime, uint16_t aui16_offTime);
  bool sendCommandSetAudioVolume (uint8_t aui8_volume);
  bool sendCommandEsc (bool b_enableReplaceOfCmd=true);
  bool sendCommandDeleteObjectPool();
  bool sendCommandUpdateLanguagePool();
  bool sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t aui16_numObjects);

  bool sendNonVolatileDeleteVersion( const char* versionLabel7chars );

  bool queueOrReplace (SendUpload_c& ar_sendUpload, bool b_enableReplaceOfCmd=true);
  void dumpQueue();

  void enableSameCommandCheck() { mb_checkSameCommand = true; }
  void disableSameCommandCheck() { mb_checkSameCommand = false; }

  bool isClientActive() const { return getVtDisplayState() == VtClientDisplayStateActive; }
  bool isVtActive() const;
  vtClientDisplayState_t getVtDisplayState() const { return men_displayState; }

  IsoAgLib::iVtClientObjectPool_c& getPool() const {
    return mrc_pool;
  }
  IsoAgLib::iVtClientDataStorage_c& getVtClientDataStorage() const {
    return m_dataStorageHandler;
  }

  int getMultitonInst() { return mrc_wsMasterIdentItem.getMultitonInst(); }

private:
  class MultiSendEventHandlerProxy_c : public MultiSendEventHandler_c 
  {
  public:
    typedef VtClientConnection_c Owner_t;

    MultiSendEventHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}
    ~MultiSendEventHandlerProxy_c() {}

  private:
    void reactOnStateChange(const SendStream_c& sendStream)
      { mrt_owner.reactOnStateChange(sendStream); }

    MultiSendEventHandlerProxy_c(MultiSendEventHandlerProxy_c const &); // make class non-copyable
    MultiSendEventHandlerProxy_c &operator=(MultiSendEventHandlerProxy_c const &); // make class non-copyable

    Owner_t &mrt_owner;
  }; // MultiSendEventHandlerProxy_c
  MultiSendEventHandlerProxy_c mt_multiSendEventHandler;

  void reactOnStateChange(const SendStream_c& sendStream);

private:
  friend class VtClient_c;

  void timeEventSearchForNewVt();

  //! @return true for successful assignment, false if SA couldn't be found.
  bool storeAuxAssignment( const CanPkgExt_c& arc_data );

  //! @return error-code from
  //! @return rui16_functionObjId
  uint8_t storeAux2Assignment( Stream_c& arc_stream, uint16_t& rui16_functionObjId );

  void doStart();
  void doStop();

  //! @param ai32_fakeOffTimeDurationMs The number of ms the VT is faked
  //!                                   from now on to be offline.
  void fakeVtOffPeriod (int32_t ai32_fakeOffTimeDurationMs) { mi32_fakeVtOffUntil = HAL::getTime() + ai32_fakeOffTimeDurationMs; }
  void fakeVtOffStop() { mi32_fakeVtOffUntil = -1; }
  void checkAndHandleVtStateChange();

  void startCurrentUploadPhase();
  void indicateUploadPhaseCompletion();
  void indicateUploadCompletion(); // all phases completed.
  void checkPoolPhaseRunningMultiSend(); // check for abort/retry when upload a partial object pool

  // Send out Get Memory command and move on state engine to expect a Get Memory Response
  void sendGetMemory();

  void initObjectPoolUploadingPhases (uploadPoolType_t ren_uploadPoolType, IsoAgLib::iVtObject_c** rppc_listOfUserPoolUpdateObjects=NULL, uint16_t aui16_numOfUserPoolUpdateObjects=0);
  /** sets all states to successfull uploading and call the hook function! */
  void finalizeUploading();

  bool startUploadCommand();
  void finishUploadCommand();
  /** sets state to "OPCannotUpload"... */
  void vtOutOfMemory();
  void setObjectPoolUploadingLanguage();

  void sendMessage( uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 );

  /** set display state of vt client (from VT Status Message) */
  void setVtDisplayState( uint8_t ui8_sa );

  bool isPreferredVTTimeOut() const;

  bool isVersionFound(Stream_c& arc_stream) const;
  void startUploadVersion();
  void startLoadVersion();

private: // attributes
  /** static instance to store temporarily before push_back into list */
  static SendUpload_c msc_tempSendUpload;
  IsoAgLib::iVtClientObjectPool_c& mrc_pool;

  bool mb_vtAliveCurrent;
  bool mb_checkSameCommand;

  IdentItem_c& mrc_wsMasterIdentItem;
  VtClient_c& mrc_vtClient; // back ref.
  VtServerInstance_c* mpc_vtServerInstance;

  bool mb_usingVersionLabel; // if NOT using version label, "marrp7c_versionLabel" has random values!
  char marrp7c_versionLabel[ 7 ];

  /// General Object-Pool state (empty, loaded, etc.)
  objectPoolState_t men_objectPoolState;

  /// the following languages are
  /// -2: need to lookup language from VtServerInstance's language, if available
  /// -1: not supported language (==> so using default language for upload, but important to differentiate for the application!)
  ///  0: default language (first in \<workingset\>-object)
  ///  1: second language
  ///  2: third language
  int8_t mi8_vtLanguage; // always valid, as we're waiting for a VT's language first before starting anything...

  /** Upload-State & Variables */
  uploadType_t men_uploadType;
  uploadCommandState_t men_uploadCommandState; // state only used if men_uploadType == "UploadCommand"
  uploadPoolState_t men_uploadPoolState;       // state only used if men_uploadType == "UploadPool"
  uploadPoolType_t men_uploadPoolType;

  UploadPhase_s ms_uploadPhasesAutomatic [UploadPhaseLAST+1]; // automatic pool upload with all needed parts (lang indep, lang dep)
  unsigned int mui_uploadPhaseAutomatic; // not of type "UploadPhase_t",
  // because we're doing arithmetics with it and can go out-of-bounds,
  // which results in undefined behavior (mostly only in -O2, so beware)

  UploadPhase_s ms_uploadPhaseUser; // user triggered upload phase...
  IsoAgLib::iVtObject_c** mppc_uploadPhaseUserObjects;

public: // for iVtObjectStreamer - quick hack due to no multiple-inheritance (IAR-compiler)
  int8_t mi8_objectPoolUploadingLanguage; // only valid if "initially uploading" or "language updating"
  int8_t mi8_objectPoolUploadedLanguage;  // only valid if "ObjectPoolUploadedSuccessfully"

  uint16_t mui16_objectPoolUploadingLanguageCode;
  uint16_t mui16_objectPoolUploadedLanguageCode;

private:
  uint32_t mui32_uploadTimestamp;
  uint32_t mui32_uploadTimeout;

  uint8_t mui8_commandParameter; // this is kinda used as a cache only, because it's a four-case if-else to get the first byte!
  uint8_t mui8_uploadError;

  SendStream_c::sendSuccess_t men_sendSuccess;

  uint16_t mui16_inputStringId;
  uint8_t mui8_inputStringLength;
  int32_t mi32_nextWsMaintenanceMsg;

  bool mb_receiveFilterCreated;

  uint8_t mui8_clientId;

  vtClientDisplayState_t men_displayState;

  #ifdef USE_LIST_FOR_FIFO
  // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
  // as each SendCommand_c item is just 16Byte large, and an application
  // can require a lot of items in the list ( examples need up to 150 items especially
  // during init ), CHUNK Allocation is the strategy of choice
  // Malloc_Alloc would cause too much overhead
  // Numbers for 145 items: Malloc_Alloc: 3480Bytes; Chunk_Alloc: 2568Byte -> 912Byte fewer with Chunk Alloc
  // ( single instance allocation can also cause time problems and could result in heavy
  //   memory fragmentation ==>> here CHUNK Alloc is the only choice )
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  STL_NAMESPACE::list<SendUpload_c,MALLOC_TEMPLATE(SendUpload_c) >  mq_sendUpload;
  #else
  STL_NAMESPACE::list<SendUpload_c>  mq_sendUpload;
  #endif
  #else
  STL_NAMESPACE::queue<SendUpload_c> mq_sendUpload;
  #endif

  STL_NAMESPACE::list<AuxAssignment_s> mlist_auxAssignments;

  Aux2Inputs_c m_aux2Inputs;
  Aux2Functions_c m_aux2Functions;

  ObjectPoolStreamer_c mc_iVtObjectStreamer;

  int32_t mi32_timeWsAnnounceKey;
  int32_t mi32_fakeVtOffUntil;

  IsoAgLib::iVtClientObjectPool_c::RegisterPoolMode_en men_registerPoolMode;

  bool mb_commandsToBus;

  IsoName_c mc_preferredVt;
  int32_t mi32_bootTime_ms;

  IsoAgLib::iVtClientDataStorage_c& m_dataStorageHandler; 

  CLASS_SCHEDULER_TASK_PROXY(VtClientConnection_c)

  SchedulerTaskProxy_c m_schedulerTaskProxy;
};


inline bool VtClientConnection_c::sendCommandChangeNumericValue (IsoAgLib::iVtObject_c* apc_object, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{ return sendCommandChangeNumericValue(apc_object->getID(), byte1, byte2, byte3, byte4, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeAttribute    (IsoAgLib::iVtObject_c* apc_object, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{ return sendCommandChangeAttribute(apc_object->getID(), attrId, byte1, byte2, byte3, byte4, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeSoftKeyMask  (IsoAgLib::iVtObject_c* apc_object, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd)
{ return sendCommandChangeSoftKeyMask(apc_object->getID(), maskType, newSoftKeyMaskID, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeStringValue (IsoAgLib::iVtObject_c* apc_object, const char* apc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd)
{ return sendCommandChangeStringValue(apc_object->getID(), apc_newValue, overrideSendLength, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeActiveMask (IsoAgLib::iVtObject_c* apc_object, IsoAgLib::iVtObject_c* apc_mask, bool b_enableReplaceOfCmd)
{ return sendCommandChangeActiveMask( apc_object->getID(), apc_mask->getID(), b_enableReplaceOfCmd ); }

inline bool VtClientConnection_c::sendCommandChangeChildPosition (IsoAgLib::iVtObject_c* apc_object, IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_enableReplaceOfCmd)
{ return sendCommandChangeChildPosition(apc_object->getID(), apc_childObject->getID(), x, y, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeChildLocation (IsoAgLib::iVtObject_c* apc_object, IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd)
{ return sendCommandChangeChildLocation(apc_object->getID(), apc_childObject->getID(), dx, dy, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeBackgroundColour (IsoAgLib::iVtObject_c* apc_object, uint8_t newColour,  bool b_enableReplaceOfCmd)
{ return sendCommandChangeBackgroundColour(apc_object->getID(), newColour, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeSize (IsoAgLib::iVtObject_c* apc_object, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd)
{ return sendCommandChangeSize(apc_object->getID(), newWidth, newHeight, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeFillAttributes (IsoAgLib::iVtObject_c* apc_object, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd)
{ return sendCommandChangeFillAttributes(apc_object->getID(), newFillType, newFillColour, newFillPatternObject, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeFontAttributes (IsoAgLib::iVtObject_c* apc_object, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd)
{ return sendCommandChangeFontAttributes( apc_object->getID(), newFontColour, newFontSize, newFontType, newFontStyle, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandChangeLineAttributes (IsoAgLib::iVtObject_c* apc_object, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd)
{ return sendCommandChangeLineAttributes( apc_object->getID(), newLineColour, newLineWidth, newLineArt, b_enableReplaceOfCmd); }

inline bool VtClientConnection_c::sendCommandHideShow( IsoAgLib::iVtObject_c* apc_object, uint8_t b_hideOrShow, bool b_enableReplaceOfCmd)
{ return sendCommandHideShow(apc_object->getID(), b_hideOrShow, b_enableReplaceOfCmd); }

} // __IsoAgLib

#endif
