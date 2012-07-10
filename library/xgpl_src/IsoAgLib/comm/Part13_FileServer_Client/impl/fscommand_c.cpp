/*
  fscommand_c.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include <cstring>

// own
#include "fscommand_c.h"
#include "fsclientservercommunication_c.h"

// ISOAgLib
#include <IsoAgLib/comm/Part13_FileServer_Client/impl/fsmanager_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/util/iassert.h>

// debug
#if DEBUG_FILESERVER
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif

#if defined(_MSC_VER)
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib
{

const int gci_NR_REQUEST_ATTEMPTS=5;
const int gci_REQUEST_REPEAT_TIME=6500;
/* TODO may need to create a timeout based on number of bytes being read */
const int gci_REQUEST_REPEAT_BUSY_TIME=24000;

uint8_t FsCommand_c::pui8_maintenanceBuffer[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

FsCommand_c::FsCommand_c(
    FsClientServerCommunication_c &rc_inCsCom,
    FsServerInstance_c &rc_inFileserver)
  : mc_schedulerTask(*this)
  , mt_multiSendEventHandler(*this)
  , ui8_tan( 0 )
  , rc_csCom( rc_inCsCom )
  , rc_fileserver( rc_inFileserver )
  , ui8_nrOpenFiles( 0 )
  , b_keepConnectionOpen( false )
  , i32_lastAliveSent( -1 )
  , ui8_packetLength(0)
  , pui8_receiveBuffer( NULL )
  , ui32_recBufAllocSize( 0 )
  , ui8_recTan( 0 )
  , b_receivedResponse( true )
  , i32_lastrequestAttempt( -1 )
  , ui8_requestAttempts( 0 ) // value will be reset when needed
  , en_sendSuccessNotify( __IsoAgLib::SendStream_c::SendSuccess )
  , ui8_errorCode( 0 )
  , pui8_currentDirectory( NULL )
  , ui16_curDirAllocSize( 0 )
  , ui8_fileHandle( 0 )
  , pui8_fileName( NULL )
  , ui16_fileNameAllocSize( 0 )
  , ui8_flags( 0 )
  , b_archive( false )
  , b_system( false )
  , b_caseSensitive( false )
  , b_removable( false )
  , b_longFilenames( false )
  , b_isDirectory( false )
  , b_isVolume( false )
  , b_hidden( false )
  , b_readOnly( false )
  , ui8_possitionMode( 0 )
  , i32_offset( 0 )
  , ui32_possition( 0 )
  , ui16_count( 0 )
  , b_reportHiddenFiles( false )
  , pui8_data( NULL )
  , ui16_dataAllocSize( 0 )
  , v_dirData()
  , b_readDirectory( false )
  , ui8_hiddenAtt( 0 )
  , ui8_readOnlyAtt( 0 )
  , ui16_date( 0 )
  , ui16_time( 0 )
  , b_receiveFilterCreated( false )
  , mb_initialQueryStarted( false )
  , mb_initializingFileserver( rc_fileserver.isBeingInitialized() )
  , mb_waitForMultiSendFinish( false )
  , mb_retryMultiPacketSend( false)
{
  // Scheduler
  getSchedulerInstance().registerClient (&mc_schedulerTask);

#if DEBUG_FILESERVER
  INTERNAL_DEBUG_DEVICE << "FsCommand created!" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
}


FsCommand_c::~FsCommand_c()
{
  doCleanUp();

  if (b_receiveFilterCreated)
  {
    // Single-Packet
    IsoFilter_s tempIsoFilter (*this, IsoAgLib::iMaskFilter_c( 0x3FFFFFFUL, (FS_TO_CLIENT_PGN << 8) ),
                               &rc_csCom.getClientIdentItem().getIsoItem()->isoName(),
                               &rc_fileserver.getIsoName(),
                               8 );

    const bool cb_isoFilterRemoved
      = getIsoFilterManagerInstance4Comm().removeIsoFilter( tempIsoFilter );

    (void) cb_isoFilterRemoved;
    isoaglib_assert (cb_isoFilterRemoved == true);

    // Multi-Packet
    getMultiReceiveInstance4Comm().deregisterClient(*this, rc_csCom.getClientIdentItem().getIsoItem()->isoName(), FS_TO_CLIENT_PGN, 0x3FFFFLU, &getFileserver().getIsoName());
  }

  // Scheduler
  getSchedulerInstance().unregisterClient (&mc_schedulerTask);

#if DEBUG_FILESERVER
  INTERNAL_DEBUG_DEVICE << "FsCommand destroyed!" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
}


bool
FsCommand_c::timeEvent(void)
{
  if (!rc_csCom.getClientIdentItem().isClaimedAddress())
    return true;

  if (!b_receiveFilterCreated)
  {
    // Multi-Packet (completely SA->DA specific!)
    getMultiReceiveInstance4Comm().registerClientIso (*this, rc_csCom.getClientIdentItem().getIsoItem()->isoName(), FS_TO_CLIENT_PGN, 0x3FFFFLU, false, false, &getFileserver().getIsoName());

    // Single-Packet (completely SA->DA specific!)
    IsoFilter_s tempIsoFilter (*this, IsoAgLib::iMaskFilter_c( 0x3FFFFFFUL, (FS_TO_CLIENT_PGN << 8) ),
                               &rc_csCom.getClientIdentItem().getIsoItem()->isoName(),
                               &getFileserver().getIsoName(),
                               8 );

    isoaglib_assert (!getIsoFilterManagerInstance4Comm().existIsoFilter( tempIsoFilter ));
    getIsoFilterManagerInstance4Comm().insertIsoFilter( tempIsoFilter, true );

    b_receiveFilterCreated = true;
  }

  if (mb_waitForMultiSendFinish)
  { // Wait for multisend to finish...
    // If we couldn't start yet
    if (mb_retryMultiPacketSend)
    { // re-try...
      sendMultiPacketTry();
    }
    else
    { // was started, so check progress
      switch (en_sendSuccessNotify)
      {
        case SendStream_c::Running:
          /* just wait */
          break;

        case SendStream_c::SendSuccess:
          mb_waitForMultiSendFinish = false;
          // NOW we have really sent out the request
          // and can wait for the answer/timeout!
          i32_lastrequestAttempt = HAL::getTime();
          break;

        case SendStream_c::SendAborted:
          // retry (currently) unlimited
          sendMultiPacketTry();
          break;
      }
    }
  }

  /// First of all, check if we are a command for
  /// A) Querying the FS's Properties/Volumes
  /// or
  /// B) normal application commands
  if (mb_initializingFileserver)
  { // A)
    // do request the properties here, as we
    // have the IdentItem claimed definitely here!
    if (!mb_initialQueryStarted)
    {
      mb_initialQueryStarted = true;
      requestProperties();
    }
    // else: nothing to do if init sequence was started
  }
  // else B): nop right now.

  int32_t i32_requestRepeatTime = gci_REQUEST_REPEAT_TIME;
  if ( getFileserver().getBusy() )
    i32_requestRepeatTime = gci_REQUEST_REPEAT_BUSY_TIME;

  // Check for time-out
  if ( !b_receivedResponse
    && (i32_lastrequestAttempt >= 0) // command sent out completely (important if TP/ETP)
    && ((HAL::getTime() - i32_lastrequestAttempt) > i32_requestRepeatTime) )
  { // Time-out
    if (ui8_requestAttempts < gci_NR_REQUEST_ATTEMPTS)
    { // Retry
      sendRequest (RequestRetry);
    }
    else
    { // Give up
      if (mb_initializingFileserver)
      {
        getFileserver().setState (FsServerInstance_c::unusable);
        // nothing more to do, this instance needs to be removed
        // (will be done in timeEvent) because the FS failed.
        return true;
      }
      else
      {
        switch (en_lastCommand)
        {
          case en_noCommand:
            isoaglib_assert (!"Internal error. Shouldn't be in en_noCommand state when giveing up with the current command.");
            break;
          case en_getCurrentDirectory:
            rc_csCom.getCurrentDirectoryResponse(IsoAgLib::fsFileserverNotResponding, (uint8_t *)NULL);
            break;
          case en_changeCurrentDirectory:
            rc_csCom.changeCurrentDirectoryResponse(IsoAgLib::fsFileserverNotResponding, (uint8_t *)NULL);
            break;
          case en_openFile:
            rc_csCom.openFileResponse(IsoAgLib::fsFileserverNotResponding, 0, false, false, false, false, false, false, false);
            break;
          case en_seekFile:
            rc_csCom.seekFileResponse(IsoAgLib::fsFileserverNotResponding, 0);
            break;
          case en_readFile:
            if (b_readDirectory)
            {
              clearDirectoryList();

              rc_csCom.readDirectoryResponse(IsoAgLib::fsFileserverNotResponding, v_dirData);
            }
            else
              rc_csCom.readFileResponse(IsoAgLib::fsFileserverNotResponding, 0, (uint8_t *)NULL);
            break;
          case en_writeFile:
            rc_csCom.writeFileResponse(IsoAgLib::fsFileserverNotResponding, 0);
            break;
          case en_closeFile:
            rc_csCom.closeFileResponse(IsoAgLib::fsFileserverNotResponding);
            break;
          case en_moveFile:
            rc_csCom.moveFileResponse(IsoAgLib::fsFileserverNotResponding);
            break;
          case en_deleteFile:
            rc_csCom.deleteFileResponse(IsoAgLib::fsFileserverNotResponding);
            break;
          case en_getFileAttributes:
            rc_csCom.getFileAttributesResponse(IsoAgLib::fsFileserverNotResponding, false, false, false, false, false, false, false);
            break;
          case en_setFileAttributes:
            rc_csCom.setFileAttributesResponse(IsoAgLib::fsFileserverNotResponding);
            break;
          case en_getFileDateTime:
            rc_csCom.getFileDateTimeResponse(IsoAgLib::fsFileserverNotResponding, 0, 0, 0, 0, 0, 0);
            break;
          case en_initializeVolume:
            rc_csCom.initializeVolumeResponse(IsoAgLib::fsFileserverNotResponding, false, false, false, false, false, false, false);
            break;
          default:
  #if DEBUG_FILESERVER
            INTERNAL_DEBUG_DEVICE << "Repetition of command not defined!" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
            break;
        }

        b_receivedResponse = true;
        en_lastCommand = en_noCommand;
        i32_offset = 0;
      }
    }
  }

  // do we have open files or request is active? -> send maintenance messages.
  if ( (ui8_nrOpenFiles > 0) || !b_receivedResponse || b_keepConnectionOpen )
  { // Do Alive Sending
    if ( (i32_lastAliveSent == -1) || ( (HAL::getTime () - (uint32_t)i32_lastAliveSent) > 2000 ) )
    {
      CanPkgExt_c canpkgext;
      canpkgext.setExtCanPkg8(0x07, 0x00, CLIENT_TO_FS_PGN >> 8,
                              getFileserver().getIsoItem().nr(),
                              rc_csCom.getClientIdentItem().getIsoItem()->nr(),
                              pui8_maintenanceBuffer[0], pui8_maintenanceBuffer[1], pui8_maintenanceBuffer[2],
                              pui8_maintenanceBuffer[3], pui8_maintenanceBuffer[4], pui8_maintenanceBuffer[5],
                              pui8_maintenanceBuffer[6], pui8_maintenanceBuffer[7]);

      getIsoBusInstance4Comm() << canpkgext;
      i32_lastAliveSent = HAL::getTime();
    }
  }
  else
  { // No Alive Sending
    if (i32_lastAliveSent != -1)
    { // there was Alive Sending before
      doCleanUp();
      // reset timestamp so that at next timeEvent the Alive is
      // being sent out and not delayed until lastAliveSent+2000
      // and the doCleanUp() is only done once.
      i32_lastAliveSent = -1;
    }
  }
  return true;
}


bool
FsCommand_c::processPartStreamDataChunk (Stream_c& refc_stream, bool rb_isFirstChunk, bool rb_isLastChunkAndACKd)
{
  if (refc_stream.getStreamInvalid())
    return false; // don't keep the stream, we've "processed" it, so remove it (if it's the last chunk!)

  if (rb_isFirstChunk)
  {
    i32_offset = 0;
    ui8_recTan = refc_stream.get();
    if (ui8_recTan != ui8_tan)
    {
      refc_stream.setStreamInvalid();
#if DEBUG_FILESERVER
      INTERNAL_DEBUG_DEVICE << "TAN does not match expected one!" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      return false; // return value won't be interpreted on first chunk, so don't care...
    }
  }
  // else: further chunks - no checks...

  uint16_t ui16_notParsedSize
    = refc_stream.getNotParsedSize();

  if ( (uint32_t)(i32_offset + ui16_notParsedSize) > ui32_recBufAllocSize )
  {
    refc_stream.setStreamInvalid();
#if DEBUG_FILESERVER
    INTERNAL_DEBUG_DEVICE << "More data than allocated! " << ui32_recBufAllocSize << " " << uint32_t(i32_offset + ui16_notParsedSize) << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    return false; // don't keep the stream, we've processed it right now, so remove it
  }

  for (uint8_t i = 0; i < ui16_notParsedSize; ++i)
    pui8_receiveBuffer[i + i32_offset] = refc_stream.get();

  i32_offset += ui16_notParsedSize;

  if (rb_isLastChunkAndACKd)
  {
    b_receivedResponse = true;
    switch ( refc_stream.getFirstByte() )
    {
      case en_getCurrentDirectory:
        decodeGetCurrentDirectoryResponse();
        rc_csCom.getCurrentDirectoryResponse(IsoAgLib::iFsError(ui8_errorCode), pui8_currentDirectory);
        i32_offset = 0;
        en_lastCommand = en_noCommand;

        // GetCurDir response MAY indicate a ready FsCSC
        // so notify the FsCSC (which will catch double notifies)
        rc_csCom.notifyOnFsReady();

        return false; // don't keep the stream, we've processed it right now, so remove it

      case en_readFile:
        en_lastCommand = en_noCommand;
        if ( b_readDirectory )
        {
          decodeReadDirectoryResponse();
          if (mb_initializingFileserver)
          {
            closeFile(ui8_fileHandle);
          }
          else
          {
            rc_csCom.readDirectoryResponse(IsoAgLib::iFsError(ui8_errorCode), v_dirData);
          }
        }
        else
        {
          decodeReadFileResponse();
          rc_csCom.readFileResponse(IsoAgLib::iFsError(ui8_errorCode), ui16_count, pui8_data);
        }
        i32_offset = 0;
        return false; // don't keep the stream, we've processed it right now, so remove it
      default:
        return false; // don't keep the stream, we've processed it right now, so remove it
    }
  }
  return false; // don't keep the stream, we've processed it right now, so remove it
}


void
FsCommand_c::reactOnAbort (Stream_c& /*refc_stream*/)
{
  // nothing to be done, FS will hopefully send a retry.
  // if not, we will retry after a timeout, too.
}


bool
FsCommand_c::reactOnStreamStart(const ReceiveStreamIdentifier_c& /*refc_ident*/, uint32_t rui32_totalLen)
{
  const uint32_t cui32_newSize = rui32_totalLen - 1; // -1 for "FirstByte" that's already read.

  if (cui32_newSize > ui32_recBufAllocSize)
  {
    if ( pui8_receiveBuffer != NULL )
      delete [] pui8_receiveBuffer;

    ui32_recBufAllocSize = cui32_newSize;
    pui8_receiveBuffer = new uint8_t[ui32_recBufAllocSize];
  }
  return true;
}


bool
FsCommand_c::processMsg( const CanPkg_c& arc_data )
{
  CanPkgExt_c pkg( arc_data, getMultitonInst() );
  if (pkg.getUint8Data(1) != ui8_tan && pkg.getUint8Data(0) != en_requestProperties)
  {
#if DEBUG_FILESERVER
      INTERNAL_DEBUG_DEVICE << "TAN does not match expected one!" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    return true;
  }

  switch (pkg.getUint8Data(0))
  {
    case en_requestProperties:
      if (mb_initializingFileserver)
      {
        const FsServerInstance_c::FsVersion_en ce_versionNumber =
          FsServerInstance_c::FsVersion_en(pkg.getUint8Data(1));
        const uint8_t cui8_maxSimOpenFiles = pkg.getUint8Data(2);
        const uint8_t cui8_fsCapabilities = pkg.getUint8Data(3);
        // Check if that FS is okay for us to operate...
        if (ce_versionNumber >= FsServerInstance_c::FsVersionFDIS) // currently only don't support DIS fileservers...
        { // supported FileServer.
          getFileserver().setFsProperties (ce_versionNumber, cui8_maxSimOpenFiles, cui8_fsCapabilities);
          b_receivedResponse = true;
          ++ui8_tan;
          openFile((uint8_t *)"\\\\", false, false, false, true, false, true);
          return true;
        }
        else
        { // Unsupported old version of the FileServer.
          getFileserver().setState (FsServerInstance_c::unusable);
        }
      }
      // else: not initializing, ignore such a message
      return true;

    case en_changeCurrentDirectory:
      rc_csCom.changeCurrentDirectoryResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)), pui8_fileName);
      b_receivedResponse = true;
      ++ui8_tan;
      en_lastCommand = en_noCommand;
      return true;

    case en_openFile:
      decodeOpenFileResponse( pkg );

      if (ui8_errorCode == IsoAgLib::fsSuccess)
        ++ui8_nrOpenFiles;

      if (mb_initializingFileserver)
      {
        seekFile(ui8_fileHandle, 2, 0);
        return true;
      }

      en_lastCommand = en_noCommand;
      rc_csCom.openFileResponse(IsoAgLib::iFsError(ui8_errorCode), ui8_fileHandle, b_caseSensitive, b_removable, b_longFilenames, b_isDirectory,  b_isVolume, b_hidden, b_readOnly);

      return true;

    case en_seekFile:
      // in case of "isBeingInitialized" two Seek commands are being executed!
      //init case get volumes or real external seek file?
      if (mb_initializingFileserver && ui8_possitionMode == 0)
      {
        b_receivedResponse = true;
        ++ui8_tan;
	      if (ui32_possition == 0)
        {
#if DEBUG_FILESERVER
	        INTERNAL_DEBUG_DEVICE << "exploratory seek failed: using read length of 16\n" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
	        readDirectory(ui8_fileHandle, 16, true);
   	    }
     	  else
          readDirectory(ui8_fileHandle, ui32_possition, true);

        return true;
      }

      decodeSeekFileResponse( pkg );

      //init case get volumes or real external seek file?
      if (mb_initializingFileserver && ui8_possitionMode == 2)
      {
        seekFile(ui8_fileHandle, 0, 0);
      }
      else
      {
        en_lastCommand = en_noCommand;
        rc_csCom.seekFileResponse(IsoAgLib::iFsError(ui8_errorCode), ui32_possition);
      }

      return true;

    case en_readFile:
      if (6 > ui32_recBufAllocSize)
      {
        if ( pui8_receiveBuffer != NULL )
          delete [] pui8_receiveBuffer;

        ui32_recBufAllocSize = 6;
        pui8_receiveBuffer = new uint8_t[ui32_recBufAllocSize];
      }

      for (uint8_t i = 0; i < 6; ++i)
        pui8_receiveBuffer[i] = pkg.getUint8Data(i + 2);

      en_lastCommand = en_noCommand;
      if ( b_readDirectory )
      {
        decodeReadDirectoryResponse();
        if (mb_initializingFileserver)
        {
          closeFile(ui8_fileHandle);
        }
        else
        {
          rc_csCom.readDirectoryResponse(IsoAgLib::iFsError(ui8_errorCode), v_dirData);
        }
      }
      else
      {
        decodeReadFileResponse();
        rc_csCom.readFileResponse(IsoAgLib::iFsError(ui8_errorCode), ui16_count, pui8_data);
      }

      return true;

    case en_writeFile:
      rc_csCom.writeFileResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)), (pkg.getUint8Data(3) | pkg.getUint8Data(4) << 0x08));
      b_receivedResponse = true;
      ++ui8_tan;
      en_lastCommand = en_noCommand;

      return true;

    case en_closeFile:
      ui8_errorCode = pkg.getUint8Data(2);
      b_receivedResponse = true;
      ++ui8_tan;

      if ( (ui8_errorCode == IsoAgLib::fsSuccess) && (ui8_nrOpenFiles > 0) )
        --ui8_nrOpenFiles;

      //init case get volumes or real external seek file?
      if (mb_initializingFileserver)
      {
        getFileserver().setVolumes(v_dirData);
        getFileserver().setState (FsServerInstance_c::usable);
        return true;
      }

      en_lastCommand = en_noCommand;
      rc_csCom.closeFileResponse(IsoAgLib::iFsError(ui8_errorCode));

      return true;

    case en_moveFile:
      b_receivedResponse = true;
      ++ui8_tan;
      rc_csCom.moveFileResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)));
      en_lastCommand = en_noCommand;
      return true;

    case en_deleteFile:
      ++ui8_tan;
      b_receivedResponse = true;
      en_lastCommand = en_noCommand;
      rc_csCom.deleteFileResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)));
      return true;

    case en_getFileAttributes:
      ++ui8_tan;
      b_receivedResponse = true;
      decodeAttributes(pkg.getUint8Data(3));
      rc_csCom.getFileAttributesResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
      en_lastCommand = en_noCommand;
      return true;

    case en_setFileAttributes:
      ++ui8_tan;
      b_receivedResponse = true;
      rc_csCom.setFileAttributesResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)));
      en_lastCommand = en_noCommand;
      return true;

    case en_getFileDateTime:
      b_receivedResponse = true;
      ++ui8_tan;

      ui16_date = pkg.getUint8Data(3) | (pkg.getUint8Data(4) << 8);
      ui16_time = pkg.getUint8Data(5) | (pkg.getUint8Data(6) << 8);

      rc_csCom.getFileDateTimeResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)), (uint16_t)(1980 + ((ui16_date >> 9) & 0x7F)), (ui16_date >> 5) & 0xF, (ui16_date) & 0x1F, (ui16_time >> 11) & 0x1F, (ui16_time >> 5) & 0x3F, 2 * ((ui16_time) & 0x1F));
      en_lastCommand = en_noCommand;
      return true;

    case en_initializeVolume:
      b_receivedResponse = true;
      ++ui8_tan;

      decodeAttributes(pkg.getUint8Data(3));

      rc_csCom.initializeVolumeResponse(IsoAgLib::iFsError(pkg.getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
      en_lastCommand = en_noCommand;
      return true;
    default:
#if DEBUG_FILESERVER
      INTERNAL_DEBUG_DEVICE << "got message with content (decimal): " << (uint32_t)pkg.getUint8Data(0) << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      return true;
  }
}


void
FsCommand_c::sendRequest (RequestType_en aen_reqType)
{
  switch (aen_reqType)
  {
    case RequestInitial:
      ui8_requestAttempts = 1;
      break;

    case RequestRetry:
      ++ui8_requestAttempts;
      break;

    default:
      isoaglib_assert (!"INTERNAL-FAILURE: Wrong RequestType. Shouldn't happen!");
  }

#if DEBUG_FILESERVER
  INTERNAL_DEBUG_DEVICE << "Sending Request " << int(pui8_sendBuffer[0]) << " -> Fileserver [Try No. " << int(ui8_requestAttempts) << "]." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

  if (ui8_packetLength <= 8)
  {
    sendSinglePacket();

    // timestamp time of sending.
    i32_lastrequestAttempt = HAL::getTime();
    b_receivedResponse = false;
  }
  else
  {
    /// initially try to start MultiPacket sending
    sendMultiPacketTry();

    mb_waitForMultiSendFinish = true;
      // -1: Don't check timeout, wait for completion of MultiPacket first...
    i32_lastrequestAttempt = -1;
    b_receivedResponse = false;

  }
}


void
FsCommand_c::sendSinglePacket()
{
  CanPkgExt_c isoPkg;
  isoPkg.setExtCanPkg8(
    0x07,
    0x00,
    CLIENT_TO_FS_PGN >> 8,
    getFileserver().getIsoItem().nr(),
    rc_csCom.getClientIdentItem().getIsoItem()->nr(),
    pui8_sendBuffer[0], pui8_sendBuffer[1], pui8_sendBuffer[2], pui8_sendBuffer[3],
    pui8_sendBuffer[4], pui8_sendBuffer[5], pui8_sendBuffer[6], pui8_sendBuffer[7]);

  getIsoBusInstance4Comm() << isoPkg;
}


void
FsCommand_c::sendMultiPacketTry()
{
  mb_retryMultiPacketSend =
    !getMultiSendInstance4Comm().sendIsoTarget(
      rc_csCom.getClientIdentItem().getIsoItem()->isoName(),
      getFileserver().getIsoName(),
      pui8_sendBuffer,
      ui8_packetLength,
      CLIENT_TO_FS_PGN,
      &mt_multiSendEventHandler);
}


void
FsCommand_c::requestProperties()
{
  en_lastCommand = en_requestProperties;

  pui8_sendBuffer[0] = en_requestProperties;
  pui8_sendBuffer[1] = 0xFF;
  pui8_sendBuffer[2] = 0xFF;
  pui8_sendBuffer[3] = 0xFF;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest (RequestInitial);
}


void
FsCommand_c::setKeepConnectionOpen( bool b_keepOpen, bool b_forceClose )
{
  b_keepConnectionOpen = b_keepOpen;
  if ( !b_keepOpen && b_forceClose )
  {
    /* connection is being force closed, clear out number of open files */
    ui8_nrOpenFiles = 0;
  }
}


IsoAgLib::iFsCommandErrors
FsCommand_c::getCurrentDirectory()
{
  en_lastCommand = en_getCurrentDirectory;

  pui8_sendBuffer[0] = en_getCurrentDirectory;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = 0xFF;
  pui8_sendBuffer[3] = 0xFF;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest (RequestInitial);
  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::changeCurrentDirectory(uint8_t *pui8_newDirectory)
{
  en_lastCommand = en_changeCurrentDirectory;

  uint16_t ui16_length = CNAMESPACE::strlen((const char*)pui8_newDirectory);
  uint8_t ui8_bufferPosition = 0;
  if ((ui16_length + 1) > ui16_fileNameAllocSize)
  {
    if (pui8_fileName != NULL)
      delete [] pui8_fileName;
    ui16_fileNameAllocSize = ui16_length + 1;
    pui8_fileName = new uint8_t[ui16_fileNameAllocSize];
  }
  pui8_fileName[ui16_length] = 0;

  for (uint16_t i = 0; i < ui16_length; ++i)
    pui8_fileName[i] = pui8_newDirectory[i];

  pui8_sendBuffer[ui8_bufferPosition++] = en_changeCurrentDirectory;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;
  pui8_sendBuffer[ui8_bufferPosition++] = static_cast<uint8_t>(ui16_length);
  pui8_sendBuffer[ui8_bufferPosition++] = static_cast<uint8_t>(ui16_length >> 8);

  for (uint16_t i = 0; i < ui16_length; ++i)
    pui8_sendBuffer[ui8_bufferPosition + i] = pui8_fileName[i];

  ui8_packetLength = ui8_bufferPosition + ui16_length;

  sendRequest (RequestInitial);

  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::openFile(uint8_t *pui8_inFileName, bool b_openExclusive, bool b_openForAppend, bool b_createNewFile, bool b_openForReading, bool b_openForWriting, bool b_openDirectory)
{
  if (en_lastCommand == en_noCommand)
    en_lastCommand = en_openFile;

  uint16_t ui16_length = CNAMESPACE::strlen((const char*)pui8_inFileName);
  if ((ui16_length + 1) > ui16_fileNameAllocSize)
  {
    if (pui8_fileName != NULL)
      delete [] pui8_fileName;
    ui16_fileNameAllocSize = ui16_length + 1;
    pui8_fileName = new uint8_t[ui16_fileNameAllocSize];
  }
  pui8_fileName[ui16_length] = 0;
  uint8_t ui8_bufferPosition = 0;

  pui8_sendBuffer[ui8_bufferPosition++] = en_openFile;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;

  ui8_flags = 0x0;

  if (b_openExclusive)
  {
    ui8_flags = ui8_flags | (0x1 << 4);
  }
  if (b_openForAppend)
  {
    ui8_flags = ui8_flags | (0x1 << 3);
  }
  if (b_createNewFile)
  {
    ui8_flags = ui8_flags | (0x1 << 2);
  }

  if (b_openDirectory)
  {
    ui8_flags = ui8_flags | 0x3;
  }
  else if (b_openForReading && b_openForWriting)
  {
    ui8_flags = ui8_flags | 0x2;
  }
  else if (b_openForReading)
  {
  }
  else if (b_openForWriting)
  {
    ui8_flags = ui8_flags | 0x1;
  }
  else
  {
    return IsoAgLib::fsCommandWrongFlag;
  }

  pui8_sendBuffer[ui8_bufferPosition++] = ui8_flags;
  pui8_sendBuffer[ui8_bufferPosition++] = (0xFFu & ui16_length);
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_length >> 8;

  for (uint16_t i = 0; (i < ui16_length) || (5 + i < 8); ++i)
  {
    if (i < ui16_length)
    {
      pui8_fileName[i] = pui8_inFileName[i];
      pui8_sendBuffer[ui8_bufferPosition + i] = pui8_fileName[i];
    }
    else
      pui8_sendBuffer[ui8_bufferPosition + i] = 0xff;
  }

  ui8_packetLength = ui8_bufferPosition + ui16_length;

  sendRequest (RequestInitial);

  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::seekFile(uint8_t ui8_inFileHandle, uint8_t ui8_inPossitionMode, int32_t i32_inOffset)
{
  if (en_lastCommand == en_noCommand)
    en_lastCommand = en_seekFile;

  ui8_fileHandle = ui8_inFileHandle;
  ui8_possitionMode = ui8_inPossitionMode;
  i32_offset = i32_inOffset;

  pui8_sendBuffer[0] = en_seekFile;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = ui8_possitionMode;
  pui8_sendBuffer[4] = i32_offset;
  pui8_sendBuffer[5] = i32_offset >> 0x08;
  pui8_sendBuffer[6] = i32_offset >> 0x10;
  pui8_sendBuffer[7] = i32_offset >> 0x18;

  ui8_packetLength = 8;

  sendRequest (RequestInitial);

  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::readFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount)
{
  b_readDirectory = false;
  return readFile(ui8_inFileHandle, ui16_inCount, false);
}


IsoAgLib::iFsCommandErrors
FsCommand_c::readDirectory(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, bool b_inReportHiddenFiles)
{
  b_readDirectory = true;
  return readFile(ui8_inFileHandle, ui16_inCount, b_inReportHiddenFiles);
}


IsoAgLib::iFsCommandErrors
FsCommand_c::writeFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, uint8_t *pui8_inData)
{
  en_lastCommand = en_writeFile;

  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = en_writeFile;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = static_cast<uint8_t>(ui16_inCount);
  pui8_sendBuffer[4] = static_cast<uint8_t>(ui16_inCount >> 8);

  for (uint16_t ui16_writeDataSz = 0; ui16_writeDataSz < ui16_inCount; ++ui16_writeDataSz)
  {
    pui8_sendBuffer[ui16_writeDataSz + 5] = pui8_inData[ui16_writeDataSz];
  }

  ui8_packetLength = ui16_inCount + 5;

  sendRequest (RequestInitial);

  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::closeFile(uint8_t ui8_inFileHandle)
{
  en_lastCommand = en_closeFile;

  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = en_closeFile;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = 0xFF;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest (RequestInitial);
  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::moveFile(uint8_t *pui8_sourceName, uint8_t *pui8_destName, bool b_recursive, bool b_force, bool b_copy)
{
  uint8_t ui8_bufferPosition = 0;

  en_lastCommand = en_moveFile;

  pui8_sendBuffer[ui8_bufferPosition++] = en_moveFile;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;

  uint8_t ui8_fileHandleMode = 0x00;

  if (b_recursive)
    ui8_fileHandleMode = ui8_fileHandleMode | 0x04;
  if (b_force)
    ui8_fileHandleMode = ui8_fileHandleMode | 0x02;
  if (b_copy)
    ui8_fileHandleMode = ui8_fileHandleMode | 0x01;

  pui8_sendBuffer[ui8_bufferPosition++] = ui8_fileHandleMode;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);
  uint16_t ui16_destLength = CNAMESPACE::strlen((const char *)pui8_destName);

  pui8_sendBuffer[ui8_bufferPosition++] = (0xFFu & ui16_srcLength);
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength >> 0x08;
  pui8_sendBuffer[ui8_bufferPosition++] = (0xFFu & ui16_destLength);
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_destLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[ui8_bufferPosition + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  for (uint16_t ui16_iDst = 0; ui16_iDst < ui16_destLength; ++ui16_iDst)
    pui8_sendBuffer[ui8_bufferPosition + ui16_srcLength + ui16_iDst] = pui8_destName[ui16_iDst];

  ui8_packetLength = ui8_bufferPosition + ui16_srcLength + ui16_destLength;

  sendRequest (RequestInitial);
  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::deleteFile (uint8_t *pui8_sourceName, bool b_recursive, bool b_force)
{
  uint8_t ui8_bufferPosition = 0;

  en_lastCommand = en_deleteFile;

  pui8_sendBuffer[ui8_bufferPosition++] = en_deleteFile;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;

  uint8_t ui8_fileHandleMode = 0x00;

  if (b_recursive)
    ui8_fileHandleMode = ui8_fileHandleMode | 0x04;
  if (b_force)
    ui8_fileHandleMode = ui8_fileHandleMode | 0x02;

  pui8_sendBuffer[ui8_bufferPosition++] = ui8_fileHandleMode;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[ui8_bufferPosition++] = (0xFFu & ui16_srcLength);
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength >> 8;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[ui8_bufferPosition + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = ui8_bufferPosition + ui16_srcLength;

  sendRequest (RequestInitial);
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors
FsCommand_c::getFileAttributes(uint8_t *pui8_sourceName)
{
  en_lastCommand = en_getFileAttributes;

  pui8_sendBuffer[0] = en_getFileAttributes;
  pui8_sendBuffer[1] = ui8_tan;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[2] = static_cast<uint8_t>(ui16_srcLength);
  pui8_sendBuffer[3] = static_cast<uint8_t>(ui16_srcLength >> 8);

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[4 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 4 + ui16_srcLength;

  sendRequest (RequestInitial);
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors
FsCommand_c::setFileAttributes(uint8_t *pui8_sourceName, uint8_t  ui8_inHiddenAtt, uint8_t ui8_inReadOnlyAtt)
{
  en_lastCommand = en_setFileAttributes;
  ui8_hiddenAtt = ui8_inHiddenAtt;
  ui8_readOnlyAtt = ui8_inReadOnlyAtt;

  pui8_sendBuffer[0] = en_setFileAttributes;
  pui8_sendBuffer[1] = ui8_tan;

  pui8_sendBuffer[2] = 0xF0 | ui8_hiddenAtt << 2 | ui8_readOnlyAtt;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[3] = static_cast<uint8_t>(ui16_srcLength);
  pui8_sendBuffer[4] = static_cast<uint8_t>(ui16_srcLength >> 8);

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[5 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 5 + ui16_srcLength;

  sendRequest (RequestInitial);
  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors
FsCommand_c::getFileDateTime(uint8_t *pui8_sourceName)
{
  en_lastCommand = en_getFileDateTime;

  pui8_sendBuffer[0] = en_getFileDateTime;
  pui8_sendBuffer[1] = ui8_tan;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[2] = static_cast<uint8_t>(ui16_srcLength);
  pui8_sendBuffer[3] = static_cast<uint8_t>(ui16_srcLength >> 8);

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[4 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 4 + ui16_srcLength;

  sendRequest (RequestInitial);
  return IsoAgLib::fsCommandNoError;
}


IsoAgLib::iFsCommandErrors FsCommand_c::initializeVolume(
#if DEBUG_FILESERVER
  uint8_t *pui8_pathName, uint32_t ui32_space, bool b_createVolumeUsingSpace, bool b_createNewVolume
#else
  uint8_t * /*pui8_pathName*/, uint32_t /*ui32_space*/, bool /*b_createVolumeUsingSpace*/, bool /*b_createNewVolume*/
#endif
)
{
  en_lastCommand = en_initializeVolume;

//TODO implement this one...

#if DEBUG_FILESERVER
  INTERNAL_DEBUG_DEVICE << "NOT IMPLEMENTED initializeVolume pathname: " << pui8_pathName << " space: " << ui32_space << " attributes: " << b_createVolumeUsingSpace << " " << b_createNewVolume << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  return IsoAgLib::fsCommandNoError;
}


void
FsCommand_c::decodeAttributes(uint8_t ui8_attributes)
{
  b_caseSensitive = ((ui8_attributes & 0x80) != 0);
  b_removable = ((ui8_attributes & 0x40) == 0);
  b_longFilenames = ((ui8_attributes & 0x20) != 0);
  b_isDirectory = ((ui8_attributes & 0x10) != 0);
  b_isVolume = ((ui8_attributes & 0x8) != 0);
  b_hidden = ((ui8_attributes & 0x2) != 0);
  b_readOnly = ((ui8_attributes & 0x1) != 0);
}


void
FsCommand_c::decodeGetCurrentDirectoryResponse()
{
  uint16_t ui16_length;
  uint8_t *pui8_receivePointer;

  ui8_errorCode = pui8_receiveBuffer[0];

  ui16_length = pui8_receiveBuffer[9] | (pui8_receiveBuffer[10] << 8);
  pui8_receivePointer = &pui8_receiveBuffer[11];

  b_receivedResponse = true;
  ++ui8_tan;
  if ((ui16_length + 1) > ui16_curDirAllocSize)
  {
    if (pui8_currentDirectory != NULL)
      delete [] pui8_currentDirectory;
    ui16_curDirAllocSize = ui16_length + 1;
    pui8_currentDirectory = new uint8_t[ui16_curDirAllocSize];
  }
  pui8_currentDirectory[ui16_length] = 0;

  for (uint16_t i = 0; i < ui16_length; ++i)
  {
    pui8_currentDirectory[i] = *pui8_receivePointer;
    ++pui8_receivePointer;
  }
}


void
FsCommand_c::decodeOpenFileResponse( const CanPkg_c& arc_data )
{
  b_receivedResponse = true;
  ++ui8_tan;
  ui8_errorCode = arc_data.getUint8Data(2);
  ui8_fileHandle = arc_data.getUint8Data(3);
  decodeAttributes(arc_data.getUint8Data(4));
}


void
FsCommand_c::decodeSeekFileResponse( const CanPkg_c& arc_data )
{
  b_receivedResponse = true;
  ++ui8_tan;
  ui8_errorCode = arc_data.getUint8Data(2);

  ui32_possition = static_cast<uint32_t>(arc_data.getUint8Data(4)) | (static_cast<uint32_t>(arc_data.getUint8Data(5)) << 0x08) | (static_cast<uint32_t>(arc_data.getUint8Data(6)) << 0x10) | (static_cast<uint32_t>(arc_data.getUint8Data(7)) << 0x18);
}


void
FsCommand_c::decodeReadFileResponse()
{
  b_receivedResponse = true;
  ++ui8_tan;

  ui8_errorCode = pui8_receiveBuffer[0];
  ui16_count = pui8_receiveBuffer[1] | (pui8_receiveBuffer[2] << 0x08);

  if (ui16_count > ui16_dataAllocSize)
  {
    if (pui8_data != NULL)
      delete [] pui8_data;
    ui16_dataAllocSize = ui16_count;
    pui8_data = new uint8_t[ui16_dataAllocSize];
  }

  if ( ((uint32_t)ui16_count + 3) > ui32_recBufAllocSize )
  {
    ui16_count = ui32_recBufAllocSize - 3;
#if DEBUG_FILESERVER
    INTERNAL_DEBUG_DEVICE << "message size larger then allocated buffer " << ui32_recBufAllocSize << " " << uint32_t(ui16_count + 3) << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  }

  for (uint16_t i = 0; i < ui16_count; ++i)
  {
    pui8_data[i] = pui8_receiveBuffer[i + 3];
  }
}


void
FsCommand_c::decodeReadDirectoryResponse()
{
  b_receivedResponse = true;
  ++ui8_tan;
  IsoAgLib::iFsDirectoryPtr ps_tmpDir;

  ui8_errorCode = pui8_receiveBuffer[0];

  clearDirectoryList();

  i32_offset = 3;

  for (uint16_t ui16_nrEntries = 0; ui16_nrEntries < ui16_count; ++ui16_nrEntries)
  {
    if ((uint32_t)i32_offset >= ui32_recBufAllocSize)
      break;

    ps_tmpDir = new IsoAgLib::iFsDirectory();

    ps_tmpDir->pui8_filename = new uint8_t[pui8_receiveBuffer[i32_offset] + 1];
    ps_tmpDir->pui8_filename[pui8_receiveBuffer[i32_offset]] = 0;

    for (uint8_t ui8_nameLength = 0; ui8_nameLength < pui8_receiveBuffer[i32_offset]; ++ui8_nameLength)
      ps_tmpDir->pui8_filename[ui8_nameLength] = pui8_receiveBuffer[ui8_nameLength + i32_offset + 1];

    i32_offset += (pui8_receiveBuffer[i32_offset] + 1);
    if ((uint32_t)i32_offset >= ui32_recBufAllocSize)
      break;

    decodeAttributes(pui8_receiveBuffer[i32_offset]);

    ps_tmpDir->b_caseSensitive = b_caseSensitive;
    ps_tmpDir->b_removable = b_removable;
    ps_tmpDir->b_longFilenames = b_longFilenames;
    ps_tmpDir->b_isDirectory = b_isDirectory;
    ps_tmpDir->b_isVolume = b_isVolume;
    ps_tmpDir->b_hidden = b_hidden;
    ps_tmpDir->b_readOnly = b_readOnly;

    ++i32_offset;
    if ((uint32_t)(i32_offset + 1) >= ui32_recBufAllocSize)
      break;

    ps_tmpDir->ui16_date = pui8_receiveBuffer[i32_offset] | (pui8_receiveBuffer[i32_offset + 1] << 0x08);

    i32_offset += 2;
    if ((uint32_t)(i32_offset + 1) >= ui32_recBufAllocSize)
      break;

    ps_tmpDir->ui16_time = pui8_receiveBuffer[i32_offset] | (pui8_receiveBuffer[i32_offset + 1] << 0x08);

    i32_offset += 2;
    if ((uint32_t)(i32_offset + 3) >= ui32_recBufAllocSize)
      break;

    ps_tmpDir->ui32_size = static_cast<uint32_t>(pui8_receiveBuffer[i32_offset]) | (static_cast<uint32_t>(pui8_receiveBuffer[i32_offset + 1]) << 0x08) | (static_cast<uint32_t>(pui8_receiveBuffer[i32_offset + 2]) << 0x10) | (static_cast<uint32_t>(pui8_receiveBuffer[i32_offset + 3]) << 0x18);

    i32_offset += 4;

    v_dirData.push_back(ps_tmpDir);
  }
}


void
FsCommand_c::clearDirectoryList()
{
  IsoAgLib::iFsDirectoryPtr ps_tmpDir;
  while (!v_dirData.empty())
  {
    ps_tmpDir = v_dirData.back();
    v_dirData.pop_back();
    if (ps_tmpDir->pui8_filename != NULL)
      delete [] ps_tmpDir->pui8_filename;
    delete ps_tmpDir;
  }
}


void
FsCommand_c::doCleanUp()
{
  if (pui8_receiveBuffer != NULL)
  {
    delete [] pui8_receiveBuffer;
    pui8_receiveBuffer = NULL;
  }
  ui32_recBufAllocSize = 0;

  if (pui8_currentDirectory != NULL)
  {
    delete [] pui8_currentDirectory;
    pui8_currentDirectory = NULL;
  }
  ui16_curDirAllocSize = 0;

  if (pui8_fileName != NULL)
  {
    delete [] pui8_fileName;
    pui8_fileName = NULL;
  }
  ui16_fileNameAllocSize = 0;

  if (pui8_data != NULL)
  {
    delete [] pui8_data;
    pui8_data = NULL;
  }
  ui16_dataAllocSize = 0;

  clearDirectoryList();
}


IsoAgLib::iFsCommandErrors
FsCommand_c::readFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, bool b_inReportHiddenFiles)
{
  if (en_lastCommand == en_noCommand)
    en_lastCommand = en_readFile;

  ui8_fileHandle = ui8_inFileHandle;
  ui16_count = ui16_inCount;
  b_reportHiddenFiles = b_inReportHiddenFiles;
  uint8_t ui8_bufferPosition = 0;

  pui8_sendBuffer[ui8_bufferPosition++] = en_readFile;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_fileHandle;
  pui8_sendBuffer[ui8_bufferPosition++] = static_cast<uint8_t>(ui16_count);
  pui8_sendBuffer[ui8_bufferPosition++] = static_cast<uint8_t>(ui16_count >> 8);

  pui8_sendBuffer[ui8_bufferPosition++] = b_inReportHiddenFiles;

  while (ui8_bufferPosition < 8)
    pui8_sendBuffer[ui8_bufferPosition++] = 0xFF;

  ui8_packetLength = ui8_bufferPosition;

  sendRequest (RequestInitial);

  return IsoAgLib::fsCommandNoError;
}

void
FsCommand_c::reactOnStateChange(const SendStream_c& sendStream)
{
  en_sendSuccessNotify = sendStream.getSendSuccess();
}

} // __IsoAgLib
