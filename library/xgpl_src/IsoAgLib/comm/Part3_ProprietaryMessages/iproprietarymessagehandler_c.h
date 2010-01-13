/*
  iproprietarymessagehandler_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef iPROPRIETARYMESSAGEHANDLER_C_H
#define iPROPRIETARYMESSAGEHANDLER_C_H

#include "impl/proprietarymessagehandler_c.h"
#include "iproprietarymessageclient_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib
{
  /** Central handler for proprietary PGN communication */
  class iProprietaryMessageHandler_c : private __IsoAgLib::ProprietaryMessageHandler_c
  {
  public:
  /** register the proprietary message client pointer in an internal list of all clients.
      Derive and register from the member attributes:

            ui32_can, mui32_canFilter, mc_isonameRemoteECU, mpc_localIdent

      the suitable CAN message receive filters.
      The internal implementation will take care to adapt the receive filter as soon as
      the SA of the remote or local is changed.
   */
  void registerProprietaryMessageClient (iProprietaryMessageClient_c* client);


  /** force an update of the CAN receive filter, as new data has been set in an already
      registered client.
      @return true, when the client has been found, so that an update has been performed
   */
  bool triggerClientDataUpdate (iProprietaryMessageClient_c* client);

  /** send the data in
            ProprietaryMessageClient_c::ms_sendData
      the data can be accessed directly by
           iProprietaryMessageHandler_c as its a friend of ProprietaryMessageClient_c
      the variable ui32_sendPeriodicMSec (in ProprietaryMessageClient_c) will be
      used to control repeated sending
   */
  void sendData (iProprietaryMessageClient_c& client);

  };
};

#endif



