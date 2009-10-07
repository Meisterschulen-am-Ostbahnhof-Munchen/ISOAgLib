/***************************************************************************
                          proprietarymessageclient_c.cpp -
                             -------------------
    begin                : Tue Oct 31 2006
    copyright            : (C) 1999 - 2007 by Christoph Staub
    email                : c.staub@osb-ag:de
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

#include <IsoAgLib/comm/ext/ProprietaryCan/impl/proprietarymessageclient_c.h>
#include <IsoAgLib/comm/ext/ProprietaryCan/impl/proprietarymessagehandler_c.h>

namespace __IsoAgLib
{

  /** default constructor
      initializes the parameter for filter and mask to "novalue"
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c(): mui32_canMask(scui32_noMask), mui32_canFilter(scui32_noFilter),
                                                            mc_isonameRemoteECU(screfc_noIsoName), mpc_localIdent(spc_nolocalIdent),
                                                            men_sendSuccess(MultiSend_c::SendSuccess)
  {
    setSendPeriodMsec(0);
    mui32_nextSendTimeStamp = 0;
    /* register the client */
    getProprietaryMessageHandlerInstance4Comm().registerProprietaryMessageClient(this);
  }


  /** second constructor
      initializes the parameter to actual values
    */
  ProprietaryMessageClient_c::ProprietaryMessageClient_c( uint32_t aui32_mask, uint32_t aui32_filter,
                                                          const IsoName_c& acrc_rremoteECU,
                                                          const IdentItem_c& apc_localIdent)
                                                          : men_sendSuccess(MultiSend_c::SendSuccess),
                                                            mpc_localIdent(NULL)
  {
    setSendPeriodMsec(0);
    mui32_nextSendTimeStamp = 0;
    /* register the client */
    getProprietaryMessageHandlerInstance4Comm().registerProprietaryMessageClient(this);
    /* define receive filter */
    defineReceiveFilter( aui32_mask, aui32_filter, acrc_rremoteECU, &apc_localIdent);
  }

  ProprietaryMessageClient_c::~ProprietaryMessageClient_c()
  {
    /* deregister the client */
    getProprietaryMessageHandlerInstance4Comm().deregisterProprietaryMessageClient(this);
  }

  /** will be used by ProprietaryMessageHandler_c for definition of CAN-Filter
      trigger an update of CAN receive filters with call of
        - ProprietaryMessageHandler_c::triggerClientDataUpdate()
      @return true when wanted PGN is in allowed range
    */
  bool ProprietaryMessageClient_c::defineReceiveFilter( uint32_t aui32_mask, uint32_t aui32_filter, const IsoName_c& acrc_rremoteECU, const IdentItem_c* apc_localIdent)
  {
    // require a minimum mask, so that no other messages could be retrieved, but only PROP A/A2/B
    if ( ( ((aui32_mask & 0x2FF0000) == 0x2FF0000) && ((aui32_filter & 0x2FF0000) == 0x0EF0000) ) || /** Proprietary A/A2 may be receivable with one filter/mask combination! */
         ( ((aui32_mask & 0x3FF0000) == 0x3FF0000) && ((aui32_filter & 0x3FF0000) == 0x0FF0000) )    /** Proprietary B */
       )
    {
      /** set actual values for filter, mask, remote and local ident */
      mui32_canMask = aui32_mask;
      mui32_canFilter = aui32_filter;
      mc_isonameRemoteECU = acrc_rremoteECU;
      mpc_localIdent = apc_localIdent;

      /** force an update */
      getProprietaryMessageHandlerInstance4Comm().triggerClientDataUpdate(this);
      return(true);
    }
    return(false);
  }

  /** function to tell "i will send data" to the handler */
  void ProprietaryMessageClient_c::sendDataToHandler()
  {
    getProprietaryMessageHandlerInstance4Comm().sendData(*this);
  }

  /** set time period in milliseconds for repeated send of the data that has been stored in c_sendData()
      only one message is sent when period == 0
    */
  void ProprietaryMessageClient_c::setSendPeriodMsec(uint32_t aui32_sendPeriodMsec)
  {
    // notify Handler because we may be next that need sending, so it may has to reschedule!
    /** see if we have to repeat the send somewhen and have to adjust our timeInterval accordingly? */
    if (aui32_sendPeriodMsec != 0)
    { // periodic sending is requested
      if (mui32_sendPeriodicMsec != 0)
      { // periodic sending was already activated
        mui32_nextSendTimeStamp -= mui32_sendPeriodicMsec;
        mui32_nextSendTimeStamp += aui32_sendPeriodMsec;
      }
      else
      { // no periodic sending was activated before, so just set the time to the next one!
        /* saving the time stamp of this client */
        mui32_nextSendTimeStamp = HAL::getTime() + mui32_sendPeriodicMsec;
      }
    }
    // set time period in msec
    mui32_sendPeriodicMsec = aui32_sendPeriodMsec;
    // notify scheduler to re-thing his scheduling after we've changed something now
    getProprietaryMessageHandlerInstance4Comm().updateSchedulingInformation (this);
  }

};
