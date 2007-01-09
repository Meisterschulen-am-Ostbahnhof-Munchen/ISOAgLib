/***************************************************************************
                          ProprietaryMessageHandler_c.cpp -
                             -------------------
    begin                : Tue Oct 31 2006
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

/**
        ISO 11783 proprietary message
        61184 = EF00 hex Prop A

        65280 = FF00 hex Prop B_00

        65535 = FFFF hex Prop B_255

        126720 = 1EF00 hex Prop A2
*/

#include <list>
#include <IsoAgLib/comm/ProprietaryCan/impl/proprietarymessagehandler_c.h>

namespace __IsoAgLib
{
    /** initialize directly after the singleton instance is created.
    */
    void ProprietaryMessageHandler_c::singletonInit()
    {
      setAlreadyClosed();
      init();
    }

    /** delivers an instance of proprietaryMessageHandler
     */
    #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
      /** C-style function, to get access to the unique GPS_c singleton instance
        * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
        */
      ProprietaryMessageHandler_c& getProprietaryMessageHandlerInstance(uint8_t rui8_instance)
      {
        return ProprietaryMessageHandler_c::instance( rui8_instance );
      }
    #else
      /** C-style function, to get access to the unique GPS_c singleton instance */
      ProprietaryMessageHandler_c& getProprietaryMessageHandlerInstance( void )
      {
        return ProprietaryMessageHandler_c::instance();
      }
    #endif

    /** deliver reference to data pkg as reference to CANPkgExt_c
      to implement the base virtual function correct
    */
    CANPkgExt_c& ProprietaryMessageHandler_c::dataBase()
    {
      return c_data;
    }

    /**
        die Parameter in init(##### const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode #####) werden
        wahrscheinlich nicht gebraucht
     */
    void ProprietaryMessageHandler_c::init()
    {
      if (checkAlreadyClosed())
      {
        getSchedulerInstance4Comm().registerClient( this );
        c_data.setSingletonKey( c_data.getSingletonVecKey() );
        // register to get ISO monitor list changes
        __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );
        clearAlreadyClosed();
      }
    }

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown
     */
    void ProprietaryMessageHandler_c::close( )
    {
      if ( ! checkAlreadyClosed() )
      {
        // avoid another call
        setAlreadyClosed();
        // unregister from timeEvent() call by Scheduler_c
        getSchedulerInstance4Comm().unregisterClient( this );
        // unregister ISO monitor list changes
        __IsoAgLib::getIsoMonitorInstance4Comm().deregisterSaClaimHandler( this );
      }
    }

    /** DESTRUCTOR
     */
    ProprietaryMessageHandler_c::~ProprietaryMessageHandler_c()
    {
      close();
    }

    /** register the proprietary message client pointer in an internal list of all clients.
        Derive and register from the member attributes:
            ui32_can, ui32_canFilter, c_isonameRemoteECU, pc_localIdent
        the suitable CAN message receive filters.
        The internal implementation will take care to adapt the receive filter as soon as
        the SA of the remote or local is changed.
        @return true if client is registered otherwise false
     */
    bool ProprietaryMessageHandler_c::registerProprietaryMessageClient (ProprietaryMessageClient_c* rpc_proprietaryclient)
    {
      // look in the whole list
      for ( ProprietaryMessageClientVectorConstIterator_t client_iterator = vec_proprietaryclient.begin(); client_iterator != vec_proprietaryclient.end(); client_iterator++ )
      { // client is registered
        if ( (*client_iterator).pc_client ==  rpc_proprietaryclient ) return true;
      }
      // store old list size
      const unsigned int oldClientSize = vec_proprietaryclient.size();

      /* define receive filter with "no"-values
      scui32_noFilter marks the whole filter as "not set"*/
      IsoAgLib::iISOFilter_s s_tempIsoFilter (static_cast<IsoAgLib::iCANCustomer_c&>(static_cast<__IsoAgLib::CANCustomer_c&>(*this)), scui32_noFilter, scui32_noMask, NULL, NULL);
      ClientNode_t t_tempClientNode (rpc_proprietaryclient, s_tempIsoFilter);
      // push back new client
      vec_proprietaryclient.push_back (t_tempClientNode);
      // return true if new client is registered
      return ( vec_proprietaryclient.size() > oldClientSize ) ? true : false;
    }

    /** deregister a ProprietaryMessageClient */
    bool ProprietaryMessageHandler_c::deregisterProprietaryMessageClient (ProprietaryMessageClient_c* rpc_proprietaryclient)
    {
      /* define receive filter with "no"-values*/
      rpc_proprietaryclient->defineReceiveFilter(scui32_noFilter, scui32_noMask,screfc_noIsoName,rpc_nolocalIdent);
      /* store list size */
      const unsigned int oldClientSize = vec_proprietaryclient.size();
      // look in the whole list
      for ( ProprietaryMessageClientVectorIterator_t client_iterator = vec_proprietaryclient.begin(); client_iterator != vec_proprietaryclient.end(); client_iterator++ )
      {
        // same Client -> erase client from list
        if ( (*client_iterator).pc_client == rpc_proprietaryclient )
        {
          vec_proprietaryclient.erase (client_iterator);
          break;
        }
      }
      // true if client ist deregistered
      return (vec_proprietaryclient.size() < oldClientSize) ? true : false;
    }

    /** force an update of the CAN receive filter, as new data has been set in an already
        registered client.
        @return true, when the client has been found, so that an update has been performed
     */
    bool ProprietaryMessageHandler_c::triggerClientDataUpdate(ProprietaryMessageClient_c* rpc_proprietaryclient)
    {
      // look in the whole list
      for ( ProprietaryMessageClientVectorIterator_t client_iterator = vec_proprietaryclient.begin(); client_iterator != vec_proprietaryclient.end(); client_iterator++ )
      {
        // if client is found in the list
        if ( (*client_iterator).pc_client == rpc_proprietaryclient )
        {
          // = (*client_iterator).pc_localIdent->getName();
          if ( (rpc_proprietaryclient->pc_localIdent != NULL ) && (!rpc_proprietaryclient->pc_localIdent->isClaimedAddress()) )
          { // client not yet ready
            return false;
          }
          const IsoAgLib::iISOName_c* pc_localIsoName;
          if (rpc_proprietaryclient->pc_localIdent == NULL)
          { // if we have no IdentItem, we have no IsoName
            pc_localIsoName = NULL; // or set to ISONameUnspecified - don't care, is the same!!
          }
          else
          {
            pc_localIsoName = &rpc_proprietaryclient->pc_localIdent->isoName();
          }
          // filter and mask are also the same
          if ( (*client_iterator).s_isoFilter != IsoAgLib::iISOFilter_s (static_cast<IsoAgLib::iCANCustomer_c&>(static_cast<__IsoAgLib::CANCustomer_c&>(*this)),
                                                                         rpc_proprietaryclient->ui32_canFilter,
                                                                         rpc_proprietaryclient->ui32_canMask,
                                                                         &rpc_proprietaryclient->c_isonameRemoteECU,
                                                                         pc_localIsoName) )
          {
            /// ## if old filter is not equal to "no filter"
            if ((*client_iterator).s_isoFilter.getFilter() != scui32_noFilter)
            {
              // delete filter
               __IsoAgLib::getIsoFilterManagerInstance4Comm().removeIsoFilter ((*client_iterator).s_isoFilter);
            }
            /** set new filter */
            IsoAgLib::iISOFilter_s s_tempIsoFilter (static_cast<IsoAgLib::iCANCustomer_c&>(static_cast<__IsoAgLib::CANCustomer_c&>(*this)),
                                                    rpc_proprietaryclient->ui32_canFilter,
                                                    rpc_proprietaryclient->ui32_canMask,
                                                    &rpc_proprietaryclient->c_isonameRemoteECU,
                                                    pc_localIsoName);

            /// ## if new filter is not equal to "no filter"
            if (rpc_proprietaryclient->ui32_canFilter != scui32_noFilter)
            {
              //  insert new filter
              __IsoAgLib::getIsoFilterManagerInstance4Comm().insertIsoFilter(s_tempIsoFilter);
            }
            // update filter and mask
            (*client_iterator).s_isoFilter = s_tempIsoFilter;
            // update has performed
            return true;
          }
        }
      }
      return false;
    }

    /** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
        called by ...........
      * @param refc_isoName const reference to the item which ISOItem_c state is changed
      * @param rpc_newItem pointer to the currently corresponding ISOItem_c
     */
    void ProprietaryMessageHandler_c::reactOnMonitorListAdd( const __IsoAgLib::ISOName_c& /*refc_isoName*/, const __IsoAgLib::ISOItem_c* rpc_newItem )
    {
      // look in the whole list
      for ( ProprietaryMessageClientVectorConstIterator_t client_iterator = vec_proprietaryclient.begin(); client_iterator != vec_proprietaryclient.end(); client_iterator++ )
      {
        // look for the ident (if Proprietary B messages no ident is there)
        if ( (*client_iterator).pc_client->pc_localIdent)
        {
          // whether the client has claimed address
          if ( (*client_iterator).pc_client->pc_localIdent->isClaimedAddress() )
          {
            // address has claimed -> isoItem
            if ( (*client_iterator).pc_client->pc_localIdent->getIsoItem()  == rpc_newItem )
            {
              // insertFilter now
              triggerClientDataUpdate( (*client_iterator).pc_client );
            }
          }
        }
      }
    }

    /** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
      * @param refc_isoName const reference to the item which ISOItem_c state is changed
      * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
      */
    void ProprietaryMessageHandler_c::reactOnMonitorListRemove( const ISOName_c& /*refc_isoName*/, uint8_t /*rui8_oldSa*/ )
    {
    }

    /** send the data in
            ProprietaryMessageClient_c::s_sendData
        the data can be accessed directly by
           iProprietaryMessageHandler_c as its a friend of ProprietaryMessageClient_c
        the variable ui32_sendPeriodicMSec (in ProprietaryMessageClient_c) will be
        used to control repeated sending
     */
    void ProprietaryMessageHandler_c::sendData(ProprietaryMessageClient_c& client)
    {
      /** get a CAN instance */
      CANIO_c& c_can = getCanInstance4Comm();
      IsoAgLib::GenericData_c& refc_sendData = client.getDataSend();

      /** length <= 8 Bytes */
      if (refc_sendData.getLen() <= 8)
      { /** single packet */

        c_data.setIdent (refc_sendData.getIdent(), Ident_c::ExtendedIdent);

        /** if PGN is proprietary A PGN then add destination address */
        if ( ( ( c_data.isoPgn()) & 0x3FF00 ) == PROPRIETARY_A_PGN)
        {
          /** add destination address */
          c_data.setISONameForDA( client.c_isonameRemoteECU );
        }
        /** @todo do we need to NULL-check the localIdent? I don't think so */
        /** @todo the localIdent should always have an ISOName - if it's not yet unified,
                  then the sending will not find it in the ISOMonitor_c-list so simply no
                  packet is being sent */
        c_data.setISONameForSA (client.pc_localIdent->isoName());

        /** fix for Proprietary PGN */
        c_data.setIsoPri(6);
        /** set the right length of data */
        c_data.setLen(refc_sendData.getLen());
        /** set data  */
        for ( uint8_t i= 0; i < refc_sendData.getLen(); i++ )
        {
          c_data.setUint8Data(i, refc_sendData.getDataUi8(i));
        }
        /** sending */
        c_can << c_data;
      }
      else
      { /** multi-packet */
          /** variable should be evaluated */
//        const bool cb_couldStartMultiSend =
          getMultiSendInstance4Comm().sendIsoTarget (client.pc_localIdent->isoName(),
                                                    client.c_isonameRemoteECU,
                                                    refc_sendData.getDataStream(0),
                                                    refc_sendData.getLen(),
                                                    refc_sendData.getIdent() >> 8,
                                                    client.en_sendSuccess);
      }
    }

    /**  Operation:  Function for Debugging in Scheduler_c
    */
    const char* ProprietaryMessageHandler_c::getTaskName() const
    {
      return "ProprietaryMessageHandler_c()";
    }

  /** process received moving msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a msg
      process message can work with the received data. method has to be overloaded by the application
      proprietaryMessageHandler is deciding whether Mask and Ident are ok
      @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
    */
  bool ProprietaryMessageHandler_c::processMsg()
  {
    // look in the whole list
    for ( ProprietaryMessageClientVectorIterator_t client_iterator = vec_proprietaryclient.begin(); client_iterator != vec_proprietaryclient.end(); client_iterator++ )
    {
      if ( ( (c_data.isoPgn() << 8) & (*client_iterator).pc_client->ui32_canMask) ==  (*client_iterator).pc_client->ui32_canFilter )
      { // PGN check okay
        if ( ( (*client_iterator).pc_client->c_isonameRemoteECU.isUnspecified() || ((*client_iterator).pc_client->c_isonameRemoteECU == c_data.getISONameForSA().toConstIisoName_c())))
        { // SA check okay
          if ( (c_data.isoPgn() & 0xFF00) == 0xEF00)
          { // DA check
          /** @todo global???  DA = 0xFF (an alle) */
            if (! (( (*client_iterator).pc_client->pc_localIdent == NULL) || ((*client_iterator).pc_client->pc_localIdent->isoName() == c_data.getISONameForDA().toConstIisoName_c())))
            { // DA did NOT match
              continue;
            }
          } // Destination Address matched
          (*client_iterator).pc_client->s_receivedData.setIdent( c_data.ident() );
          /**  */
          (*client_iterator).pc_client->s_receivedData.setDataStream(0, c_data.getUint8DataConstPointer(), c_data.getLen());
          /** process message from client */
          (*client_iterator).pc_client->processMsg();
          /** s_receivedData muss noch geloescht werden */
        }
      }
    }
    return (true);
  }

    /** functions with actions, which must be performed periodically
      -> called periodically by Scheduler_c
      ==> sends base data msg if configured in the needed rates
      @return true -> all planned activities performed in allowed time
    */
    bool ProprietaryMessageHandler_c::timeEvent()
    {
/*      if (Scheduler_c::getAvailableExecTime() == 0) return false;

      SystemMgmt_c& c_systemMgmt = getSystemMgmtInstance4Comm();
      CANIO_c &c_can = getCanInstance4Comm();

      if ( ( !checkIsoFilterCreated() ) && ( c_systemMgmt.existActiveLocalIsoMember() ) )
      {
        // check if needed receive filters for ISO are active
        //setIsoFilterCreated();
        // create FilterBox_c for PROPRIETARY_PGN's
        // 0xFEA0 -> Proprietary A PGN
        // 0x1EF00 -> Proprietary A2 PGN
        // Bereich 0xFF00 bis 0xFFFF -> proprietary B PGN

        //c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x0FF00) << 8),
        //                  (static_cast<MASK_TYPE>(PROPRIETARY_B_PGN) << 8), true, Ident_c::ExtendedIdent);
      }

      if ( ( getDevKey() != NULL ) && (!getSystemMgmtInstance4Comm().existLocalMemberDevKey(*getDevKey(), true)) )
      { // local dev key for sending is registrated, but it is not yet fully claimed
        // --> nothing to do
        return true;
      }*/
    return (true);
    }
//(*client_iterator).pc_client->s_receivedData.setDataStream( 0, c_data.getUint8Data(0), c_data.getLen() );
};
