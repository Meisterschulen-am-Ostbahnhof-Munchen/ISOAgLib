/*
  fscommand_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include <cstring>

#include "fscommand_c.h"
#include "fsclientservercommunication_c.h"
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#if DEBUG_FILESERVER
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


#define NR_REQUEST_ATTEMPTS 5
#define REQUEST_REPEAT_TIME 6500
/* TODO may need to create a timeout based on number of bytes being read */
#define REQUEST_REPEAT_BUSY_TIME 24000

namespace __IsoAgLib
{

uint8_t FsCommand_c::pui8_maintenanceBuffer[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

bool FsCommand_c::timeEvent(void)
{
  if (!rc_csCom.getClientIdentItem().isClaimedAddress()) return true;

  if (!b_receiveFilterCreated)
  {
    // Multi-Packet
    getMultiReceiveInstance4Comm().registerClientIso (*this, rc_csCom.getClientIdentItem().getIsoItem()->isoName(), FS_TO_CLIENT_PGN);

    // Single-Packet
    IsoFilter_s tempIsoFilter (*this, 0x3FFFF00UL, (FS_TO_CLIENT_PGN << 8),
                               &rc_csCom.getClientIdentItem().getIsoItem()->isoName(),
                               NULL,
                               8, Ident_c::ExtendedIdent);

    if (!getIsoFilterManagerInstance4Comm().existIsoFilter( tempIsoFilter ))
    { // no suitable FilterBox_c exist -> create it
      getIsoFilterManagerInstance4Comm().insertIsoFilter( tempIsoFilter, true );
    }

    b_receiveFilterCreated = true;
  }

  int32_t i32_requestRepeatTime = REQUEST_REPEAT_TIME;
  if ( rc_filerserver.getBusy() )
    i32_requestRepeatTime = REQUEST_REPEAT_BUSY_TIME;

  if (!b_receivedResponse && ((HAL::getTime() - i32_lastrequestAttempt) > i32_requestRepeatTime) )
  {
    if (ui8_requestAttempts < NR_REQUEST_ATTEMPTS)
    {
      sendRequest();
    }
    else
    {
      switch (en_lastCommand)
      {
        case en_noCommand:
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
          EXTERNAL_DEBUG_DEVICE << "Repetition of command not defined!" << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif
          break;
      }

      b_receivedResponse = true;
      en_lastCommand = en_noCommand;
      ui8_requestAttempts = 0;
      i32_offset = 0;
    }
  }

  // do we have open files or request is active? -> send maintenance messages.
  if ( (ui8_nrOpenFiles > 0) || !b_receivedResponse || b_keepConnectionOpen )
  {
    if ( (i32_lastAliveSent == -1) || ( (HAL::getTime () - (uint32_t)i32_lastAliveSent) > 2000 ) )
    {
      CANPkgExt_c canpkgext;
      canpkgext.setExtCanPkg8(0x07, 0x00, CLIENT_TO_FS_PGN >> 8,
                              rc_filerserver.getIsoItem().nr(),
                              rc_csCom.getClientIdentItem().getIsoItem()->nr(),
                              pui8_maintenanceBuffer[0], pui8_maintenanceBuffer[1], pui8_maintenanceBuffer[2],
                              pui8_maintenanceBuffer[3], pui8_maintenanceBuffer[4], pui8_maintenanceBuffer[5],
                              pui8_maintenanceBuffer[6], pui8_maintenanceBuffer[7]);

      getCanInstance4Comm() << canpkgext;
      i32_lastAliveSent = HAL::getTime();
    }
  }
  else
  {
    if ( (i32_lastAliveSent != -1) && ( (HAL::getTime () - (uint32_t)i32_lastAliveSent) > 6000 ) )
    {
      doneCleanUp();
      i32_lastAliveSent = -1;
    }
  }
  return true;
}

//process packet-stream. after completion, react on message.
bool
FsCommand_c::processPartStreamDataChunk (Stream_c& refc_stream, bool rb_isFirstChunk, bool rb_isLastChunkAndACKd)
{
  uint16_t ui16_notParsedSize;

  if (refc_stream.getStreamInvalid())
    return false; // don't keep the stream, we've processed it right now, so remove it

  if (rb_isFirstChunk)
  {
    i32_offset = 0;
    ui8_recTan = refc_stream.get();
    if (ui8_recTan != ui8_tan)
    {
#if DEBUG_FILESERVER
      EXTERNAL_DEBUG_DEVICE << "TAN does not match expected one!" << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif
      return false; // don't keep the stream, we've processed it right now, so remove it
    }
  }
  else
  {
    if (ui8_recTan != ui8_tan)
      return false; // don't keep the stream, we've processed it right now, so remove it
  }

  ui16_notParsedSize = refc_stream.getNotParsedSize();

  if ( (uint32_t)(i32_offset + ui16_notParsedSize) > ui32_recBufAllocSize )
  {
#if DEBUG_FILESERVER
    EXTERNAL_DEBUG_DEVICE << "More data than allocated! " << ui32_recBufAllocSize << " " << uint32_t(i32_offset + ui16_notParsedSize) << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif

    return false; // don't keep the stream, we've processed it right now, so remove it
  }

  for (uint8_t i = 0; i < ui16_notParsedSize; ++i)
    pui8_receiveBuffer[i + i32_offset] = refc_stream.get();

  i32_offset += ui16_notParsedSize;

  if (rb_isLastChunkAndACKd)
  {
    b_receivedResponse = true;
    ui8_requestAttempts = 0;
    switch ( refc_stream.getFirstByte() )
    {
      //get current directroy
      case 0x10:
        decodeGetCurrentDirectoryResponse();
        rc_csCom.getCurrentDirectoryResponse(IsoAgLib::iFsError(ui8_errorCode), pui8_currentDirectory);
        i32_offset = 0;
        en_lastCommand = en_noCommand;
        return false; // don't keep the stream, we've processed it right now, so remove it
      //read file response
      case 0x22:
        en_lastCommand = en_noCommand;
        if ( b_readDirectory )
        {
          decodeReadDirectoryResponse();
          if (rc_filerserver.getInitStatus() == FsServerInstance_c::online)
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


//nothing to be done, new message will be sent periodically.
void FsCommand_c::reactOnAbort (Stream_c& /*refc_stream*/)
{
}

//initializes the buffer for a message stream
bool FsCommand_c::reactOnStreamStart(const ReceiveStreamIdentifier_c& /*refc_ident*/, uint32_t rui32_totalLen)
{
  uint32_t ui32_tempSize = rui32_totalLen - 1;

  if (ui32_tempSize > ui32_recBufAllocSize)
  {
    if ( pui8_receiveBuffer != NULL )
      delete [] pui8_receiveBuffer;

    ui32_recBufAllocSize = ui32_tempSize;
    pui8_receiveBuffer = new uint8_t[ui32_recBufAllocSize];
  }
  return true;
}

//process single packages.
bool FsCommand_c::processMsg()
{
  if (data().getUint8Data(1) != ui8_tan && data().getUint8Data(0) != 0x00  && data().getUint8Data(0) != 0x01)
  {
#if DEBUG_FILESERVER
      EXTERNAL_DEBUG_DEVICE << "TAN does not match expected one!" << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif
    return true;
  }

  switch (data().getUint8Data(0))
  {
    case 0x00:
      if (rc_filerserver.getInitStatus() == FsServerInstance_c::offline)
      {
        rc_filerserver.setPropertiesReqeusted();
        initFileserver();
      }
      rc_filerserver.updateServerStatus(data().getUint8Data(1), data().getUint8Data(2), HAL::getTime());
      return true;
    //get current directory
    case 0x01:
      rc_filerserver.setFsProperties(data().getUint8Data(1), data().getUint8Data(2), data().getUint8Data(3));
      i8_fsVersion = data().getUint8Data(1);
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ++ui8_tan;
      openFile((uint8_t *)"\\\\", false, false, false, true, false, true);
      return true;
    //change current directory
    case 0x11:
      rc_csCom.changeCurrentDirectoryResponse(IsoAgLib::iFsError(data().getUint8Data(2)), pui8_fileName);
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ++ui8_tan;
      en_lastCommand = en_noCommand;
      return true;
    //open file
    case 0x20:
      decodeOpenFileResponse();

      if (ui8_errorCode == IsoAgLib::fsSuccess)
        ++ui8_nrOpenFiles;

      //init case get volumes or real external open file?
      if (rc_filerserver.getInitStatus() == FsServerInstance_c::online)
      {
        seekFile(ui8_fileHandle, 2, 0);
      }
      else if (en_lastCommand == en_getFileAttributes && ui8_errorCode && (pui8_fileName == NULL))
      {
        en_lastCommand = en_noCommand;
        rc_csCom.getFileAttributesResponse(IsoAgLib::iFsError(ui8_errorCode), false, false, false, false, false, false, false);
      }
      else if (en_lastCommand == en_setFileAttributes && ui8_errorCode && (pui8_fileName == NULL))
      {
        en_lastCommand = en_noCommand;
        rc_csCom.setFileAttributesResponse(IsoAgLib::iFsError(ui8_errorCode));
      }
      else if (en_lastCommand == en_getFileDateTime && ui8_errorCode && (pui8_fileName == NULL))
      {
        en_lastCommand = en_noCommand;
        rc_csCom.getFileDateTimeResponse(IsoAgLib::iFsError(ui8_errorCode), 0, 0, 0, 0, 0, 0);
      }
      else if ((en_lastCommand == en_getFileAttributes ||
                en_lastCommand == en_setFileAttributes ||
                en_lastCommand == en_getFileDateTime) &&
                ui8_errorCode && pui8_fileName)
      {
        openFile(pui8_fileName, false, true, false, false, false, true);

        if (pui8_fileName != NULL)
          delete [] pui8_fileName;
        pui8_fileName = (uint8_t *)NULL;
      }
      else if (en_lastCommand == en_getFileAttributes)
      {
        getFileAttributesDIS(ui8_fileHandle);
      }
      else if (en_lastCommand == en_setFileAttributes)
      {
        setFileAttributesDIS(ui8_fileHandle);
      }
      else if (en_lastCommand == en_getFileDateTime)
      {
        getFileDateTimeDIS(ui8_fileHandle);
      }
      else
      {
        en_lastCommand = en_noCommand;
        rc_csCom.openFileResponse(IsoAgLib::iFsError(ui8_errorCode), ui8_fileHandle, b_caseSensitive, b_removable, b_longFilenames, b_isDirectory,  b_isVolume, b_hidden, b_readOnly);
      }

      return true;
    //seek file
    case 0x21:
      //init case get volumes or real external seek file?
      if (rc_filerserver.getInitStatus() == FsServerInstance_c::online && ui8_possitionMode == 0)
      {
        b_receivedResponse = true;
        ui8_requestAttempts = 0;
        ++ui8_tan;
	      if (ui32_possition == 0)
        {
#if DEBUG_FILESERVER
	        STL_NAMESPACE::cerr << "exploratory seek failed: using read length of 16\n" << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif
	        readDirectory(ui8_fileHandle, 16, true);
   	    }
     	  else
          readDirectory(ui8_fileHandle, ui32_possition, true);

        return true;
      }

      decodeSeekFileResponse();

      //init case get volumes or real external seek file?
      if (rc_filerserver.getInitStatus() == FsServerInstance_c::online && ui8_possitionMode == 2)
      {
        seekFile(ui8_fileHandle, 0, 0);
      }
      else
      {
        en_lastCommand = en_noCommand;
        rc_csCom.seekFileResponse(IsoAgLib::iFsError(ui8_errorCode), ui32_possition);
      }

      return true;

    //read file
    case 0x22:
      if (6 > ui32_recBufAllocSize)
      {
        if ( pui8_receiveBuffer != NULL )
          delete [] pui8_receiveBuffer;

        ui32_recBufAllocSize = 6;
        pui8_receiveBuffer = new uint8_t[ui32_recBufAllocSize];
      }

      for (uint8_t i = 0; i < 6; ++i)
        pui8_receiveBuffer[i] = data().getUint8Data(i + 2);

      en_lastCommand = en_noCommand;
      if ( b_readDirectory )
      {
        decodeReadDirectoryResponse();
        if (rc_filerserver.getInitStatus() == FsServerInstance_c::online)
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
    //write file
    case 0x23:
      rc_csCom.writeFileResponse(IsoAgLib::iFsError(data().getUint8Data(2)), (data().getUint8Data(3) | data().getUint8Data(4) << 0x08));
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ++ui8_tan;
      en_lastCommand = en_noCommand;

      return true;
    //close file
    case 0x24:
      ui8_errorCode = data().getUint8Data(2);
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ++ui8_tan;

      if ( (ui8_errorCode == IsoAgLib::fsSuccess) && (ui8_nrOpenFiles > 0) )
        --ui8_nrOpenFiles;

      //init case get volumes or real external seek file?
      if ((rc_filerserver.getInitStatus() == FsServerInstance_c::online) && (ui8_errorCode == IsoAgLib::fsSuccess))
      {
        rc_filerserver.setVolumes(v_dirData);
      }
      else if (en_lastCommand == en_getFileAttributes)
      {
        en_lastCommand = en_noCommand;
        rc_csCom.getFileAttributesResponse(IsoAgLib::iFsError(data().getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
      }
      else if (en_lastCommand == en_getFileAttributes)
      {
        en_lastCommand = en_noCommand;
        rc_csCom.setFileAttributesResponse(IsoAgLib::iFsError(data().getUint8Data(2)));
      }
      else if (en_lastCommand == en_getFileDateTime)
      {
        en_lastCommand = en_noCommand;

        rc_csCom.getFileDateTimeResponse(IsoAgLib::iFsError(data().getUint8Data(2)), (uint16_t)(1980 + ((ui16_date >> 9) & 0x7F)), (ui16_date >> 5) & 0xF, (ui16_date) & 0x1F, (ui16_time >> 11) & 0x1F, (ui16_time >> 5) & 0x3F, 2 * ((ui16_time) & 0x1F));
      }
      else
      {
        en_lastCommand = en_noCommand;
        rc_csCom.closeFileResponse(IsoAgLib::iFsError(ui8_errorCode));
      }

      return true;
    //move file
    case 0x30:
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ++ui8_tan;

      if (en_lastCommand == en_moveFile)
        rc_csCom.moveFileResponse(IsoAgLib::iFsError(data().getUint8Data(2)));
      else if (en_lastCommand == en_deleteFile)
        rc_csCom.deleteFileResponse(IsoAgLib::iFsError(data().getUint8Data(2)));

      en_lastCommand = en_noCommand;
      return true;
    //delete file
    case 0x31:
      ++ui8_tan;
      ui8_requestAttempts = 0;

      if (en_lastCommand == en_deleteFile)
      {
        b_receivedResponse = true;
        en_lastCommand = en_noCommand;
        rc_csCom.deleteFileResponse(IsoAgLib::iFsError(data().getUint8Data(2)));
      }
      else if (en_lastCommand == en_getFileAttributes)
      {
        decodeAttributes(data().getUint8Data(3));
        closeFile(ui8_fileHandle);
      }
      return true;
    //get file attributes
    case 0x32:
      ui8_requestAttempts = 0;
      ++ui8_tan;

      if (en_lastCommand == en_getFileAttributes)
      {
        b_receivedResponse = true;
        decodeAttributes(data().getUint8Data(3));
        rc_csCom.getFileAttributesResponse(IsoAgLib::iFsError(data().getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
        en_lastCommand = en_noCommand;
      }
      else if (en_lastCommand == en_setFileAttributes)
      {
        closeFile(ui8_fileHandle);
      }
      return true;
    //set file attributes
    case 0x33:
      ui8_requestAttempts = 0;
      ++ui8_tan;

      if (en_lastCommand == en_setFileAttributes)
      {
        b_receivedResponse = true;

        rc_csCom.setFileAttributesResponse(IsoAgLib::iFsError(data().getUint8Data(2)));
        en_lastCommand = en_noCommand;
      }
      else if (en_lastCommand == en_getFileDateTime)
      {
        ui16_date = data().getUint8Data(3) | (data().getUint8Data(4) << 8);
        ui16_time = data().getUint8Data(5) | (data().getUint8Data(6) << 8);
        closeFile(ui8_fileHandle);
      }
      return true;
    //get file date and time.
    case 0x34:
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ++ui8_tan;

      ui16_date = data().getUint8Data(3) | (data().getUint8Data(4) << 8);
      ui16_time = data().getUint8Data(5) | (data().getUint8Data(6) << 8);

      rc_csCom.getFileDateTimeResponse(IsoAgLib::iFsError(data().getUint8Data(2)), (uint16_t)(1980 + ((ui16_date >> 9) & 0x7F)), (ui16_date >> 5) & 0xF, (ui16_date) & 0x1F, (ui16_time >> 11) & 0x1F, (ui16_time >> 5) & 0x3F, 2 * ((ui16_time) & 0x1F));
      en_lastCommand = en_noCommand;
      return true;
    //init volume
    case 0x40:
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ++ui8_tan;

      decodeAttributes(data().getUint8Data(3));

      rc_csCom.initializeVolumeResponse(IsoAgLib::iFsError(data().getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
      en_lastCommand = en_noCommand;
      return true;
    default:
#if DEBUG_FILESERVER
      EXTERNAL_DEBUG_DEVICE << "got message with content: " << STL_NAMESPACE::hex << (uint32_t)data().getUint8Data(0) << STL_NAMESPACE::dec << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif
      return true;
  }
}

//sends request
void FsCommand_c::sendRequest()
{
  b_receivedResponse = false;
  i32_lastrequestAttempt = HAL::getTime();
  ++ui8_requestAttempts;

  //single packet
  if (ui8_packetLength <= 8)
  {
    CANPkgExt_c canpkgext;

    canpkgext.setExtCanPkg8 (0x07, 0x00, CLIENT_TO_FS_PGN >> 8, rc_filerserver.getIsoItem().nr(), rc_csCom.getClientIdentItem().getIsoItem()->nr(),
                             pui8_sendBuffer[0], pui8_sendBuffer[1], pui8_sendBuffer[2], pui8_sendBuffer[3],
                             pui8_sendBuffer[4], pui8_sendBuffer[5], pui8_sendBuffer[6], pui8_sendBuffer[7]);

    getCanInstance4Comm() << canpkgext;
  //multi-packet
  }
  else
  {
    getMultiSendInstance4Comm().sendIsoTarget(rc_csCom.getClientIdentItem().getIsoItem()->isoName(),
        rc_filerserver.getIsoName(),
        pui8_sendBuffer,
        ui8_packetLength,
        CLIENT_TO_FS_PGN,
        en_sendSuccessNotify);
  }
}

void FsCommand_c::close()
{
}

void FsCommand_c::initFileserver()
{
  if (rc_filerserver.getInitStatus() != FsServerInstance_c::offline) {
    en_lastCommand = en_requestProperties;

    pui8_sendBuffer[0] = 0x01;
    pui8_sendBuffer[1] = 0xFF;
    pui8_sendBuffer[2] = 0xFF;
    pui8_sendBuffer[3] = 0xFF;
    pui8_sendBuffer[4] = 0xFF;
    pui8_sendBuffer[5] = 0xFF;
    pui8_sendBuffer[6] = 0xFF;
    pui8_sendBuffer[7] = 0xFF;

    ui8_packetLength = 8;

    sendRequest();
  }
}

void FsCommand_c::setKeepConnectionOpen( bool b_keepOpen, bool b_forceClose )
{
  b_keepConnectionOpen = b_keepOpen;
  if ( !b_keepOpen && b_forceClose )
  {
    /* connection is being force closed, clear out number of open files */
    ui8_nrOpenFiles = 0;
  }
}

IsoAgLib::iFsCommandErrors FsCommand_c::getCurrentDirectory()
{
  en_lastCommand = en_getCurrentDirectory;

  pui8_sendBuffer[0] = 0x10;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = 0xFF;
  pui8_sendBuffer[3] = 0xFF;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest();
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::changeCurrentDirectory(uint8_t *pui8_newDirectory)
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

  pui8_sendBuffer[ui8_bufferPosition++] = 0x11;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_length;

  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_length >> 8;
  else if (i8_fsVersion != 0)
    return IsoAgLib::fsWrongFsVersion;

  for (uint16_t i = 0; i < ui16_length; ++i)
    pui8_sendBuffer[ui8_bufferPosition + i] = pui8_fileName[i];

  ui8_packetLength = ui8_bufferPosition + ui16_length;

  sendRequest();

  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::openFile(uint8_t *pui8_inFileName, bool b_openExclusive, bool b_openForAppend, bool b_createNewFile, bool b_openForReading, bool b_openForWriting, bool b_openDirectory)
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

  pui8_sendBuffer[ui8_bufferPosition++] = 0x20;
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
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_length;

  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_length >> 8;
  else if (i8_fsVersion != 0)
    return IsoAgLib::fsWrongFsVersion;

  for (uint8_t i = 0; (i < ui16_length) || (5 + i < 8); ++i)
  {
    if (i < ui16_length)
    {
      pui8_fileName[i] = pui8_inFileName[i];
      pui8_sendBuffer[ui8_bufferPosition + i] = pui8_fileName[i];
    }
    else if (i8_fsVersion == 1 && (i >= ui16_length))
      pui8_sendBuffer[ui8_bufferPosition + i] = 0xff;
  }

  ui8_packetLength = ui8_bufferPosition + ui16_length;

  sendRequest();

  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::seekFile(uint8_t ui8_inFileHandle, uint8_t ui8_inPossitionMode, int32_t i32_inOffset)
{
  if (en_lastCommand == en_noCommand)
    en_lastCommand = en_seekFile;

  ui8_fileHandle = ui8_inFileHandle;
  ui8_possitionMode = ui8_inPossitionMode;
  i32_offset = i32_inOffset;

  pui8_sendBuffer[0] = 0x21;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = ui8_possitionMode;
  pui8_sendBuffer[4] = i32_offset;
  pui8_sendBuffer[5] = i32_offset >> 0x08;
  pui8_sendBuffer[6] = i32_offset >> 0x10;
  pui8_sendBuffer[7] = i32_offset >> 0x18;

  ui8_packetLength = 8;

  sendRequest();

  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::readFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount)
{
  b_readDirectory = false;
  return readFile(ui8_inFileHandle, ui16_inCount, false);
}

IsoAgLib::iFsCommandErrors FsCommand_c::readDirectory(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, bool b_inReportHiddenFiles)
{
  b_readDirectory = true;
  return readFile(ui8_inFileHandle, ui16_inCount, b_inReportHiddenFiles);
}

IsoAgLib::iFsCommandErrors FsCommand_c::writeFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, uint8_t *pui8_inData)
{
  en_lastCommand = en_writeFile;

  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = 0x23;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = ui16_inCount;
  pui8_sendBuffer[4] = ui16_inCount >> 0x08;

  for (uint16_t ui16_writeDataSz = 0; ui16_writeDataSz < ui16_inCount; ++ui16_writeDataSz)
  {
    pui8_sendBuffer[ui16_writeDataSz + 5] = pui8_inData[ui16_writeDataSz];
  }

  ui8_packetLength = ui16_inCount + 5;

  sendRequest();

  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::closeFile(uint8_t ui8_inFileHandle)
{
  en_lastCommand = en_closeFile;

  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = 0x24;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = 0xFF;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest();
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::moveFile(uint8_t *pui8_sourceName, uint8_t *pui8_destName, bool b_recursive, bool b_force, bool b_copy)
{
  uint8_t ui8_bufferPosition = 0;

  en_lastCommand = en_moveFile;

  pui8_sendBuffer[ui8_bufferPosition++] = 0x30;
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

  pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength;
  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength >> 0x08;
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_destLength;
  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_destLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[ui8_bufferPosition + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  for (uint16_t ui16_iDst = 0; ui16_iDst < ui16_destLength; ++ui16_iDst)
    pui8_sendBuffer[ui8_bufferPosition + ui16_srcLength + ui16_iDst] = pui8_destName[ui16_iDst];

  ui8_packetLength = ui8_bufferPosition + ui16_srcLength + ui16_destLength;

  sendRequest();
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::deleteFile(uint8_t *pui8_sourceName, bool b_recursive, bool b_force)
{
  uint8_t ui8_bufferPosition = 0;

  en_lastCommand = en_deleteFile;

  if (i8_fsVersion == 0)
    pui8_sendBuffer[ui8_bufferPosition++] = 0x30;
  else if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = 0x31;

  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;

  uint8_t ui8_fileHandleMode = 0x00;

  if (b_recursive)
    ui8_fileHandleMode = ui8_fileHandleMode | 0x04;
  if (b_force)
    ui8_fileHandleMode = ui8_fileHandleMode | 0x02;

  pui8_sendBuffer[ui8_bufferPosition++] = ui8_fileHandleMode;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  if (i8_fsVersion == 0)
  {
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength;
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength >> 0x08;
  }
  else if (i8_fsVersion == 1)
  {
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength;
    pui8_sendBuffer[ui8_bufferPosition++] = 0x00;
  }

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[ui8_bufferPosition + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = ui8_bufferPosition + ui16_srcLength;

  sendRequest();
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::getFileAttributes(uint8_t *pui8_sourceName)
{
  en_lastCommand = en_getFileAttributes;

  if (i8_fsVersion == 0)
    return openFile(pui8_sourceName, false, true, false, true, false, false);

  pui8_sendBuffer[0] = 0x32;
  pui8_sendBuffer[1] = ui8_tan;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[2] = ui16_srcLength;
  pui8_sendBuffer[3] = ui16_srcLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[4 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 4 + ui16_srcLength;

  sendRequest();
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::setFileAttributes(uint8_t *pui8_sourceName, uint8_t  ui8_inHiddenAtt, uint8_t ui8_inReadOnlyAtt)
{
  en_lastCommand = en_setFileAttributes;
  ui8_hiddenAtt = ui8_inHiddenAtt;
  ui8_readOnlyAtt = ui8_inReadOnlyAtt;

  if (i8_fsVersion == 0)
    return openFile(pui8_sourceName, false, true, false, true, false, false);

  pui8_sendBuffer[0] = 0x33;
  pui8_sendBuffer[1] = ui8_tan;

  pui8_sendBuffer[2] = 0xF0 | ui8_hiddenAtt << 2 | ui8_readOnlyAtt;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[3] = ui16_srcLength;
  pui8_sendBuffer[4] = ui16_srcLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[5 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 5 + ui16_srcLength;

  sendRequest();
  return IsoAgLib::fsCommandNoError;
}

IsoAgLib::iFsCommandErrors FsCommand_c::getFileDateTime(uint8_t *pui8_sourceName)
{
  en_lastCommand = en_getFileDateTime;

  if (i8_fsVersion == 0)
    return openFile(pui8_sourceName, false, true, false, true, false, false);

  pui8_sendBuffer[0] = 0x34;
  pui8_sendBuffer[1] = ui8_tan;

  uint16_t ui16_srcLength = CNAMESPACE::strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[2] = ui16_srcLength;
  pui8_sendBuffer[3] = ui16_srcLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ++ui16_iSrc)
    pui8_sendBuffer[4 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 4 + ui16_srcLength;

  sendRequest();
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
  //EXTERNAL_DEBUG_DEVICE << "initializeVolume pathname: " << pui8_pathName << " space: " << ui32_space << " attributes: " << b_createVolumeUsingSpace << b_createNewVolume << EXTERNAL_DEBUG_DEVICE_ENDL;
  EXTERNAL_DEBUG_DEVICE << "initializeVolume pathname: NOT IMPLEMENTED" << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif
  return IsoAgLib::fsCommandNoError;
}


void FsCommand_c::decodeAttributes(uint8_t ui8_attributes)
{
  if (i8_fsVersion == 1)
  {
    b_caseSensitive = ui8_attributes & 0x80;
    b_removable = !(ui8_attributes & 0x40);
    b_longFilenames = ui8_attributes & 0x20;
    b_isDirectory = ui8_attributes & 0x10;
    b_isVolume = ui8_attributes & 0x8;
    b_hidden = ui8_attributes & 0x2;
    b_readOnly = ui8_attributes & 0x1;
  }
  else if (i8_fsVersion == 0)
  {
    b_removable = !(ui8_attributes & 0x40);
    b_archive = ui8_attributes & 0x20;
    b_isDirectory = ui8_attributes & 0x10;
    b_system = ui8_attributes & 0x04;
    b_hidden = ui8_attributes & 0x2;
    b_readOnly = ui8_attributes & 0x1;

    //set other values with default values
    b_caseSensitive = false;
    b_longFilenames = false;
    b_isVolume = false;
  }
}

void FsCommand_c::decodeGetCurrentDirectoryResponse()
{
  uint16_t ui16_length;
  uint8_t *pui8_receivePointer;

  ui8_errorCode = pui8_receiveBuffer[0];

  if (i8_fsVersion == 0)
  {
    ui16_length = pui8_receiveBuffer[9];
    pui8_receivePointer = &pui8_receiveBuffer[10];
  }
  else if (i8_fsVersion == 1)
  {
    ui16_length = pui8_receiveBuffer[9] | (pui8_receiveBuffer[10] << 8);
    pui8_receivePointer = &pui8_receiveBuffer[11];
  }
  else
  {
    ui16_length = 0;
    pui8_receivePointer = (uint8_t *)NULL;
    ui8_errorCode = IsoAgLib::fsFileserverVersionNotSupported;
  }

  b_receivedResponse = true;
  ui8_requestAttempts = 0;
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

void FsCommand_c::decodeOpenFileResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
  ++ui8_tan;
  ui8_errorCode = data().getUint8Data(2);
  ui8_fileHandle = data().getUint8Data(3);
  decodeAttributes(data().getUint8Data(4));
}

void FsCommand_c::decodeSeekFileResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
  ++ui8_tan;
  ui8_errorCode = data().getUint8Data(2);

  ui32_possition = static_cast<uint32_t>(data().getUint8Data(4)) | (static_cast<uint32_t>(data().getUint8Data(5)) << 0x08) | (static_cast<uint32_t>(data().getUint8Data(6)) << 0x10) | (static_cast<uint32_t>(data().getUint8Data(7)) << 0x18);
}

void FsCommand_c::decodeReadFileResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
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
    EXTERNAL_DEBUG_DEVICE << "message size larger then allocated buffer " << ui32_recBufAllocSize << " " << uint32_t(ui16_count + 3) << EXTERNAL_DEBUG_DEVICE_ENDL;
#endif
  }

  for (uint16_t i = 0; i < ui16_count; ++i)
  {
    pui8_data[i] = pui8_receiveBuffer[i + 3];
  }
}

void FsCommand_c::decodeReadDirectoryResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
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


FsCommand_c::~FsCommand_c()
{
  doneCleanUp();

  if (b_receiveFilterCreated)
  {
    // Single-Packet
    IsoFilter_s tempIsoFilter (*this, 0x3FFFF00UL, (FS_TO_CLIENT_PGN << 8),
                               &rc_csCom.getClientIdentItem().getIsoItem()->isoName(),
                               NULL,
                               8, Ident_c::ExtendedIdent);

    if (getIsoFilterManagerInstance4Comm().existIsoFilter( tempIsoFilter ))
    { // corresponding FilterBox_c exist -> delete it
      getIsoFilterManagerInstance4Comm().removeIsoFilter(  tempIsoFilter );
    }

    // Multi-Packet
    getMultiReceiveInstance4Comm().deregisterClient(*this, rc_csCom.getClientIdentItem().getIsoItem()->isoName(), FS_TO_CLIENT_PGN, 0x3FFFF);
  }

  // Scheduler
  getSchedulerInstance4Comm().unregisterClient(this);
}

FsCommand_c::FsCommand_c (FsClientServerCommunication_c &rc_inCsCom, FsServerInstance_c &rc_inFilerserver)
  : ui8_tan( 0 )
  , rc_csCom( rc_inCsCom )
  , rc_filerserver( rc_inFilerserver )
  , ui8_nrOpenFiles( 0 )
  , b_keepConnectionOpen( false )
  , i32_lastAliveSent( -1 )
  , pui8_receiveBuffer( NULL )
  , ui32_recBufAllocSize( 0 )
  , b_receivedResponse( true )
  , pui8_currentDirectory( NULL )
  , ui16_curDirAllocSize( 0 )
  , pui8_fileName( NULL )
  , ui16_fileNameAllocSize( 0 )
  , i32_offset( 0 )
  , pui8_data( NULL )
  , ui16_dataAllocSize( 0 )
  , b_readDirectory( false )
  , b_receiveFilterCreated( false )
{
  i8_fsVersion = rc_filerserver.getStandardVersion();

  // Scheduler
  getSchedulerInstance4Comm().registerClient(this);

}

void FsCommand_c::getFileAttributesDIS(uint8_t ui8_inFileHandle)
{
  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = 0x31;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = 0xFF;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest();
}

void FsCommand_c::setFileAttributesDIS(uint8_t ui8_inFileHandle)
{
  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = 0x32;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = 0xF0 | ui8_hiddenAtt << 3 | ui8_readOnlyAtt;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest();
}

void FsCommand_c::getFileDateTimeDIS(uint8_t ui8_inFileHandle)
{
  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = 0x33;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = 0xFF;
  pui8_sendBuffer[4] = 0xFF;
  pui8_sendBuffer[5] = 0xFF;
  pui8_sendBuffer[6] = 0xFF;
  pui8_sendBuffer[7] = 0xFF;

  ui8_packetLength = 8;

  sendRequest();
}

void FsCommand_c::clearDirectoryList()
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

void FsCommand_c::doneCleanUp()
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

IsoAgLib::iFsCommandErrors FsCommand_c::readFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, bool b_inReportHiddenFiles)
{
  if (en_lastCommand == en_noCommand)
    en_lastCommand = en_readFile;

  ui8_fileHandle = ui8_inFileHandle;
  ui16_count = ui16_inCount;
  b_reportHiddenFiles = b_inReportHiddenFiles;
  uint8_t ui8_bufferPosition = 0;

  pui8_sendBuffer[ui8_bufferPosition++] = 0x22;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_fileHandle;
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_count;
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_count >> 8;


  if (i8_fsVersion == 1)
  {
    pui8_sendBuffer[ui8_bufferPosition++] = b_inReportHiddenFiles;
  }
  else if (i8_fsVersion != 0)
  {
    return IsoAgLib::fsWrongFsVersion;
  }

  while (ui8_bufferPosition < 8)
    pui8_sendBuffer[ui8_bufferPosition++] = 0xFF;

  ui8_packetLength = ui8_bufferPosition;

  sendRequest();

  return IsoAgLib::fsCommandNoError;
}

}
