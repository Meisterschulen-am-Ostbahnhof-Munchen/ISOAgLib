/***************************************************************************
                            fscommand_c.cpp
                             -------------------
    begin                : Tue Oct 10 2007
    copyright            : (C) 2007 - 2008 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
 * Copyright (C) 2007 - 2008 Dipl.-Inform. Achim Spangler                  *
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
#include <string.h>

#include "fscommand_c.h"
#include "fsclientservercommunication_c.h"
#include <IsoAgLib/driver/can/icanio_c.h>

#define NR_REQUEST_ATTEMPTS 5
#define REQUEST_REPEAT_TIME 5000

namespace __IsoAgLib
{

uint8_t FsCommand_c::pui8_maintenanceBuffer[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

bool FsCommand_c::timeEvent(void)
{
  //are we waiting for a response? if not, delete all filters.
  if (b_receivedResponse)
  {
    deleteMultiReceiveFilter();
  }

  if (!b_receivedResponse && (HAL::getTime() - i32_lastrequestAttempt) > REQUEST_REPEAT_TIME)
  {
    if (ui8_requestAttempts < NR_REQUEST_ATTEMPTS)
    {
      sendRequest();
    } else {
      switch (en_lastCommand)
      {
        case en_noCommand:
          break;
        case en_getCurrentDirectory:
          rc_csCom.getCurrentDirectoryResponse(fsFileserverNotResponding, (uint8_t *)NULL);
          break;
        case en_changeCurrentDirectory:
          rc_csCom.changeCurrentDirectoryResponse(fsFileserverNotResponding, (uint8_t *)NULL);
          break;
        case en_openFile:
          rc_csCom.openFileResponse(fsFileserverNotResponding, 0, false, false, false, false, false, false, false);
          break;
        case en_seekFile:
          rc_csCom.seekFileResponse(fsFileserverNotResponding, 0);
          break;
        case en_readFile:
          rc_csCom.readFileResponse(fsFileserverNotResponding, 0, (uint8_t *)NULL);
          break;
        case en_writeFile:
          rc_csCom.writeFileResponse(fsFileserverNotResponding, 0);
          break;
        case en_closeFile:
          rc_csCom.closeFileResponse(fsFileserverNotResponding);
          break;
        case en_moveFile:
          rc_csCom.moveFileResponse(fsFileserverNotResponding);
          break;
        case en_deleteFile:
          rc_csCom.deleteFileResponse(fsFileserverNotResponding);
          break;
        case en_getFileAttributes:
          rc_csCom.getFileAttributesResponse(fsFileserverNotResponding, false, false, false, false, false, false, false);
          break;
        case en_setFileAttributes:
          rc_csCom.setFileAttributesResponse(fsFileserverNotResponding);
          break;
        case en_getFileDateTime:
          rc_csCom.getFileDateTimeResponse(fsFileserverNotResponding, 0, 0, 0, 0, 0, 0);
          break;
        case en_initializeVolume:
          rc_csCom.initializeVolumeResponse(fsFileserverNotResponding, false, false, false, false, false, false, false);
          break;
        default:
#ifdef DEBUG
          std::cout << "Repetition of command not defined!" << std::endl;
#endif
          break;
      }
      b_receivedResponse = true;
      en_lastCommand = en_noCommand;
      i32_lastrequestAttempt = -1;
      ui8_requestAttempts = 0;
      i32_offset = 0;
    }
  }

  // do we have open files? -> send maintenance messages.
  if (ui8_nrOpenFiles > 0 && (i32_lastAliveSent == -1 || (HAL::getTime () - (uint32_t)i32_lastAliveSent) > 2000))
  {
    CANPkgExt_c canpkgext;
    canpkgext.setExtCanPkg8 (0x07, 0x00, CLIENT_TO_FS_PGN >> 8,
                             rc_filerserver.getIsoItem().nr(),
                             rc_csCom.getClientIdentItem().getIsoItem()->nr(),
                             pui8_maintenanceBuffer[0], pui8_maintenanceBuffer[1], pui8_maintenanceBuffer[2],
                             pui8_maintenanceBuffer[3], pui8_maintenanceBuffer[4], pui8_maintenanceBuffer[5],
                             pui8_maintenanceBuffer[6], pui8_maintenanceBuffer[7]);

    getCanInstance() << canpkgext;
    i32_lastAliveSent = HAL::getTime ();
  }
  return true;
}

//process packet-stream. after completion, react on message.
bool FsCommand_c::processPartStreamDataChunk(IsoAgLib::iStream_c& refc_stream, bool rb_isFirstChunk, bool rb_isLastChunkAndACKd)
{
  uint16_t ui16_notParsedSize;

  if (refc_stream.getStreamInvalid()) return false;

  if (rb_isFirstChunk)
  {
    uint8_t ui8_tmpTan = refc_stream.get();
    if (ui8_tmpTan != ui8_tan)
    {
#ifdef DEBUG
      std::cout << "TAN does not match expected one!" << std::endl;
#endif
      return false;
    }
  }

  ui16_notParsedSize = refc_stream.getNotParsedSize();

  for (uint8_t i = 0; i < ui16_notParsedSize; i++)
    pui_receiveBuffer[i + i32_offset] = refc_stream.get();

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
        rc_csCom.getCurrentDirectoryResponse(iFsError(ui8_errorCode), pui8_currentDirectory);
        i32_offset = 0;
        delete[] pui_receiveBuffer;
        en_lastCommand = en_noCommand;
        return true;
      //read file response
      case 0x22:

        ui16_count = pui_receiveBuffer[1] | (pui_receiveBuffer[2] << 0x08);
        en_lastCommand = en_noCommand;
        if (ui16_count == (uint16_t)(i32_offset - 3))
        {
          decodeReadFileResponse();
          rc_csCom.readFileResponse(iFsError(ui8_errorCode), ui16_count, pui8_data);
        } else {
          decodeReadDirectoryResponse();

          if (rc_filerserver.getInitStatus() == FsServerInstance_c::online) {
            closeFile(ui8_fileHandle);
          } else {
            rc_csCom.readDirectoryResponse(iFsError(ui8_errorCode), v_dirData);
          }
        }
        delete[] pui_receiveBuffer;
        i32_offset = 0;
        return true;
      default:
        return false;
    }
  }
  return true;
}

//nothing to be done, new message will be sent periodically.
void FsCommand_c::reactOnAbort(IsoAgLib::iStream_c& /*refc_stream*/)
{
}

//initializes the buffer for a message stream
bool FsCommand_c::reactOnStreamStart(const IsoAgLib::ReceiveStreamIdentifier_c& /*refc_ident*/, uint32_t rui32_totalLen)
{
  pui_receiveBuffer = new uint8_t[rui32_totalLen - 1];
  return true;
}
//process single packages.
bool FsCommand_c::processMsg()
{
  if (data().getUint8Data(1) != ui8_tan && data().getUint8Data(0) != 0x00  && data().getUint8Data(0) != 0x01)
  {
#ifdef DEBUG
      std::cout << "TAN does not match expected one!" << std::endl;
#endif
    return false;
  }

  switch (data().getUint8Data(0)) {
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
      ui8_tan++;
      openFile((uint8_t *)"\\\\", false, false, false, true, false, true);
      return true;
    //change current directory
    case 0x11:
      rc_csCom.changeCurrentDirectoryResponse(iFsError(data().getUint8Data(2)), pui8_fileName);
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ui8_tan++;
      en_lastCommand = en_noCommand;
      return true;
    //open file
    case 0x20:
      decodeOpenFileResponse();

      if (!ui8_errorCode)
        ui8_nrOpenFiles++;

      //init case get volumes or real external open file?
      if (rc_filerserver.getInitStatus() == FsServerInstance_c::online)
      {
        seekFile(ui8_fileHandle, 2, 0);
      }
      else if (en_lastCommand == en_getFileAttributes && ui8_errorCode && !pui8_fileName)
      {
        en_lastCommand = en_noCommand;
        rc_csCom.getFileAttributesResponse(iFsError(ui8_errorCode), false, false, false, false, false, false, false);
      }
      else if (en_lastCommand == en_setFileAttributes && ui8_errorCode && !pui8_fileName)
      {
        en_lastCommand = en_noCommand;
        rc_csCom.setFileAttributesResponse(iFsError(ui8_errorCode));
      }
      else if (en_lastCommand == en_getFileDateTime && ui8_errorCode && !pui8_fileName)
      {
        en_lastCommand = en_noCommand;
        rc_csCom.getFileDateTimeResponse(iFsError(ui8_errorCode), 0, 0, 0, 0, 0, 0);
      }
      else if ((en_lastCommand == en_getFileAttributes ||
                en_lastCommand == en_setFileAttributes ||
                en_lastCommand == en_getFileDateTime) &&
                ui8_errorCode && pui8_fileName)
      {
        openFile(pui8_fileName, false, true, false, false, false, true);
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
        rc_csCom.openFileResponse(iFsError(ui8_errorCode), ui8_fileHandle, b_caseSensitive, b_removable, b_longFilenames, b_isDirectory,  b_isVolume, b_hidden, b_readOnly);
      }

      return true;
    //seek file
    case 0x21:
      //init case get volumes or real external seek file?
      if (rc_filerserver.getInitStatus() == FsServerInstance_c::online && ui8_possitionMode == 0)
      {
        b_receivedResponse = true;
        ui8_requestAttempts = 0;
        ui8_tan++;
        readFile(ui8_fileHandle, ui32_possition, true);
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
        rc_csCom.seekFileResponse(iFsError(ui8_errorCode), ui32_possition);
      }

      return true;

    //read file
    case 0x22:
      pui_receiveBuffer = new uint8_t[6];

      for (uint8_t i = 0; i < 6; i++)
        pui_receiveBuffer[i] = data().getUint8Data(i + 2);

      decodeReadFileResponse();
      en_lastCommand = en_noCommand;

      rc_csCom.readFileResponse(iFsError(ui8_errorCode), ui16_count, pui8_data);

      return true;
    //write file
    case 0x23:
      rc_csCom.writeFileResponse(iFsError(data().getUint8Data(2)), (data().getUint8Data(3) | data().getUint8Data(4) << 0x08));
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ui8_tan++;
      en_lastCommand = en_noCommand;

      return true;
    //close file
    case 0x24:
      ui8_errorCode = data().getUint8Data(2);
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ui8_tan++;

      if (!ui8_errorCode)
        ui8_nrOpenFiles--;

      //init case get volumes or real external seek file?
      if (rc_filerserver.getInitStatus() == FsServerInstance_c::online && !ui8_errorCode)
      {
        rc_filerserver.setVolumes(v_dirData);
      }
      else if (en_lastCommand == en_getFileAttributes)
      {
        en_lastCommand = en_noCommand;
        rc_csCom.getFileAttributesResponse(iFsError(data().getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
      }
      else if (en_lastCommand == en_getFileAttributes)
      {
        en_lastCommand = en_noCommand;
        rc_csCom.setFileAttributesResponse(iFsError(data().getUint8Data(2)));
      }
      else if (en_lastCommand == en_getFileDateTime)
      {
        en_lastCommand = en_noCommand;

        rc_csCom.getFileDateTimeResponse(iFsError(data().getUint8Data(2)), (uint16_t)(1980 + ((ui16_date >> 9) & 0x7F)), (ui16_date >> 5) & 0xF, (ui16_date) & 0x1F, (ui16_time >> 11) & 0x1F, (ui16_time >> 5) & 0x3F, 2 * ((ui16_time) & 0x1F));
      }
      else
      {
        en_lastCommand = en_noCommand;
        rc_csCom.closeFileResponse(iFsError(ui8_errorCode));
      }

      return true;
    //move file
    case 0x30:
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ui8_tan++;

      if (en_lastCommand == en_moveFile)
        rc_csCom.moveFileResponse(iFsError(data().getUint8Data(2)));
      else if (en_lastCommand == en_deleteFile)
        rc_csCom.deleteFileResponse(iFsError(data().getUint8Data(2)));

      en_lastCommand = en_noCommand;
      return true;
    //delete file
    case 0x31:
      ui8_tan++;
      ui8_requestAttempts = 0;

      if (en_lastCommand == en_deleteFile)
      {
        b_receivedResponse = true;
        en_lastCommand = en_noCommand;
        rc_csCom.deleteFileResponse(iFsError(data().getUint8Data(2)));
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
      ui8_tan++;

      if (en_lastCommand == en_getFileAttributes)
      {
        b_receivedResponse = true;
        decodeAttributes(data().getUint8Data(3));
        rc_csCom.getFileAttributesResponse(iFsError(data().getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
        en_lastCommand = en_noCommand;
      } else if (en_lastCommand == en_setFileAttributes)
      {
        closeFile(ui8_fileHandle);
      }
      return true;
    //set file attributes
    case 0x33:
      ui8_requestAttempts = 0;
      ui8_tan++;

      if (en_lastCommand == en_setFileAttributes)
      {
        b_receivedResponse = true;

        rc_csCom.setFileAttributesResponse(iFsError(data().getUint8Data(2)));
        en_lastCommand = en_noCommand;
      } else if (en_lastCommand == en_getFileDateTime)
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
      ui8_tan++;

      ui16_date = data().getUint8Data(3) | (data().getUint8Data(4) << 8);
      ui16_time = data().getUint8Data(5) | (data().getUint8Data(6) << 8);

      rc_csCom.getFileDateTimeResponse(iFsError(data().getUint8Data(2)), (uint16_t)(1980 + ((ui16_date >> 9) & 0x7F)), (ui16_date >> 5) & 0xF, (ui16_date) & 0x1F, (ui16_time >> 11) & 0x1F, (ui16_time >> 5) & 0x3F, 2 * ((ui16_time) & 0x1F));
      en_lastCommand = en_noCommand;
      return true;
    //init volume
    case 0x40:
      b_receivedResponse = true;
      ui8_requestAttempts = 0;
      ui8_tan++;

      decodeAttributes(data().getUint8Data(3));

      rc_csCom.initializeVolumeResponse(iFsError(data().getUint8Data(2)), b_caseSensitive, b_removable, b_longFilenames, b_isDirectory, b_isVolume, b_hidden, b_readOnly);
      en_lastCommand = en_noCommand;
      return true;
    default:
#ifdef DEBUG
      std::cout << "got message with content: " << std::hex << (uint32_t)data().getUint8Data(0) << std::dec << std::endl;
#endif
      return false;
  }
  return false;
}

//sends request
void FsCommand_c::sendRequest()
{
  b_receivedResponse = false;
  i32_lastrequestAttempt = HAL::getTime();
  ui8_requestAttempts++;

  //single packet
  if (ui8_packetLength <= 8)
  {
    CANPkgExt_c canpkgext;

    canpkgext.setExtCanPkg8 (0x07, 0x00, CLIENT_TO_FS_PGN >> 8, rc_filerserver.getIsoItem().nr(), rc_csCom.getClientIdentItem().getIsoItem()->nr(),
                             pui8_sendBuffer[0], pui8_sendBuffer[1], pui8_sendBuffer[2], pui8_sendBuffer[3],
                             pui8_sendBuffer[4], pui8_sendBuffer[5], pui8_sendBuffer[6], pui8_sendBuffer[7]);

    getCanInstance4Comm() << canpkgext;
  //multi-packet
  } else {
    getMultiSendInstance().sendIsoTarget(rc_csCom.getClientIdentItem().getIsoItem()->isoName(),
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

iFsCommandErrors FsCommand_c::getCurrentDirectory()
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

  registerMultiReceiveFilter();
  sendRequest();
  return fsCommandNoError;
}

iFsCommandErrors FsCommand_c::changeCurrentDirectory(uint8_t *pui8_newDirectory)
{
  en_lastCommand = en_changeCurrentDirectory;

  uint16_t ui16_length = strlen((const char*)pui8_newDirectory);
  uint8_t ui8_bufferPosition = 0;
  pui8_fileName = new uint8_t[ui16_length + 1];
  pui8_fileName[ui16_length] = 0;

  for (uint16_t i = 0; i < ui16_length; i++)
    pui8_fileName[i] = pui8_newDirectory[i];

  pui8_sendBuffer[ui8_bufferPosition++] = 0x11;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_length;

  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_length >> 8;
  else if (i8_fsVersion != 0)
    return fsWrongFsVersion;

  for (uint16_t i = 0; i < ui16_length; i++)
    pui8_sendBuffer[ui8_bufferPosition + i] = pui8_fileName[i];

  ui8_packetLength = ui8_bufferPosition + ui16_length;

  sendRequest();

  return fsCommandNoError;
}

iFsCommandErrors FsCommand_c::openFile(uint8_t *pui8_inFileName, bool b_openExclusive, bool b_openForAppend, bool b_createNewFile, bool b_openForReading, bool b_openForWriting, bool b_openDirectory)
{
  if (en_lastCommand == en_noCommand)
    en_lastCommand = en_openFile;

  uint16_t ui16_length = strlen((const char*)pui8_inFileName);
  pui8_fileName = new uint8_t[ui16_length + 1];
  pui8_fileName[ui16_length] = 0;
  uint8_t ui8_bufferPosition = 0;

  pui8_sendBuffer[ui8_bufferPosition++] = 0x20;
  pui8_sendBuffer[ui8_bufferPosition++] = ui8_tan;

  ui8_flags = 0x0;

  if (b_openExclusive) {
    ui8_flags = ui8_flags | (0x1 << 4);
  }
  if (b_openForAppend) {
    ui8_flags = ui8_flags | (0x1 << 3);
  }
  if (b_createNewFile) {
    ui8_flags = ui8_flags | (0x1 << 2);
  }

  if (b_openDirectory) {
    ui8_flags = ui8_flags | 0x3;
  } else if (b_openForReading && b_openForWriting) {
    ui8_flags = ui8_flags | 0x2;
  } else if (b_openForReading) {
  } else if (b_openForWriting) {
    ui8_flags = ui8_flags | 0x1;
  } else {
    return fsCommandWrongFlag;
  }

  pui8_sendBuffer[ui8_bufferPosition++] = ui8_flags;
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_length;

  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_length >> 8;
  else if (i8_fsVersion != 0)
    return fsWrongFsVersion;

  for (uint8_t i = 0; (i < ui16_length) || (5 + i < 8); i++)
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

  return fsCommandNoError;
}

iFsCommandErrors FsCommand_c::seekFile(uint8_t ui8_inFileHandle, uint8_t ui8_inPossitionMode, int32_t i32_inOffset)
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

  return fsCommandNoError;
}

iFsCommandErrors FsCommand_c::readFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, bool b_inReportHiddenFiles)
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
  } else if (i8_fsVersion != 0) {
    return fsWrongFsVersion;
  }

  while (ui8_bufferPosition < 8)
    pui8_sendBuffer[ui8_bufferPosition++] = 0xFF;

  ui8_packetLength = ui8_bufferPosition;

  registerMultiReceiveFilter();
  sendRequest();

  return fsCommandNoError;
}
iFsCommandErrors FsCommand_c::writeFile(uint8_t ui8_inFileHandle, uint16_t ui16_inCount, uint8_t *pui8_inData)
{
  en_lastCommand = en_writeFile;

  ui8_fileHandle = ui8_inFileHandle;

  pui8_sendBuffer[0] = 0x23;
  pui8_sendBuffer[1] = ui8_tan;
  pui8_sendBuffer[2] = ui8_fileHandle;
  pui8_sendBuffer[3] = ui16_inCount;
  pui8_sendBuffer[4] = ui16_inCount >> 0x08;

  for (uint16_t ui16_writeDataSz = 0; ui16_writeDataSz < ui16_inCount; ui16_writeDataSz++)
  {
    pui8_sendBuffer[ui16_writeDataSz + 5] = pui8_inData[ui16_writeDataSz];
  }

  ui8_packetLength = ui16_inCount + 5;

  sendRequest();

  return fsCommandNoError;
}

iFsCommandErrors FsCommand_c::closeFile(uint8_t ui8_inFileHandle)
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
  return fsCommandNoError;
}

iFsCommandErrors FsCommand_c::moveFile(uint8_t *pui8_sourceName, uint8_t *pui8_destName, bool b_recursive, bool b_force, bool b_copy)
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

  uint16_t ui16_srcLength = strlen((const char *)pui8_sourceName);
  uint16_t ui16_destLength = strlen((const char *)pui8_destName);

  pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength;
  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_srcLength >> 0x08;
  pui8_sendBuffer[ui8_bufferPosition++] = ui16_destLength;
  if (i8_fsVersion == 1)
    pui8_sendBuffer[ui8_bufferPosition++] = ui16_destLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ui16_iSrc++)
    pui8_sendBuffer[ui8_bufferPosition + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  for (uint16_t ui16_iDst = 0; ui16_iDst < ui16_destLength; ui16_iDst++)
    pui8_sendBuffer[ui8_bufferPosition + ui16_srcLength + ui16_iDst] = pui8_destName[ui16_iDst];

  ui8_packetLength = ui8_bufferPosition + ui16_srcLength + ui16_destLength;

  sendRequest();
  return fsCommandNoError;
}
iFsCommandErrors FsCommand_c::deleteFile(uint8_t *pui8_sourceName, bool b_recursive, bool b_force)
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

  uint16_t ui16_srcLength = strlen((const char *)pui8_sourceName);

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

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ui16_iSrc++)
    pui8_sendBuffer[ui8_bufferPosition + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = ui8_bufferPosition + ui16_srcLength;

  sendRequest();
  return fsCommandNoError;
}
iFsCommandErrors FsCommand_c::getFileAttributes(uint8_t *pui8_sourceName)
{
  en_lastCommand = en_getFileAttributes;

  if (i8_fsVersion == 0)
    return openFile(pui8_sourceName, false, true, false, true, false, false);

  pui8_sendBuffer[0] = 0x32;
  pui8_sendBuffer[1] = ui8_tan;

  uint16_t ui16_srcLength = strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[2] = ui16_srcLength;
  pui8_sendBuffer[3] = ui16_srcLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ui16_iSrc++)
    pui8_sendBuffer[4 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 4 + ui16_srcLength;

  sendRequest();
  return fsCommandNoError;
}
iFsCommandErrors FsCommand_c::setFileAttributes(uint8_t *pui8_sourceName, uint8_t  ui8_inHiddenAtt, uint8_t ui8_inReadOnlyAtt)
{ 
  en_lastCommand = en_setFileAttributes;
  ui8_hiddenAtt = ui8_inHiddenAtt;
  ui8_readOnlyAtt = ui8_inReadOnlyAtt;

  if (i8_fsVersion == 0)
    return openFile(pui8_sourceName, false, true, false, true, false, false);

  pui8_sendBuffer[0] = 0x33;
  pui8_sendBuffer[1] = ui8_tan;

  pui8_sendBuffer[2] = 0xF0 | ui8_hiddenAtt << 2 | ui8_readOnlyAtt;

  uint16_t ui16_srcLength = strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[3] = ui16_srcLength;
  pui8_sendBuffer[4] = ui16_srcLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ui16_iSrc++)
    pui8_sendBuffer[5 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 5 + ui16_srcLength;

  sendRequest();
  return fsCommandNoError;
}
iFsCommandErrors FsCommand_c::getFileDateTime(uint8_t *pui8_sourceName)
{
  en_lastCommand = en_getFileDateTime;

  if (i8_fsVersion == 0)
    return openFile(pui8_sourceName, false, true, false, true, false, false);

  pui8_sendBuffer[0] = 0x34;
  pui8_sendBuffer[1] = ui8_tan;

  uint16_t ui16_srcLength = strlen((const char *)pui8_sourceName);

  pui8_sendBuffer[2] = ui16_srcLength;
  pui8_sendBuffer[3] = ui16_srcLength >> 0x08;

  for (uint16_t ui16_iSrc = 0; ui16_iSrc < ui16_srcLength; ui16_iSrc++)
    pui8_sendBuffer[4 + ui16_iSrc] = pui8_sourceName[ui16_iSrc];

  ui8_packetLength = 4 + ui16_srcLength;

  sendRequest();
  return fsCommandNoError;
}

iFsCommandErrors FsCommand_c::initializeVolume(uint8_t */*pui8_pathName*/, uint32_t /*ui32_space*/, bool /*b_createVolumeUsingSpace*/, bool /*b_createNewVolume*/)
{
  en_lastCommand = en_initializeVolume;

//TODO implement this one...

#ifdef DEBUG
  std::cout << "initializeVolume pathname: " << pui8_pathName << " space: " << ui32_space << " attributes: " << b_createVolumeUsingSpace << b_createNewVolume << std::endl;
#endif
  return fsCommandNoError;
}

void FsCommand_c::registerMultiReceiveFilter()
{
  getMultiReceiveInstance4Comm().registerClient(*this, rc_csCom.getClientIdentItem().getIsoItem()->isoName(), FS_TO_CLIENT_PGN);
}

void FsCommand_c::deleteMultiReceiveFilter()
{
  getMultiReceiveInstance().deregisterClient(*this, rc_csCom.getClientIdentItem().getIsoItem()->isoName(), FS_TO_CLIENT_PGN, 0x3FFFF);
}

void FsCommand_c::registerPacketFilter()
{
  bool b_atLeastOneFilterAdded = false;

  b_atLeastOneFilterAdded |= (
        getCanInstance4Comm().insertFilter (*this,
                                            (0x3FF0000UL),
                                            ((static_cast<MASK_TYPE>(FS_TO_CLIENT_PGN)<<8) /*|
                                            c_clientIdentItem.getIsoItem()->nr()<<8 |
                                            rc_fileServer->getSa()*/),
                                            false,
                                            Ident_c::ExtendedIdent, 8)
                            != NULL);

  if (b_atLeastOneFilterAdded)
    getCanInstance4Comm().reconfigureMsgObj();
}

void FsCommand_c::deletePacketFilter()
{
  getCanInstance4Comm().deleteFilter(*this,
                                     (0x3FF0000UL),
                                     ((static_cast<MASK_TYPE>(FS_TO_CLIENT_PGN)<<8) /*|
                                     c_clientIdentItem.getIsoItem()->nr()<<8 |
                                     rc_fileServer->getSa()*/),
                                     Ident_c::ExtendedIdent);
}

void FsCommand_c::decodeAttributes(uint8_t ui8_attributes)
{
  if (i8_fsVersion == 1) {
    b_caseSensitive = ui8_attributes & 0x80;
    b_removable = !(ui8_attributes & 0x40);
    b_longFilenames = ui8_attributes & 0x20;
    b_isDirectory = ui8_attributes & 0x10;
    b_isVolume = ui8_attributes & 0x8;
    b_hidden = ui8_attributes & 0x2;
    b_readOnly = ui8_attributes & 0x1;
  } else if (i8_fsVersion == 0) {
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

  ui8_errorCode = pui_receiveBuffer[0];

  if (i8_fsVersion == 0)
  {
    ui16_length = pui_receiveBuffer[9];
    pui8_receivePointer = &pui_receiveBuffer[10];
  } else if (i8_fsVersion == 1) {
    ui16_length = pui_receiveBuffer[9] | (pui_receiveBuffer[10] << 8);
    pui8_receivePointer = &pui_receiveBuffer[11];
  } else {
    ui16_length = 0;
    pui8_receivePointer = (uint8_t *)NULL;
    ui8_errorCode = fsFileserverVersionNotSupported;
  }

  b_receivedResponse = true;
  ui8_requestAttempts = 0;
  ui8_tan++;
  pui8_currentDirectory = new uint8_t[ui16_length + 1];
  pui8_currentDirectory[ui16_length] = 0;

  for (uint16_t i = 0; i < ui16_length; i++)
  {
    pui8_currentDirectory[i] = *pui8_receivePointer;
    pui8_receivePointer++;
  }
}

void FsCommand_c::decodeOpenFileResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
  ui8_tan++;
  ui8_errorCode = data().getUint8Data(2);
  ui8_fileHandle = data().getUint8Data(3);
  decodeAttributes(data().getUint8Data(4));
}

void FsCommand_c::decodeSeekFileResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
  ui8_tan++;
  ui8_errorCode = data().getUint8Data(2);

  ui32_possition = data().getUint8Data(4) | (data().getUint8Data(5) << 0x08) | (data().getUint8Data(6) << 0x10) | (data().getUint8Data(7) << 0x18);
}

void FsCommand_c::decodeReadFileResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
  ui8_tan++;

  ui8_errorCode = pui_receiveBuffer[0];
  ui16_count = pui_receiveBuffer[1] | (pui_receiveBuffer[2] << 0x08);

  pui8_data = new uint8_t[ui16_count];

  for (uint16_t i = 0; i < ui16_count; i++)
  {
    pui8_data[i] = pui_receiveBuffer[i + 3];
  }
}

void FsCommand_c::decodeReadDirectoryResponse()
{
  b_receivedResponse = true;
  ui8_requestAttempts = 0;
  ui8_tan++;
  struct iFsDirectory *ps_tmpDir;

  ui8_errorCode = pui_receiveBuffer[0];

  v_dirData.clear();

  i32_offset = 3;

  for (uint16_t ui16_nrEntries = 0; ui16_nrEntries < ui16_count; ui16_nrEntries++)
  {
    ps_tmpDir = new iFsDirectory();

    ps_tmpDir->pui8_filename = new uint8_t[pui_receiveBuffer[i32_offset] + 1];
    ps_tmpDir->pui8_filename[pui_receiveBuffer[i32_offset]] = 0;

    for (uint8_t ui8_nameLength = 0; ui8_nameLength < pui_receiveBuffer[i32_offset]; ui8_nameLength++)
      ps_tmpDir->pui8_filename[ui8_nameLength] = pui_receiveBuffer[ui8_nameLength + i32_offset + 1];

    i32_offset += (pui_receiveBuffer[i32_offset] + 1);

    decodeAttributes(pui_receiveBuffer[i32_offset]);

    ps_tmpDir->b_caseSensitive = b_caseSensitive;
    ps_tmpDir->b_removable = b_removable;
    ps_tmpDir->b_longFilenames = b_longFilenames;
    ps_tmpDir->b_isDirectory = b_isDirectory;
    ps_tmpDir->b_isVolume = b_isVolume;
    ps_tmpDir->b_hidden = b_hidden;
    ps_tmpDir->b_readOnly = b_readOnly;

    i32_offset++;

    ps_tmpDir->ui16_date = pui_receiveBuffer[i32_offset] | (pui_receiveBuffer[i32_offset + 1] << 0x08);
    i32_offset += 2;
    ps_tmpDir->ui16_time = pui_receiveBuffer[i32_offset] | (pui_receiveBuffer[i32_offset + 1] << 0x08);
    i32_offset += 2;
    ps_tmpDir->ui32_size = pui_receiveBuffer[i32_offset] | (pui_receiveBuffer[i32_offset + 1] << 0x08) | (pui_receiveBuffer[i32_offset + 1] << 0x10) | (pui_receiveBuffer[i32_offset + 1] << 0x18);
    i32_offset += 4;

    v_dirData.push_back(ps_tmpDir);
  }
}

FsCommand_c::~FsCommand_c()
{
  getSchedulerInstance4Comm().unregisterClient(this);
  deletePacketFilter();
}

FsCommand_c::FsCommand_c(FsClientServerCommunication_c &rc_inCsCom, FsServerInstance_c &rc_inFilerserver) :
                         rc_csCom(rc_inCsCom), rc_filerserver(rc_inFilerserver)
{
  ui8_tan = 0;
  getSchedulerInstance4Comm().registerClient(this);
  ui8_nrOpenFiles = 0;
  i32_lastAliveSent = -1;
  b_receivedResponse = true;
  i8_fsVersion = rc_filerserver.getStandardVersion();
  i32_offset = 0;

  registerPacketFilter();
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

}
