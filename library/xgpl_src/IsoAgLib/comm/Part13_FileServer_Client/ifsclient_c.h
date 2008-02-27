/***************************************************************************
                            ifsclient_c.h
                             -------------------
    begin                : Tue Oct 01 2007
    copyright            : (C) 2007 - 2008 by Dipl.-Inform. Achim Spangler
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
#ifndef IFSCLIENT_C_H
#define IFSCLIENT_C_H

#include <vector>

/* FileServer interfaces includes */
#include "ifsserverinstance_c.h"
#include "ifsstructs.h"

namespace IsoAgLib
{

/**
  * Interface class used for communication with a fileserver on the ISO-BUS. A application that wants to use the services
  * of a fileserver has to implement the methods given in this interface and register to the FsManager_c. As soon as fileserver
  * are on the bus, the FsManager reports thoser fileserver to the implementing class. It can then decide if it wants to
  * communicate to this fileserver. If it wants to communcate to the fileserver it has to register to this filerserver.
  * the fsclientservercommunication starts to request the fileservers additional properties (current directory) and notifies the
  * implementing class once all requested values have been reported.
  *
  * Requests are always done using the FsClientServerCommunication_c object for the implementing class. Responses are received
  * directly by the implementing class via the *Response methods.
  */
class iFsClient_c
{
  public:

    /** virtual destructor */
    virtual ~iFsClient_c() {}

    /**
      * virtual function called once a new fileserver has been initialized. implementing class has to decide what to do with that
      * information.
      */
    virtual void finishedConnectingToFileserver(iFsServerInstance_c &/*rc_fileServer*/) {}
    /**
      * when registering to a fileserver, the fsclientservercommunication starts to request additional information on the fileserver.
      * once this inforamtion has been received the client is notified using this function.
      */
    virtual void fileserverReady() {}
    /**
      * If the request for information on the fileserver was not sucessful,this function is called with an error code.
      */
    virtual void fileserverError(iFsError /*ui8_errorCode*/) {}

    /**
      * Method called once the client's fileserver has gone offline.
      */
    virtual bool notifyOnFileServerOffline(iFsServerInstance_c &/*rc_fsServerInstance*/) { return true; }

    /**
      * methods that are called once the requested information has been received.
      */

    /**
      * The registered fileserver's properties as defined in the ISO 11873 standard.
      */
    virtual void getFileServerPropertiesResponse(uint8_t /*ui8_versionNumber*/, uint8_t /*ui8_maxNumberOpenFiles*/, bool /*b_fsMultiVolumes*/) {}

    /**
      * After the call for a current directory change, this function is used to receive the successstatus of the command.
      */
    virtual void changeCurrentDirectoryResponse(iFsError /*ui8_errorCode*/) {}
    /**
      * After the call to open a file, this function is used to receive the successstatus of the command and the additional
      * information on the opened file. IMPORTANT: The filehandle is used for all further file access!
      */

    virtual void openFileResponse(iFsError /*ui8_errorCode*/,
                          uint8_t /*ui8_fileHandle*/,
                          bool /*b_caseSensitive*/,
                          bool /*b_removable*/,
                          bool /*b_longFilenames*/,
                          bool /*b_directory*/, 
                          bool /*b_volume*/,
                          bool /*b_hidden*/,
                          bool /*b_readOnly*/) {}
    /**
      * After the call to seek in a file, this function is used to receive the successstatus of the command and the new possition
      * of the filepointer of the file.
      */

    virtual void seekFileResponse(iFsError /*ui8_errorCode*/, uint32_t /*ui32_position*/) {}
    /**
      * After the call to read a file, this function is used to receive the successstatus of the command and the read file content.
      * IMPORTANT: the content of th file has to be copied to the applications memory as it can be changed by the library.
      */

    virtual void readFileResponse(iFsError /*ui8_errorCode*/, uint16_t /*ui16_dataLength*/, uint8_t */*pui8_data*/) {}
    /**
      * After the call to read a file, this function is used to receive the successstatus of the command if the file was a directory.
      * The content of the file is returned.
      */
    virtual void readDirectoryResponse(iFsError /*ui8_errorCode*/, std::vector<struct iFsDirectory *> /*v_directories*/) {}
    /**
      * After the call to write data to a file, this function is used to receive the successstatus of the command and the number of
      * written data.
      */
    virtual void writeFileResponse(iFsError /*ui8_errorCode*/, uint16_t /*ui16_dataWritten*/) {}
    /**
      * After the call to close a file, this function is used to receive the successstatus of the command.
      */
    virtual void closeFileResponse(iFsError /*ui8_errorCode*/) {}

    /**
      * After the call to move a file, this function is used to receive the successstatus of the command.
      */
    virtual void moveFileResponse(iFsError /*ui8_errorCode*/) {}
    /**
      * After the call to delete a file, this function is used to receive the successstatus of the command.
      */
    virtual void deleteFileResponse(iFsError /*ui8_errorCode*/) {}
    /**
      * After the call to get a file's attributes, this function is used to receive the successstatus of the command. In addition
      * the files attributes are returned.
      */
    virtual void getFileAttributesResponse(iFsError /*ui8_errorCode*/,
                                   bool /*b_caseSensitive*/,
                                   bool /*b_removable*/,
                                   bool /*b_longFilenames*/,
                                   bool /*b_directory*/,
                                   bool /*b_volume*/,
                                   bool /*b_hidden*/,
                                   bool /*b_readOnly*/) {}
    /**
      * After the call to set a files attributes, this function is used to receive the successstatus of the command.
      */
    virtual void setFileAttributesResponse(iFsError /*ui8_errorCode*/) {}
    /**
      * After the call to get a files date and time, this function is used to receive the successstatus of the command and the files
      * date and time.
      */
    virtual void getFileDateTimeResponse(iFsError /*ui8_errorCode*/,
                                 uint16_t /*ui16_fileYear*/,
                                 uint8_t /*ui8_fileMonth*/,
                                 uint8_t /*ui8_fileDay*/,
                                 uint8_t /*ui8_fileHour*/,
                                 uint8_t /*ui8_fileMinute*/,
                                 uint8_t /*ui8_fileSecond*/) {}

    /**
      * After the call to initialize a volume, this function is used to receive the successstatus of the command and the volumes
      * properties.
      */
    virtual void initializeVolumeResponse(iFsError /*ui8_errorCode*/,
                                   bool /*b_caseSensitive*/,
                                   bool /*b_removable*/,
                                   bool /*b_longFilenames*/,
                                   bool /*b_directory*/,
                                   bool /*b_volume*/,
                                   bool /*b_hidden*/,
                                   bool /*b_readOnly*/) {}

    /// FileServer access response functions END
};

}

#endif
