/***************************************************************************
                          proprietarymessageclient_c.cpp -
                             -------------------
    begin                : Tue Nov 23 2006
    copyright            : (C) 1999 - 2006 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

#include <IsoAgLib/comm/ProprietaryCan/impl/proprietarymessageclient_c.h>
#include <IsoAgLib/comm/ProprietaryCan/impl/proprietarymessagehandler_c.h>

namespace __IsoAgLib
{

  /** default constructor
      initializes the parameter for filter and mask to "novalue"
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c(): ui32_canFilter(scui32_noFilter), ui32_canMask(scui32_noMask),
                                                            c_isonameRemoteECU(screfc_noIsoName), pc_localIdent(rpc_nolocalIdent)
  {
    getProprietaryMessageHandlerInstance().registerProprietaryMessageClient(this);
  }

  /** second constructor
      initializes the parameter to actual values
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c( uint32_t rui32_filter, uint32_t rui32_mask,
                                                          const IsoAgLib::iISOName_c& rrefc_rremoteECU,
                                                          const IsoAgLib::iIdentItem_c& rpc_localIdent)
  {
    /* register the client*/
    getProprietaryMessageHandlerInstance().registerProprietaryMessageClient(this);
    /* define receive filter */
    defineReceiveFilter( rui32_mask, rui32_filter, rrefc_rremoteECU, &rpc_localIdent);
  }

  /** destructor which have nothing to do
    */
  ProprietaryMessageClient_c::~ProprietaryMessageClient_c() {}

  bool processMsg();

  /** will be used by ProprietaryMessageHandler_c for definition of CAN-Filter
      trigger an update of CAN receive filters with call of
        - ProprietaryMessageHandler_c::triggerClientDataUpdate()
      @return true when wanted PGN is in allowed range
    */
  bool ProprietaryMessageClient_c::defineReceiveFilter( uint32_t rui32_mask, uint32_t rui32_filter, const IsoAgLib::iISOName_c& rrefc_rremoteECU,
                                                                          const IsoAgLib::iIdentItem_c* rpc_localIdent)
  {
    if ( ((rui32_mask & 0x0FF0000) == 0x0EF0000) && ((rui32_filter & 0x0FF0000) == 0x0EF0000)   || /** Proprietary A */
         ((rui32_mask & 0x3FF0000) == 0x3EF0000) && ((rui32_filter & 0x3FF0000) == 0x3EF0000)   || /** Proprietary A2 */
         ((rui32_mask & 0x0FF0000) == 0x0FF0000) && (0x0FF00 <= (rui32_filter << 8) <= 0x0FFFF)    /** Proprietary B */
       )
    {
      /** set actual values for filter, mask, remote and local ident */
      ui32_canFilter = rui32_filter;
      ui32_canMask = rui32_mask;
      c_isonameRemoteECU = rrefc_rremoteECU;
      pc_localIdent = rpc_localIdent;

      /* forces an update */
      getProprietaryMessageHandlerInstance().triggerClientDataUpdate(this);
      return(true);
    }
    return(false);
  }

  /** function to tell "i will send data" to the handler */
  void ProprietaryMessageClient_c::sendDataToHandler()
  {
    getProprietaryMessageHandlerInstance().sendData(this);
  }

  /** process received moving msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a msg
      process message can work with the received data. method has to be overloaded by the application
      proprietaryMessageHandler is deciding whether Mask and Ident are ok
      @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
    */
  bool ProprietaryMessageClient_c::processMsg()
  {
  return (true);
  }

};
