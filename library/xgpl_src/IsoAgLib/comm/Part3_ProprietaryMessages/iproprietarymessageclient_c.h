/***************************************************************************
    iproprietarymessageclient_c.h -
                             -------------------
    begin                : Tue Oct 31 2006
    copyright            : (C) 1999 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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

#ifndef IPROPRIETARYMESSAGECLIENT_C_H
#define IPROPRIETARYMESSAGECLIENT_C_H

#include "impl/proprietarymessageclient_c.h"

#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

// Begin Namespace IsoAgLib
namespace IsoAgLib
{

  /** Handler for a specific registered proprietary PGN */
  class iProprietaryMessageClient_c : private __IsoAgLib::ProprietaryMessageClient_c
  {
  public:
    /** Constructor of the API class registers the new instance immediatly at ProprietaryMessageHandler_c
        so that each succeeding setup call will be reflected there by call of
    */
    iProprietaryMessageClient_c()
    : ProprietaryMessageClient_c() {}

    /** Constructor of the API class registers the new instance immediatly at ProprietaryMessageHandler_c
        so that each succeeding setup call will be reflected there by call of
    */
    iProprietaryMessageClient_c(uint32_t aui32_filter, uint32_t aui32_mask,
                                const IsoAgLib::iIsoName_c& acrc_rremoteECU,
                                const IsoAgLib::iIdentItem_c& apc_localIdent)
    : ProprietaryMessageClient_c(aui32_filter, aui32_mask, static_cast<const __IsoAgLib::IsoName_c&>(acrc_rremoteECU), static_cast<const __IsoAgLib::IdentItem_c&>(apc_localIdent)) {}

    virtual ~iProprietaryMessageClient_c() {}

    /** receives the messages
        has to be overloaded by the Application
    */
    virtual void processProprietaryMsg() = 0;

    /** define receive filter which will be used by ProprietaryMessageHandler for definition of CAN filter.
        trigger an update of CAN receive filters with call of
              ProprietaryMessageHandler::tiggerClientDataUpdate()
        @return true, when wanted PGN is from allowed range
    */
    bool defineReceiveFilter( uint32_t aui32_mask, uint32_t aui32_filter, const IsoAgLib::iIsoName_c& acrc_rremoteECU,
                                                                          const IsoAgLib::iIdentItem_c* apc_localIdent)
    { return ProprietaryMessageClient_c::defineReceiveFilter (aui32_mask, aui32_filter, static_cast<const __IsoAgLib::IsoName_c&>(acrc_rremoteECU), static_cast<const __IsoAgLib::IdentItem_c*>(apc_localIdent)); }

    /** function to tell "i will send data" to the handler */
    void sendDataToHandler() { ProprietaryMessageClient_c::sendDataToHandler(); }

    /** User can check if the sendData is currently being used because MultiSend_c
        is streaming out right now. In this case DO NOT MODIFY the
        GenericData_c SendData via getDataSend() !!!
    */
    bool isSending() { return ProprietaryMessageClient_c::isSending(); }

    /** the application shall only get a constant reference to the received data is only set by the friend class
        __IsoAgLib::ProprietaryMessageHandler_c
    */
    const iGenericData_c& getDataReceive() const { return ProprietaryMessageClient_c::getDataReceive(); }

    /** the application should be able to set the data for send
    */
    iGenericData_c& getDataSend() { return ProprietaryMessageClient_c::getDataSend(); }

    /** set time period on millisec for repeated send of the data that has been stored in c_sendData.
        period = 0 means: send only one message for each call of iProprietaryMessageHandler::sendData()
    */
    void setSendPeriodMsec (uint32_t aui32_sendPeriodMsec) { ProprietaryMessageClient_c::setSendPeriodMsec(aui32_sendPeriodMsec); }
  };
};

#endif



