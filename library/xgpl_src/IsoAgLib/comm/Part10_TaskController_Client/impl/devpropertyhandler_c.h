/*
  devpropertyhandler_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifdef PROPRIETARY_DEVPROPERTYHANDLER_H
  #define _devproperty_handler_header <PROPRIETARY_DEVPROPERTYHANDLER_H>
  #include _devproperty_handler_header
#else

#ifndef DEVPROPERTYHANDLER_C_H
#define DEVPROPERTYHANDLER_C_H

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/processpkg_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part3_DataLink/imultisendstreamer_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisendeventhandler_c.h>

#include <map>
#include <list>

namespace IsoAgLib { class iProcess_c;};

namespace __IsoAgLib {


class LanguageLabel_c {
  char str[2+1]; // 2 language characters + 0x00-termination
public:
  LanguageLabel_c (const HUGE_MEM char* label = "de");
  LanguageLabel_c (const LanguageLabel_c& ar_str);

  const LanguageLabel_c& operator=(const LanguageLabel_c& c_langLabel);
  const LanguageLabel_c& operator=(const HUGE_MEM uint8_t* ui8_str);
  bool operator<(const LanguageLabel_c& c_langLabel) const;
  bool operator==(const LanguageLabel_c& c_langLabel);
  const char* get() const { return str; }
};


class DevicePool_c {
public:
  const HUGE_MEM uint8_t* p_DevicePool;
  uint32_t devicePoolLength;

  DevicePool_c (const HUGE_MEM uint8_t* pcui8_bytestream, const uint32_t ui32_length);

  const DevicePool_c& operator=(const DevicePool_c& c_devicePool);
};


class SendUploadDevProp_c : public SendUploadBase_c
{
public:
  SendUploadDevProp_c() : SendUploadBase_c() {}

  SendUploadDevProp_c (uint16_t aui16_objId, const char* apc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte)
    { set(aui16_objId, apc_string, overrideSendLength, ui8_cmdByte); }

  void set (uint16_t aui16_objId, const char* apc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte);
};


class MultiSendPkg_c;
class DevPropertyHandler_c : public IsoAgLib::iMultiSendStreamer_c
{

  public:
    DevPropertyHandler_c();
    virtual ~DevPropertyHandler_c()  {};

    /** initialisation for DevPropertyHandler_c */
    void init( ProcessPkg_c *apc_data);

    /** start processing of a process msg */
    bool processMsg( ProcessPkg_c& arc_data );

    bool timeEvent( void );

    bool registerDevicePool (const IdentItem_c* apc_wsMasterIdentItem, const HUGE_MEM uint8_t* apc_devicePoolByteArray, const uint32_t aui32_bytestreamLength, bool mb_setToDefault);
    bool sendCommandChangeDesignator(uint16_t apui16_objectID, const char* apc_newString, uint8_t stringLength);

    /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards */
    virtual void setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes);

    /** set cache for data source to stream start */
    virtual void resetDataNextStreamPart();

    /** save current send position in data source - neeed for resend on send problem */
    virtual void saveDataNextStreamPart ();

    /** reactivate previously stored data source position - used for resend on problem */
    virtual void restoreDataNextStreamPart ();

    /** calculate the size of the data source */
    virtual uint32_t getStreamSize ();

    /** get the first byte, which is the COMMAND-byte and should be given back CONST! */
    virtual uint8_t getFirstByte ();

    void reset() { ui16_currentSendPosition = ui16_storedSendPosition = 0; };

    void updateTcStateReceived(uint8_t aui8_lastTcState) { mui8_lastTcState = aui8_lastTcState; mi32_tcStateLastReceived = HAL::getTime();};
    void setTcSourceAddress(uint8_t rtcSourceAddress) { mui8_tcSourceAddress = rtcSourceAddress;};

  private:
    class MultiSendEventHandlerProxy_c : public MultiSendEventHandler_c {
    public:
      typedef DevPropertyHandler_c Owner_t;

      MultiSendEventHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

      ~MultiSendEventHandlerProxy_c() {}

    private:
      void reactOnStateChange(const SendStream_c& sendStream)
      {
         mrt_owner.reactOnStateChange(sendStream);
      }

      // IsoRequestPgnHandlerProxy_c shall not be copyable. Otherwise
      // the reference to the containing object would become invalid.
      MultiSendEventHandlerProxy_c(MultiSendEventHandlerProxy_c const &);

      MultiSendEventHandlerProxy_c &operator=(MultiSendEventHandlerProxy_c const &);

      Owner_t &mrt_owner;
    }; // MultiSendEventHandlerProxy_c
    MultiSendEventHandlerProxy_c mt_multiSendEventHandler;

    void reactOnStateChange(const SendStream_c& sendStream);

  private:

    void sendPoolActivatieMsg();

    uint16_t ui16_currentSendPosition;
    uint16_t ui16_storedSendPosition;
    /** enum type for defining all message types for possible changes in ISO definitions ;) */
    enum en_processDataMsg {procCmdPar_RequestVersionMsg = 0x0,
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

    enum PoolState_t    { OPNotRegistered, OPRegistered, OPSuccessfullyUploaded, OPCannotBeUploaded };
    enum UploadState_t  { StatePresettings, StateUploadInit, StateUploadPool, StateUploadCommand, StateIdle };
    enum UploadSteps_t  { UploadNone,                             //state when nothing is to do
                          UploadStart,                            //reached after init()-call
                          UploadWaitForStructureLabelResponse,    //wait for structure label
                          UploadWaitForLocalizationLabelResponse, //wait for localization label
                          UploadWaitForUploadInit,                //wait for state UploadInit -> is reached when at least one pool is mapped as active pool
                          UploadWaitForDeleteResponse,            //before uploading a new pool we have to delete the old one
                          UploadWaitForVersionResponse,           //wait for version response
                          UploadRetryUpload,                      //"start"-state for re-upload after fail
                          UploadWaitForRequestOPTransferResponse, //wait upload enabling
                          UploadUploading,                        //OP is uploading
                          UploadWaitForOPTransferResponse,        //wait for end of pool
                          UploadWaitForOPActivateResponse,        //incate the pool complete and ready to use
                          UploadFailed                            //Upload failed
                        };
    enum UploadCommandState_t { UploadCommandWaitingForCommandResponse,           //wait for response to change designator
                                UploadMultiSendCommandWaitingForCommandResponse,  //wait for response to change designator
                                UploadCommandTimedOut                             //upload was timed out
                              };

    friend class IsoAgLib::iProcess_c;
    /** check if there's already been at least one tc_statusMessage in the last 3 seconds
        @return true if at least one tc_statusMessage - false if there's not yet been one or the last one is more than 3 seconds old */
    bool isTcAlive (int32_t i32_currentTime);

    void sendWorkingSetTaskMsg(int32_t i32_currentTime);

    /** local instances: call to check
      - address claim completed at least 6sec in the past
      - TC status message received
     */
    void checkInitState();

    uint8_t getTcSourceAddress () { return mui8_tcSourceAddress; };

    bool queuePoolInMap(const HUGE_MEM uint8_t* apc_devicePoolByteArray, uint32_t aui32_bytestreamlength, bool mb_setToDefault);

    void initUploading();
    void getPoolForUpload();
    void outOfMemory();
    void startUpload();

    void startUploadCommandChangeDesignator();
    void finishUploadCommandChangeDesignator();

    int32_t mi32_tcStateLastReceived;
    uint8_t mui8_lastTcState;
    int32_t mi32_timeStartWaitAfterAddrClaim;
    bool mb_initDone;
    int32_t mi32_timeWsTaskMsgSent;

    bool mb_setToDefault;
    bool mb_tcAliveNew;

    bool mb_receivedStructureLabel;
    bool mb_receivedLocalizationLabel;

    ProcessPkg_c* mpc_data;

    STL_NAMESPACE::map<LanguageLabel_c, DevicePool_c> mmap_deviceDescription; //map with language label and related bytestream

    uint8_t mui8_tcSourceAddress;

    uint8_t mui8_versionLabel;
    char marrpch_structureLabel[7];
    char marrpch_localizationLabel[7];

    DevicePool_c* mpc_devDefaultDeviceDescription;

    DevicePool_c* mpc_devPoolForUpload;

    /** has to be set using registerDevicePool (...) so that DevPropertyHandler_c
        can interact in the name of the wsMaster */
    const IdentItem_c* mpc_wsMasterIdentItem;

    /** Upload-State & Variables */
    PoolState_t men_poolState;
    UploadState_t men_uploadState;
    UploadSteps_t men_uploadStep;
    UploadCommandState_t men_uploadCommand;

    uint32_t mui32_uploadTimestamp;
    uint32_t mui32_uploadTimeout;

    uint8_t mui8_commandParameter;

    STL_NAMESPACE::list<SendUploadDevProp_c>  ml_sendUpload;

    SendStream_c::sendSuccess_t men_sendSuccess;

    int32_t mi32_timeWsAnnounceKey;
};

}
#endif

#endif
// endif for "#ifdef PROPRIETARY_DEVPROPERTYHANDLER_H" workaround

