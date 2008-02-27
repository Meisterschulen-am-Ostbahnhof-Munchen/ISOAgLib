/***************************************************************************
                            ifsmanager_c.h
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
#ifndef IFSMANAGER_C_H
#define IFSMANAGER_C_H

//base class
#include "impl/fsmanager_c.h"

//IsoAgLib includes
#include "ifsclient_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

class iFsManager_c : private __IsoAgLib::FsManager_c
{
  public :
    /**
      * Registered a client to the fsmanager and returnes the clients fsclientservercommunication used for interaction with a
      * fileserver.
      */
    iFsClientServerCommunication_c *initFsClient(iIdentItem_c &rc_identItem, iFsClient_c &rc_fsClient, std::vector<iFsWhitelist_c *> v_fsWhitelist)
    {
      return __IsoAgLib::FsManager_c::initFsClient(static_cast<__IsoAgLib::IdentItem_c&>(rc_identItem), (rc_fsClient), v_fsWhitelist)->toInterfacePointer();
    }

  private:
    /** allow getIFsManagerInstance() access to shielded base class.
      *  otherwise __IsoAgLib::getfsManagerInstance() wouldn't be accepted by compiler
      */
    #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
      friend iFsManager_c& getIFsManagerInstance (uint8_t rui8_instance);
    #else
      friend iFsManager_c& getIFsManagerInstance (void);
    #endif
};

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique fsManagerInstance_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iFsManager_c& getIFsManagerInstance (uint8_t rui8_instance = 0)
  { return static_cast<iFsManager_c&>(__IsoAgLib::getFsManagerInstance(rui8_instance)); }
#else
  /** C-style function, to get access to the unique fsManagerInstance_c singleton instance */
  inline iFsManager_c& getIFsManagerInstance (void)
  { return static_cast<iFsManager_c&>(__IsoAgLib::getFsManagerInstance()); }
#endif

//End namespace
}


#endif
