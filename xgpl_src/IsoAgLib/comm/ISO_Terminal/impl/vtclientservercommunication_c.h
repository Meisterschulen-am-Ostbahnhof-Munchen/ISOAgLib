/***************************************************************************
            vtclientservercommunication_c.h  - class for managing the
                                               communication between vt
                                               client and server
                             -------------------
    begin                : Wed Jul 05 2006
    copyright            : (C) 2006 by Dipl.-Inf.(FH) Martina Winkler
    email                : m.winkler@osb-ag:de
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
#ifndef VT_CLIENT_SERVER_COMMUNICATION_H
#define VT_CLIENT_SERVER_COMMUNICATION_H

#include <IsoAgLib/comm/ISO_Terminal/impl/isoterminalpkg_c.h>
#include <IsoAgLib/comm/ISO_Terminal/iisoterminalobjectpool_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#include <IsoAgLib/comm/Multipacket/imultisendstreamer_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>

#define USE_LIST_FOR_FIFO

#ifdef USE_LIST_FOR_FIFO
  #include <list>
#else
  #include <queue>
#endif

namespace IsoAgLib {
  class iVtObjectString_c;
  class iVtClientServerCommunication_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class SendUpload_c : public SendUploadBase_c
{
public:
  /** StringUpload constructor that initializes all fields of this class (use only for Change String Value TP Commands) */
  SendUpload_c() : SendUploadBase_c() {}

  SendUpload_c (vtObjectString_c* rpc_objectString)
    {set(rpc_objectString);}

  SendUpload_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout)
    : SendUploadBase_c( byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, rui32_timeout ), mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    , ppc_vtObjects (NULL)
    {}
  SendUpload_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout, IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t rui16_numObjects)
    : SendUploadBase_c( byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, rui32_timeout ), mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    , ppc_vtObjects (rppc_vtObjects)
    , ui16_numObjects (rui16_numObjects)
    {}
  SendUpload_c (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte = 179 /*is standard case for VT Change String Value (TP)*/)
    : SendUploadBase_c( rui16_objId, rpc_string, overrideSendLength, ui8_cmdByte ), mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    , ppc_vtObjects (NULL)
    {}
  SendUpload_c (uint8_t* rpui8_buffer, uint32_t bufferSize)
    : SendUploadBase_c (rpui8_buffer, bufferSize)
    , mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    , ppc_vtObjects (NULL)
    {}

  void set (vtObjectString_c* rpc_objectString);
  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout);
  void set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout, IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t rui16_numObjects);
  void set (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte = 179 /*is standard case for VT Change String Value (TP)*/);
  void set (uint8_t* rpui8_buffer, uint32_t bufferSize);

  SendUpload_c (const SendUpload_c& ref_source)
    : SendUploadBase_c(ref_source)
    , mssObjectString(ref_source.mssObjectString)
    , ppc_vtObjects (ref_source.ppc_vtObjects)
    , ui16_numObjects (ref_source.ui16_numObjects)
    {}

  const SendUpload_c& operator= (const SendUpload_c& ref_source)
  {
    SendUploadBase_c::operator=(ref_source);
    mssObjectString = ref_source.mssObjectString;
    ppc_vtObjects = ref_source.ppc_vtObjects;
    ui16_numObjects = ref_source.ui16_numObjects;
    return ref_source;
  }

  /// Use either an MultiSendStreamer or a direct ui8-Buffer
  __IsoAgLib::vtObjectString_c* mssObjectString;

  IsoAgLib::iVtObject_c** ppc_vtObjects;
  uint16_t ui16_numObjects; // don't care for if "ppc_vtObjects==NULL"
};


/** helper class for low level streaming.
  This function was excluded from ISOTerminal_c,
  as some STL aware compilers don't support multiple inheritance
  ( e.g. IAR ). So this helper construction was defined.
*/
class VtClientServerCommStreamer_c : public IsoAgLib::iMultiSendStreamer_c
{
public:
  VtClientServerCommStreamer_c(IsoAgLib::iIsoTerminalObjectPool_c& rrefc_pool) : refc_pool (rrefc_pool) {}

  virtual ~VtClientServerCommStreamer_c(){}
  /** place next data to send direct into send puffer of pointed
      stream send package - MultiSendStreamer_c will send this
      puffer afterwards
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes);

  /** set cache for data source to stream start
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void resetDataNextStreamPart();

  /** save current send position in data source - neeed for resend on send problem
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void saveDataNextStreamPart();

  /** reactivate previously stored data source position - used for resend on problem
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void restoreDataNextStreamPart();

  /** calculate the size of the data source
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  uint32_t getStreamSize() { return (ui32_streamSize != 0) ? ui32_streamSize : ui32_streamSizeLang; }

  uint8_t getFirstByte() { return 0x11; } // If ISOTerminal streams out, it's because of an Annex C. Object Pool Upload, so 0x11 can be returned ALWAYS!

  uint32_t ui32_objectStreamPosition;
  uint32_t ui32_objectStreamPositionStored;
  uint32_t ui32_streamSize;
  /// Following variable indicates if we're uploading TWO parts or not!
  uint32_t ui32_streamSizeLang; // if 0, there's only one upload. if != 0 and the first upload finishes, this value is copied to "ui32_streamSize" and this value is set to 0!
  /// Following variable indicated if we're uploading A USER-GIVEN SET OF OBJECTS (User triggered partial pool update)
  IsoAgLib::iVtObject_c** pc_userPoolUpdateObjects;

  /** pointers needed by MultiSendStreamer */
  IsoAgLib::iVtObject_c* HUGE_MEM * pc_iterObjects;
  IsoAgLib::iVtObject_c* HUGE_MEM * pc_iterObjectsStored;

  IsoAgLib::iIsoTerminalObjectPool_c& refc_pool;
#define ISO_VT_UPLOAD_BUFFER_SIZE 128
  uint8_t uploadBuffer [ISO_VT_UPLOAD_BUFFER_SIZE];
  uint8_t uploadBufferFilled;
  uint8_t uploadBufferPosition;

  uint8_t uploadBufferStored [ISO_VT_UPLOAD_BUFFER_SIZE];
  uint8_t uploadBufferFilledStored;
  uint8_t uploadBufferPositionStored;

  int8_t i8_objectPoolUploadingLanguage; // only valid if "initially uploading" or "language updating"
  int8_t i8_objectPoolUploadedLanguage;  // only valid if "ObjectPoolUploadedSuccessfully"

  uint16_t ui16_objectPoolUploadingLanguageCode;
  uint16_t ui16_objectPoolUploadedLanguageCode;
};

// forward declaration
class VtServerInstance_c;
class ISOTerminal_c;
/** class for managing the communication between vt client and server */
class VtClientServerCommunication_c : public CANCustomer_c
{
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
    UploadPoolWaitingForLoadVersionResponse,
    UploadPoolWaitingForMemoryResponse,
    UploadPoolUploading,
    UploadPoolWaitingForEOOResponse,
    UploadPoolWaitingForStoreVersionResponse,
    UploadPoolFailed
  }; /* completely uploaded is now detected by "OPUploadedSuccessfully" */

  enum uploadCommandState_t {
    UploadCommandWaitingForCommandResponse,
    UploadCommandTimedOut,
    UploadCommandWithoutResponse /*, UploadCommandFailed*/
  };

  enum uploadPoolType_t {
    UploadPoolTypeCompleteInitially,
    UploadPoolTypeLanguageUpdate,
    UploadPoolTypeUserPoolUpdate
  };

  // UploadCommandFailed is obsolete, as we're not retrying and error-responses any more.
  // UploadCommandResponseless is used for cmd:0x11 Object Pool Transfer, as there's NO response sent from the VT and it's a "special" upload...

  virtual void reactOnAbort (IsoAgLib::iStream_c& rpc_stream);
  virtual bool reactOnStreamStart (const IsoAgLib::ReceiveStreamIdentifier_c& rc_ident, uint32_t rui32_totalLen);
  virtual bool processPartStreamDataChunk (IsoAgLib::iStream_c& rpc_stream, bool rb_isFirstChunk, bool rb_isLastChunk);

  /** constructor of VtClientServerCommunication_c
   */
  VtClientServerCommunication_c (IdentItem_c& refc_wsMasterIdentItem, ISOTerminal_c &ref_isoTerminal, IsoAgLib::iIsoTerminalObjectPool_c& rrefc_pool, char* rpc_versionLabel, uint8_t ui8_clientId);


  /** explicit conversion to reference of interface class type */
  IsoAgLib::iVtClientServerCommunication_c& toInterfaceReference();
  /** explicit conversion to reference of interface class type */
  IsoAgLib::iVtClientServerCommunication_c* toInterfacePointer();

  /** default destructor, which initiate sending address release for all own identities
  @see VtClientServerCommunication_c::~VtClientServerCommunication_c
  */
  virtual ~VtClientServerCommunication_c();

  /** periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent();
  /** timeEvent sub-functions to get a better overview of the timeEvent main-function */
  void timeEventSendLanguagePGN();
  void timeEventUploadPoolTimeoutCheck();
  void timeEventPrePoolUpload();
  bool timeEventPoolUpload();

  /** function that handles incoming acknowledgement messages */
  bool processMsgAck();

  /** function that handles incoming language pgn */
  void notifyOnVtsLanguagePgn();

  /** function that handles incoming language pgn */
  void notifyOnVtStatusMessage() { c_streamer.refc_pool.eventVtStatusMsg(); }

  virtual bool processMsg();

  virtual CANPkgExt_c& dataBase() { return c_data; }

  uint16_t getVtObjectPoolDimension();
  uint16_t getVtObjectPoolSoftKeyWidth();
  uint16_t getVtObjectPoolSoftKeyHeight();
  uint32_t getUploadBufferSize();
  uint8_t  getUserClippedColor (uint8_t colorValue, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour);

  IdentItem_c& getIdentItem()            { return refc_wsMasterIdentItem; }
  VtServerInstance_c& getVtServerInst();
  VtServerInstance_c* getVtServerInstPtr() { return pc_vtServerInstance; }

  void notifyOnNewVtServerInstance  (VtServerInstance_c& ref_newVtServerInst);
  void notifyOnVtServerInstanceLoss (VtServerInstance_c& ref_oldVtServerInst);

  /** sendCommand... methods */
  bool sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t ui32_timeout, bool b_enableReplaceOfCmd=true);
  bool sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t ui32_timeout, bool b_enableReplaceOfCmd=true, IsoAgLib::iVtObject_c** rppc_vtObjects=NULL, uint16_t rui16_numObjects=0);
  bool sendCommandForDEBUG (uint8_t* rpui8_buffer, uint32_t ui32_size);

  bool sendCommandChangeNumericValue (IsoAgLib::iVtObject_c* rpc_object, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeAttribute    (IsoAgLib::iVtObject_c* rpc_object, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSoftKeyMask  (IsoAgLib::iVtObject_c* rpc_object, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeStringValue  (IsoAgLib::iVtObject_c* rpc_object, const char* rpc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true); // no response, no timeout... it's that simple...
  bool sendCommandChangeStringValue  (IsoAgLib::iVtObjectString_c* rpc_objectstring, bool b_enableReplaceOfCmd=true); // no response, no timeout... it's that simple...

  bool sendCommandChangeChildPosition (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildLocation (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true);

  bool sendCommandChangeBackgroundColour (IsoAgLib::iVtObject_c* rpc_object, uint8_t newColour,  bool b_enableReplaceOfCmd=true);
  bool sendCommandChangePriority         (IsoAgLib::iVtObject_c* rpc_object, int8_t newPriority, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSize             (IsoAgLib::iVtObject_c* rpc_object, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeEndPoint         (IsoAgLib::iVtObject_c* rpc_object, uint16_t newWidth, uint16_t newHeight, uint8_t newLineDirection, bool b_enableReplaceOfCmd=true);

  bool sendCommandChangeFillAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeFontAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeLineAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true);

  bool sendCommandControlAudioDevice (uint8_t rui8_repetitions, uint16_t rui16_frequency, uint16_t rui16_onTime, uint16_t rui16_offTime);
  bool sendCommandSetAudioVolume (uint8_t rui8_volume);

  /** version of the change command functions with UID value as parameter - works independent from iVtObject_c* pointer */
  bool sendCommandChangeNumericValue (uint16_t rui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeAttribute    (uint16_t rui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSoftKeyMask  (uint16_t rui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeStringValue  (uint16_t rui16_objectUid, const char* rpc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true); // no response, no timeout... it's that simple...
  bool sendCommandChangeChildPosition (uint16_t rui16_objectUid, uint16_t rui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeChildLocation (uint16_t rui16_objectUid, uint16_t rui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeBackgroundColour (uint16_t rui16_objectUid, uint8_t newColour,  bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSize             (uint16_t rui16_objectUid, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeFillAttributes (uint16_t rui16_objectUid, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeFontAttributes (uint16_t rui16_objectUid, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeLineAttributes (uint16_t rui16_objectUid, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true);



  bool sendCommandSetGraphicsCursor (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetForegroundColour (IsoAgLib::iVtObject_c* rpc_object, uint8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetBackgroundColour (IsoAgLib::iVtObject_c* rpc_object, uint8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetGCLineAttributes (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectLineAttributes_c* const newLineAttributes, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetGCFillAttributes (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectFillAttributes_c* const newFillAttributes, bool b_enableReplaceOfCmd=true);
  bool sendCommandSetGCFontAttributes (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectFontAttributes_c* const newFontAttributes, bool b_enableReplaceOfCmd=true);
  bool sendCommandEraseRectangle (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawPoint (IsoAgLib::iVtObject_c* rpc_object, bool  b_enableReplaceOfCmd=true);
  bool sendCommandDrawLine (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawRectangle (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawClosedEllipse (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawPolygon (IsoAgLib::iVtObject_c* rpc_object, uint16_t ui16_numOfPoints, const IsoAgLib::iVtPoint_c* const rpc_data, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawText (IsoAgLib::iVtObject_c* rpc_object, uint8_t ui8_textType, uint8_t ui8_numOfCharacters, const char *rpc_newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandPanViewport (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd=true);
  bool sendCommandZoomViewport (IsoAgLib::iVtObject_c* rpc_object, int8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandPanAndZoomViewport (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, int8_t newValue, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeViewportSize (IsoAgLib::iVtObject_c* rpc_object, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);
  bool sendCommandDrawVtObject (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObject_c* const pc_VtObject, bool b_enableReplaceOfCmd=true);
  bool sendCommandCopyCanvas2PictureGraphic (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic, bool b_enableReplaceOfCmd=true);
  bool sendCommandCopyViewport2PictureGraphic (IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObjectPictureGraphic, bool b_enableReplaceOfCmd=true);

  bool sendCommandGetAttributeValue (IsoAgLib::iVtObject_c* rpc_object, const uint8_t cui8_attrID, bool b_enableReplaceOfCmd=true);


  bool sendCommandDeleteObjectPool();
  bool sendCommandUpdateLanguagePool();
  bool sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t rui16_numObjects);

  bool queueOrReplace (SendUpload_c& rref_sendUpload, bool b_enableReplaceOfCmd=true);
  void dumpQueue();

  void enableSameCommandCheck() { b_checkSameCommand = true; }
  void disableSameCommandCheck() { b_checkSameCommand = false; }

  bool isVtActive();

private:
  friend class ISOTerminal_c;

  void doStart();
  void doStop();

  void checkVtStateChange();

  /** sends "Get Memory" to start uploading process... */
  void startObjectPoolUploading (uploadPoolType_t ren_uploadPoolType, IsoAgLib::iVtObject_c** rppc_listOfUserPoolUpdateObjects=NULL, uint16_t rui16_numOfUserPoolUpdateObjects=0);
  /** sets all states to successfull uploading and call the hook function! */
  void finalizeUploading();

  /** send "End of Object Pool" message */
  void indicateObjectPoolCompletion();
  bool startUploadCommand();
  void finishUploadCommand();
  /** sets state to "OPCannotUpload"... */
  void vtOutOfMemory();
  void setObjectPoolUploadingLanguage();

private: // attributes
  /** static instance to store temporarily before push_back into list */
  static SendUpload_c sc_tempSendUpload;
  bool b_vtAliveCurrent;
  bool b_checkSameCommand;

  /** has to be set using registerIsoObjectPool (...) so that IsoTerminal
    can interact in the name of the wsMaster
  */
  IdentItem_c& refc_wsMasterIdentItem;

  ISOTerminal_c& refc_isoTerminal; // back ref.

  VtServerInstance_c* pc_vtServerInstance;  // back p.

  bool b_usingVersionLabel; // if NOT using version label, "p7c_versionLabel" has random values!
  char p7c_versionLabel [7];

  /// General Object-Pool state (empty, loaded, etc.)
  objectPoolState_t en_objectPoolState;

  /// the following languages are
  /// -1: not supported language (==> so using default language for upload, but important to differentiate for the application!)
  ///  0: default language (first in <workingset>-object)
  ///  1: second language
  ///  2: third language
  int8_t i8_vtLanguage; // always valid, as we're waiting for a VT's language first before starting anything...

  /** Upload-State & Variables */
  uploadType_t en_uploadType;
  uploadCommandState_t en_uploadCommandState; // state only used if en_uploadType == "UploadCommand"
  uploadPoolState_t en_uploadPoolState;       // state only used if en_uploadType == "UploadPool"
  uploadPoolType_t en_uploadPoolType;

  uint32_t ui32_uploadTimestamp;
  uint32_t ui32_uploadTimeout;

  uint8_t ui8_commandParameter; // this is kinda used as a cache only, because it's a four-case if-else to get the first byte!
  uint8_t ui8_uploadError;
  uint8_t ui8_uploadRetry;

  MultiSend_c::sendSuccess_t en_sendSuccess;

  uint16_t ui16_inputStringId;
  uint8_t ui8_inputStringLength;
  int32_t i32_nextWsMaintenanceMsg;

  /** receive filters are already created */
  bool b_receiveFilterCreated;

  ISOTerminalPkg_c c_data;

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
  STL_NAMESPACE::list<SendUpload_c,STL_NAMESPACE::__malloc_alloc_template<0> >  q_sendUpload;
  #else
  STL_NAMESPACE::list<SendUpload_c>  q_sendUpload;
  #endif
  #else
  STL_NAMESPACE::queue<SendUpload_c> q_sendUpload;
  #endif

  VtClientServerCommStreamer_c c_streamer;
};

}
#endif
