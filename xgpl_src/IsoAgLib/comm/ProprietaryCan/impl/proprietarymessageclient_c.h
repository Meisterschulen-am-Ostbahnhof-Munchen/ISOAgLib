/***************************************************************************
                          proprietarymessageclient_c.h -
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
 * You should have received a copy of the GNU General Public Licentemplate *
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


#ifndef PROPRIETARYMESSAGECLIENT_H
#define PROPRIETARYMESSAGECLIENT_H

#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/canpkgext_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/ProprietaryCan/impl/genericdata_c.h>

namespace IsoAgLib
{ // forward declarations for friends
  class iProprietaryMessageHandler_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{
  // forward declarations for friends
  class ProprietaryMessageHandler_c;

  /** initialization parameter for filter must be > 29 Bit */
  static const uint32_t scui32_noFilter = 0xFFFFFFFF;

  /** initialization parameter for mask must be > 29 Bit */
  static const uint32_t scui32_noMask = 0xFFFFFFFF;

  /** initialization parameter for IsoName */
  static const IsoAgLib::iISOName_c& screfc_noIsoName = IsoAgLib::iISOName_c::ISONameUnspecified;

  /** initialization parameter for local ident */
  static const IsoAgLib::iIdentItem_c* rpc_nolocalIdent = NULL;

  /** Class proprietarymessageclient_c
      internal base class where each object represents one "proprietary PGN, Local node, remote node"-set
    */
  class ProprietaryMessageClient_c
  {
      public:

      /** default constructor
          initializes the parameter for filter and mask to "novalue"
        */
      ProprietaryMessageClient_c();

      /** second constructor
          initializes the parameter for filter and mask to ""
        */
      ProprietaryMessageClient_c(uint32_t rui32_filter, uint32_t rui32_mask,
                                 const IsoAgLib::iISOName_c& rrefc_rremoteECU,
                                 const IsoAgLib::iIdentItem_c& rpc_localIdent);

      /** destructor which has nothing to do
        */
      virtual ~ProprietaryMessageClient_c();

      /** virtual bool processMsg() must be overloaded by the application
        */
      virtual bool processMsg() = 0;

      /** the application shall only get a constant reference to the received data
          is only set by the friend class __IsoAgLib::ProprietaryMessageHandler_c
        */
      const IsoAgLib::GenericData_c& getDataReceive() const
      {
        return (s_receivedData);
      }

      /** the application shall be able to set the data for send
        */
      IsoAgLib::GenericData_c& getDataSend()
      {
        return(s_sendData);
      }

      /** function to tell "i will send data" to the handler */
      void sendDataToHandler();

      /** will be used by ProprietaryMessageHandler_c for definition of CAN-Filter
          trigger an update of CAN receive filters with call of
            - ProprietaryMessageHandler_c::triggerClientDataUpdate()
          @return true when wanted PGN is in allowed range
        */
      bool defineReceiveFilter( uint32_t rui32_mask, uint32_t rui32_filter, const IsoAgLib::iISOName_c& rrefc_rremoteECU,
                                                                        const IsoAgLib::iIdentItem_c* rpc_localIdent);

      /** set time period in milliseconds for repeated send of the data that has been stored in c_sendData()
          only one message is send when period = 0
        */
      void setSendPeriodMsec(uint32_t rui32_sendPeriodMsec)
      {
        // set time period in msec
        ui32_sendPriodicMsec = rui32_sendPeriodMsec;
      }

    private:

      /** allow the IsoAgLib Handler class to directly access the data so that only the internal handler class
          can set the received data. The application shall only read received data.
       */
      friend class IsoAgLib::iProprietaryMessageHandler_c;
      friend class __IsoAgLib::ProprietaryMessageHandler_c;

      /** information for definition of received filters, the application sets the data
          ProprietaryMessageHandler_c::registerProprietaryMessageClient() reads the following
          attributes directly (see friend) to verify allowed PGN range and to set appropriate
          CAN receive filters */

      /** Filter to define receive compare */
      uint32_t ui32_canFilter;
      /** Mask for selection of decision bits */
      uint32_t ui32_canMask;

      /**
          define individual remote ECU for selection of message receive and selection of message send target
          set iISOName_c to Unspecified, when messages from global shall received and messages shall be sent
          to global
      */
      IsoAgLib::iISOName_c c_isonameRemoteECU;

      /** local ident for spec of sending and recieving filter definition
      */
      const IsoAgLib::iIdentItem_c* pc_localIdent;

      /** data structure for send and receive - separated iProprietaryMessageHandler_c places received data HERE.
          before it calls ProprietaryMessageClient_c::processMsg()
      */
      IsoAgLib::GenericData_c s_receivedData;

      /** application should place here any data that should be send
      */
      IsoAgLib::GenericData_c s_sendData;

      /** is used to control repeated sending
          0 - send only one time when iProprietaryMessageHandler_c::sendData is called
          else the sendData() starts periodically send of data
       */
      uint32_t ui32_sendPriodicMsec;
  };
};

#endif
