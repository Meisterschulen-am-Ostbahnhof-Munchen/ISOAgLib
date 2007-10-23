/***************************************************************************
                            devpropertyhandler_c.h
                             -------------------
    begin                : Tue Jun 23 2005
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef DEVPROPERTYHANDLER_C_H
#define DEVPROPERTYHANDLER_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "processpkg_c.h"
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/Multipacket/imultisendstreamer_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>

#include <map>
#include <list>

namespace IsoAgLib { class iProcess_c;};

namespace __IsoAgLib {

class LanguageLabel_c {
  char str[2];
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


class MultiSendPkg_c;
class DevPropertyHandler_c : public IsoAgLib::iMultiSendStreamer_c
{

  public:
    DevPropertyHandler_c();
    virtual ~DevPropertyHandler_c()  {};

    /** initialisation for DevPropertyHandler_c */
    void init( ProcessPkg_c *apc_data);

    /** start processing of a process msg */
    bool processMsg();

    bool timeEvent( void );

    bool registerDevicePool (const IdentItem_c* apc_wsMasterIdentItem, const HUGE_MEM uint8_t* apc_devicePoolByteArray, const uint32_t aui32_bytestreamLength, bool b_setToDefault);
    bool sendCommandChangeDesignator(uint16_t apui16_objectID, const char* apc_newString, uint8_t stringLength);

    ProcessPkg_c& data(){return *pc_data;};

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

    void updateTcStateReceived(uint8_t aui8_lastTcState) { ui8_lastTcState = aui8_lastTcState; i32_tcStateLastReceived = HAL::getTime();};
    void setTcSourceAddress(uint8_t rtcSourceAddress) { tcSourceAddress = rtcSourceAddress;};

  private:
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

    uint8_t getTcSourceAddress () { return tcSourceAddress; };

    bool queuePoolInMap(const HUGE_MEM uint8_t* apc_devicePoolByteArray, uint32_t aui32_bytestreamlength, bool b_setToDefault);

    void initUploading();
    void getPoolForUpload();
    void outOfMemory();
    void startUpload();

    void startUploadCommandChangeDesignator();
    void finishUploadCommandChangeDesignator();

    int32_t i32_tcStateLastReceived;
    uint8_t ui8_lastTcState;
    int32_t i32_timeStartWaitAfterAddrClaim;
    bool b_initDone;
    int32_t i32_timeWsTaskMsgSent;

    bool b_setToDefault;
    bool tcAliveNew;

    bool b_receivedStructureLabel;
    bool b_receivedLocalizationLabel;

    ProcessPkg_c* pc_data;

    STL_NAMESPACE::map<LanguageLabel_c, DevicePool_c> map_deviceDescription; //map with language label and related bytestream

    uint8_t tcSourceAddress;

    uint8_t ui8_versionLabel;
    char pch_structureLabel[7];
    char pch_localizationLabel[7];

    DevicePool_c* pc_devDefaultDeviceDescription;

    DevicePool_c* pc_devPoolForUpload;

    /** has to be set using registerDevicePool (...) so that DevPropertyHandler_c
        can interact in the name of the wsMaster */
    const IdentItem_c* pc_wsMasterIdentItem;

    /** Upload-State & Variables */
    PoolState_t en_poolState;
    UploadState_t en_uploadState;
    UploadSteps_t en_uploadStep;
    UploadCommandState_t en_uploadCommand;

    uint32_t ui32_uploadTimestamp;
    uint32_t ui32_uploadTimeout;

    uint8_t ui8_uploadRetry;
    uint8_t ui8_commandParameter;

    STL_NAMESPACE::list<SendUploadBase_c>  l_sendUpload;

    MultiSend_c::sendSuccess_t en_sendSuccess;

    int32_t i32_timeWsAnnounceKey;
};

}
#endif
