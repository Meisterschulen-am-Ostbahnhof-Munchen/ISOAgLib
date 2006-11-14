/***************************************************************************
                          ivtclientservercommunication.h - Interface class
                                        for vt-client-server communication
                             -------------------
    begin                : Thu Oct 05 2006
    copyright            : (C) 2006 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
    type                 : Header
    $LastChangedDate: 2006-10-05 09:49:23 +0200 (Do, 05 Okt 2006) $
    $LastChangedRevision:  $
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
 * Copyright (C) 2000 - 2006 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IVT_CLIENT_SERVER_COMMUNICATION_H
#define IVT_CLIENT_SERVER_COMMUNICATION_H

//include the declaration of the internal object
#include "impl/vtclientservercommunication_c.h"
#include "iisoterminal_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Instance Object for one VT-Client-Server communication channel
  @author Dipl.-Inf. Martin Wodok
  */
class iVtClientServerCommunication_c : private __IsoAgLib::VtClientServerCommunication_c
{
public:

  bool isVtActive() { return VtClientServerCommunication_c::isVtActive(); }

  bool sendCommandUpdateObjectPool (IsoAgLib::iVtObject_c** rppc_vtObjects, uint16_t rui16_numObjects)
    { return VtClientServerCommunication_c::sendCommandUpdateObjectPool (rppc_vtObjects, rui16_numObjects); }

private:
  /** PRIVATE constructor to forbid instantiation of this interface class.
    * it can only be static_cast'ed to this class, not constructed!
    */
  iVtClientServerCommunication_c (iIdentItem_c& rrefc_wsMasterIdentItem,iISOTerminal_c& rref_isoTerminal,iIsoTerminalObjectPool_c& rrefc_pool,char* rpc_versionLabel, uint8_t ui8_clientId)
  : __IsoAgLib::VtClientServerCommunication_c(
      static_cast<__IsoAgLib::IdentItem_c&>(rrefc_wsMasterIdentItem),
      static_cast<__IsoAgLib::ISOTerminal_c&>( rref_isoTerminal),
      rrefc_pool,rpc_versionLabel, ui8_clientId )
  {}

  friend class iISOTerminal_c;
  friend class __IsoAgLib::VtClientServerCommunication_c;
};

}
#endif
