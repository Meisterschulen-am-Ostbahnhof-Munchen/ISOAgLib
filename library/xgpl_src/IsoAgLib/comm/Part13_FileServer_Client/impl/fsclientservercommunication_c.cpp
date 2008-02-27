/***************************************************************************
                            fsclientservercommunication_c.cpp
                             -------------------
    begin                : Tue Oct 01 2007
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
#include "fsclientservercommunication_c.h"

#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>

#include "../ifsclientservercommunication_c.h"

#include "fsserverinstance_c.h"
#include "fsmanager_c.h"

#include <IsoAgLib/comm/Part3_DataLink/impl/streamlinear_c.h>

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>

#include "../ifsstructs.h"

#include <iostream>
#include <vector>

#define ERR_REQUEST_IN_PROGRESS 1

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** constructor to init client-server communication without fileserver */
FsClientServerCommunication_c::FsClientServerCommunication_c(IdentItem_c &rc_identItem, IsoAgLib::iFsClient_c &rc_fsClient, std::vector<iFsWhitelist_c *> v_inFsWhitelist) : c_fsClient(rc_fsClient), c_identItem(rc_identItem)
{
  b_notifiedOnFileservers = false;
  b_finishedRegistering = false;
  c_fileServer = NULL;
  pui8_currentDirectory = 0;
  pc_commandHandler = 0;
  v_fsWhitelist = v_inFsWhitelist;
}

/** explicit conversion to reference of interface class type */
IsoAgLib::iFsClientServerCommunication_c* FsClientServerCommunication_c::toInterfacePointer()
{
  return static_cast<IsoAgLib::iFsClientServerCommunication_c*>(this);
}

bool FsClientServerCommunication_c::notifyOnNewFileServer(FsServerInstance_c &rc_fsServerInstance)
{
  b_notifiedOnFileservers = true;

  for (std::vector<iFsWhitelist_c *>::iterator it_whitelist = v_fsWhitelist.begin();
       it_whitelist != v_fsWhitelist.end();
       it_whitelist++)
  {
    if ((*it_whitelist)->b_requestRemoveableMedium == rc_fsServerInstance.getSupportsMultiVolumes() &&
        ((*it_whitelist)->i32_requestedManufacturer == rc_fsServerInstance.getIsoName().manufCode() ||
          (*it_whitelist)->i32_requestedManufacturer == -1) &&
        (*it_whitelist)->ui8_minSimultaniouslyOpenFiles <= rc_fsServerInstance.getMaxNrOpenFiles())
    {
      c_fsClient.finishedConnectingToFileserver(*rc_fsServerInstance.toInterfacePointer());
      return true;
    }
  }

  return false;
}

bool FsClientServerCommunication_c::notifyOnFileServerOffline(FsServerInstance_c &rc_fsServerInstance)
{
  if (pc_commandHandler != NULL)
  {
    delete pc_commandHandler;
    pc_commandHandler = NULL;

    return c_fsClient.notifyOnFileServerOffline(*(rc_fsServerInstance.toInterfacePointer()));
  }
  return true;
}

void FsClientServerCommunication_c::requestFsConnection(FsServerInstance_c &rc_FileServer)
{
  c_fileServer = &rc_FileServer;
  b_finishedRegistering = false;

  pc_commandHandler = new FsCommand_c(*this, *c_fileServer);

  pc_commandHandler->getCurrentDirectory();
}

/// FileServer access functions
    iFsCommandErrors FsClientServerCommunication_c::changeCurrentDirectory(uint8_t *pui8_newDirectory)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->changeCurrentDirectory(pui8_newDirectory);
    }

    iFsCommandErrors FsClientServerCommunication_c::openFile(uint8_t *pui8_fileName, bool b_openExclusive, bool b_openForAppend, bool b_createNewFile, bool b_openForReading, bool b_openForWriting, bool b_openDirectory)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->openFile(pui8_fileName, b_openExclusive, b_openForAppend, b_createNewFile, b_openForReading, b_openForWriting, b_openDirectory);
    }
    iFsCommandErrors FsClientServerCommunication_c::seekFile(uint8_t ui8_fileHandle, uint8_t ui8_possitionMode, int32_t i32_offset)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->seekFile(ui8_fileHandle, ui8_possitionMode, i32_offset);
    }
    iFsCommandErrors FsClientServerCommunication_c::readFile(uint8_t ui8_fileHandle, uint16_t ui16_count, bool b_reportHiddenFiles)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->readFile(ui8_fileHandle, ui16_count, b_reportHiddenFiles);
    }
    iFsCommandErrors FsClientServerCommunication_c::writeFile(uint8_t ui8_fileHandle, uint16_t ui16_count, uint8_t *pui8_data)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->writeFile(ui8_fileHandle, ui16_count, pui8_data);
    }
    iFsCommandErrors FsClientServerCommunication_c::closeFile(uint8_t ui8_fileHandle)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->closeFile(ui8_fileHandle);
    }

    iFsCommandErrors FsClientServerCommunication_c::moveFile(uint8_t *pui8_sourceName, uint8_t *pui8_destName, bool b_recursive, bool b_force, bool b_copy)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->moveFile(pui8_sourceName, pui8_destName, b_recursive, b_force, b_copy);
    }
    iFsCommandErrors FsClientServerCommunication_c::deleteFile(uint8_t *pui8_fileName, bool b_recursive, bool b_force)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->deleteFile(pui8_fileName, b_recursive, b_force);
    }
    iFsCommandErrors FsClientServerCommunication_c::getFileAttributes(uint8_t *pui8_fileName)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->getFileAttributes(pui8_fileName);
    }
    iFsCommandErrors FsClientServerCommunication_c::setFileAttributes(uint8_t *pui8_fileName, uint8_t ui8_hiddenAtt, uint8_t ui8_readOnlyAtt)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->setFileAttributes(pui8_fileName, ui8_hiddenAtt, ui8_readOnlyAtt);
    }
    iFsCommandErrors FsClientServerCommunication_c::getFileDateTime(uint8_t *pui8_fileName)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->getFileDateTime(pui8_fileName);
    }

    iFsCommandErrors FsClientServerCommunication_c::initializeVolume(uint8_t *pui8_pathName, uint32_t ui32_space, bool b_createVolumeUsingSpace, bool b_createNewVolume)
    {
      if (!pc_commandHandler)
        return fsCommandNotPressent;
      if (pc_commandHandler->isBusy())
        return fsCommandBusy;
      else
        return pc_commandHandler->initializeVolume(pui8_pathName, ui32_space, b_createVolumeUsingSpace, b_createNewVolume);
    }


/// FileServer access functions END



    /// FileServer access response functions
    void FsClientServerCommunication_c::getCurrentDirectoryResponse(iFsError ui8_errorCode, uint8_t *piu8_newCurrentDirectory)
    {
      if (!ui8_errorCode)
      {
        uint16_t ui16_length = strlen((char *)piu8_newCurrentDirectory);
        pui8_currentDirectory = new uint8_t[ui16_length + 1];
        pui8_currentDirectory[ui16_length] = 0;

        for (uint16_t i = 0; i < ui16_length; i++)
        {
          pui8_currentDirectory[i] = piu8_newCurrentDirectory[i];
        }
      }
      c_fsClient.fileserverReady();

      c_fileServer->setUsed();

    }

    void FsClientServerCommunication_c::changeCurrentDirectoryResponse(iFsError ui8_errorCode, uint8_t *piu8_newCurrentDirectory)
    {
      if (!ui8_errorCode)
      {
        uint16_t ui16_length = strlen((char *)piu8_newCurrentDirectory);
        pui8_currentDirectory = new uint8_t[ui16_length + 1];
        pui8_currentDirectory[ui16_length] = 0;

        for (uint16_t i = 0; i < ui16_length; i++)
        {
          pui8_currentDirectory[i] = piu8_newCurrentDirectory[i];
        }
      }

      c_fsClient.changeCurrentDirectoryResponse(ui8_errorCode);
    }

    void FsClientServerCommunication_c::moveFileResponse(iFsError ui8_errorCode)
    {
      c_fsClient.moveFileResponse(ui8_errorCode);
    }
    void FsClientServerCommunication_c::deleteFileResponse(iFsError ui8_errorCode)
    {
      c_fsClient.deleteFileResponse(ui8_errorCode);
    }
    void FsClientServerCommunication_c::getFileAttributesResponse(iFsError ui8_errorCode,
                                   bool b_caseSensitive,
                                   bool b_removable,
                                   bool b_longFilenames,
                                   bool b_directory,
                                   bool b_volume,
                                   bool b_hidden,
                                   bool b_readOnly)
    {
      c_fsClient.getFileAttributesResponse(ui8_errorCode, b_caseSensitive, b_removable, b_longFilenames, b_directory, b_volume, b_hidden, b_readOnly);
    }
    void FsClientServerCommunication_c::setFileAttributesResponse(iFsError ui8_errorCode)
    {
      c_fsClient.setFileAttributesResponse(ui8_errorCode);
    }
    void FsClientServerCommunication_c::getFileDateTimeResponse(iFsError ui8_errorCode,
                                 uint16_t ui16_fileYear,
                                 uint8_t ui8_fileMonth,
                                 uint8_t ui8_fileDay,
                                 uint8_t ui8_fileHour,
                                 uint8_t ui8_fileMinute,
                                 uint8_t ui8_fileSecond)
    {
      c_fsClient.getFileDateTimeResponse(ui8_errorCode, ui16_fileYear, ui8_fileMonth, ui8_fileDay, ui8_fileHour, ui8_fileMinute, ui8_fileSecond);
    }

    void FsClientServerCommunication_c::initializeVolumeResponse(iFsError ui8_errorCode,
                                   bool b_caseSensitive,
                                   bool b_removable,
                                   bool b_longFilenames,
                                   bool b_directory,
                                   bool b_volume,
                                   bool b_hidden,
                                   bool b_readOnly)
    {
      c_fsClient.initializeVolumeResponse(ui8_errorCode, b_caseSensitive, b_removable, b_longFilenames, b_directory, b_volume, b_hidden, b_readOnly);
    }

    /// FileServer access response functions END


//End namespace
}
