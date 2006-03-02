/***************************************************************************
                          isoterminal_c.h  - central ISO terminal management
                             -------------------
    begin                : Wed Jul 09 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#ifndef ISO_TERMINAL_H
#define ISO_TERMINAL_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>



/// MOVE OUT TO "localsettings_s.h" !!!!!!!!
  /**
    struct that stores the Language and Units information
    received from the LANGUAGE_PGN
  */
  typedef struct localSettings_s {
    uint32_t lastReceived; /* Timestamp of last reception */
    uint32_t lastRequested;

    /* the following data is extracted from one "VT Status Message" */
    uint16_t languageCode; /* 2 ASCII chars packed into 16 bit! "en", "de", etc. */
    // Number Formats
    uint8_t  nDecimalPoint;
    uint8_t  nTimeFormat;
    // Date Formats
    uint8_t  dFormat;
    // Unit Formats
    uint8_t  uDistance;
    uint8_t  uArea;
    uint8_t  uVolume;
    uint8_t  uMass;
    uint8_t  uTemperature;
    uint8_t  uPressure;
    uint8_t  uForce;
    uint8_t  uUnitsSystem;
  } localSettings_s;



#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/comm/ISO_Terminal/impl/isoterminalpkg_c.h>
#include <IsoAgLib/comm/ISO_Terminal/iisoterminalobjectpool_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisendstreamer_c.h>
#include <IsoAgLib/comm/Multipacket/multireceiveclient_c.h>
#include <IsoAgLib/comm/Multipacket/impl/stream_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>


#define USE_LIST_FOR_FIFO

#ifdef USE_LIST_FOR_FIFO
  #include <list>
#else
  #include <queue>
#endif

#include <vector>


namespace IsoAgLib {
class iVtObjectString_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class SendUpload_c : public SendUploadBase_c{
public:
  /**
    StringUpload constructor that initializes all fields of this class (use only for Change String Value TP Commands)
  */
  SendUpload_c (vtObjectString_c* rpc_objectString);

  SendUpload_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout)
    : SendUploadBase_c( byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, rui32_timeout ), mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    {};
  SendUpload_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout)
    : SendUploadBase_c( byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, rui32_timeout ), mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    {};
  SendUpload_c (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte = 179 /*is standard case for VT Change String Value (TP)*/)
    : SendUploadBase_c( rui16_objId, rpc_string, overrideSendLength, ui8_cmdByte ), mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    {};
  SendUpload_c (const SendUpload_c& ref_source) : SendUploadBase_c(ref_source), mssObjectString(ref_source.mssObjectString) {};
  SendUpload_c (uint8_t* rpui8_buffer, uint32_t bufferSize)
    : SendUploadBase_c( rpui8_buffer, bufferSize ), mssObjectString(NULL)  /// Use BUFFER - NOT MultiSendStreamer!
    {};

  const SendUpload_c& operator= (const SendUpload_c& ref_source)
  { SendUploadBase_c::operator=(ref_source);mssObjectString= ref_source.mssObjectString;return ref_source;};

  /// Use either an MultiSendStreamer or a direct ui8-Buffer
  __IsoAgLib::vtObjectString_c* mssObjectString;
};


/** helper class for low level streaming.
  This function was excluded from ISOTerminal_c,
  as some STL aware compilers don't support multiple inheritance
  ( e.g. IAR ). So this helper construction was defined.
*/
class ISOTerminalStreamer_c : public MultiSendStreamer_c
{

public:
  virtual ~ISOTerminalStreamer_c(){};
  /** place next data to send direct into send puffer of pointed
      stream send package - MultiSendStreamer_c will send this
      puffer afterwards
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes);

  /** set cache for data source to stream start
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void resetDataNextStreamPart ();

  /** save current send position in data source - neeed for resend on send problem
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void saveDataNextStreamPart ();

  /** reactivate previously stored data source position - used for resend on problem
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  void restoreDataNextStreamPart ();

  /** calculate the size of the data source
      - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
    */
  uint32_t getStreamSize () { return ui32_streamSize; };

  uint8_t getFirstByte () { return 0x11; } // If ISOTerminal streams out, it's because of an Annex C. Object Pool Upload, so 0x11 can be returned ALWAYS!

  uint32_t ui32_objectStreamPosition;
  uint32_t ui32_objectStreamPositionStored;
  uint32_t ui32_streamSize;

  /** pointers needed by Scheduler_cMultiSendStreamer */
  IsoAgLib::iVtObject_c** pc_iterObjects;
  IsoAgLib::iVtObject_c** pc_iterObjectsStored;

  // the values of registerIsoObjectPool gets stored here
  IsoAgLib::iIsoTerminalObjectPool_c* pc_pool;
#define ISO_VT_UPLOAD_BUFFER_SIZE 128
  uint8_t uploadBuffer [ISO_VT_UPLOAD_BUFFER_SIZE];
  uint8_t uploadBufferFilled;
  uint8_t uploadBufferPosition;

  uint8_t uploadBufferStored [ISO_VT_UPLOAD_BUFFER_SIZE];
  uint8_t uploadBufferFilledStored;
  uint8_t uploadBufferPositionStored;
};

#if 0
/**
  obsolete class that stores an 8-byte command message along with its timeout
*/
class SendCommand_c {
public:
  /**
    default constructor that initializes all fields of this class
    @param byte1..byte8 the 8 data bytes
    @param rui32_timeout the timeout in milliseconds
  */
  SendCommand_c (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout);

  uint8_t arr_commandBuffer [8];
};
#endif

class ISOTerminal_c;
typedef SINGLETON_DERIVED(ISOTerminal_c,ElementBase_c) SingletonISOTerminal_c;
/**
  central IsoAgLib terminal management object
  */
class ISOTerminal_c : public SingletonISOTerminal_c , public IsoAgLib::MultiReceiveClient_c {
public:

  enum objectPoolState_t { OPNoneRegistered, OPRegistered, OPUploadedSuccessfully, OPCannotBeUploaded };

  enum uploadType_t { UploadIdle, UploadPool, UploadCommand };
  enum uploadPoolState_t { UploadPoolInit, UploadPoolWaitingForLoadVersionResponse, UploadPoolWaitingForMemoryResponse, UploadPoolUploading, UploadPoolWaitingForEOOResponse, UploadPoolWaitingForStoreVersionResponse, UploadPoolFailed}; /* completely uploaded is now detected by "OPUploadedSuccessfully" */
  enum uploadCommandState_t { UploadCommandWaitingForCommandResponse, UploadCommandTimedOut /*, UploadCommandFailed*/ };
  // UploadCommandFailed is obsolete, as we're not retrying and error-responses any more.


  /**
    struct that stores the "Get Number Of Soft Keys Response",
    "Get Text Font Data Response" and "Get Hardware Response"
  */
  typedef struct vtCapabilities_s {
    uint32_t lastReceivedHardware;
    uint32_t lastRequestedHardware;
    uint8_t  hwGraphicType; // 0, 1 or 2 (2, 16 or 256 colors)
    uint8_t  hwHardware; // bit 0: touchscreen, bit 1: pointing device, bit 2: multiple freuency audio, bit 3: adjustable audio volume
    uint16_t hwWidth;   // hwWidth and
    uint16_t hwHeight;  // hwHeight have to be the same (only square mask dimension!)

    uint32_t lastReceivedFont;
    uint32_t lastRequestedFont;
    uint16_t fontSizes;
    uint8_t  fontTypes;

    uint32_t lastReceivedSoftkeys;
    uint32_t lastRequestedSoftkeys;
    uint8_t  skPhysical;
    uint8_t  skVirtual;
    uint8_t  skWidth;
    uint8_t  skHeight;
  } vtCapabilities_s;


  virtual void reactOnAbort(IsoAgLib::iStream_c* rpc_stream);
  virtual bool reactOnStreamStart(IsoAgLib::ReceiveStreamIdentifier_c rc_ident, uint32_t rui32_totalLen);
  virtual bool processPartStreamDataChunk(IsoAgLib::iStream_c* rpc_stream, bool rb_isFirstChunk, bool rb_isLastChunk);

  /**
    default destructor, which initiate sending address release for all own identities
    @see MemberMonitor::~MemberMonitor
  */
  virtual ~ISOTerminal_c();

  /**
    initialise element which can't be done during construct and registerIsoObjectPool
    possible errors:
  */
  bool init(IdentItem_c* rpc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c* rpc_pool, char* rpc_versionLabel);

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

  /**
    periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );

  /**
    function that handles incoming can messages
  */
  virtual bool processMsg();

  /**
    register given object pool for uploading when possible.
    @param rpc_wsMasterIdentItem pointer to an IdentItem_c instance for that the ISO_Terminal acts
    @param rpc_pool pointer to a pool class instance
    @param rpc_versionLabel pointer to a 7-char name under which the object pool is loaded/stored (NULL for disabling non-volatile operation)
    @param rpc_vtEventHandler pointer to an instance of IsoAgLib::iIsoTerminalEventHandler_c so the hook funtions for key activation, input values and successfull objectpool upload can be invoked
  */
  bool registerIsoObjectPool (IdentItem_c* rpc_wsMasterIdentItem, IsoAgLib::iIsoTerminalObjectPool_c* rpc_pool, char* rpc_versionLabel);

  /**
    de-register registered object pool and versionLabel
  */
  bool deregisterIsoObjectPool ();

  /**
    deliver reference to data pkg
    @return reference to the member BasePkg_c, which encapsulates the CAN send structure
  */
  ISOTerminalPkg_c& data(){return c_data;};

  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase();


  ////////////////////////
  // INTERFACE FUNTIONS //
  ////////////////////////


  /**
    check if there's already been at least one vt_statusMessage in the last 3 seconds
    @return true if at least one vt_statusMessage - false if there's not yet been one or the last one is more than 3 seconds old
  */
  bool isVtActive ();

  /**
    retrieve terminal's source address.
    ATTENTION: check only if (isVtActive == true)
    @return uint8_t source address of virtual terminal
  */
// the following define should be globally defined in the project settings...
#ifdef FAKE_VT_PROPERTIES
  void fakeVtProperties (uint16_t rui16_dimension, uint16_t rui16_skWidth, uint16_t rui16_skHeight, uint8_t rui16_colorDepth, uint16_t rui16_fontSizes)
  {
    vtCapabilities_a.hwWidth = rui16_dimension;
    vtCapabilities_a.skWidth = rui16_skWidth;
    vtCapabilities_a.skHeight = rui16_skHeight;
    vtCapabilities_a.hwGraphicType = rui16_colorDepth;
    vtCapabilities_a.fontSizes = rui16_fontSizes;
  }
#endif
  uint8_t            getVtSourceAddress ()           { return vtSourceAddress; };
  uint16_t           getVtObjectPoolDimension ()     { return c_streamer.pc_pool->getDimension(); };
  uint16_t           getVtObjectPoolSoftKeyWidth ()  { return c_streamer.pc_pool->getSkWidth(); };
  uint16_t           getVtObjectPoolSoftKeyHeight () { return c_streamer.pc_pool->getSkHeight(); };
  uint32_t           getVtHardwareDimension ()       { return (uint32_t) (vtCapabilities_a.hwWidth); };
  uint16_t           getVtFontSizes ()               { return vtCapabilities_a.fontSizes; };
  vtCapabilities_s*  getVtCapabilities ()            { return &vtCapabilities_a; };
  const IsoAgLib::vtState_s* getVtState () const     { return &vtState_a; };
  localSettings_s*   getLocalSettings ()             { return &localSettings_a; };
  uint32_t           getUploadBufferSize ();
  uint8_t            getUserClippedColor (uint8_t colorValue, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour) {
                       uint8_t colorDepth = getVtCapabilities()->hwGraphicType;
                       if (((colorDepth == 0) && (colorValue > 1)) || ((colorDepth == 1) && (colorValue > 16))) {
                         return c_streamer.pc_pool->convertColour (colorValue, colorDepth, obj, whichColour);
                       } else {
                         return colorValue;
                       }
                     };

  /** sendCommand... methods */
  bool sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t ui32_timeout, bool b_enableReplaceOfCmd=true);
  bool sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t ui32_timeout, bool b_enableReplaceOfCmd=true);
  bool sendCommandForDEBUG (uint8_t* rpui8_buffer, uint32_t ui32_size);

  bool sendCommandChangeNumericValue (IsoAgLib::iVtObject_c* rpc_object, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeAttribute    (IsoAgLib::iVtObject_c* rpc_object, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeSoftKeyMask  (IsoAgLib::iVtObject_c* rpc_object, uint8_t maskType, uint16_t newSoftKeyMaskID, bool b_enableReplaceOfCmd=true);
  bool sendCommandChangeStringValue  (IsoAgLib::iVtObject_c* rpc_object, const char* rpc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd=true); // no response, no timeout... it's that simple...
  bool sendCommandChangeStringValue  (IsoAgLib::iVtObjectString_c* rpc_objectstring, bool b_enableReplaceOfCmd=true); // no response, no timeout... it's that simple...

  //  Operation: sendCommandChangeChildLocation
  //! Parameter:
  //! @param rpc_object:
  //! @param childObjectId:
  //! @param dx:
  //! @param dy:
  bool sendCommandChangeChildPosition (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t x, int16_t y, bool b_enableReplaceOfCmd=true);

// ADDED BY BRAD COX 26-AUG-2004 FOR CHANGE CHILD LOCATION COMMAND
  //  Operation: sendCommandChangeChildLocation
  //! Parameter:
  //! @param rpc_object:
  //! @param childObjectId:
  //! @param dx:
  //! @param dy:
  bool sendCommandChangeChildLocation (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd=true);
// ADDED BY BRAD COX 26-AUG-2004 FOR CHANGE BACKGROUND COLOUR COMMAND
  //  Operation: sendCommandChangeBackgroundColor
  //! Parameter:
  //! @param objectId:
  //! @param colorValue:
  bool sendCommandChangeBackgroundColour(IsoAgLib::iVtObject_c* rpc_object, uint8_t newColour, bool b_enableReplaceOfCmd=true);
// ADDED BY BRAD COX 26-AUG-2004 FOR CHANGE PRIORITY COMMAND
  //  Operation: sendCommandChangePriority
  //! Parameter:
  //! @param objectId:
  //! @param colorValue:
  bool sendCommandChangePriority(IsoAgLib::iVtObject_c* rpc_object, int8_t newPriority, bool b_enableReplaceOfCmd=true);
// ADDED BY BRAD COX 26-AUG-2004 FOR CHANGE SIZE COMMAND
  //  Operation: sendCommandChangeSize
  //! Parameter:
  //! @param rpc_object:
  //! @param newWidth:
  //! @param newHeight:
  bool sendCommandChangeSize(IsoAgLib::iVtObject_c* rpc_object,uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd=true);

  //  Operation: sendCommandChangeEndPoint
  //! Parameter:
  //! @param rpc_object:
  //! @param newWidth:
  //! @param newHeight:
  //! @param newLineDirection:
  bool sendCommandChangeEndPoint(IsoAgLib::iVtObject_c* rpc_object, uint16_t newWidth, uint16_t newHeight, uint8_t newLineDirection, bool b_enableReplaceOfCmd=true);


// ADDED BY BRAD COX 17-SEP-2004 TO IMPLEMENT CHANGE FILL ATTRIBUTES COMMAND
  //  Operation: sendCommandChangeSize
  //! Parameter:
  //! @param rpc_object:
  //! @param newFillType:
  //! @param newFillColour:
  //! @param newFillPatternObject:
  bool sendCommandChangeFillAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd=true);

  bool sendCommandChangeFontAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd=true);

  bool sendCommandChangeLineAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd=true);

  bool sendCommandDeleteObjectPool ();

  bool queueOrReplace(SendUpload_c& rref_sendUpload, bool b_enableReplaceOfCmd=true);

  void dumpQueue();

  void enableSameCommandCheck() { b_checkSameCommand = true; };
  void disableSameCommandCheck() { b_checkSameCommand = false; };

private:
  friend class SINGLETON_DERIVED(ISOTerminal_c,ElementBase_c);
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of ISOTerminal_c within application
    */
  ISOTerminal_c();

  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit();

  void doStart();
  void doStop();

  void checkVtStateChange();

  /** sends "Get Memory" to start uploading process... */
  void startObjectPoolUploading ();

  /** sets all states to successfull uploading and call the hook function! */
  void finalizeUploading ();

  /** send "End of Object Pool" message */
  void indicateObjectPoolCompletion ();

  bool startUploadCommand ();

  void finishUploadCommand ();

    /** sets state to "OPCannotUpload"... */
  void vtOutOfMemory();

private: // attributes

  /** temp data where received data is put */
  ISOTerminalPkg_c c_data;

  /** register is receive filters are already created */
  bool b_receiveFilterCreated;

  bool b_vtAliveCurrent;

  bool b_checkSameCommand;

  /** gets set as soon as the first "VT Status Message" gets received */
  uint8_t vtSourceAddress;

  /** stores the last "VT Status Message" */
  IsoAgLib::vtState_s vtState_a;

  /** gets set as soon as the responses for the requests arrive */
  vtCapabilities_s vtCapabilities_a;

  /** stores the Local Settings like Language, Units, Date Format */
  localSettings_s localSettings_a;

  /**
    has to be set using registerIsoObjectPool (...) so that IsoTerminal
    can interact in the name of the wsMaster
  */
  IdentItem_c* pc_wsMasterIdentItem;

  /**
    here the version number of the terminal gets stored as soon as the
    "VT Get Memory Response" was received
  */
  uint8_t iso11783version; // defaults to 0xFE for not yet initialized

  char* pc_versionLabel; // NULL if no Version Name is given


  /// General Object-Pool state (empty, loaded, etc.)
  objectPoolState_t en_objectPoolState;

  /**
    Upload-State & Variables
  */
  uploadType_t en_uploadType;
  uploadCommandState_t en_uploadCommandState; // state only used if en_uploadType == "UploadCommand"
  uploadPoolState_t en_uploadPoolState;       // state only used if en_uploadType == "UploadPool"

  uint32_t ui32_filterAckPGN; // if NOT 0, it was inserted, so it has to be deleted then at close...

  uint32_t ui32_uploadTimestamp;
  uint32_t ui32_uploadTimeout;

  uint8_t ui8_commandParameter; // this is kinda used as a cache only, because it's a four-case if-else to get the first byte!


  uint8_t ui8_uploadError;
  uint8_t ui8_uploadRetry;
  MultiSend_c::sendSuccess_t en_sendSuccess;

  uint16_t ui16_inputStringId;
  uint8_t ui8_inputStringLength;

  int32_t i32_nextWsMaintenanceMsg;

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

  ISOTerminalStreamer_c c_streamer;
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Scheduler_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  ISOTerminal_c& getIsoTerminalInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Scheduler_c singleton instance */
  ISOTerminal_c& getIsoTerminalInstance( void );
#endif

}
#endif
