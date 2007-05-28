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

// LOESCHE_POOL will send DeleteVersion INSTEAD of LoadVersion
//#define LOESCHE_POOL
//#define GET_VERSIONS
//#define USE_SIMPLE_AUX_RESPONSE


#include "vtclientservercommunication_c.h"
#include "../ivtclientservercommunication_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>
// #include <IsoAgLib/hal/system.h>
#include <IsoAgLib/comm/Multipacket/impl/multireceive_c.h>
// #include <IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h>
#include <supplementary_driver/driver/datastreams/volatilememory_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isofiltermanager_c.h>
// #include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
// #include "vttypes.h"
#include "../ivtobjectpicturegraphic_c.h"
#include "../ivtobjectgraphicscontext_c.h"
#include "../ivtobjectlineattributes_c.h"
#include "../ivtobjectfillattributes_c.h"
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectstring_c.h"
#include "../ivtobjectworkingset_c.h"



#if defined(DEBUG) || defined(DEBUG_HEAP_USEAGE)
  #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #include <IsoAgLib/util/impl/util_funcs.h>
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif


#ifdef DEBUG_HEAP_USEAGE
  static uint16_t sui16_lastPrintedBufferCapacity = 0;
  static uint16_t sui16_lastPrintedSendUploadQueueSize = 0;
  static uint16_t sui16_lastPrintedMaxSendUploadQueueSize = 0;
  static uint16_t sui16_sendUploadQueueSize = 0;
  static uint16_t sui16_maxSendUploadQueueSize = 0;
#endif

static const uint8_t scui8_cmdCompareTableMin = 0x92;
static const uint8_t scui8_cmdCompareTableMax = 0xBD;

/// this table is used to identify if a command can override an earlier command of same function
/// 1<<databyte to indicate which databytes to compaire to decide if command is replaced or not
static const uint8_t scpui8_cmdCompareTable[(scui8_cmdCompareTableMax-scui8_cmdCompareTableMin)+1] = {
/// (1<<0) means DO NOT OVERRIDE THESE COMMANDS AT ALL
/* 0x92 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND
/* 0x93 */ 0 , //invalid command
/* 0x94 */ 0 , //invalid command
/* 0x95 */ 0 , //invalid command
/* 0x96 */ 0 , //invalid command
/* 0x97 */ 0 , //invalid command
/* 0x98 */ 0 , //invalid command
/* 0x99 */ 0 , //invalid command
/* 0x9A */ 0 , //invalid command
/* 0x9B */ 0 , //invalid command
/* 0x9C */ 0 , //invalid command
/* 0x9D */ 0 , //invalid command
/* 0x9E */ 0 , //invalid command
/* 0x9F */ 0 , //invalid command
/* 0xA0 */ (1<<1) | (1<<2) ,
/* 0xA1 */ (1<<1) | (1<<2) ,
/* 0xA2 */ (1<<1) | (1<<2) ,
/* 0xA3 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Control Audio Device)
/* 0xA4 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Set Audio Volume)
/* 0xA5 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Change Child Location), as it's relative!!!
/* 0xA6 */ (1<<1) | (1<<2) ,
/* 0xA7 */ (1<<1) | (1<<2) ,
/* 0xA8 */ (1<<1) | (1<<2) , // Change Numeric Value (all has been done for THIS ONE originally ;-)
/* 0xA9 */ (1<<1) | (1<<2) ,
/* 0xAA */ (1<<1) | (1<<2) ,
/* 0xAB */ (1<<1) | (1<<2) ,
/* 0xAC */ (1<<1) | (1<<2) ,
/* 0xAD */ (1<<1) | (1<<2) , /** @todo Maybe change to delete in between and push_back new?! */ // (Change Active Mask)
/* 0xAE */ (1<<1) | (1<<2) | (1<<3) ,
/* 0xAF */ (1<<1) | (1<<2) | (1<<3) ,
/* 0xB0 */ (1<<1) | (1<<2) ,
/* 0xB1 */ (1<<1) | (1<<2) | (1<<3) ,
/* 0xB2 */ (1<<0) , //NEVER OVERRIDE THIS COMMAND (Delete Object Pool)
/* 0xB3 */ (1<<1) | (1<<2) ,
/* 0xB4 */ (1<<1) | (1<<2) | (1<<3) | (1<<4), // (Change Child Position)
/* 0xB5 */ 0 , //invalid command
/* 0xB6 */ 0 , //invalid command
/* 0xB7 */ 0 , //invalid command
/* 0xB8 */ (1<<0) ,  //NEVER OVERRIDE THIS COMMAND (Graphics Context)
/* 0xB9 */ (1<<1) | (1<<2) | (1<<3) | (1<<4), // (Get Attribute Value)
/* 0xBA */ 0, //invalid command
/* 0xBB */ 0, //invalid command
/* 0xBC */ 0, //invalid command
/* 0xBD */ (1<<0) //NEVER OVERRIDE THIS COMMAND (Lock/Unlock Mask)
};


namespace __IsoAgLib {


void SendUpload_c::set (vtObjectString_c* rpc_objectString)
{
  ppc_vtObjects = NULL;

  mssObjectString = rpc_objectString;

  if (mssObjectString->getStreamer()->getStreamSize() < 9)
    ui8_retryCount = DEF_Retries_NormalCommands;
  else
    ui8_retryCount = DEF_Retries_TPCommands;

  ui32_uploadTimeout = DEF_TimeOut_ChangeStringValue;
}

void SendUpload_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout)
{ SendUploadBase_c::set(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, rui32_timeout);
  mssObjectString = NULL;
  ppc_vtObjects = NULL; /// Use BUFFER - NOT MultiSendStreamer!
  ui16_numObjects = 0;
}
void SendUpload_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout, IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t rui16_numObjects)
{
  SendUploadBase_c::set( byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, rui32_timeout );
  mssObjectString = NULL;  /// Use BUFFER - NOT MultiSendStreamer!
  ppc_vtObjects = rppc_vtObjects;
  ui16_numObjects = rui16_numObjects;
}
void SendUpload_c::set (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte)
{
  SendUploadBase_c::set( rui16_objId, rpc_string, overrideSendLength, ui8_cmdByte );
  mssObjectString = NULL;  /// Use BUFFER - NOT MultiSendStreamer!
  ppc_vtObjects = NULL;
}
void SendUpload_c::set (uint8_t* rpui8_buffer, uint32_t bufferSize)
{
  SendUploadBase_c::set (rpui8_buffer, bufferSize);
  mssObjectString = NULL;  /// Use BUFFER - NOT MultiSendStreamer!
  ppc_vtObjects = NULL;
}


/** place next data to send direct into send puffer of pointed
    stream send package - MultiSendStreamer_c will send this
    puffer afterwards
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function
 */
void
VtClientServerCommStreamer_c::setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes)
{
  while ((uploadBufferFilled-uploadBufferPosition) < bytes)
  {
    // copy down the rest of the buffer (we have no ring buffer here!)
    int j = uploadBufferFilled - uploadBufferPosition;
    for (int i=0; i<j; i++)
      uploadBuffer [i] = uploadBuffer [i+uploadBufferPosition];
    // adjust pointers
    uploadBufferPosition = 0;
    uploadBufferFilled = j;

    // stream some more bytes into internal ISO_VT_UPLOAD_BUFFER_SIZE byte buffer...
    uint16_t bytes2Buffer;
    while ((bytes2Buffer = ((vtObject_c*)(*pc_iterObjects))->stream (uploadBuffer+uploadBufferFilled, ISO_VT_UPLOAD_BUFFER_SIZE-uploadBufferFilled, ui32_objectStreamPosition)) == 0)
    {
      pc_iterObjects++;
      ui32_objectStreamPosition = 0;
    }
    uploadBufferFilled += bytes2Buffer;
    ui32_objectStreamPosition += bytes2Buffer;
  }
  mspData->setDataPart (uploadBuffer, uploadBufferPosition, bytes);
  uploadBufferPosition += bytes;
}


/** set cache for data source to stream start
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function */
void
VtClientServerCommStreamer_c::resetDataNextStreamPart()
{
  if (pc_userPoolUpdateObjects)
  { // user-given partial pool upload
    pc_iterObjects = pc_userPoolUpdateObjects;
  }
  else
  { // language pool update
    uint8_t ui8_streamOffset;
    if (ui32_streamSize > 0)
    { // stream GENERAL PART - that is **INDEX 0**
      ui8_streamOffset = 0;
    }
    else
    { // stream LANGUAGE PART - that is **INDEX 1..NrOfLang**
      const int8_t ci8_realUploadingLanguage = (i8_objectPoolUploadingLanguage < 0) ? 0 : i8_objectPoolUploadingLanguage;
      ui8_streamOffset = ci8_realUploadingLanguage + 1; // skip general pool
    }
    pc_iterObjects = refc_pool.getIVtObjects()[ui8_streamOffset];
  }
  ui32_objectStreamPosition = 0;
  uploadBufferPosition = 0;
  uploadBufferFilled = 1;
  uploadBuffer [0] = 0x11; // Upload Object Pool!

  // ! ui32_streamSize is constant and is initialized in "StartObjectPoolUploading"
  // ! refc_pool       is constant and is initialized in "StartObjectPoolUploading"

  // following should not be needed to be reset, as this set by "saveDataNextStreamPart"
  // ? ui32_objectStreamPositionStored
  // ? pc_iterObjectsStored;
  // ? uploadBufferStored [ISO_VT_UPLOAD_BUFFER_SIZE];
  // ? uploadBufferFilledStored;
  // ? uploadBufferPositionStored;
}


/** save current send position in data source - neeed for resend on send problem
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function */
void
VtClientServerCommStreamer_c::saveDataNextStreamPart()
{
  pc_iterObjectsStored = pc_iterObjects;
  ui32_objectStreamPositionStored = ui32_objectStreamPosition;
  uploadBufferPositionStored = uploadBufferPosition;
  uploadBufferFilledStored = uploadBufferFilled;
  for (int i=0; i<ISO_VT_UPLOAD_BUFFER_SIZE; i++)
    uploadBufferStored [i] = uploadBuffer [i];
}


/** reactivate previously stored data source position - used for resend on problem
    - implementation of the abstract IsoAgLib::MultiSendStreamer_c function */
void
VtClientServerCommStreamer_c::restoreDataNextStreamPart()
{
  pc_iterObjects = pc_iterObjectsStored;
  ui32_objectStreamPosition = ui32_objectStreamPositionStored;
  uploadBufferPosition = uploadBufferPositionStored;
  uploadBufferFilled = uploadBufferFilledStored;
  for (int i=0; i<ISO_VT_UPLOAD_BUFFER_SIZE; i++)
    uploadBuffer [i] = uploadBufferStored [i];
}

/** *****************************************************/
/** ** Vt Client Server Communication Implementation ****/
/** *****************************************************/

/** static instance to store temporarily before push_back into list */
SendUpload_c VtClientServerCommunication_c::sc_tempSendUpload;


void
VtClientServerCommunication_c::reactOnAbort (IsoAgLib::iStream_c& /*rrefc_stream*/)
{
  c_streamer.refc_pool.eventStringValueAbort();
}


// handle all string values between length of 9 and 259 bytes
bool
VtClientServerCommunication_c::reactOnStreamStart (const IsoAgLib::ReceiveStreamIdentifier_c& rc_ident, uint32_t rui32_totalLen)
{
  // if SA is not the address from the vt -> don't react on stream
  if ((rc_ident.getSaIsoName()) != (pc_vtServerInstance->getIsoName().toConstIisoName_c())) return false;
  //handling string value >= 9 Bytes
  if (rui32_totalLen > (4 /* H.18 byte 1-4 */ + 255 /* max string length */))
    return false;
  return true;
}


bool
VtClientServerCommunication_c::processPartStreamDataChunk (IsoAgLib::iStream_c& rrefc_stream, bool rb_isFirstChunk, bool rb_isLastChunk)
{
  if (rrefc_stream.getStreamInvalid()) return false;

  switch ( rrefc_stream.getFirstByte() )
  {
    case 0x8:
      if (rb_isFirstChunk)  // check for command input string value H.18
      {
        ui16_inputStringId = rrefc_stream.getNextNotParsed() | (rrefc_stream.getNextNotParsed() << 8);
        ui8_inputStringLength = rrefc_stream.getNextNotParsed();

        const uint16_t ui16_totalstreamsize = rrefc_stream.getByteTotalSize();
        if (ui16_totalstreamsize != (ui8_inputStringLength + 4))
        {
          rrefc_stream.setStreamInvalid();
          return false;
        }
      }
      c_streamer.refc_pool.eventStringValue (ui16_inputStringId, ui8_inputStringLength, rrefc_stream, rrefc_stream.getNotParsedSize(), rb_isFirstChunk, rb_isLastChunk);
      break;

    default:
      break;
  }

  return false;
}


/** default constructor, which can optional set the pointer to the containing
  Scheduler_c object instance
 */
VtClientServerCommunication_c::VtClientServerCommunication_c (IdentItem_c& ref_wsMasterIdentItem, ISOTerminal_c &ref_isoTerminal, IsoAgLib::iIsoTerminalObjectPool_c& rrefc_pool, char* rpc_versionLabel, uint8_t rui8_clientId)
  : b_vtAliveCurrent (false) // so we detect the rising edge when the VT gets connected!
  , b_checkSameCommand (true)
  , refc_wsMasterIdentItem (ref_wsMasterIdentItem)
  , refc_isoTerminal (ref_isoTerminal)
  , en_displayState (VtClientDisplayStateHidden)
  , c_streamer (rrefc_pool)
{
  pc_vtServerInstance = NULL;
  i8_vtLanguage = -1;
  b_receiveFilterCreated = false;
  ui8_clientId = rui8_clientId;

  // the generated initAllObjectsOnce() has to ensure to be idempotent! (vt2iso-generated source does this!)
  c_streamer.refc_pool.initAllObjectsOnce (SINGLETON_VEC_KEY_PARAMETER_VAR);
  // now let all clients know which client they belong to
  if (ui8_clientId > 0) // the iVtObjects are initialised with 0 as default index
  {
    for (uint16_t ui16_objIndex = 0; ui16_objIndex < c_streamer.refc_pool.getNumObjects(); ui16_objIndex++)
      c_streamer.refc_pool.getIVtObjects()[0][ui16_objIndex]->setClientID (ui8_clientId);
    for (uint8_t ui8_objLangIndex = 0; ui8_objLangIndex < c_streamer.refc_pool.getNumLang(); ui8_objLangIndex++)
      for (uint16_t ui16_objIndex = 0; ui16_objIndex < c_streamer.refc_pool.getNumObjectsLang(); ui16_objIndex++)
        c_streamer.refc_pool.getIVtObjects()[ui8_objLangIndex+1][ui16_objIndex]->setClientID (ui8_clientId);
  }

  if (rpc_versionLabel)
  {
    const uint32_t cui_len = CNAMESPACE::strlen (rpc_versionLabel);
    if ( ((c_streamer.refc_pool.getNumLang() == 0) && (cui_len > 7))
      || ((c_streamer.refc_pool.getNumLang()  > 0) && (cui_len > 5))
       )
    { // too long, fail!
      getILibErrInstance().registerError (iLibErr_c::Precondition, iLibErr_c::IsoTerminal);
      en_objectPoolState = OPCannotBeUploaded; // has to be checked after calling this constructor!
      return;
    }
    unsigned int i=0;
    for (; i<cui_len; i++) p7c_versionLabel [i] = rpc_versionLabel [i];
    for (; i<7;       i++) p7c_versionLabel [i] = ' '; // ASCII: Space
    b_usingVersionLabel = true;
  }
  else
  {
    b_usingVersionLabel = false;
  }

  en_objectPoolState = OPInitial;      // try to upload until state == UploadedSuccessfully || CannotBeUploaded
  en_uploadType = UploadPool;          // Start Pool Uploading sequence!!
  en_uploadPoolState = UploadPoolInit; // with "UploadInit
//en_uploadPoolType = X;               // don't care as no pool is being uploaded! --> will be set in startObjectPoolUploading
}


/** default destructor, which initiate sending address release for all own identities
  @see VtClientServerCommunication_c::~VtClientServerCommunication_c
 */
VtClientServerCommunication_c::~VtClientServerCommunication_c()
{
  getMultiReceiveInstance4Comm().deregisterClient (*this);
  getIsoFilterManagerInstance().removeIsoFilter (ISOFilter_s (*this, (0x3FFFF00UL), (VT_TO_ECU_PGN << 8),       &getIdentItem().isoName(), NULL, 8));
  getIsoFilterManagerInstance().removeIsoFilter (ISOFilter_s (*this, (0x3FFFF00UL), (ACKNOWLEDGEMENT_PGN << 8), &getIdentItem().isoName(), NULL, 8));
}


void
VtClientServerCommunication_c::timeEventSendLanguagePGN()
{
  // Get Local Settings (may not be reached, when terminal is switched on after ECU, as VT sends LNAGUAGE Info on startup!
  c_data.setExtCanPkg3 (6, 0, REQUEST_PGN_MSG_PGN>>8,
                        pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                        (LANGUAGE_PGN & 0xFF), ((LANGUAGE_PGN >> 8)& 0xFF), ((LANGUAGE_PGN >> 16)& 0xFF));
  getCanInstance4Comm() << c_data;      // Command: REQUEST_PGN_MSG_PGN
  pc_vtServerInstance->getLocalSettings()->lastRequested = HAL::getTime();
}


void
VtClientServerCommunication_c::timeEventUploadPoolTimeoutCheck()
{
  /// Do TIME-OUT Checks ALWAYS!
  if ((en_uploadPoolState == UploadPoolWaitingForLoadVersionResponse)
       || (en_uploadPoolState == UploadPoolWaitingForMemoryResponse)
       || (en_uploadPoolState == UploadPoolWaitingForEOOResponse))
  { // waiting for initial stuff was timed out
    if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
    {
      en_uploadPoolState = UploadPoolFailed;
      ui32_uploadTimestamp = HAL::getTime();
      ui32_uploadTimeout = DEF_WaitFor_Reupload; // wait 5 secs for possible reuploading...
    }
    /// @todo for now we allow parallel uploads
    /// in both cases (time out or success) we need to reset the upload pool flag to enable other clients to upload their pools
    /// refc_isoTerminal.resetFlagForPoolUpload (this);
  }

  if (en_uploadPoolState == UploadPoolWaitingForStoreVersionResponse)
  { // store version was timed out
    if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
    { // we couldn't store for some reason, but don't care, finalize anyway...
#ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "StoreVersion TimedOut!" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      finalizeUploading();
    }
  }

  if (en_uploadPoolState == UploadPoolUploading)
  { // Check if we expect an (OBJECTPOOL) UPLOAD to fail/finish?
    switch (en_sendSuccess)
    {
      case __IsoAgLib::MultiSend_c::Running:
      { // do nothing, still wait.
      } break;
      case __IsoAgLib::MultiSend_c::SendAborted:
      { // aborted sending
        /// re-send the current stream!
        getMultiSendInstance4Comm().sendIsoTarget (refc_wsMasterIdentItem.isoName(), pc_vtServerInstance->getIsoName(), &c_streamer, ECU_TO_VT_PGN, en_sendSuccess);
      } break;
      case __IsoAgLib::MultiSend_c::SendSuccess:
      { // see what part we just finished
        if (c_streamer.ui32_streamSize > 0)
        { // we just finished streaming the GENERAL part, so let's see if we have a LANGUAGE part to stream?
          c_streamer.ui32_streamSize = 0; // indicate completion of GENERAL upload!
          if (c_streamer.ui32_streamSizeLang > 0)
          { // start LANGUAGE part upload!
            getMultiSendInstance4Comm().sendIsoTarget (refc_wsMasterIdentItem.isoName(), pc_vtServerInstance->getIsoName(), &c_streamer, ECU_TO_VT_PGN, en_sendSuccess);
            break; // we're uploading, do not indicate pool completion ;)
          }
        } // else we just finished streaming the LANGUAGE part, so upload is finished now!
        // successfully sent, so we now do have to send out the "End of Object Pool Message"
        indicateObjectPoolCompletion(); // Send "End of Object Pool" message
      } break;
    } // switch
  }
}

void
VtClientServerCommunication_c::timeEventPrePoolUpload()
{
  /// first you have to get number of softkeys, text font data and hardware before you could upload
  if (!pc_vtServerInstance->getVtCapabilities()->lastReceivedSoftkeys
       && ((pc_vtServerInstance->getVtCapabilities()->lastRequestedSoftkeys == 0)
       || ((HAL::getTime()-pc_vtServerInstance->getVtCapabilities()->lastRequestedSoftkeys) > 1000)))
  { // Get Number Of Soft Keys
    c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8,
                          pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                          194, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    getCanInstance4Comm() << c_data;      // Command: Get Technical Data --- Parameter: Get Number Of Soft Keys
    pc_vtServerInstance->getVtCapabilities()->lastRequestedSoftkeys = HAL::getTime();
#ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "Requested first property (C2)..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  }

  if (pc_vtServerInstance->getVtCapabilities()->lastReceivedSoftkeys
      && (!pc_vtServerInstance->getVtCapabilities()->lastReceivedFont)
      && ((pc_vtServerInstance->getVtCapabilities()->lastRequestedFont == 0) || ((HAL::getTime()-pc_vtServerInstance->getVtCapabilities()->lastRequestedFont) > 1000)))
  { // Get Text Font Data
    c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8,
                          pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                          195, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    getCanInstance4Comm() << c_data;      // Command: Get Technical Data --- Parameter: Get Text Font Data
    pc_vtServerInstance->getVtCapabilities()->lastRequestedFont = HAL::getTime();
#ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "Requested fonts (C3)..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  }

  if (pc_vtServerInstance->getVtCapabilities()->lastReceivedSoftkeys
      && pc_vtServerInstance->getVtCapabilities()->lastReceivedFont
      && (!pc_vtServerInstance->getVtCapabilities()->lastReceivedHardware)
      && ((pc_vtServerInstance->getVtCapabilities()->lastRequestedHardware == 0)
      || ((HAL::getTime()-pc_vtServerInstance->getVtCapabilities()->lastRequestedHardware) > 1000)))
  { // Get Hardware
    c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8,
                          pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                          199, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    getCanInstance4Comm() << c_data;      // Command: Get Technical Data --- Parameter: Get Hardware
    pc_vtServerInstance->getVtCapabilities()->lastRequestedHardware = HAL::getTime();
#ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "Requested hardware (C7)..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  }
}


bool
VtClientServerCommunication_c::timeEventPoolUpload()
{
  // Do MAIN-Phase a) at INIT and b) <timeout> seconds after FAIL
  if (((en_uploadPoolState == UploadPoolFailed) && (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)))
        || (en_uploadPoolState == UploadPoolInit))
  {
    /** @todo for now allow parallel uploads
    if (!refc_isoTerminal.getFlagForPoolUpload (this))
    { // any other client is already uploading its pool, so wait!
      en_uploadPoolState = UploadPoolFailed;
      ui32_uploadTimestamp = HAL::getTime();
      ui32_uploadTimeout = DEF_WaitFor_FreeUpload; // wait 1 secs for possible free uploading...
      return true;
    } */
    // Take the version that's been set up NOW and try to load/upload it.
    setObjectPoolUploadingLanguage();
    // Do we want to try to "Load Version" or go directly to uploading?
    // Added this preprocessor so loading of object pools can be prevented for development purposes
#ifdef NO_LOAD_VERSION
#else
    if (b_usingVersionLabel)
    {
#ifdef GET_VERSIONS
      // this is for test purposes only!
      static int b_getVersionsSendTime = 0;
      if (b_getVersionsSendTime == 0)
      { // send out get versions first
        c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8,
                              pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                              223, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
        getCanInstance4Comm() << c_data;     // Command: Non Volatile Memory --- Parameter: Load Version
        b_getVersionsSendTime = HAL::getTime();
      }
      if ((b_getVersionsSendTime+500) > HAL::getTime())
      { // wait for answer first
        return true;
      }
#endif
      char lang1, lang2;
      if (c_streamer.ui16_objectPoolUploadingLanguageCode != 0x0000)
      {
        lang1 = c_streamer.ui16_objectPoolUploadingLanguageCode >> 8;
        lang2 = c_streamer.ui16_objectPoolUploadingLanguageCode & 0xFF;
      }
      else
      {
        lang1 = p7c_versionLabel [5];
        lang2 = p7c_versionLabel [6];
      }

      // Try to "Non Volatile Memory - Load Version" first!
      c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
#ifdef LOESCHE_POOL
                            210,
#else
                            209,
#endif
                            p7c_versionLabel [0], p7c_versionLabel [1], p7c_versionLabel [2], p7c_versionLabel [3], p7c_versionLabel [4], lang1, lang2);
      getCanInstance4Comm() << c_data;     // Command: Non Volatile Memory --- Parameter: Load Version
                                            //(Command: Non Volatile Memory --- Parameter: Delete Version - just a quick hack!)
#ifdef LOESCHE_POOL
      startObjectPoolUploading (UploadPoolTypeCompleteInitially);
#else
      // start uploading after reception of LoadVersion Response
      en_uploadPoolState = UploadPoolWaitingForLoadVersionResponse;
      en_uploadPoolType = UploadPoolTypeCompleteInitially; // need to set this, so that eventObjectPoolUploadedSucessfully is getting called (also after load, not only after upload)
      ui32_uploadTimeout = DEF_TimeOut_LoadVersion;
      ui32_uploadTimestamp = HAL::getTime();
#ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "Trying Load Version (D1) for Version ["<<p7c_versionLabel [0]<< p7c_versionLabel [1]<< p7c_versionLabel [2]<< p7c_versionLabel [3]<< p7c_versionLabel [4]<< lang1<< lang2<<"]..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
#endif
    }
    else
#endif
    { // NO_LOAD_VERSION
      // Start uploading right now, no "LoadVersion" first
      startObjectPoolUploading (UploadPoolTypeCompleteInitially); // no language specific upload - normal upload!
    }
  }
  return false;
}


/** periodically event
  @return true -> all planned activities performed in allowed time
 */
bool
VtClientServerCommunication_c::timeEvent(void)
{
  if (ElementBase_c::getAvailableExecTime() == 0) return false;
#ifdef DEBUG_HEAP_USEAGE
  if ((sui16_lastPrintedSendUploadQueueSize < sui16_sendUploadQueueSize)
  || (sui16_lastPrintedMaxSendUploadQueueSize < sui16_maxSendUploadQueueSize))
  { // MAX amount of sui16_sendUploadQueueSize or sui16_maxSendUploadQueueSize
    sui16_lastPrintedSendUploadQueueSize = sui16_sendUploadQueueSize;
    sui16_lastPrintedMaxSendUploadQueueSize = sui16_maxSendUploadQueueSize;
    getRs232Instance4Comm() << "Max: " << sui16_sendUploadQueueSize << " Items in FIFO, "
                            << sui16_sendUploadQueueSize << " x SendUpload_c: Mal-Alloc: "
                            << sizeSlistTWithMalloc (sizeof (SendUpload_c), sui16_sendUploadQueueSize)
                            << "/" << sizeSlistTWithMalloc (sizeof (SendUpload_c), 1)
                            << ", Chunk-Alloc: "
                            << sizeSlistTWithChunk (sizeof (SendUpload_c), sui16_sendUploadQueueSize)
                            << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
  }
#endif

  // do further activities only if registered ident is initialised as ISO and already successfully address-claimed...
  if (!refc_wsMasterIdentItem.isClaimedAddress()) return true;

  if (!b_receiveFilterCreated)
  { /*** MultiReceive/IsoFilterManager Registration ***/
    getMultiReceiveInstance4Comm().registerClient (*this, getIdentItem().isoName(), VT_TO_ECU_PGN);
    getIsoFilterManagerInstance().insertIsoFilter (ISOFilter_s (*this, (0x3FFFF00UL), (VT_TO_ECU_PGN << 8),       &getIdentItem().isoName(), NULL, 8));
    getIsoFilterManagerInstance().insertIsoFilter (ISOFilter_s (*this, (0x3FFFF00UL), (ACKNOWLEDGEMENT_PGN << 8), &getIdentItem().isoName(), NULL, 8));

    b_receiveFilterCreated = true;
  }

  /*** Regular start is here (the above preconditions should be satisfied if system is finally set up. ***/
  /*******************************************************************************************************/
  System_c::triggerWd();

  // VT Alive checks
  checkVtStateChange();

  // Do nothing if there's no VT active
  if (!isVtActive()) return true;

  // be kind and wait until WSMaster-/Slave-Announce have finished (AddressClaim is already checked above!!), do NOTHING else before.
  if (!refc_wsMasterIdentItem.getIsoItem()->isClaimedAndWsAnnounced()) return true;

  // Check if WS-Maintenance is needed
  if ((i32_nextWsMaintenanceMsg <= 0) || (HAL::getTime() >= i32_nextWsMaintenanceMsg))
  { // Do periodically WS-Maintenance sending (every second)
    c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                          0xFF, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    getCanInstance4Comm() << c_data;     // G.2: Function: 255 / 0xFF Working Set Maintenance Message

    i32_nextWsMaintenanceMsg = HAL::getTime() + 1000;
  }

    // If our IsoItem has claimed address, immediately try to get the LANGUAGE_PGN from VT/anyone ;-) (regardless of pool-upload!)
  if ((!pc_vtServerInstance->getLocalSettings()->lastReceived) && ((pc_vtServerInstance->getLocalSettings()->lastRequested == 0) || ((HAL::getTime()-pc_vtServerInstance->getLocalSettings()->lastRequested) > 2000)))
  { // Try every 2 seconds to get the LANGUAGE_PGN, be polite to not bombard the VT...
    /** @todo Give up somewhen?? Or retry really every 2 seconds? */
    timeEventSendLanguagePGN();
  }

  // lastReceived will be set if vtserverinstance processes the language pgn
  if (!pc_vtServerInstance->getLocalSettings()->lastReceived)
    return true; // do not proceed if LANGUAGE not yet received!

  if (en_objectPoolState == OPCannotBeUploaded)
    return true; /** @todo is this correctly assumed? -> if it couldn't be uploaded, only disconnecting/connecting VT helps! */

  /// Now from here on the Pool's state is: "OPInitial" or "OPUploadedSuccessfully"
  /// UPLOADING --> OBJECT-POOL<--
  if (en_uploadType == UploadPool)
  {
    timeEventUploadPoolTimeoutCheck();

    // Handled down below are (as they're no TIME-OUTs but INITIATIONs):
    // if (UploadPoolFailed)
    // if (UploadPoolInit)

    // Do we have to request (any) vt capabilities?
    if (!(pc_vtServerInstance->getVtCapabilities()->lastReceivedFont && pc_vtServerInstance->getVtCapabilities()->lastReceivedHardware && pc_vtServerInstance->getVtCapabilities()->lastReceivedSoftkeys))
    { /// Pool-Upload: PRE Phase (Get VT-Properties)
      timeEventPrePoolUpload();
    }
    else
    { /// Handle other than Response states now here: INITIALIZING-States!
      // - if (UploadPoolFailed)
      // - if (UploadPoolInit)
      /// Pool-Upload: MAIN Phase (Try2Load / Upload / Try2Save)
      if (timeEventPoolUpload()) return true;
    }
  }

  /// UPLOADING --> COMMAND <-- ///
  // Can only be done if the objectpool is successfully uploaded!
  if (en_objectPoolState != OPUploadedSuccessfully) return true;

  /// FROM HERE ON THE OBJECT-POOL >>IS<< UPLOADED SUCCESSFULLY
  /// HANDLE CASE A) AND B) FROM HERE NOW
  /// A) NOW HERE THE LANGUAGE CHANGE IS HANDLED
  if ((c_streamer.i8_objectPoolUploadingLanguage == -2) // indicates no update running
       && (i8_vtLanguage != c_streamer.i8_objectPoolUploadedLanguage))
  { // update languages on the fly
    setObjectPoolUploadingLanguage();
    /// NOTIFY THE APPLICATION so it can enqueue some commands that are processed BEFORE the update is done
    /// e.g. switch to a "Wait while changing language..." datamask.
    c_streamer.refc_pool.eventPrepareForLanguageChange (c_streamer.i8_objectPoolUploadingLanguage, c_streamer.ui16_objectPoolUploadingLanguageCode);

    sendCommandUpdateLanguagePool();
    // we keep (c_streamer.i8_objectPoolUploadingLanguage != -2), so a change in between doesn't care and won't happen!!
  }

  /// B) NOW HERE THE RUNTIME COMMANDS ARE BEING HANDLED
  if (en_uploadType == UploadCommand)
  { // NO Response/timeOut for (C.2.3 Object Pool Transfer Message) "UploadObjectPool" - Only for "UploadMultiPacketCommand"
    switch (en_sendSuccess)
    {
      case __IsoAgLib::MultiSend_c::SendAborted:
        // If aborted, retry regardless of "ui8_uploadRetry", as it was a multisend problem, not a problem of the command itself!
        startUploadCommand();
        break;

      case __IsoAgLib::MultiSend_c::Running:
        // increase sent time-stamp, so it matches best the time when the multisend has finished sending, so that the timeout counts from that time on!
        ui32_uploadTimestamp = HAL::getTime();
        break;

      case __IsoAgLib::MultiSend_c::SendSuccess: // no break, handle along with default: wait for response!
      default:
      { // successfully sent...
        if (en_uploadCommandState == UploadCommandWithoutResponse)
        { // no response is awaited, so we finished sending
          finishUploadCommand();
          break; // and done!
        }

        if (en_uploadCommandState == UploadCommandWaitingForCommandResponse) // won't reach here when "Running", as timestamp is getting get to now above!
        { // Waiting for an answer - Did it time out?
          if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
            en_uploadCommandState = UploadCommandTimedOut;
        } // don't break, as "UploadCommandTimedOut" is handled right below!

        // Are we in "Upload Command"-State and the last Upload failed/timed out?
        if ((en_uploadCommandState == UploadCommandTimedOut))
        {
          if (ui8_uploadRetry > 0)
          {
            ui8_uploadRetry--;
            startUploadCommand();
          }
          else
          { // No more retries, simply finish this job and go Idle!
            finishUploadCommand(); // will pop the SendUpload, as it can't be correctly sent after <retry> times. too bad.
          }
        }
        break;
      }
    }
  } // UploadCommand

  // Is a) no Upload running and b) some Upload to do?
  if ((en_uploadType == UploadIdle) && !q_sendUpload.empty())
  { // Set Retry & Start Uploading
    ui8_uploadRetry = (*(q_sendUpload.begin())).ui8_retryCount;
    startUploadCommand();
  }
  return true;
}


/** process received ack messages
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c */
bool
VtClientServerCommunication_c::processMsgAck()
{
  // shouldn't be possible, but check anyway to get sure.
  if (!pc_vtServerInstance) return false;

  // don't react on NACKs from other VTs than the one we're communicating with!
  if (pc_vtServerInstance->getVtSourceAddress() != c_data.isoSa()) return false;

  if (c_data.getUint8Data (0) != 0x01) return true; // Only react if "NOT ACKNOWLEDGE"!

#if !defined(IGNORE_VTSERVER_NACK)  // The NACK must be ignored for the Mueller VT Server
  // check if we have Agrocom/Mller with Version < 3, so we IGNORE this NACK BEFORE the pool is finally uploaded.
  bool b_ignoreNack = false; // normally DO NOT ignore NACK
  if (getIsoMonitorInstance4Comm().existIsoMemberNr (c_data.isoSa()))
  { // sender exists in isomonitor, so query its Manufacturer Code
    const uint16_t cui16_manufCode = getIsoMonitorInstance().isoMemberNr (c_data.isoSa()).isoName().manufCode();
    if (((cui16_manufCode == 98) /*Mller Elektronik*/ || (cui16_manufCode == 103) /*Agrocom*/) &&
          ((pc_vtServerInstance->getVtCapabilities()->lastReceivedVersion == 0) ||
          (pc_vtServerInstance->getVtCapabilities()->iso11783version < 3)))
    {
      if (en_objectPoolState != OPUploadedSuccessfully)
      { // mueller/agrocom hack - ignore upload while no objectpool is displayed
        b_ignoreNack = true;
      }
    }
  }

  if (!b_ignoreNack)
  {
    // for now ignore source address which must be VT of course. (but in case a NACK comes in before the first VT Status Message
    // Check if a VT-related message was NACKed. Check embedded PGN for that
    const uint32_t cui32_pgn =  uint32_t (c_data.getUint8Data (5)) |
                               (uint32_t (c_data.getUint8Data (6)) << 8) |
                               (uint32_t (c_data.getUint8Data (7)) << 16);
    switch (cui32_pgn)
    {
      case ECU_TO_VT_PGN:
      case WORKING_SET_MEMBER_PGN:
      case WORKING_SET_MASTER_PGN:
        /// fake NOT-alive state of VT for now!
        pc_vtServerInstance->resetVtAlive(); // set VTalive to FALSE, so the queue will be emptied down below on the state change check.
#ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "\n==========================================================================================="
                              << "\n=== VT NACKed "<<cui32_pgn<<", starting all over again -> faking VT loss in the following: ===";
#endif
        refc_wsMasterIdentItem.getIsoItem()->sendSaClaim(); // optional, but better do this: Repeat address claim!
        checkVtStateChange(); // will also notify application by "eventEnterSafeState"
        break;
    } // switch
  }
#endif

  return true; // (N)ACK for our SA will NOT be of interest for anyone else...
}


/** process received language pgn
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
 */
void
VtClientServerCommunication_c::notifyOnVtsLanguagePgn()
{
  i8_vtLanguage = -1; // indicate that VT's language is not supported by this WS, so the default language should be used

  if (pc_vtServerInstance)
  {
    const uint8_t cui8_languages = c_streamer.refc_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().numberOfLanguagesToFollow;
    for (int i=0; i<cui8_languages; i++)
    {
      const uint8_t* lang = c_streamer.refc_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().languagesToFollow[i].language;
      if (pc_vtServerInstance->getLocalSettings()->languageCode == ((lang[0] << 8) | lang[1]))
      {
        i8_vtLanguage = i; // yes, VT's language is directly supported by this workingset
        break;
      }
    }
    c_streamer.refc_pool.eventLanguagePgn (*pc_vtServerInstance->getLocalSettings());
  }
}

void
VtClientServerCommunication_c::notifyOnVtStatusMessage()
{
  // set client display state appropriately
  setVtDisplayState (true, getVtServerInst().getVtState()->saOfActiveWorkingSetMaster);

  c_streamer.refc_pool.eventVtStatusMsg();
}

/** process received can messages
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
 */
bool
VtClientServerCommunication_c::processMsg()
{
//  #ifdef DEBUG
//  INTERNAL_DEBUG_DEVICE << "Incoming Message: c_data.isoPgn=" << c_data.isoPgn() << " - HAL::getTime()=" << HAL::getTime() << " - data[0]=" << (uint16_t)c_data.getUint8Data (0) << "...  ";
//  #endif

  /** @todo check for can-pkg-length==8???? */

  if ((c_data.isoPgn() & 0x3FF00) == ACKNOWLEDGEMENT_PGN)
  { // Pass on to ACK-Processing!
    return processMsgAck();
  }

  uint8_t ui8_uploadCommandError; // who is interested in the errorCode anyway?
  uint8_t ui8_errByte=0; // from 1-8, or 0 for NO errorHandling, as NO user command (was intern command like C0/C2/C3/C7/etc.)

#define MACRO_setStateDependantOnError(errByte) \
  ui8_errByte = errByte;

  // If VT is not active, don't react on PKGs addressed to us, as VT's not active ;)
  if (!isVtActive()) return true;

  // check below not needed any more with new isofilter!?!??!??! */
  if ((c_data.isoPgn() & 0x3FFFF) != (VT_TO_ECU_PGN | getIdentItem().getIsoItem()->nr())) return false;

  /// process all VT_TO_ECU addressed to us
  switch (c_data.getUint8Data (0))
  {
    /*************************************/
    /*** ### VT Initiated Messages ### ***/
    case 0x00: // Command: "Control Element Function", parameter "Soft Key"
    case 0x01: // Command: "Control Element Function", parameter "Button"
      c_streamer.refc_pool.eventKeyCode (c_data.getUint8Data (1) /* key activation code (pressed, released, held) */,
                                         c_data.getUint8Data (2) | (c_data.getUint8Data (3) << 8) /* objID of key object */,
                                         c_data.getUint8Data (4) | (c_data.getUint8Data (5) << 8) /* objID of visible mask */,
                                         c_data.getUint8Data (6) /* key code */,
                                         c_data.getUint8Data (0) /* 0 for sk, 1 for button -- matches wasButton? boolean */ );
      break;
    case 0x02: // Command: "Control Element Function", parameter "Pointing Event"
      c_streamer.refc_pool.eventPointingEvent (c_data.getUint8Data (1) | (c_data.getUint8Data (2) << 8) /* X position in pixels */,
                                              c_data.getUint8Data (3) | (c_data.getUint8Data (4) << 8) /* Y position in pixels */);
      break;
    case 0x04: // Command: "Control Element Function", parameter "VT ESC"
        /// if no error occured, that ESC is for an opened input dialog!!! Do not handle here!!!
        if (c_data.getUint8Data (3) != 0x0)
        {
          c_streamer.refc_pool.eventVtESC();
        }
        break;
    case 0x05: // Command: "Control Element Function", parameter "VT Change Numeric Value"
      c_streamer.refc_pool.eventNumericValue (uint16_t(c_data.getUint8Data (1)) | (uint16_t(c_data.getUint8Data (2)) << 8) /* objID */,
                                              c_data.getUint8Data (4) /* 1 byte value */,
                                              uint32_t(c_data.getUint8Data (4)) | (uint32_t(c_data.getUint8Data (5)) << 8) | (uint32_t(c_data.getUint8Data (6)) << 16)| (uint32_t(c_data.getUint8Data (7)) << 24) /* 4 byte value */);
      break;
    case 0x08:  // Command: "Control Element Function", parameter "VT Input String Value"
      if (c_data.getUint8Data (3) <= 4) //within a 8 byte long cmd can be only a 4 char long string
      {
        VolatileMemory_c c_vmString (c_data.getUint8DataConstPointer(4));
        c_streamer.refc_pool.eventStringValue (uint16_t(c_data.getUint8Data (1)) | (uint16_t(c_data.getUint8Data (2)) << 8) /* objID */,
                                               c_data.getUint8Data (3) /* total number of bytes */, c_vmString,
                                               c_data.getUint8Data (3) /* total number of bytes */, true, true);
      }
      break;
    case 0x09:  // Command: "Command", parameter "Display Activation"
      setVtDisplayState (false, c_data.getUint8Data (1));

      // replace PGN, DA, SA and send back as answer
      c_data.setIsoPgn (ECU_TO_VT_PGN);
      c_data.setIsoSa (refc_wsMasterIdentItem.getIsoItem()->nr());
      c_data.setIsoPs (pc_vtServerInstance->getVtSourceAddress());
      getCanInstance4Comm() << c_data; // Command: "Command", parameter "Display Activation Response"
      break;

    /***************************************************/
    /*** ### ECU Initiated Messages (=Responses) ### ***/
    case 0x12: // Command: "End of Object Pool Transfer", parameter "Object Pool Ready Response"
      if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForEOOResponse))
      { /// *** INITIAL POOL UPLOAD ***
        if (c_data.getUint8Data (1) == 0)
        { /// NO Error with UPLOADING pool
// Added this preprocessor so storing of object pools can be prevented for development purposes
#ifndef NO_STORE_VERSION
          if (b_usingVersionLabel)
          { // Store Version and finalize after "Store Version Response"
            char lang1, lang2;
            if (c_streamer.ui16_objectPoolUploadingLanguageCode != 0x0000)
            {
              lang1 = c_streamer.ui16_objectPoolUploadingLanguageCode >> 8;
              lang2 = c_streamer.ui16_objectPoolUploadingLanguageCode & 0xFF;
            }
            else
            {
              lang1 = p7c_versionLabel [5];
              lang2 = p7c_versionLabel [6];
            }
            c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                                  208 /* D0 */, p7c_versionLabel [0], p7c_versionLabel [1], p7c_versionLabel [2], p7c_versionLabel [3], p7c_versionLabel [4], lang1, lang2);
            getCanInstance4Comm() << c_data;     // Command: Non Volatile Memory --- Parameter: Store Version

                      // Now wait for response
            en_uploadPoolState = UploadPoolWaitingForStoreVersionResponse;
            ui32_uploadTimeout = DEF_TimeOut_StoreVersion;
            ui32_uploadTimestamp = HAL::getTime();
          }
          else
#endif // NO_STORE_VERSION
          { // Finalize now!
            finalizeUploading();
          }
        }
        else
        {
          en_uploadPoolState = UploadPoolFailed; // errorcode in ui8_uploadError;
          en_objectPoolState = OPCannotBeUploaded;
          ui8_uploadError = c_data.getUint8Data (2);
          /// @todo for now allow parallel uploads
          ///refc_isoTerminal.resetFlagForPoolUpload (this);
        }
      }
      else if ((en_uploadType == UploadCommand) && (en_uploadCommandState == UploadCommandWaitingForCommandResponse))
      { /// *** LANGUAGE POOL UPDATE ***
        MACRO_setStateDependantOnError(2)
        finalizeUploading(); // indicate that the language specific objects have been updated. also the user will get notified.
      }
      break;

#ifdef USE_SIMPLE_AUX_RESPONSE
    /***************************************/
    /*** ### AUX Assignment Messages ### ***/
    case 0x20:
    { // Command: "Auxiliary Control", parameter "Auxiliary Assignment"
      c_data.setIsoPgn (ECU_TO_VT_PGN);
      c_data.setIsoSa (refc_wsMasterIdentItem.getIsoItem()->nr());
      c_data.setIsoPs (pc_vtServerInstance->getVtSourceAddress());
      getCanInstance4Comm() << c_data;
            /// For now simply respond without doing anything else with this information. simply ack the assignment!
    } break;
#endif

    /***************************************************/
    /*** ### ECU Initiated Messages (=Responses) ### ***/
    // ### Error field is also on byte 2 (index 1)
    case 0xA3: // Command: "Command", parameter "Control Audio Device Response"
    case 0xA4: // Command: "Command", parameter "Set Audio Volume Response"
    case 0xB2: // Command: "Command", parameter "Delete Object Pool Response"
      MACRO_setStateDependantOnError (2)
      break;

    // ### Error field is also on byte 4 (index 3)
    case 0xA6: // Command: "Command", parameter "Change Size Response"
    case 0xA8: // Command: "Command", parameter "Change Numeric Value Response"
    case 0xA9: // Command: "Command", parameter "Change End Point Response"
    case 0xAA: // Command: "Command", parameter "Change Font Attributes Response"
    case 0xAB: // Command: "Command", parameter "Change Line Attributes Response"
    case 0xAC: // Command: "Command", parameter "Change Fill Attributes Response"
    case 0xAD: // Command: "Command", parameter "Change Active Mask Response"
    case 0x92: // Command: "Command", parameter "ESC Response"
      MACRO_setStateDependantOnError (4)
      break;

    // ### Error field is also on byte nr. 5 (index 4)
    case 0xA0: // Command: "Command", parameter "Hide/Show Object Response" (Container)
    case 0xA1: // Command: "Command", parameter "Enable/Disable Object Response" (Input Object)
    case 0xA2: // Command: "Command", parameter "Select Input Object Response"
    case 0xA7: // Command: "Command", parameter "Change Background Colour Response"
    case 0xAF: // Command: "Command", parameter "Change Attribute Response"
    case 0xB0: // Command: "Command", parameter "Change Priority Response"
    case 0xB8: // Command: "Command", parameter "Graphics Context Command"
      MACRO_setStateDependantOnError (5)
      break;

    // ### Error field is also on byte 6 (index 5)
    case 0xA5: // Command: "Command", parameter "Change Child Location Response"
    case 0xAE: // Command: "Command", parameter "Change Soft Key Mask Response"
    case 0xB3: // Command: "Command", parameter "Change String Value Response"
    case 0xB4: // Command: "Command", parameter "Change Child Position Response"
      MACRO_setStateDependantOnError (6)
      break;

    // ### Error field is on byte 7 (index 6)
    case 0xB1: // Command: "Command", parameter "Change List Item Response"
      MACRO_setStateDependantOnError (7)
      break;

    case 0xB9: // Command: "Get Technical Data", parameter "Get Attribute Value"
      MACRO_setStateDependantOnError (7)
      // client requested any attribute value for an object in the pool -> create ram struct if not yet existing
      if ((c_data.getUint8Data (1) == 0xFF) && (c_data.getUint8Data (2) == 0xFF)) // object id is set to 0xFFFF to indicate error response
      {
        /// what to do if attribute value request returns error response???
      }
      else
      {
        // first store object ID for later use
        uint16_t ui16_objID = c_data.getUint8Data(1) | (c_data.getUint8Data (2) << 8);

        /// search for suitable iVtObject in all object lists of the client (pointer array to all fix and language dependent iVtObjects)

        uint8_t ui8_arrIndex = 0;
        bool b_objectFound = false;

        // first check if first item is the requested one -> working is the first item list no matter what objectID it has
        if (ui16_objID == c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex][0]->getID())
          c_streamer.refc_pool.eventAttributeValue(c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex][0],
                                                  c_data.getUint8Data(3),
                                                  c_data.getUint8DataPointer()+4);
        else
        {
          // if last item of the list was reached or the requested object was found
          while (c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex] != NULL)
          {
            uint16_t ui16_arrBegin = 1;
            uint16_t ui16_arrMiddle;
            uint16_t ui16_arrEnd;

            // first item in list contains all fix objects of the pool (when language changes, these object stay the same)
            if (ui8_arrIndex == 0)
              ui16_arrEnd = c_streamer.refc_pool.getNumObjects() - 1;
            else
              ui16_arrEnd = c_streamer.refc_pool.getNumObjectsLang() - 1;

            // if object is among these we can leave the while-loop
            if ((ui16_objID < c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex][ui16_arrBegin]->getID())
                || (ui16_objID > c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex][ui16_arrEnd]->getID())) // range check
            {
              ui8_arrIndex++;
              continue; // try next object list, the requested object could not be found in the current list
            }

            while (ui16_arrBegin <= ui16_arrEnd)
            {
              ui16_arrMiddle = ui16_arrBegin + ((ui16_arrEnd - ui16_arrBegin) / 2);

              if (c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex][ui16_arrMiddle]->getID() == ui16_objID) // objID found?
              {
                b_objectFound = true;
                c_streamer.refc_pool.eventAttributeValue(c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex][ui16_arrMiddle],
                                                        c_data.getUint8Data(3),
                                                        c_data.getUint8DataPointer()+4);
                break;
              }
              else
              {
                if (c_streamer.refc_pool.getIVtObjects()[ui8_arrIndex][ui16_arrMiddle]->getID() > ui16_objID)
                  ui16_arrEnd = ui16_arrMiddle - 1;
                else
                  ui16_arrBegin = ui16_arrMiddle + 1;
              }
            }

            if ((ui8_arrIndex == 0) && b_objectFound) // an object is either a fix object or a language dependent object (at least once in any language dependent list)
              break;

            ui8_arrIndex++;
          }
        }
      }
      break;
    case 0xBD: // Command: "Command", parameter "Lock/Unlock Mask Response"
      MACRO_setStateDependantOnError (3)
      break;
    case 0xC0: // Command: "Get Technical Data", parameter "Get Memory Size Response"
      pc_vtServerInstance->setVersion();
      if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForMemoryResponse))
      {
        if (c_data.getUint8Data (2) == 0)
        { // start uploading, there MAY BE enough memory
          en_uploadPoolState = UploadPoolUploading;
          getMultiSendInstance4Comm().sendIsoTarget (refc_wsMasterIdentItem.isoName(), pc_vtServerInstance->getIsoName(), &c_streamer, ECU_TO_VT_PGN, en_sendSuccess);
        }
        else
          vtOutOfMemory();
      }
      break;
    case 0xC2: // Command: "Get Technical Data", parameter "Get Number Of Soft Keys Response"
      pc_vtServerInstance->setSoftKeyData();
      break;
    case 0xC3: // Command: "Get Technical Data", parameter "Get Text Font Data Response"
      pc_vtServerInstance->setTextFontData();
      break;
    case 0xC7: // Command: "Get Technical Data", parameter "Get Hardware Response"
      pc_vtServerInstance->setHardwareData();
      break;
    case 0xD0: // Command: "Non Volatile Memory", parameter "Store Version Response"
      if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForStoreVersionResponse))
      {
        switch (c_data.getUint8Data (5) & 0x0F)
        {
          case 0: // Successfully stored
          case 1: // Not used
          case 2: // Version label not known
          case 8: // General error
            break;
          case 4: // Insufficient memory available
          default: // well....
            getILibErrInstance().registerError(iLibErr_c::RemoteServiceOutOfMemory, iLibErr_c::IsoTerminal);
            break;
        }
        finalizeUploading();
      }
      break;
    case 0xD1: // Command: "Non Volatile Memory", parameter "Load Version Response"
      if ((en_uploadType == UploadPool) && (en_uploadPoolState == UploadPoolWaitingForLoadVersionResponse))
      {
        if ((c_data.getUint8Data (5) & 0x0F) == 0)
        { // Successfully loaded
          finalizeUploading ();
#ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) without error..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
        }
        else
        {
          if (c_data.getUint8Data (5) & (1<<2))
          { // Bit 2: // Insufficient memory available
#ifdef DEBUG
            INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) with error OutOfMem..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
            vtOutOfMemory();
          }
          else
          { // Not used
            // General error
            // Version label not known
            startObjectPoolUploading (UploadPoolTypeCompleteInitially); // Send out pool! send out "Get Technical Data - Get Memory Size", etc. etc.
#ifdef DEBUG
            INTERNAL_DEBUG_DEVICE << "Received Load Version Response (D1) with VersionNotFound..." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
          }
        }
      }
      break;

    default:
      // handle proprietary messages from an AGCO VT
      if (    c_data.getUint8Data (0) >= 0x60
           && c_data.getUint8Data (0) <= 0x7F
         )
      {
      #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "\n%% Proprietary command ";
      #endif
        MACRO_setStateDependantOnError( c_streamer.refc_pool.eventProprietaryCommand( pc_vtServerInstance->getIsoName().toConstIisoName_c() ) )
      }
      break;

  } // switch

  // Was it some command that requires queue-deletion & error processing?
  if (ui8_errByte != 0)
  {
    if (en_uploadType == UploadCommand)
    { /* if Waiting or Timedout (or Failed <shouldn't happen>) */
      if (en_sendSuccess == __IsoAgLib::MultiSend_c::SendSuccess)
      { /// Our command was successfully sent & responded to, so remove it from the queue
        if (ui8_commandParameter == c_data.getUint8Data (0))
        { /* okay, right response for our current command! */
          // special treatment for Get Attribute Value command -> error byte is also being used as value byte for successful response
          if ((ui8_commandParameter == 0xB9) && (c_data.getUint16Data (2-1) != 0xFFFF))
            ui8_uploadCommandError = 0;
          else
            ui8_uploadCommandError = c_data.getUint8Data (ui8_errByte-1);
          /// Inform user on success/error of this command
          c_streamer.refc_pool.eventCommandResponse (ui8_uploadCommandError, c_data.getUint8DataConstPointer()); // pass "ui8_uploadCommandError" in case it's only important if it's an error or not. get Cmd and all databytes from "c_data.name()"
#ifdef DEBUG
          if (ui8_uploadCommandError != 0)
          { /* error */
            INTERNAL_DEBUG_DEVICE << ">>> Command " << (uint32_t) ui8_commandParameter<< " failed with error " << (uint32_t) ui8_uploadCommandError << "!" << INTERNAL_DEBUG_DEVICE_ENDL;
          }
#endif
          finishUploadCommand(); // finish command no matter if "okay" or "error"...
        }
      }
      else
      { /// Our command was Aborted or is still running, so do NOT remove from the queue
        /// let timeEvent try again / wait for stream to finish sending
      }
    }
  } // VT to this ECU
  return true;
}


uint32_t
VtClientServerCommunication_c::getUploadBufferSize()
{
  return q_sendUpload.size();
}


uint8_t
VtClientServerCommunication_c::getUserClippedColor (uint8_t colorValue, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour)
{
  if (pc_vtServerInstance)
  {
    uint8_t colorDepth = pc_vtServerInstance->getVtCapabilities()->hwGraphicType;
    if (((colorDepth == 0) && (colorValue > 1)) || ((colorDepth == 1) && (colorValue > 16)))
      return c_streamer.refc_pool.convertColour (colorValue, colorDepth, obj, whichColour);
  }
  return colorValue;
}


void
VtClientServerCommunication_c::notifyOnNewVtServerInstance (VtServerInstance_c& ref_newVtServerInst)
{
  if (pc_vtServerInstance) return;

  pc_vtServerInstance = &ref_newVtServerInst;
}


void
VtClientServerCommunication_c::notifyOnVtServerInstanceLoss (VtServerInstance_c& ref_oldVtServerInst)
{
  if (&ref_oldVtServerInst == pc_vtServerInstance)
    pc_vtServerInstance = NULL;
}


/** special 9 byte sendCommand function for Change Child Position Command (no one else uses 9 byte commands!)
  @returns true if there was place in the SendUpload-Buffer (should always be the case now)
 */
bool
VtClientServerCommunication_c::sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t ui32_timeout, bool b_enableReplaceOfCmd)
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued 9-bytes: " << q_sendUpload.size() << " -> ";
#endif

  sc_tempSendUpload.set (byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, ui32_timeout);

  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

/** @returns true if there was place in the SendUpload-Buffer (should always be the case now) */
bool
VtClientServerCommunication_c::sendCommand (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t ui32_timeout, bool b_enableReplaceOfCmd, IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t rui16_numObjects)
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued 8-bytes: " << q_sendUpload.size() << " -> ";
  std::cout << "   and client sa = " << (int)dataBase().isoSa() << " and client id = " << (int)getClientId() << std::endl;
#endif

  sc_tempSendUpload.set (byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, ui32_timeout, rppc_vtObjects, rui16_numObjects);

  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandForDEBUG (uint8_t* rpui8_buffer, uint32_t ui32_size)
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued Debug-TP-bytes: " << q_sendUpload.size() << " -> ";
#endif

  sc_tempSendUpload.set (rpui8_buffer, ui32_size);

  return queueOrReplace (sc_tempSendUpload, false);
}

bool
VtClientServerCommunication_c::sendCommandChangeNumericValue (IsoAgLib::iVtObject_c* rpc_object, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return sendCommand (168 /* Command: Command --- Parameter: Change Numeric Value */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      0xFF, byte1, byte2, byte3, byte4,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeAttribute (IsoAgLib::iVtObject_c* rpc_object, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return sendCommand (175 /* Command: Command --- Parameter: Change Attribute */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      attrId, byte1, byte2, byte3, byte4,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeSoftKeyMask (IsoAgLib::iVtObject_c* rpc_object, uint8_t maskType, uint16_t newSoftKeyMask, bool b_enableReplaceOfCmd)
{
  return sendCommand (174 /* Command: Command --- Parameter: Change Soft Key Mask */,
                      maskType,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newSoftKeyMask & 0xFF, newSoftKeyMask >> 8,
                      0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeStringValue (IsoAgLib::iVtObject_c* rpc_object, const char* rpc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd)
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued string-ref: " << q_sendUpload.size() << " -> ";
#endif

  sc_tempSendUpload.set (rpc_object->getID(), rpc_newValue, overrideSendLength);

  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeStringValue (IsoAgLib::iVtObjectString_c* rpc_objectString, bool b_enableReplaceOfCmd)
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued stringObject-mss: " << q_sendUpload.size() << " -> ";
#endif

  sc_tempSendUpload.set (rpc_objectString);

  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeChildPosition (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t x, int16_t y, bool b_enableReplaceOfCmd)
{
  return sendCommand (180 /* Command: Command --- Parameter: Change Child Position */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      rpc_childObject->getID() & 0xFF, rpc_childObject->getID() >> 8,
                      x & 0xFF, x >> 8,
                      y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

//! should only be called with valid values ranging -127..0..128 (according to ISO!!!)
bool
VtClientServerCommunication_c::sendCommandChangeChildLocation (IsoAgLib::iVtObject_c* rpc_object, IsoAgLib::iVtObject_c* rpc_childObject, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd)
{
  return sendCommand (165 /* Command: Command --- Parameter: Change Child Location */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      rpc_childObject->getID() & 0xFF, rpc_childObject->getID() >> 8,
                      dx+127, dy+127, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeBackgroundColour (IsoAgLib::iVtObject_c* rpc_object, uint8_t newColour, bool b_enableReplaceOfCmd)
{
  return sendCommand (167 /* Command: Command --- Parameter: Change Background Color */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newColour, 0xFF, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangePriority (IsoAgLib::iVtObject_c* rpc_object, int8_t newPriority, bool b_enableReplaceOfCmd)
{
  if(newPriority < 3)
  {
    // only bother to send if priority is a legal value
    return sendCommand (176 /* Command: Command --- Parameter: Change Priority */,
                        rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                        newPriority, 0xFF, 0xFF, 0xFF, 0xFF,
                        DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
  }
  else
    return false;
}

bool
VtClientServerCommunication_c::sendCommandChangeSize (IsoAgLib::iVtObject_c* rpc_object,uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd)
{
  return sendCommand (166 /* Command: Command --- Parameter: Change Size */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newWidth & 0xFF, newWidth >> 8,
                      newHeight & 0xFF, newHeight >> 8,
                      0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeEndPoint (IsoAgLib::iVtObject_c* rpc_object,uint16_t newWidth, uint16_t newHeight, uint8_t newLineAttributes, bool b_enableReplaceOfCmd)
{
  return sendCommand (169 /* Command: Command --- Parameter: Change Size */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newWidth & 0xFF, newWidth >> 8,
                      newHeight & 0xFF, newHeight >> 8,
                      newLineAttributes,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeFillAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd)
{
  return sendCommand (172 /* Command: Command --- Parameter: Change FillAttributes */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newFillType, newFillColour,
                      (newFillType == 3) ? newFillPatternObject->getID() & 0xFF : 0xFF,
                      (newFillType == 3) ? newFillPatternObject->getID() >> 8 : 0xFF,
                      0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeFontAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd)
{
  return sendCommand (170 /* Command: Command --- Parameter: Change FontAttributes */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newFontColour, newFontSize, newFontType, newFontStyle, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeLineAttributes (IsoAgLib::iVtObject_c* rpc_object, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd)
{
  return sendCommand (171 /* Command: Command --- Parameter: Change LineAttributes */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      newLineColour, newLineWidth, newLineArt & 0xFF, newLineArt >> 8, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandControlAudioDevice (uint8_t rui8_repetitions, uint16_t rui16_frequency, uint16_t rui16_onTime, uint16_t rui16_offTime)
{
  return sendCommand (163 /* Command: Command --- Parameter: Control Audio Device */,
                      rui8_repetitions,
                      rui16_frequency & 0xFF, rui16_frequency >> 8,
                      rui16_onTime & 0xFF, rui16_onTime >> 8,
                      rui16_offTime & 0xFF, rui16_offTime >> 8,
                      DEF_TimeOut_NormalCommand, false); // don't care for enable-same command stuff
}

bool
VtClientServerCommunication_c::sendCommandSetAudioVolume (uint8_t rui8_volume)
{
  return sendCommand (164 /* Command: Command --- Parameter: Set Audio Volume */,
                      rui8_volume, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, DEF_TimeOut_NormalCommand, false); // don't care for enableReplaceOfCommand parameter actually
}

bool
VtClientServerCommunication_c::sendCommandDeleteObjectPool()
{
  return sendCommand (178 /* Command: Command --- Parameter: Delete Object Pool */,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, DEF_TimeOut_NormalCommand, true); // don't care for enableReplaceOfCommand parameter actually
}

bool
VtClientServerCommunication_c::sendCommandUpdateLanguagePool()
{
  /// Enqueue a fake command which will trigger the language object pool update to be multi-sent out. using 0x11 here, as this is the command then and won't be used
  return sendCommand (0x11 /* Command: Object Pool Transfer --- Parameter: Object Pool Transfer */,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, DEF_TimeOut_EndOfObjectPool, false) // replaces COULD happen if user-triggered sequences are there.
      && sendCommand (0x12 /* Command: Object Pool Transfer --- Parameter: Object Pool Ready */,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, DEF_TimeOut_EndOfObjectPool, false); // replaces COULD happen if user-triggered sequences are there.
}

bool
VtClientServerCommunication_c::sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t rui16_numObjects)
{
  /// Enqueue a fake command which will trigger the language object pool update to be multi-sent out. using 0x11 here, as this is the command then and won't be used
  return sendCommand (0x11 /* Command: Object Pool Transfer --- Parameter: Object Pool Transfer */,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, DEF_TimeOut_EndOfObjectPool, false, rppc_vtObjects, rui16_numObjects) // replaces COULD happen if user-triggered sequences are there.
      && sendCommand (0x12 /* Command: Object Pool Transfer --- Parameter: Object Pool Ready */,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, DEF_TimeOut_EndOfObjectPool, false); // replaces COULD happen if user-triggered sequences are there.
}


//////////////
bool
VtClientServerCommunication_c::sendCommandChangeNumericValue (uint16_t rui16_objectUid, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return sendCommand (168 /* Command: Command --- Parameter: Change Numeric Value */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      0xFF, byte1, byte2, byte3, byte4,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeAttribute (uint16_t rui16_objectUid, uint8_t attrId, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, bool b_enableReplaceOfCmd)
{
  return sendCommand (175 /* Command: Command --- Parameter: Change Attribute */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      attrId, byte1, byte2, byte3, byte4,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeSoftKeyMask (uint16_t rui16_objectUid, uint8_t maskType, uint16_t newSoftKeyMask, bool b_enableReplaceOfCmd)
{
  return sendCommand (174 /* Command: Command --- Parameter: Change Soft Key Mask */,
                      maskType,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      newSoftKeyMask & 0xFF, newSoftKeyMask >> 8,
                      0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeStringValue (uint16_t rui16_objectUid, const char* rpc_newValue, uint16_t overrideSendLength, bool b_enableReplaceOfCmd)
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Enqueued string-ref: " << q_sendUpload.size() << " -> ";
#endif

  sc_tempSendUpload.set (rui16_objectUid, rpc_newValue, overrideSendLength);

  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeChildPosition (uint16_t rui16_objectUid, uint16_t rui16_childObjectUid, int16_t x, int16_t y, bool b_enableReplaceOfCmd)
{
  return sendCommand (180 /* Command: Command --- Parameter: Change Child Position */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      rui16_childObjectUid & 0xFF, rui16_childObjectUid >> 8,
                      x & 0xFF, x >> 8,
                      y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

//! should only be called with valid values ranging -127..0..128 (according to ISO!!!)
bool
VtClientServerCommunication_c::sendCommandChangeChildLocation (uint16_t rui16_objectUid, uint16_t rui16_childObjectUid, int16_t dx, int16_t dy, bool b_enableReplaceOfCmd)
{
  return sendCommand (165 /* Command: Command --- Parameter: Change Child Location */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      rui16_childObjectUid & 0xFF, rui16_childObjectUid >> 8,
                      dx+127, dy+127, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeBackgroundColour (uint16_t rui16_objectUid, uint8_t newColour, bool b_enableReplaceOfCmd)
{
  return sendCommand (167 /* Command: Command --- Parameter: Change Background Color */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      newColour, 0xFF, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeSize (uint16_t rui16_objectUid,uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd)
{
  return sendCommand (166 /* Command: Command --- Parameter: Change Size */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      newWidth & 0xFF, newWidth >> 8,
                      newHeight & 0xFF, newHeight >> 8,
                      0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeFillAttributes (uint16_t rui16_objectUid, uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPatternObject, bool b_enableReplaceOfCmd)
{
  return sendCommand (172 /* Command: Command --- Parameter: Change FillAttributes */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      newFillType, newFillColour,
                      (newFillType == 3) ? newFillPatternObject->getID() & 0xFF : 0xFF,
                      (newFillType == 3) ? newFillPatternObject->getID() >> 8 : 0xFF,
                      0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeFontAttributes (uint16_t rui16_objectUid, uint8_t newFontColour, uint8_t newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_enableReplaceOfCmd)
{
  return sendCommand (170 /* Command: Command --- Parameter: Change FontAttributes */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      newFontColour, newFontSize, newFontType, newFontStyle, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeLineAttributes (uint16_t rui16_objectUid, uint8_t newLineColour, uint8_t newLineWidth, uint16_t newLineArt, bool b_enableReplaceOfCmd)
{
  return sendCommand (171 /* Command: Command --- Parameter: Change LineAttributes */,
                      rui16_objectUid & 0xFF, rui16_objectUid >> 8,
                      newLineColour, newLineWidth, newLineArt & 0xFF, newLineArt >> 8, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}
/////////////

// ########## Gaphics Context ##########
//! @return Flag if successful
bool
VtClientServerCommunication_c::sendCommandSetGraphicsCursor(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd)
{
  uint16_t x=convert_n::castUI( rc_point.getX() );
  uint16_t y=convert_n::castUI( rc_point.getY() );
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_setGraphicsCursorCmdID,
                      x & 0xFF, x >> 8, y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd );
}

bool
VtClientServerCommunication_c::sendCommandSetForegroundColour(
  IsoAgLib::iVtObject_c* rpc_object, uint8_t newValue, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_setForegroundColourCmdID,
                      newValue, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandSetBackgroundColour(
  IsoAgLib::iVtObject_c* rpc_object, uint8_t newValue, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_setBackgroundColourCmdID,
                      newValue, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandSetGCLineAttributes(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectLineAttributes_c* const newLineAttributes, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_setLineAttributeCmdID,
                      newLineAttributes->getID() & 0xFF, newLineAttributes->getID() >> 8, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandSetGCFillAttributes(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectFillAttributes_c* const newFillAttributes, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_setFillAttributeCmdID,
                      newFillAttributes->getID() & 0xFF, newFillAttributes->getID() >> 8, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandSetGCFontAttributes(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectFontAttributes_c* const newFontAttributes, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_setFontAttributeCmdID,
                      newFontAttributes->getID() & 0xFF, newFontAttributes->getID() >> 8, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandEraseRectangle(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd)
{
  uint16_t x=convert_n::castUI( rc_point.getX() );
  uint16_t y=convert_n::castUI( rc_point.getY() );
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_eraseRectangleCmdID,
                      x & 0xFF, x >> 8, y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandDrawPoint(
  IsoAgLib::iVtObject_c* rpc_object, bool  b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_drawPointCmdID,
                      0xFF, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandDrawLine(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd)
{
  uint16_t x=convert_n::castUI( rc_point.getX() );
  uint16_t y=convert_n::castUI( rc_point.getY() );
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_drawLineCmdID,
                      x & 0xFF, x >> 8, y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandDrawRectangle(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd)
{
  uint16_t x=convert_n::castUI( rc_point.getX() );
  uint16_t y=convert_n::castUI( rc_point.getY() );
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_drawRectangleCmdID,
                      x & 0xFF, x >> 8, y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandDrawClosedEllipse(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd)
{
  uint16_t x=convert_n::castUI( rc_point.getX() );
  uint16_t y=convert_n::castUI( rc_point.getY() );
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_drawClosedEllipseCmdID,
                      x & 0xFF, x >> 8, y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandDrawPolygon(
  IsoAgLib::iVtObject_c* rpc_object, uint16_t ui16_numOfPoints, const IsoAgLib::iVtPoint_c* const rpc_data, bool b_enableReplaceOfCmd)
{
  // Prevent from derefernzing NULL pointer.
  if (0 == rpc_data) { ui16_numOfPoints = 0; }

  // Check if valid polgon (at least one point)
  if (0 == ui16_numOfPoints) { return false; }

  // Trivial case (like draw line) without TP.
  if (ui16_numOfPoints == 1) {
    uint16_t x = convert_n::castUI( rpc_data->getX() );
    uint16_t y = convert_n::castUI( rpc_data->getY() );
    return sendCommand( vtObjectGraphicsContext_c::e_commandID,
                        rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                        vtObjectGraphicsContext_c::e_drawPolygonCmdID,
                        x & 0xFF, x >> 8, y & 0xFF, y >> 8,
                        DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd );
  }

  // send a polygon with more than one point
  uint16_t ui16_bufferSize = 4+(ui16_numOfPoints*4);
  uint8_t *pui8_buffer = new uint8_t[ ui16_bufferSize ];
  pui8_buffer[0] = vtObjectGraphicsContext_c::e_commandID;
  pui8_buffer[1] = rpc_object->getID() & 0xFF;
  pui8_buffer[2] = rpc_object->getID() >> 8;
  pui8_buffer[3] = vtObjectGraphicsContext_c::e_drawPolygonCmdID;

  // add all points from the list to the buffer
  uint16_t ui16_index = 4;		/* first 4 bytes are set */
  for ( uint16_t ui16_currentPoint = 0;
        ui16_currentPoint < ui16_numOfPoints;
        ui16_currentPoint++ )
  {
    uint16_t x = convert_n::castUI( rpc_data[ui16_currentPoint].getX() );
    pui8_buffer[ui16_index]   = x & 0xFF;
    pui8_buffer[ui16_index+1] = x >> 8;
    uint16_t y = convert_n::castUI( rpc_data[ui16_currentPoint].getY() );
    pui8_buffer[ui16_index+2] = y & 0xFF;
    pui8_buffer[ui16_index+3] = y >> 8;
    ui16_index+=4;
  }

  // Send buffer as ISOBUS command.
  sc_tempSendUpload.set (pui8_buffer, ui16_bufferSize );
  delete[] pui8_buffer;
  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandDrawText(
  IsoAgLib::iVtObject_c* rpc_object, uint8_t ui8_textType, uint8_t ui8_numOfCharacters, const char *rpc_newValue, bool b_enableReplaceOfCmd)
{
  // Non TP case
  if (ui8_numOfCharacters <= 2) {
    uint8_t a = (ui8_numOfCharacters >= 1) ? rpc_newValue[0] : 0xFF;
    uint8_t b = (ui8_numOfCharacters >= 2) ? rpc_newValue[1] : 0xFF;
    return sendCommand( vtObjectGraphicsContext_c::e_commandID,
                        rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                        vtObjectGraphicsContext_c::e_drawTextCmdID,
                        ui8_textType, ui8_numOfCharacters, a, b,
                        DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd );
  }

  uint8_t *pui8_buffer = new uint8_t[6+ui8_numOfCharacters];
  pui8_buffer[0] = vtObjectGraphicsContext_c::e_commandID;
  pui8_buffer[1] = rpc_object->getID() & 0xFF;
  pui8_buffer[2] = rpc_object->getID() >> 8;
  pui8_buffer[3] = vtObjectGraphicsContext_c::e_drawTextCmdID;
  pui8_buffer[4] = ui8_textType;
  pui8_buffer[5] = ui8_numOfCharacters;

  for (uint8_t ui8_index = 0; ui8_index < ui8_numOfCharacters; ui8_index++)
    pui8_buffer[6+ui8_index] = rpc_newValue [ui8_index];

  sc_tempSendUpload.set (pui8_buffer, (6+ui8_numOfCharacters));
  delete[] pui8_buffer;

  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandPanViewport(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, bool b_enableReplaceOfCmd)
{
  uint16_t x = convert_n::castUI( rc_point.getX() );
  uint16_t y = convert_n::castUI( rc_point.getY() );
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_panViewportCmdID,
                      x & 0xFF, x >> 8, y & 0xFF, y >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandZoomViewport(
  IsoAgLib::iVtObject_c* rpc_object, int8_t newValue, bool b_enableReplaceOfCmd)
{
  uint8_t zoom = convert_n::castUI( newValue );
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_zoomViewportCmdID,
                      zoom, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandPanAndZoomViewport(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtPoint_c& rc_point, int8_t newValue, bool b_enableReplaceOfCmd)
{
  uint16_t x = convert_n::castUI( rc_point.getX() );
  uint16_t y = convert_n::castUI( rc_point.getY() );
  uint8_t zoom = convert_n::castUI( newValue );
  uint8_t pui8_buffer[9];
  pui8_buffer[0] = vtObjectGraphicsContext_c::e_commandID;
  pui8_buffer[1] = rpc_object->getID() & 0xFF;
  pui8_buffer[2] = rpc_object->getID() >> 8;
  pui8_buffer[3] = vtObjectGraphicsContext_c::e_panAndZoomViewportCmdID;
  pui8_buffer[4] = x & 0xFF;
  pui8_buffer[5] = x >> 8;
  pui8_buffer[6] = y & 0xFF;
  pui8_buffer[7] = y >> 8;
  pui8_buffer[8] = zoom;

  // Send buffer as ISOBUS command.
  sc_tempSendUpload.set( pui8_buffer, sizeof(pui8_buffer)/sizeof(*pui8_buffer) );
  return queueOrReplace (sc_tempSendUpload, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandChangeViewportSize(
IsoAgLib::iVtObject_c* rpc_object, uint16_t newWidth, uint16_t newHeight, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_changeViewportSizeCmdID,
                      newWidth & 0xFF, newWidth >> 8, newHeight & 0xFF, newHeight >> 8,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandDrawVtObject(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObject_c* const pc_VtObject, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_drawVTObjectCmdID,
                      pc_VtObject->getID() & 0xFF, pc_VtObject->getID() >> 8, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandCopyCanvas2PictureGraphic(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObject, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_copyCanvasToPictureGraphicCmdID,
                      pc_VtObject->getID() & 0xFF, pc_VtObject->getID() >> 8, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandCopyViewport2PictureGraphic(
  IsoAgLib::iVtObject_c* rpc_object, const IsoAgLib::iVtObjectPictureGraphic_c* const pc_VtObject, bool b_enableReplaceOfCmd)
{
  return sendCommand (vtObjectGraphicsContext_c::e_commandID,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      vtObjectGraphicsContext_c::e_copyViewportToPictureGraphicCmdID,
                      pc_VtObject->getID() & 0xFF, pc_VtObject->getID() >> 8, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}
// ########## END Graphics Context ##########

bool
VtClientServerCommunication_c::sendCommandGetAttributeValue( IsoAgLib::iVtObject_c* rpc_object, const uint8_t cui8_attrID, bool b_enableReplaceOfCmd)
{
  return sendCommand (185 /* Command: Get Technical Data --- Parameter: Get Attribute Value */,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8,
                      cui8_attrID,
                      0xFF, 0xFF, 0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}

bool
VtClientServerCommunication_c::sendCommandLockUnlockMask( IsoAgLib::iVtObject_c* rpc_object, bool b_lockMask, uint16_t ui16_lockTimeOut, bool b_enableReplaceOfCmd)
{
#ifdef DEBUG
  std::cout << "\n LOCK *** LOCK *** send lock(1)/unlock(0) message. With b_lockMask = " << b_lockMask << std::endl;
  std::cout << "   and client sa = " << (int)dataBase().isoSa() << " and client id = " << (int)getClientId() << std::endl;
#endif
  return sendCommand (189 /* Command: Command --- Parameter: Lock/Undlock Mask */,
                      b_lockMask,
                      rpc_object->getID() & 0xFF, rpc_object->getID() >> 8, /* object id of the data mask to lock */
                      ui16_lockTimeOut & 0xFF, ui16_lockTimeOut >> 8, /* lock timeout on ms or zero for no timeout */
                      0xFF, 0xFF,
                      DEF_TimeOut_NormalCommand, b_enableReplaceOfCmd);
}


bool
VtClientServerCommunication_c::queueOrReplace (SendUpload_c& rref_sendUpload, bool b_enableReplaceOfCmd)
{
  if (en_objectPoolState != OPUploadedSuccessfully)
  {
#ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "--NOT ENQUEUED - POOL NO YET COMPLETELY UPLOADED!--" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    return false;
  }
  SendUpload_c* p_queue = NULL;
  uint8_t i = 0;
#ifdef USE_LIST_FOR_FIFO
#ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  std::list<SendUpload_c,std::__malloc_alloc_template<0> >::iterator i_sendUpload;
#else
  std::list<SendUpload_c>::iterator i_sendUpload;
#endif
#else
  std::queue<SendUpload_c>::iterator i_sendUpload;
#endif
  if (b_checkSameCommand && b_enableReplaceOfCmd)
  {
      //get first equal command in queue
    for (i_sendUpload = q_sendUpload.begin(); (p_queue == NULL) && (i_sendUpload != q_sendUpload.end()); i_sendUpload++)
    { //first check if multisendstreamer is used!
      /* four cases:
      1. both use buffer
      2. both use mssObjectString
      3. mss is queued and could be replaced by buffer
      4. buffer is queued and could be replaced by mssObjectString
        */
      if ((i_sendUpload->mssObjectString == NULL) && (rref_sendUpload.mssObjectString == NULL))
      {
        if (i_sendUpload->vec_uploadBuffer[0] == rref_sendUpload.vec_uploadBuffer[0])
        {
          uint8_t ui8_offset = (rref_sendUpload.vec_uploadBuffer[0]);
          if ( (ui8_offset<scui8_cmdCompareTableMin) || (ui8_offset > scui8_cmdCompareTableMax))
          {
              // only 0x12 is possible, but no need to override, it shouldn't occur anyway!
            if ((ui8_offset == 0x12) ||
                ((ui8_offset >= 0x60) && (ui8_offset <= 0x7F)) ) /// no checking for Proprietary commands (we don't need the replace-feature here!)
              break;

            // the rest is not possible by definition, but for being sure :-)
#ifdef DEBUG
            INTERNAL_DEBUG_DEVICE << "--INVALID COMMAND! SHOULDN'T HAPPEN!!--" << INTERNAL_DEBUG_DEVICE_ENDL;
            INTERNAL_DEBUG_DEVICE << "commandByte " << (int)ui8_offset << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
            return false;
          }
          //get bitmask for the corresponding command
          uint8_t ui8_bitmask = scpui8_cmdCompareTable [ui8_offset-scui8_cmdCompareTableMin];
          if (!(ui8_bitmask & (1<<0)))
          { // go Check for overwrite...
            for (i=1;i<=7;i++)
            {
              if (((ui8_bitmask & 1<<i) !=0) && !(i_sendUpload->vec_uploadBuffer[i] == rref_sendUpload.vec_uploadBuffer[i]))
                break;
            }
            if (!(i<=7))
            { // loop ran through, all to-compare-bytes matched!
              p_queue = &*i_sendUpload; // so overwrite this SendUpload_c with the new value one
            }
          }
        }
      }
      if ((i_sendUpload->mssObjectString != NULL) && (rref_sendUpload.mssObjectString != NULL))
      {
        if ((*i_sendUpload).mssObjectString->getStreamer()->getFirstByte() == rref_sendUpload.mssObjectString->getStreamer()->getFirstByte())
        {
          if ((*i_sendUpload).mssObjectString->getStreamer()->getID() == rref_sendUpload.mssObjectString->getStreamer()->getID())
            p_queue = &*i_sendUpload;
        }
      }
      if ((i_sendUpload->mssObjectString != NULL) && (rref_sendUpload.mssObjectString == NULL))
      {
        if ((*i_sendUpload).mssObjectString->getStreamer()->getFirstByte() == rref_sendUpload.vec_uploadBuffer[0])
        {
          if ((*i_sendUpload).mssObjectString->getStreamer()->getID() == (rref_sendUpload.vec_uploadBuffer[1] | (rref_sendUpload.vec_uploadBuffer[2]<<8)))
            p_queue = &*i_sendUpload;
        }
      }
      if ((i_sendUpload->mssObjectString == NULL) && (rref_sendUpload.mssObjectString != NULL))
      {
        if ((*i_sendUpload).vec_uploadBuffer[0] == rref_sendUpload.mssObjectString->getStreamer()->getFirstByte())
        {
          if (((*i_sendUpload).vec_uploadBuffer[1] | (*i_sendUpload).vec_uploadBuffer[2]<<8) == rref_sendUpload.mssObjectString->getStreamer()->getID())
            p_queue = &*i_sendUpload;
        }
      }
    } // for
  }
  if (p_queue == NULL)
  {
  /* The SendUpload_c constructor only takes a reference, so don't change the string in the meantime!!! */
#ifdef USE_LIST_FOR_FIFO
    // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
    q_sendUpload.push_back (rref_sendUpload);
#else
    q_sendUpload.push (rref_sendUpload);
#endif
  }
  else
    *p_queue = rref_sendUpload; // overloaded "operator="
#ifdef DEBUG_HEAP_USEAGE
  sui16_sendUploadQueueSize++;
  if (sui16_sendUploadQueueSize > sui16_maxSendUploadQueueSize)
    sui16_maxSendUploadQueueSize = sui16_sendUploadQueueSize;
#endif

#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << q_sendUpload.size() << "." << INTERNAL_DEBUG_DEVICE_ENDL;
  //dumpQueue(); /* to see all enqueued cmds after every enqueued cmd */
#endif
  /** push(...) has no return value */
  return true;
}


void
VtClientServerCommunication_c::dumpQueue()
{
#ifdef USE_LIST_FOR_FIFO
#ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  std::list<SendUpload_c,std::__malloc_alloc_template<0> >::iterator i_sendUpload;
#else
  std::list<SendUpload_c>::iterator i_sendUpload;
#endif
#else
  std::queue<SendUpload_c>::iterator i_sendUpload;
#endif

  for (i_sendUpload = q_sendUpload.begin(); i_sendUpload != q_sendUpload.end(); i_sendUpload++)
  {
    if (i_sendUpload->mssObjectString == NULL)
    {
      for (uint8_t i=0; i<=7; i++)
      {
#ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << " " << (uint16_t)(i_sendUpload->vec_uploadBuffer[i]);
#endif
      }
    }
    else
    {
      MultiSendPkg_c msp;
      int i_strSize = i_sendUpload->mssObjectString->getStreamer()->getStreamSize();
      for (int i=0; i < i_strSize; i+=7) {
        i_sendUpload->mssObjectString->getStreamer()->setDataNextStreamPart (&msp, (unsigned char) ((i_strSize - i) > 7 ? 7 : (i_strSize-i)));
        for (uint8_t j=1; j<=7; j++)
        {
#ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << " " << (uint16_t)(msp[j]);
#endif
        }
      }
    }
  }
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
}


void
VtClientServerCommunication_c::doStart()
{
  /// First, trigger sending of WS-Announce
  refc_wsMasterIdentItem.getIsoItem()->triggerWsAnnounce();
  i32_nextWsMaintenanceMsg = 0; // send out ws maintenance message immediately after ws has been announced.

  if (en_objectPoolState == OPInitial)
  { // no object pool
    en_uploadType = UploadPool;
  }
  else
  { // "OP<was>UploadedSuccessfully" || "OPCannotBeUploaded" (on the previous VT: try probably other VT now)
    en_objectPoolState = OPInitial; // try (re-)uploading, not caring if it was successfully or not on the last vt!
    en_uploadType = UploadPool;          // Start Pool Uploading sequence!!
    en_uploadPoolState = UploadPoolInit; // with "UploadInit
  }
}


void
VtClientServerCommunication_c::doStop()
{
  // VT has left the system - clear all queues now, don't wait until next re-entering (for memory reasons)
#ifdef DEBUG_HEAP_USEAGE
  sui16_sendUploadQueueSize -= q_sendUpload.size();
#endif
#ifdef USE_LIST_FOR_FIFO
  // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
  q_sendUpload.clear();
#else
  while (!q_sendUpload.empty()) q_sendUpload.pop();
#endif
  /// if any upload is in progress, abort it
  if (pc_vtServerInstance) // no Vt Alives anymore but Vt still announced
  {
    getMultiSendInstance4Comm().abortSend (refc_wsMasterIdentItem.isoName(), pc_vtServerInstance->getIsoName());
  }
  else
  {
    // vt's not announced
    // that case should be handles by the multisend itself
  }
  c_streamer.refc_pool.eventEnterSafeState();

  // set display state of the client to a defined state
  en_displayState = VtClientDisplayStateHidden;
}


void
VtClientServerCommunication_c::checkVtStateChange()
{
  bool b_vtAliveOld = b_vtAliveCurrent;
  b_vtAliveCurrent = isVtActive();

  if (!b_vtAliveOld && b_vtAliveCurrent)
  { /// OFF --> ON  ***  VT has (re-)entered the system
#ifdef DEBUG
    INTERNAL_DEBUG_DEVICE
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=========================================================================="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=== VT has entered the system, trying to receive all Properties now... ==="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=== time: " << HAL::getTime() << " ==="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=========================================================================="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;

#endif
    doStart();
  }
  else if (b_vtAliveOld && !b_vtAliveCurrent)
  { /// ON -> OFF  ***  Connection to VT lost
#ifdef DEBUG
    INTERNAL_DEBUG_DEVICE
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=============================================================================="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=== VT has left the system, clearing queues --> eventEnterSafeState called ==="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=== time: " << HAL::getTime() << " ==="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << "=============================================================================="
      << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    doStop();
  }
}


bool
VtClientServerCommunication_c::isVtActive()
{
  if (!pc_vtServerInstance) return false;
  return pc_vtServerInstance->isVtActive();
}


void
VtClientServerCommunication_c::startObjectPoolUploading (uploadPoolType_t ren_uploadPoolType, IsoAgLib::iVtObject_c** rppc_listOfUserPoolUpdateObjects, uint16_t rui16_numOfUserPoolUpdateObjects)
{
  en_uploadPoolType = ren_uploadPoolType;

  // calculate mask_stream size NOW (added 1 byte for "Object Pool Upload Start" Command Byte)
  // because we can't do before we get the color-depth information (0xC7)
  c_streamer.ui32_streamSize = 0;
  c_streamer.ui32_streamSizeLang = 0;  /// Note that SizeLang is also used for UserPartialUpdates!!!!

  if (ren_uploadPoolType == UploadPoolTypeUserPoolUpdate)
  {
    // Activate User trigger Partial Pool Update
    c_streamer.pc_userPoolUpdateObjects = rppc_listOfUserPoolUpdateObjects;

    for (uint32_t curObject=0; curObject < rui16_numOfUserPoolUpdateObjects; curObject++)
      c_streamer.ui32_streamSizeLang += ((vtObject_c*)c_streamer.pc_userPoolUpdateObjects[curObject])->fitTerminal ();
  }
  else
  {
    // Activate Initial/LangUpdate UPLOAD
    c_streamer.pc_userPoolUpdateObjects = NULL;

    // *CONDITIONALLY* Calculate GENERAL Part size (always init size to 0 above!)
    if (ren_uploadPoolType == UploadPoolTypeCompleteInitially) // was "if (!rb_onlyLanguageStream)"
    {
      for (uint32_t curObject=0; curObject < c_streamer.refc_pool.getNumObjects(); curObject++)
        c_streamer.ui32_streamSize += ((vtObject_c*)c_streamer.refc_pool.getIVtObjects()[0][curObject])->fitTerminal ();
    }

    // *ALWAYS* Calculate LANGUAGE Part size (if objectpool has multilanguage!)
    if (c_streamer.refc_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().numberOfLanguagesToFollow > 0) // supporting multilanguage.
    { // only if the objectpool has 2 or more languages, it makes sense to add the language code to the version-name
      const int8_t ci8_realUploadingLanguage = (c_streamer.i8_objectPoolUploadingLanguage < 0) ? 0 : c_streamer.i8_objectPoolUploadingLanguage;
      for (uint32_t curObject=0; curObject < c_streamer.refc_pool.getNumObjectsLang(); curObject++)
        c_streamer.ui32_streamSizeLang += ((vtObject_c*)c_streamer.refc_pool.getIVtObjects()[ci8_realUploadingLanguage+1][curObject])->fitTerminal ();
    } // else: no LANGUAGE SPECIFIC objectpool, so keep this at 0 to indicate this!

    if (ren_uploadPoolType == UploadPoolTypeCompleteInitially) // was "if (!rb_onlyLanguageStream)"
    { // Issue GetMemory-Command
      c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                            192 /* 0xC0 */, 0xff, (c_streamer.ui32_streamSize+c_streamer.ui32_streamSizeLang) & 0xFF,
                            ((c_streamer.ui32_streamSize+c_streamer.ui32_streamSizeLang) >>  8) & 0xFF,
                            ((c_streamer.ui32_streamSize+c_streamer.ui32_streamSizeLang) >> 16) & 0xFF,
                            (c_streamer.ui32_streamSize+c_streamer.ui32_streamSizeLang) >> 24, 0xff, 0xff);
      getCanInstance4Comm() << c_data;     // Command: Get Technical Data --- Parameter: Get Memory Size

      c_streamer.ui32_streamSize++; // add the 0x11 byte!

      // Now proceed to uploading
      en_uploadPoolState = UploadPoolWaitingForMemoryResponse;
      ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
      ui32_uploadTimestamp = HAL::getTime();
    }
  }

  if (c_streamer.ui32_streamSizeLang > 0)
  { // only if there's at least one object being streamed up as LANGUAGE specific objectpool add the CMD byte for size calculation...
    c_streamer.ui32_streamSizeLang++; // add the 0x11 byte!
  }
}


/// finalizeUploading() is getting called after LoadVersion or UploadPool...
void
VtClientServerCommunication_c::finalizeUploading() //bool rb_wasLanguageUpdate)
{
  if (en_uploadPoolType == UploadPoolTypeUserPoolUpdate)
  { /// Was user-pool-update
  }
  else
  { /// Was complete initial pool or language pool update.
    /// in both cases we uploaded in one specific language!! so do the following:
    c_streamer.i8_objectPoolUploadedLanguage = c_streamer.i8_objectPoolUploadingLanguage;
    c_streamer.ui16_objectPoolUploadedLanguageCode = c_streamer.ui16_objectPoolUploadingLanguageCode;
    c_streamer.i8_objectPoolUploadingLanguage = -2; // -2 indicated that the language-update while pool is up IS IDLE!
    c_streamer.ui16_objectPoolUploadingLanguageCode = 0x0000;
  #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "===> finalizeUploading () with language: "<<(int)c_streamer.i8_objectPoolUploadedLanguage;
    if (c_streamer.i8_objectPoolUploadedLanguage >= 0) INTERNAL_DEBUG_DEVICE <<" ["<<uint8_t(c_streamer.ui16_objectPoolUploadedLanguageCode>>8) <<uint8_t(c_streamer.ui16_objectPoolUploadedLanguageCode&0xFF)<<"]";
    INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
    if (en_uploadPoolType == UploadPoolTypeLanguageUpdate)
    {
      // no need to set "en_objectPoolState" and "en_uploadType", this is done in "finishUploadCommand()"
    }
    else
    {
  #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "Now en_objectPoolState = OPUploadedSuccessfully;" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
      en_objectPoolState = OPUploadedSuccessfully;
      en_uploadType = UploadIdle;
    }
    c_streamer.refc_pool.eventObjectPoolUploadedSuccessfully ((en_uploadPoolType == UploadPoolTypeLanguageUpdate), c_streamer.i8_objectPoolUploadedLanguage, c_streamer.ui16_objectPoolUploadedLanguageCode);
  }
  /// @todo for now we allow parallel uploads
  ///refc_isoTerminal.resetFlagForPoolUpload (this);
}


/** Send "End of Object Pool" message */
void
VtClientServerCommunication_c::indicateObjectPoolCompletion()
{
  // successfully sent, so now send out the "End of Object Pool Message" and wait for response!
  c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                        18, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
  getCanInstance4Comm() << c_data;     // Command: Object Pool Transfer --- Parameter: Object Pool Ready
  en_uploadPoolState = UploadPoolWaitingForEOOResponse; // and wait for response to set en_uploadState back to UploadIdle;
  ui32_uploadTimeout = DEF_TimeOut_EndOfObjectPool; // wait 10 seconds for terminal to initialize pool!
  ui32_uploadTimestamp = HAL::getTime();
}


// only being called if there IS a q_sendUpload.front()
bool
VtClientServerCommunication_c::startUploadCommand()
{
  /** @todo Up to now, none cares for the return code. implement error handling in case multisend couldn't be started? */
  // Set new state
  en_uploadType = UploadCommand;
  // along with UploadCommand ALWAYS set "en_sendSuccess", not only for Multipacket!
  en_uploadCommandState = UploadCommandWaitingForCommandResponse;

  // Get first element from queue
  SendUpload_c* actSend = &q_sendUpload.front();

  // Set time-out values
  ui32_uploadTimeout = actSend->ui32_uploadTimeout;
  ui32_uploadTimestamp = HAL::getTime();

   /// Use Multi or Single CAN-Pkgs?
  //////////////////////////////////

  if ((actSend->mssObjectString == NULL) && (actSend->vec_uploadBuffer.size() < 9))
  { /// Fits into a single CAN-Pkg!
    if (actSend->vec_uploadBuffer[0] == 0x11)
    { /// Handle special case of LanguageUpdate / UserPoolUpdate
      // There's NO response for command 0x11!
      en_uploadCommandState = UploadCommandWithoutResponse;
      if (actSend->ppc_vtObjects)
      { /// User triggered Partial Pool Update
        startObjectPoolUploading (UploadPoolTypeUserPoolUpdate, actSend->ppc_vtObjects, actSend->ui16_numObjects);
      }
      else
      { /// Language Pool Update
        // Special send object pool!
        startObjectPoolUploading (UploadPoolTypeLanguageUpdate);
      }
      if (c_streamer.ui32_streamSizeLang > 0)
      { // start LANGUAGE/USER part upload!
        return getMultiSendInstance4Comm().sendIsoTarget (refc_wsMasterIdentItem.isoName(), pc_vtServerInstance->getIsoName(), &c_streamer, ECU_TO_VT_PGN, en_sendSuccess);
      }
      else
      { // shouldn't happen, but catch case anyway!
        // nothing was sent out, so set to success so it gets removed simply...
        en_sendSuccess = __IsoAgLib::MultiSend_c::SendSuccess; // as it has been sent out right now.
        return true;
      }
    }
    else
    { /// normal 8 byte package
      // Shouldn't be less than 8, else we're messin around with vec_uploadBuffer!
      c_data.setExtCanPkg8 (7, 0, ECU_TO_VT_PGN>>8, pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(),
                            actSend->vec_uploadBuffer [0], actSend->vec_uploadBuffer [1],
                            actSend->vec_uploadBuffer [2], actSend->vec_uploadBuffer [3],
                            actSend->vec_uploadBuffer [4], actSend->vec_uploadBuffer [5],
                            actSend->vec_uploadBuffer [6], actSend->vec_uploadBuffer [7]);
      getCanInstance4Comm() << c_data;
      // Save first byte for Response-Checking!
      ui8_commandParameter = actSend->vec_uploadBuffer [0];

      en_sendSuccess = __IsoAgLib::MultiSend_c::SendSuccess; // as it has been sent out right now.
      return true;
    }
  }
  else if ((actSend->mssObjectString != NULL) && (actSend->mssObjectString->getStreamer()->getStreamSize() < 9))
  { /// Fits into a single CAN-Pkg!
    uint8_t ui8_len = actSend->mssObjectString->getStreamer()->getStreamSize();

    c_data.setExtCanPkg (7, 0, ECU_TO_VT_PGN>>8, pc_vtServerInstance->getVtSourceAddress(), refc_wsMasterIdentItem.getIsoItem()->nr(), 8); // ECU->VT PGN is ALWAYS 8 Bytes!
    actSend->mssObjectString->getStreamer()->set5ByteCommandHeader (c_data.getUint8DataPointer());
    int i=5;
    for (; i < ui8_len; i++) c_data.setUint8Data( i, actSend->mssObjectString->getStreamer()->getStringToStream() [i-5] );
    for (; i < 8;       i++) c_data.setUint8Data( i, 0xFF); // pad unused bytes with "0xFF", so CAN-Pkg is of size 8!
    getCanInstance4Comm() << c_data;
    // Save first byte for Response-Checking!
    ui8_commandParameter = actSend->mssObjectString->getStreamer()->getFirstByte();

    en_sendSuccess = __IsoAgLib::MultiSend_c::SendSuccess; // as it has been sent out right now.
    return true;
  }
  else if (actSend->mssObjectString == NULL)
  { /// Use multi CAN-Pkgs [(E)TP], doesn't fit into a single CAN-Pkg!
    // Save first byte for Response-Checking!
    ui8_commandParameter = actSend->vec_uploadBuffer [0]; // Save first byte for Response-Checking!

    return getMultiSendInstance4Comm().sendIsoTarget (refc_wsMasterIdentItem.isoName(),
                                                      pc_vtServerInstance->getIsoName(),
                                                      &actSend->vec_uploadBuffer.front(),
                                                      actSend->vec_uploadBuffer.size(),
                                                      ECU_TO_VT_PGN,
                                                      en_sendSuccess);
  }
  else
  {
    // Save first byte for Response-Checking!
    ui8_commandParameter = actSend->mssObjectString->getStreamer()->getFirstByte();

    return getMultiSendInstance4Comm().sendIsoTarget (refc_wsMasterIdentItem.isoName(),
                                                      pc_vtServerInstance->getIsoName(),
                                                      (IsoAgLib::iMultiSendStreamer_c*)actSend->mssObjectString->getStreamer(),
                                                      ECU_TO_VT_PGN,
                                                      en_sendSuccess);
  }
}


void
VtClientServerCommunication_c::finishUploadCommand()
{
  en_uploadType = UploadIdle;

  if ( !q_sendUpload.empty() )
  {

    //dumpQueue(); /* to see all left queued cmds after every dequeued cmd */
    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "Dequeued (after success, timeout, whatever..): " << q_sendUpload.size() <<" -> ";
    #endif

    #ifdef USE_LIST_FOR_FIFO
    // queueing with list: queue::push <-> list::push_back; queue::front<->list::front; queue::pop<->list::pop_front
    q_sendUpload.pop_front();
    #else
    q_sendUpload.pop();
    #endif
    #ifdef DEBUG_HEAP_USEAGE
    sui16_sendUploadQueueSize--;
    #endif

    #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << q_sendUpload.size() << "." << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif

    // trigger fast reschedule if more messages are waiting
    if ( ( getUploadBufferSize() > 0 ) && ( getIsoTerminalInstance4Comm().getTimePeriod() != 4 ) )
    { // there is a command waiting
      getSchedulerInstance().changeTimePeriodAndResortTask(&(getIsoTerminalInstance4Comm()), 4);
    }
  }
#ifdef DEBUG
  else
  {
    INTERNAL_DEBUG_DEVICE << "Attempt to Dequeue while empty!" << INTERNAL_DEBUG_DEVICE_ENDL;
  }
#endif
}


void
VtClientServerCommunication_c::vtOutOfMemory()
{  // can't (up)load the pool.
  getILibErrInstance().registerError (iLibErr_c::RemoteServiceOutOfMemory, iLibErr_c::IsoTerminal);
  en_uploadPoolState = UploadPoolFailed; // no timeout needed
  en_objectPoolState = OPCannotBeUploaded;
  /// @todo for now allow parallel uploads
  ///refc_isoTerminal.resetFlagForPoolUpload (this);
}

/** set display state of vt client */
void
VtClientServerCommunication_c::setVtDisplayState (bool b_isVtStatusMsg, uint8_t ui8_saOrDisplayState)
{
  if (en_objectPoolState != OPUploadedSuccessfully) return;
  // as we don't properly seem to reset "en_objectPoolState" at doStop(), we'll for now add the extra
  // isAddress-Claimed-check here for safety:
  if (!getIdentItem().isClaimedAddress()) return;

  vtClientDisplayState_t newDisplayState;
  if (b_isVtStatusMsg) // state change triggered from VT Status Msg
  {
//     if (getVtServerInst().getMultiViewMode())
    {
      if (ui8_saOrDisplayState == getIdentItem().getIsoItem()->nr())
        newDisplayState = VtClientDisplayStateActive;
      else
      {
        if (getVtDisplayState() == VtClientDisplayStateActive)
          // only cause state change if currently displayed is active
          newDisplayState = VtClientDisplayStateInactive;
        else
          newDisplayState = getVtDisplayState();
      }
    }
//     else
//     {
//       if (ui8_saOrDisplayState == getIdentItem().getIsoItem()->nr())
//         newDisplayState = VtClientDisplayStateActive;
//       else
//       {
//         newDisplayState = VtClientDisplayStateHidden;
//       }
//     }
  }
  else // state change triggered from Display Activation Msg
  {
    if (ui8_saOrDisplayState) // display client but no input focus
    {
      if (getVtDisplayState() == VtClientDisplayStateHidden)
        newDisplayState = VtClientDisplayStateInactive;
      else
        newDisplayState = getVtDisplayState(); // if already in state inactive or active, nothing to do
    }
    else // client is no longer displayed
    {
      if (getVtDisplayState() == VtClientDisplayStateInactive)
        newDisplayState = VtClientDisplayStateHidden;
      else
        newDisplayState = getVtDisplayState(); // if already in state hidden or active, nothing to do
    }
  }

  if (newDisplayState != getVtDisplayState())
  {
    en_displayState = newDisplayState;
    c_streamer.refc_pool.eventDisplayActivation();
  }
}


void
VtClientServerCommunication_c::setObjectPoolUploadingLanguage()
{
  c_streamer.i8_objectPoolUploadingLanguage = i8_vtLanguage;
  c_streamer.ui16_objectPoolUploadingLanguageCode = 0x0000;
  if (c_streamer.refc_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().numberOfLanguagesToFollow > 1) // supporting multilanguage.
  { // only if the objectpool has 2 or more languages, it makes sense to add the language code to the version-name
    const int8_t ci8_realUploadingLanguage = (c_streamer.i8_objectPoolUploadingLanguage < 0) ? 0 : c_streamer.i8_objectPoolUploadingLanguage;
    const uint8_t* lang = c_streamer.refc_pool.getWorkingSetObject().get_vtObjectWorkingSet_a().languagesToFollow[ci8_realUploadingLanguage].language;
    c_streamer.ui16_objectPoolUploadingLanguageCode = (lang [0] << 8) | lang[1];
  }
}

/** explicit conversion to reference of interface class type */
IsoAgLib::iVtClientServerCommunication_c& VtClientServerCommunication_c::toInterfaceReference()
{
  return static_cast<IsoAgLib::iVtClientServerCommunication_c&>(*this);
}
/** explicit conversion to reference of interface class type */
IsoAgLib::iVtClientServerCommunication_c* VtClientServerCommunication_c::toInterfacePointer()
{
  return static_cast<IsoAgLib::iVtClientServerCommunication_c*>(this);
}


uint16_t
VtClientServerCommunication_c::getVtObjectPoolSoftKeyWidth()
{
  return c_streamer.refc_pool.getSkWidth();
}


uint16_t
VtClientServerCommunication_c::getVtObjectPoolDimension()
{
  return c_streamer.refc_pool.getDimension();
}

VtServerInstance_c&
VtClientServerCommunication_c::getVtServerInst()
{ // @todo Does this make sense? pc_ may be NULL!
  return *pc_vtServerInstance;
}

uint16_t
VtClientServerCommunication_c::getVtObjectPoolSoftKeyHeight()
{
  return c_streamer.refc_pool.getSkHeight();
}


} // end namespace __IsoAgLib
