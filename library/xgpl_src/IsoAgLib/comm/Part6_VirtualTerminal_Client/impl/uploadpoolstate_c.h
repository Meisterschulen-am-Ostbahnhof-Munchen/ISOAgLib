/*
  uploadpoolstate_c.h: 

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef UPLOADPOOLSTATE_C_H
#define UPLOADPOOLSTATE_C_H

#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/objectpoolstreamer_c.h>

namespace IsoAgLib { class iVtObject_c; }
namespace IsoAgLib { class iVtClientObjectPool_c; }

namespace __IsoAgLib {

  class Stream_c;
  class vtObject_c;
  class VtClientConnection_c;


  class UploadPoolState_c : public MultiSendEventHandler_c
  {
  public:
    enum UploadPoolType_t {
      UploadPoolTypeCompleteInitially,
      UploadPoolTypeLanguageUpdate,
      UploadPoolTypeUserPoolUpdate
    };
  
  private:
    enum UploadPoolState_t {
      UploadPoolInit,
      UploadPoolWaitingForVtVersionResponse,
      UploadPoolWaitingForGetVersionsResponse,
      UploadPoolWaitingForLoadVersionResponse,
      UploadPoolWaitingForMemoryResponse,
      UploadPoolUploading,
      UploadPoolWaitingForEOOResponse,
      UploadPoolWaitingForStoreVersionResponse,
      UploadPoolEndFailed,
      UploadPoolEndSuccess
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

  public: // functions
    UploadPoolState_c(
      VtClientConnection_c &,
      IsoAgLib::iVtClientObjectPool_c& ,
      const char *versionLabel,
      bool wsMaster );

    void initPool();

    void initObjectPoolUploadingPhases(
      UploadPoolType_t ren_uploadPoolType,
      IsoAgLib::iVtObject_c** rppc_listOfUserPoolUpdateObjects = NULL,
      uint16_t aui16_numOfUserPoolUpdateObjects = 0);
    void startCurrentUploadPhase();

    void processMsgVtToEcu( const CanPkgExt_c& pkg );
    void processMsgVtToEcu( Stream_c &stream );

    IsoAgLib::iVtClientObjectPool_c& getPool() const { return m_pool; }

    void notifyOnVtsLanguagePgn();
    void finalizeUploading();

    uint32_t fitTerminalWrapper( const vtObject_c& object ) const;
    bool dontUpload( const vtObject_c& object ) const;

    bool activeAuxN() const;
    bool activeAuxO() const;
    bool successfullyUploaded() const;

    const char *versionLabel() const { return( mb_usingVersionLabel ? marrp7c_versionLabel : NULL ); }

    void timeEvent();
    bool timeEventCalculateLanguage();
    void timeEventLanguageUpdate();

    void doStart();
    void doStop();

  private:
    bool isVersionFound( Stream_c& ) const;

    void handleGetVersionsResponse( Stream_c * );
    void handleEndOfObjectPoolResponse( bool success );
    void handleGetMemoryResponse( const CanPkgExt_c &pkg );
    void handleStoreVersionResponse( unsigned errorNibble );
    void handleLoadVersionResponse( unsigned errorNibble );

    void startUploadVersion();
    void startLoadVersion();
    void startGetVersions();
    void sendGetMemory( bool onlyRequestVersion );

    void timeEventUploadPoolTimeoutCheck();
    void timeEventRequestProperties();
    void timeEventPoolUpload();

    void indicateUploadPhaseCompletion();
    void indicateUploadCompletion(); // all phases completed.
    void setObjectPoolUploadingLanguage();

    void vtOutOfMemory();

    // MultiSendEventHandler_c
    virtual void reactOnStateChange( const SendStream_c& );

  private:
    VtClientConnection_c &m_connection;
    IsoAgLib::iVtClientObjectPool_c& m_pool;

    bool mb_usingVersionLabel; // if NOT using version label, "marrp7c_versionLabel" has random values!
    char marrp7c_versionLabel[ 7 ];

    uint8_t m_uploadingVersion; // if uploading a v3 client to a v2 VT (without Aux2), uploadingVersion will be v2
    ObjectPoolStreamer_c mc_iVtObjectStreamer;
    UploadPoolState_t men_uploadPoolState;
    UploadPoolType_t men_uploadPoolType;

    uint32_t mui32_uploadTimestamp;
    uint32_t mui32_uploadTimeout;

    UploadPhase_s ms_uploadPhasesAutomatic [UploadPhaseLAST+1]; // automatic pool upload with all needed parts (lang indep, lang dep)
    unsigned int mui_uploadPhaseAutomatic; // not of type "UploadPhase_t",
    // because we're doing arithmetics with it and can go out-of-bounds,
    // which results in undefined behavior (mostly only in -O2, so beware)

    UploadPhase_s ms_uploadPhaseUser; // user triggered upload phase...
    IsoAgLib::iVtObject_c** mppc_uploadPhaseUserObjects;

    int8_t mi8_objectPoolUploadingLanguage; // only valid if "initially uploading" or "language updating"
    int8_t mi8_objectPoolUploadedLanguage;  // only valid if "ObjectPoolUploadedSuccessfully"

    uint16_t mui16_objectPoolUploadingLanguageCode;
    uint16_t mui16_objectPoolUploadedLanguageCode;

    /// the following languages are
    /// -2: need to lookup language from VtServerInstance's language, if available
    /// -1: not supported language (==> so using default language for upload, but important to differentiate for the application!)
    ///  0: default language (first in \<workingset\>-object)
    ///  1: second language
    ///  2: third language
    int8_t mi8_vtLanguage; // always valid, as we're waiting for a VT's language first before starting anything...
  };


  inline void
  UploadPoolState_c::doStart()
  {
    mi8_vtLanguage = -2; // (re-)query LANGUAGE_PGN
    m_uploadingVersion = 0; // re-query version (needed for pool adaptation, e.g. omit Aux2 for v2 VTs)
  }


  inline void
  UploadPoolState_c::doStop()
  {
    men_uploadPoolState = UploadPoolInit;
  }


  inline void
  UploadPoolState_c::notifyOnVtsLanguagePgn()
  {
    mi8_vtLanguage = -2;
  }


  inline bool
  UploadPoolState_c::successfullyUploaded() const
  {
    return( men_uploadPoolState == UploadPoolEndSuccess );
  }


} // __IsoAgLib

#endif
