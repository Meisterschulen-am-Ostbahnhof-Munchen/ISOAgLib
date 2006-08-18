/***************************************************************************
                          multireceive_c.cpp - Implementation of
                          ISO 11783-Part 3 (Extended-)Transport-Protocol and
                          NMEA 2000 Fast-Packet Protocol
                          -------------------
    class                : ::MultiReceive_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:40:11 2005
    copyright            : (C) 2005 by Martin Wodok (m.wodok@osb-ag.de)
    email                : m.wodok@osb-ag.de
    $Id: multireceive_c.cpp 1258 2005-06-07 10:14:39Z wodok $

    $Log$
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
 * Copyright (C) 2000 - 2005 Dipl.-Inform. Achim Spangler                  *
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

#include "multireceive_c.h"
#include "../multireceiveclient_c.h"

// IsoAgLib
#include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/driver/can/impl/filterbox_c.h>


#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif



// helper macros
#ifndef MACRO_pgnFormatOfPGN
#define MACRO_pgnFormatOfPGN(mpPgn)   ((mpPgn>>8) & 0xFF)
#endif
#define MACRO_BYTEORDER_toLoMidHi(a)    ((a) & 0xFF), (((a) >> 8) & 0xFF), (((a) >> 16) & 0xFF)
#define MACRO_BYTEORDER_toLoMidMidHi(a) ((a) & 0xFF), (((a) >> 8) & 0xFF), (((a) >> 16) & 0xFF), (((a) >> 24) & 0xFF)
#define MACRO_BYTEORDER_toLoHi(a)       ((a) & 0xFF), (((a) >> 8) & 0xFF)



// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



// helper consts
static const uint8_t scui8_tpPriority=6;


#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiReceive_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiReceive_c& getMultiReceiveInstance( uint8_t rui8_instance ) { return MultiReceive_c::instance( rui8_instance );};
#else
  /** C-style function, to get access to the unique MultiReceive_c singleton instance */
  MultiReceive_c& getMultiReceiveInstance( void ) { return MultiReceive_c::instance();};
#endif



/** the mask is set to 1FFFF00, as we're accepting for EVERY _local_ destination address first. afterwards list_clients is getting search for matching destination address */
#define MACRO_insertFilterIfNotYetExists_mask1FFFF00_setRef(mpPGN,LocalSa,reconf,ref) \
  { \
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(mpPGN) | static_cast<MASK_TYPE>(LocalSa)) << 8); \
    ref = NULL; \
    if (!__IsoAgLib::getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, __IsoAgLib::Ident_c::ExtendedIdent)) \
    { /* create FilterBox */ \
      ref = __IsoAgLib::getCanInstance4Comm().insertFilter(*this, (0x1FFFF00UL), ui32_filter, reconf, __IsoAgLib::Ident_c::ExtendedIdent); \
    } \
  }

#define MACRO_insertFilterIfNotYetExists_mask1FFFF00_useRef(mpPGN,LocalSa,reconf,ref) \
  { \
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(mpPGN) | static_cast<MASK_TYPE>(LocalSa)) << 8); \
    if (!__IsoAgLib::getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, __IsoAgLib::Ident_c::ExtendedIdent)) \
    { /* create FilterBox */ \
      __IsoAgLib::getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filter, reconf, __IsoAgLib::Ident_c::ExtendedIdent, ref); \
    } \
  }


/** the mask is set to 1FFFF00, as we're accepting for EVERY _local_ destination address first. afterwards list_clients is getting search for matching destination address */
#define MACRO_deleteFilterIfExists_mask1FFFF00(mpPGN,LocalSa) \
  { \
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(mpPGN) | static_cast<MASK_TYPE>(LocalSa)) << 8); \
    if (__IsoAgLib::getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, __IsoAgLib::Ident_c::ExtendedIdent)) \
    { /* delete FilterBox */ \
      __IsoAgLib::getCanInstance4Comm().deleteFilter( *this, (0x1FFFF00UL), ui32_filter, __IsoAgLib::Ident_c::ExtendedIdent); \
    } \
  }


MultiReceiveClientWrapper_s::MultiReceiveClientWrapper_s( IsoAgLib::MultiReceiveClient_c* rpc_client,
                                                          uint8_t rui8_clientAddress,
                                                          uint32_t rui32_pgn,
                                                          bool rb_alsoBroadcast,
                                                          bool rb_alsoGlobalErrors
                                                          #ifdef NMEA_2000_FAST_PACKET
                                                          ,bool rb_isFastPacket
                                                          #endif
                                                          SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA
                                                        )
  : SINGLETON_PARENT_CONSTRUCTOR pc_client(rpc_client)
  , ui8_clientAddress(rui8_clientAddress)
  , ui32_pgn(rui32_pgn)
  , b_alsoBroadcast (rb_alsoBroadcast)
  , b_alsoGlobalErrors (rb_alsoGlobalErrors)
  #ifdef NMEA_2000_FAST_PACKET
  , b_isFastPacket (rb_isFastPacket) // means the PGN has to be "insertFilter"/"removeFilter"ed
  #endif
{
  if (__IsoAgLib::getIsoMonitorInstance4Comm().existIsoMemberNr(rui8_clientAddress))
    c_devKey = __IsoAgLib::getIsoMonitorInstance4Comm().isoMemberNr(rui8_clientAddress).devKey();
//  else
//    shouldn't occur...
};

// //////////////////////////////// +X2C Operation 5653 : ~MultiReceive_c
MultiReceive_c::~MultiReceive_c()
{ // ~X2C
  close();
} // -X2C


void
MultiReceive_c::notifyError (IsoAgLib::ReceiveStreamIdentifier_c& rc_streamIdent, uint8_t rui8_multiReceiveErrorCode)
{
  if (rc_streamIdent.getDa() == 0xFF)
  { // BAM
    for (std::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = list_clients.begin(); i_list_clients != list_clients.end(); i_list_clients++)
    { // // inform all clients that want Broadcast-TP-Messages
      MultiReceiveClientWrapper_s& curClientWrapper = *i_list_clients;
      if (curClientWrapper.b_alsoBroadcast) {
        curClientWrapper.pc_client->notificationOnMultiReceiveError (rc_streamIdent, rui8_multiReceiveErrorCode, false);
      }
    }
  }
  else
  { // really destin specific
    if (getClient(rc_streamIdent))
    {
      getClient(rc_streamIdent)->notificationOnMultiReceiveError (rc_streamIdent, rui8_multiReceiveErrorCode, false);
    }
    else
    {
      // global notify for clients who want notification on global errors (which noone else can take ;-))
      for (std::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = list_clients.begin(); i_list_clients != list_clients.end(); i_list_clients++)
      { // // inform all clients that want Broadcast-TP-Messages
        MultiReceiveClientWrapper_s& curClientWrapper = *i_list_clients;
        if (curClientWrapper.b_alsoGlobalErrors) {
          curClientWrapper.pc_client->notificationOnMultiReceiveError (rc_streamIdent, rui8_multiReceiveErrorCode, true);
        }
      }
    }
  }
}



// //////////////////////////////// +X2C Operation 193 : processMsg
//! @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
bool
MultiReceive_c::processMsg()
{ // ~X2C
  #define MACRO_Define_t_streamType_and_checkInvalid \
    t_streamType = (StreamType_t) (((b_eCmd) ? 0x1:0) + ((b_ePgn) ? 0x2:0)); \
    if (t_streamType == StreamSpgnEcmdINVALID) { \
      /* this type is invalid - using Extended commands on Standard-TP PGN */ \
      /* answer with ConnAbort! */ \
      notifyError(c_tmpRSI, 100); \
      sendConnAbort (t_streamType, c_tmpRSI); \
      return true; /* no other CAN-Customer should be interested in that one */\
    }

  if (data().getLen() != 8)
    return true; // All corrupt (E)TP-Packages are NOT of interest for any other CAN-Customer - who would want corrupted packages?

  bool b_ePgn=false;
  bool b_eCmd=false;
  StreamType_t t_streamType; // will be set before used, see MACRO_t_streamType_checkInvalid
  uint32_t ui32_msgSize;
  uint32_t ui32_pgn;

  // Always get sure that the can-pkg from "data()" is NOT written to unless it's FULLY PARSED!

  const uint8_t ui8_pgnFormat = data().isoPf();
  const uint8_t ui8_dataByte0 = data().getUint8Data(0);
//uint8_t ui8_da = data().isoPs();
//uint8_t ui8_sa = data().isoSa();

  ui32_pgn =  uint32_t (data().getUint8Data(5)) |
             (uint32_t (data().getUint8Data(6)) << 8) |
             (uint32_t (data().getUint8Data(7)) << 16);
  IsoAgLib::ReceiveStreamIdentifier_c c_tmpRSI (ui32_pgn, data().isoPs() /* Ps is destin adr in the (E)TP-PGNs*/, data().isoSa());

  if (anyMultiReceiveClientRegisteredForThisDa (data().isoPs()))
  { // we ONLY care for this can-pkg at all, as it's NOT directed to any of the registered clients!

    switch (ui8_pgnFormat) {
      case MACRO_pgnFormatOfPGN(ETP_CONN_MANAGE_PGN):
        b_ePgn = true; // break left out intentionally!
      case MACRO_pgnFormatOfPGN(TP_CONN_MANAGE_PGN):
        #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << "\n {CM: " << data().time() << "} ";
        #endif

      {// to allow local variables
        switch (ui8_dataByte0)
        {

          case 0x14: // decimal 20
            b_eCmd=true; // break left out intentionally

          case 0x10: // decimal 16
            /////////////////////////
            /// RTS (Request To Send)
            MACRO_Define_t_streamType_and_checkInvalid

            { // to allow local variables!

              // RTS from an SA that has already a Stream running? // regardless of PGN
              Stream_c* pc_streamFound = getStream (data().isoSa(), data().isoPs() /* Ps is destin adr in the (E)TP-PGNs*/
              #ifdef NMEA_2000_FAST_PACKET
              , /* Fast-Packet:*/ false
              #endif
              );

              if (pc_streamFound != NULL) {
                // abort, already running stream is interrupted by RTS
                notifyError(c_tmpRSI, 101);
                connAbortTellClientRemoveStream (true /* send connAbort-Msg */, pc_streamFound);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** ConnectionAbort due to Already-Running-Stream! (RTS in between) ***" << (int) data().isoSa() << " " << (int)data().isoPs() << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }

              // otherwise it is a new stream, but before check from the client if he can take it (size is okay)

              // Get Message Size
              if (ui8_dataByte0 == 0x14) ui32_msgSize = data().getUint32Data(1);
              else /* 0x10, decimal 16*/ ui32_msgSize = data().getUint16Data(1);

              // Calculate Number of Packets (only for TP-#Pkg-Check!)
              uint32_t ui32_numPkg = (ui32_msgSize + 6) / 7;
              // check for TP-RTS if pkg-count matches the calculated AND if size > 0
              if (((ui8_dataByte0 == 0x10) && (data().getUint8Data(3) != ui32_numPkg))
                || (ui32_msgSize < 9))
              { // This handles both
                notifyError(c_tmpRSI, 102);
                sendConnAbort (t_streamType, c_tmpRSI);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** ConnectionAbort due to (Wrong Pkg Amount || msgSize < 9) ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }

              // First of all, is there a client registered that handles those PGNs via (E)TP-Messages?
              IsoAgLib::MultiReceiveClient_c* pc_clientFound = getClient(c_tmpRSI);
              if (pc_clientFound == NULL)
              { // There's no client registered to take this PGN->thisAddress! */
                notifyError(c_tmpRSI, 115);
                sendConnAbort (t_streamType, c_tmpRSI);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** ConnectionAbort due to PGN requested that the MR-Client has not registered to receive ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }

              // Send the Request To Send (RTS) to the client - Does he give us a Clear To Send (CTS) ?
              if (!pc_clientFound->reactOnStreamStart (c_tmpRSI, ui32_msgSize)) {
                // Client rejects this stream!
                notifyError(c_tmpRSI, 103);
                sendConnAbort (t_streamType, c_tmpRSI);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** ConnectionAbort due to Client Rejecting the stream ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }
              // else: Client accepts this stream, so create a representation of the stream NOW -
              // - further handling is done in "timeEvent()" now!*/
              createStream(t_streamType, c_tmpRSI, ui32_msgSize);
              // the constructor above sets the Stream to "AwaitCtsSend" and "StreamRunning"
              // so next timeEvent will send out the CTS and set the pkgRemainingInBurst to a correct value!

            } // end local variables allowment!
            return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!


          case 0x16: // decimal 22
            b_eCmd=true;
            ////////////////////////////
            /// DPO (Data Packet Offset)
            MACRO_Define_t_streamType_and_checkInvalid

            { // to allow local variables!
              Stream_c* pc_streamFound = getStream (c_tmpRSI
              #ifdef NMEA_2000_FAST_PACKET
              , /* Fast-Packet: */ false
              #endif
              );

              if (pc_streamFound == NULL)
              {
                notifyError(c_tmpRSI, 104);
                sendConnAbort (t_streamType, c_tmpRSI); // according to Brad: ConnAbort
                #ifdef DEBUG
                  INTERNAL_DEBUG_DEVICE << "\n\n DPO for an unknown/unopened stream!!\n ";
                #endif
                return true; // all DPOs are not of interest for MultiSend or other CAN-Customers!
              }

              if (!b_ePgn)
              {
                notifyError(c_tmpRSI, 116);
                connAbortTellClientRemoveStream (true /* send connAbort-Msg */, pc_streamFound); // according to Brad: ConnAbort
                #ifdef DEBUG
                  INTERNAL_DEBUG_DEVICE << "\n\n DPO for a Standard-TP stream!!\n ";
                #endif
                return true; // all DPOs are not of interest for MultiSend or other CAN-Customers!
              }

              // try to set the DPO in this stream - if it's not allowed right now (or timedout), this DPO was not correct
              if (! (pc_streamFound->setDataPageOffset ( uint32_t(data().getUint8Data(2)) |
                                                        (uint32_t(data().getUint8Data(3)) << 8) |
                                                        (uint32_t(data().getUint8Data(4)) << 16))))
              { // DPO not awaited now!
                notifyError(c_tmpRSI, 105);
                connAbortTellClientRemoveStream (true /* send connAbort-Msg */, pc_streamFound);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** ConnectionAbort due to DPO at wrong 't_awaitStep' - was NOT AwaitDpo ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all DPOs are not of interest for MultiSend or other CAN-Customers!
              }
            } // end local variables allowment!
            return true; // all DPOs are not of interest for MultiSend or other CAN-Customers!




          case 0x20: // decimal 32
            ////////////////////////////////////
            /// BAM (Broadcast Announce Message)
            MACRO_Define_t_streamType_and_checkInvalid

            { // to allow local variables!
              // Is BAM directed to 0xFF (global) ?
              if (data().isoPs() != 0xFF)
              { // we do NOT take BAMs that are NOT directed to the GLOBAL (255) address
                notifyError(c_tmpRSI, 112);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** BAM to NON-GLOBAL address "<< (uint16_t) data().isoPs() <<" ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all BAMs are not of interest for MultiSend or other CAN-Customers!
              }

              // From now on it is assured that BAM is directed to 0xFF (255)

              // BAM from an SA that has already a Stream running?
              Stream_c* pc_streamFound = getStream (data().isoSa(), 255 /* 0xFF, BAM is always to GLOBAL */
              #ifdef NMEA_2000_FAST_PACKET
              , false
              #endif
              );
              if (pc_streamFound != NULL) {
                // abort already running stream is interrupted by BAM
                notifyError(c_tmpRSI, 117);
                connAbortTellClientRemoveStream (false /* do NOT send ConnAbort msg */, pc_streamFound);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** ConnectionAbort due to Already-Running-Stream! (BAM in between) ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                // return true;
                // ^^^ do NOT return, if the old BAM is "aborted" due to this BAM, try with this BAM now...
              }

              ui32_msgSize = data().getUint16Data(1);

              uint32_t ui32_numPkg = (ui32_msgSize + 6) / 7;
              // check for TP-RTS if pkg-count matches the calculated AND if size > 0
              if ((data().getUint8Data(3) != ui32_numPkg) || (ui32_msgSize < 9))
              { // This handles both
                notifyError(c_tmpRSI, 113);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << "\n*** BAM not taken due to (Wrong Pkg Number || msgSize < 9) ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }

              // NO client checks as in RTS-case above, as it's for ALL clients, they HAVE to take it =)

              // "Stream_c"'s constructor will set awaitStep to "awaitData" and timeOut to 250ms!
              createStream(t_streamType, c_tmpRSI, ui32_msgSize);
            }

            return true; // all BAMs are not of interest for MultiSend or other CAN-Customers!



          case 0xFF: // decimal 255
            ////////////////////////////////
            /// ConnAbort (Connection Abort)
            // same for TP and ETP
            MACRO_Define_t_streamType_and_checkInvalid

            { // to allow local variables!
              // also allow a BAM to be aborted by the sender himself...
              Stream_c* pc_streamFound = getStream (c_tmpRSI
              #ifdef NMEA_2000_FAST_PACKET
              , false
              #endif
              );

              if (pc_streamFound) {
                notifyError (c_tmpRSI, 107);
                connAbortTellClientRemoveStream (false /* do not send connAbort-Msg */, pc_streamFound);
                // here TRUE could be returned, but for unknown safety just let both (MS/MR) process ConnAbort!
                // Also it "could" happen that MultiReceive & MultiSend are running parallel, so this ConnAbort would be for both!
              }
              else
              {
                // ignore an incoming ConnAbort for an unopened/unknown stream!
              }
            } // end local variables allowment!
            return false; // ConnAbort is also of interest for MultiSend!!



          case 0x15: // decimal 21 ETP.CM_CTS  --> this is only sent by receiver of a multi send
          case 0x17: // decimal 23 ETP.CM_EOMA --> this is only sent by receiver of a multi send
          case 0x11: // decimal 17 TP.CM_CTS
          case 0x13: // decimal 19 TP.CM_EndOfMsgACK
            ////////////////////////
            /// CTS (for MultiSend)
            // ignore here and simply pass on to MultiSend (using return false)
            return false;

          default:
            #ifdef DEBUG
              INTERNAL_DEBUG_DEVICE << "UNKNOWN/INVALID command with (E)TP-PGN: Sending ConnAbort, not passing this on to MultiSend!!" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            t_streamType = (StreamType_t) ((b_ePgn) ? 0x2:0); // only care for ePgn or not for ConnAbort
            notifyError(c_tmpRSI, 108);
            sendConnAbort (t_streamType, c_tmpRSI);
            return true;
        }//end switch (ui8_dataByte)
      }//end allowment of local variables...
      break; // not reachable, but leave it here because if the above is changed, it's better to have the break here...


      case MACRO_pgnFormatOfPGN(ETP_DATA_TRANSFER_PGN):
        b_ePgn = true; // break left out intentionally!
      case MACRO_pgnFormatOfPGN(TP_DATA_TRANSFER_PGN):
        #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << "{DATA: " << data().time() << "} "; INTERNAL_DEBUG_FLUSH
        #endif

        /////////////////////////
        /// Data Transfer (DATA)
        { // to allow local variables!
          // Check if there's already a Stream active from this SA->DA pair (it should ;-)
          Stream_c* pc_streamFound = getStream (data().isoSa(), data().isoPs() /* Ps is destin adr in the (E)TP-PGNs*/
          #ifdef NMEA_2000_FAST_PACKET
          , false
          #endif
          );
          if (pc_streamFound == NULL) {
            // There's no stream running for this multi-packet-DATA!, this [DATA] MAY BE for MultiSend, so simply return false!
            #ifdef DEBUG
            INTERNAL_DEBUG_DEVICE << "\n*** (E)TP.DATA, but no open stream! ignoring that... ***" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            notifyError(c_tmpRSI, 111);
            return false;
          }
          // From this point on the SA/DA pair matches, so that we can return true
          if (!(pc_streamFound->handleDataPacket(data().getDataUnionConst()))) {
            // Stream was not in state of receiving DATA right now, connection abort, inform Client and close Stream!
            if (data().isoPs() == 0xFF)
            {
              notifyError(c_tmpRSI, 114);
              #ifdef DEBUG
              INTERNAL_DEBUG_DEVICE << "\n*** BAM sequence error ***" << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
            } else {
              notifyError(c_tmpRSI, 109);
              #ifdef DEBUG
              INTERNAL_DEBUG_DEVICE << "\n*** ConnectionAbort due to (E)TP.DATA, but wrong sequence number, see msg before! ***" << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
            }
            // \/ will take care of NOT sending out the connAbort/TellingClient if it was a Broadcast!
            connAbortTellClientRemoveStream (true /* send connAbort-Msg */, pc_streamFound);
          }
          // further handling (send next CTS) is moved to timeEvent, so we don't get confused with CM/DATA PGNs, as they come very fast!

          // Stream is now set to either
          // -- StreamFinished -or- AwaitCtsSend
          // OR REMOVED!

        } // end local variables allowment!
        return true; // if code execution comes to here, then the right SA/DA Pair was there so it WAS for MultiReceive, so we can return true safely!

      default:
        #ifdef NMEA_2000_FAST_PACKET
        break; // let's see if it's a fast-packet registered pgn
        #else
        return false; // PGN not managed here, so return false so that other CAN-Customers will "processMsg" them!
        #endif
    }
  } // end if directed to known dest (this check works NOT for fast packet!!!)
  #ifndef NMEA_2000_FAST_PACKET
  // This point should NOT be reached anyway! all "case" statements
  return false;
  #else
  // Check if it's registered for fast-packet receive
  for (std::list<MultiReceiveClientWrapper_s>::iterator pc_iter = list_clients.begin(); pc_iter != list_clients.end(); pc_iter++)
  { // is it fast-packet, and is it this pgn?
    /** @todo determine when to set the PS field of the pgn to "rui8_clientAddress" */
    if ((pc_iter->b_isFastPacket) && (pc_iter->ui32_pgn == data().isoPgn()))
    {
      uint8_t ui8_counterFrame = data().getUint8Data (0) & 0x1F;
      #ifdef DEBUG
      uint8_t ui8_counterSequence = (data().getUint8Data (0) >> 5) & 0x7;
      #endif

      /** @todo determine if Fast-Packet is always addressed to GLOBAL 255, 0xFF */
      Stream_c* pc_streamFound = getStream(data().isoSa(), 0xFF, /* Fast-Packet: */ true);
      if (!pc_streamFound)
      { // stream not there. create a new one if it's the first frame
        if (ui8_counterFrame == 0)
        { // First Frame => okay, create new Stream!
          /** @todo check for range of 0..223 */
          /** @todo determine when to set the PS field of the pgn to "rui8_clientAddress" */
          IsoAgLib::ReceiveStreamIdentifier_c c_fpRSI (data().isoPgn(), 0xFF /* Ps is destin adr in the (E)TP-PGNs*/, data().isoSa());
          pc_streamFound = createStream (StreamFastPacket, c_fpRSI, data().getUint8Data (1));
        }
        else
        { // else no stream open and wrong packeted number comes in.
          notifyError(c_tmpRSI, 118);
          #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << "\n*** FastPacket-Frame "<<(uint16_t)ui8_counterFrame<<", but no open stream! ignoring that... ***" << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          return true;
        }
      } // else stream there, so pass on data, just like for first frame

      if (!(pc_streamFound->handleDataPacket(data().getDataUnionConst()))) {
        // Stream was not in state of receiving DATA right now, connection abort, inform Client and close Stream!
        notifyError(c_tmpRSI, 119);
        /// Do NOT tell client on Abort of something it doesn't know about.
        //connAbortTellClientRemoveStream (false /* no ConnAbort to GlobalAddress */, pc_streamFound);
        removeStream (pc_streamFound);
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "\n*** FastPacket sequence error ***" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }

      return true;
    }
  }

  return false; // PGN not managed here, so return false so that other CAN-Customers will "processMsg" them!
  #endif
} // -X2C



// Operation: registerClient
void
MultiReceive_c::registerClient(uint32_t rui32_pgn, uint8_t rui8_clientAddress,
                               IsoAgLib::MultiReceiveClient_c* rpc_client, bool rb_alsoBroadcast, bool rb_alsoGlobalErrors
                               #ifdef NMEA_2000_FAST_PACKET
                               , bool rb_isFastPacket
                               #endif
                               )
{ // ~X2C
  std::list<MultiReceiveClientWrapper_s>::iterator list_clients_i = list_clients.begin();
  // Already in list?
  while (list_clients_i != list_clients.end()) {
    MultiReceiveClientWrapper_s iterMRCW = *list_clients_i;
    if ((iterMRCW.pc_client == rpc_client) && (iterMRCW.ui8_clientAddress == rui8_clientAddress) && (iterMRCW.ui32_pgn == rui32_pgn))
      return; // Already in list!!
    list_clients_i++;
  }
  // Not already in list, so insert!
  list_clients.push_back(MultiReceiveClientWrapper_s(rpc_client, rui8_clientAddress, rui32_pgn, rb_alsoBroadcast, rb_alsoGlobalErrors
                         #ifdef NMEA_2000_FAST_PACKET
                         , rb_isFastPacket
                         #endif
                         SINGLETON_VEC_KEY_USE4CALL
                         ) );
  #ifdef NMEA_2000_FAST_PACKET
  /// Fast-Packet additions
  if (rb_isFastPacket)
  {
    const uint32_t ui32_mask = (0x1FFFF00UL);
    const uint32_t ui32_filter = (static_cast<MASK_TYPE>(rui32_pgn << 8));
    /** @todo determine when to set the PS field of the pgn to "rui8_clientAddress" */
    if (!__IsoAgLib::getCanInstance4Comm().existFilter(*this, ui32_mask, ui32_filter, __IsoAgLib::Ident_c::ExtendedIdent))
    { /* create FilterBox */
      __IsoAgLib::getCanInstance4Comm().insertFilter(*this, ui32_mask, ui32_filter, true, __IsoAgLib::Ident_c::ExtendedIdent);
    }
  }
  #endif
} // -X2C



//  Operation: deregisterClient
void
MultiReceive_c::deregisterClient (IsoAgLib::MultiReceiveClient_c* rpc_client)
{
  // first of all remove all streams that are for this client!
  for (std::list<DEF_Stream_c_IMPL>::iterator pc_iter = list_streams.begin(); pc_iter != list_streams.end(); )
  {
    // do also erase "kept" streams!!
    if (getClient(pc_iter->getIdent()) == rpc_client)
    { // remove stream (do not call any callbacks, as deregister is likely called in the client's destructor
      pc_iter = list_streams.erase (pc_iter);
    } else {
      pc_iter++;
    }
  }

  // then remove all MultiReceiveClientWrappers for this client
  for (std::list<MultiReceiveClientWrapper_s>::iterator pc_iter = list_clients.begin(); pc_iter != list_clients.end(); )
  {
    if (pc_iter->pc_client == rpc_client)
    { // remove MultiReceiveClientWrapper_s
      #ifdef NMEA_2000_FAST_PACKET
      /// Fast-Packet additions
      if (pc_iter->b_isFastPacket) {
        const uint32_t cui32_mask = (0x1FFFF00UL);
        const uint32_t cui32_filter = (static_cast<MASK_TYPE>(pc_iter->ui32_pgn << 8));
        /** @todo determine when to set the PS field of the pgn to "rui8_clientAddress" */
        if (__IsoAgLib::getCanInstance4Comm().existFilter( *this, cui32_mask, cui32_filter, __IsoAgLib::Ident_c::ExtendedIdent))
        { /* delete FilterBox */
          __IsoAgLib::getCanInstance4Comm().deleteFilter( *this, cui32_mask, cui32_filter, __IsoAgLib::Ident_c::ExtendedIdent);
        }
      }
      #endif
      pc_iter = list_clients.erase (pc_iter);
    } else {
      pc_iter++;
    }
  }
}



// //////////////////////////////// +X2C Operation 845 : createStream
//! Parameter:
//! ONLY CALL THIS IF YOU KNOW THAT THERE'S NOT SUCH A STREAM ALREADY IN LIST!
//! @param rc_streamIdent:
Stream_c*
MultiReceive_c::createStream(StreamType_t rt_streamType, IsoAgLib::ReceiveStreamIdentifier_c rc_streamIdent, uint32_t rui32_msgSize)
{ // ~X2C
/**
  // check if not already there?? not any more!
  for (std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin();
       i_list_streams != list_streams.end();
       i_list_streams++) {
    DEF_Stream_c_IMPL* curStream = &*i_list_streams;
    if (curStream->getIdent() == rc_streamIdent) // .matchRSI (rc_streamIdent)
      return NULL;
  }
*/
  // Not there, so create!
  list_streams.push_back (DEF_Stream_c_IMPL (rt_streamType, rc_streamIdent, rui32_msgSize));
  list_streams.back().immediateInitAfterConstruction();
  return &list_streams.back();
} // -X2C



// //////////////////////////////// +X2C Operation 849 : getStream (the public one)
//! Parameter:
//! @param rc_streamIdent:
Stream_c*
MultiReceive_c::getStream(IsoAgLib::ReceiveStreamIdentifier_c rc_streamIdent
                          #ifdef NMEA_2000_FAST_PACKET
                          , bool rb_fastPacket
                          #endif
                          )
{ // ~X2C
  std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin();
  while (i_list_streams != list_streams.end())
  {
    DEF_Stream_c_IMPL& curStream = *i_list_streams;
    #ifdef NMEA_2000_FAST_PACKET
    if ((curStream.getIdent() == rc_streamIdent) && (rb_fastPacket == (curStream.getStreamType() == StreamFastPacket))) // .matchRSI (rc_streamIdent) and either "rb_fastPacket == false and Stream not FP" or "rb_fastPacket == true and Stream is FP"!
    #else
    if (curStream.getIdent() == rc_streamIdent) // .matchRSI (rc_streamIdent)
    #endif
    {
      if (curStream.getStreamingState() != StreamFinishedJustKept)
      { // only return streams that are not "kept". ignore kept streams here for further processing!
        return &curStream;
      }
    }
    i_list_streams++;
  }
  return NULL;
} // -X2C


//  Operation: getStream (private!)
//! Parameter:
//! @param rc_streamIdent:
//! @return NULL for "doesn't exist", otherwise valid "DEF_Stream_c_IMPL*"
Stream_c* MultiReceive_c::getStream(uint8_t sa, uint8_t da
                                    #ifdef NMEA_2000_FAST_PACKET
                                    , bool rb_fastPacket
                                    #endif
                                    )
{
  std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin();
  while (i_list_streams != list_streams.end()) {
    DEF_Stream_c_IMPL& curStream = *i_list_streams;
    #ifdef NMEA_2000_FAST_PACKET
    if ((curStream.getIdent().matchSaDa (sa, da)) && (rb_fastPacket == (curStream.getStreamType() == StreamFastPacket)))
    #else
    if (curStream.getIdent().matchSaDa (sa, da))
    #endif
    {
      if (curStream.getStreamingState() == StreamFinished)
      { // finish it now, because a new one will have to be created NOW - do we can't wait with finishing until timeEvent!
        const bool cb_keepStream = finishStream (curStream);
        if (cb_keepStream)
        { // keep it, do NOT delete
          i_list_streams++;
          continue;
        }
        else
        { // delete it
          i_list_streams = list_streams.erase (i_list_streams);
          continue;
        }
      }
      if (curStream.getStreamingState() != StreamFinishedJustKept)
      { // only return streams that are not "kept". ignore kept streams here for further processing!
        return &curStream;
      }
    }
    i_list_streams++;
  }
  return NULL;
}

// //////////////////////////////// +X2C Operation : removeStream
//! Parameter:
//! @param rpc_stream:
//! Only removes the stream from list, won't call any clients or alike...
void
MultiReceive_c::removeStream(Stream_c* rpc_stream)
{ // ~X2C
  for (std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin();
       i_list_streams != list_streams.end();
       i_list_streams++) {
    if (rpc_stream == (&*i_list_streams))
    { // also let "kept" streams be erased!
      list_streams.erase (i_list_streams);
      return;
    }
  }
} // -X2C



// //////////////////////////////// +X2C Operation : processStreamDataChunk_ofMatchingClient
//! Parameter:
bool /* keep it? */
MultiReceive_c::processStreamDataChunk_ofMatchingClient(Stream_c* pc_stream, bool b_lastChunk)
{
  bool b_firstChunk = false;
  if (pc_stream->getBurstNumber() == 1) {
    // For first chunk processing, fill in the "ui8_streamFirstByte" field into the stream, so that the Client will now and later know what type of stream it is and how to handle the later Chunks...
    uint8_t ui8_firstByte;
    *pc_stream >> ui8_firstByte;
    pc_stream->setFirstByte(ui8_firstByte);
    b_firstChunk = true;
  }

  // There must be a registered client, we can assume that here!
  bool b_keepIt = getClient (pc_stream->getIdent())->processPartStreamDataChunk (pc_stream, b_firstChunk, b_lastChunk);
  if (!b_lastChunk) return false; // result doesn't care if it's not the last chunk
  return b_keepIt;
}


bool
MultiReceive_c::finishStream (DEF_Stream_c_IMPL& rrefc_stream)
{
  bool b_keepStream = false;
  #ifdef NMEA_2000_FAST_PACKET
  if ((rrefc_stream.getStreamType() == StreamFastPacket) || (rrefc_stream.getIdent().getDa() == 0xFF))
  { // FastPacket or BAM
  #else
  if (rrefc_stream.getIdent().getDa() == 0xFF)
  { // BAM
  #endif
    for (std::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = list_clients.begin(); i_list_clients != list_clients.end(); i_list_clients++)
    { // // inform all clients that want Broadcast-TP-Messages
      MultiReceiveClientWrapper_s& curClientWrapper = *i_list_clients;
      if (curClientWrapper.ui32_pgn == rrefc_stream.getIdent().getPgn())
      {
        if (
            #ifdef NMEA_2000_FAST_PACKET
            curClientWrapper.b_isFastPacket ||
            #endif
            curClientWrapper.b_alsoBroadcast)
        {
          curClientWrapper.pc_client->processPartStreamDataChunk (&rrefc_stream, /*firstChunk*/true/*it's only one, don't care*/, /*b_lastChunk*/true); // don't care about result, as BAMs will NOT be kept anyway!
        }
      }
    }
    // and DO NOT continue (==> keep b_keepStream == false), so the stream will be removed, as BAM-streams can NOT be kept!
  }
  else
  { // destination specific
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "\nSending End of Message Acknowledge out!" << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    sendEndOfMessageAck (&rrefc_stream);

    if (processStreamDataChunk_ofMatchingClient (&rrefc_stream, true))
    { // keep stream (in "FinishedJustKept" kinda state
      rrefc_stream.setStreamFinishedJustKept();
      b_keepStream = true;
    }
  }
  return b_keepStream;
}



// //////////////////////////////// +X2C Operation 2851 : timeEvent
//! Parameter:
//! @param ri32_endTime:
bool
MultiReceive_c::timeEvent( void )
{ // ~X2C
  std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin();
  while(i_list_streams != list_streams.end())
  {
    DEF_Stream_c_IMPL& refc_stream = *i_list_streams;
    // BEGIN timeEvent every Stream_c

    // Check how to proceed with this Stream
    if (refc_stream.getStreamingState() == StreamFinishedJustKept)
    { // those streams are only stored for later processing, do NOTHING with them!
      i_list_streams++;
      continue;
    }
    else if (refc_stream.getStreamingState() == StreamFinished)
    {
      const bool cb_keepStream = finishStream (refc_stream);

      if (cb_keepStream)
      { // do NOT delete stream then!
        i_list_streams++;
      }
      else
      { // if not "continue"d, remove Stream
        i_list_streams = list_streams.erase (i_list_streams);
      }
      continue;
    }
    else if ((refc_stream.getNextComing() == AwaitCtsSend) && (refc_stream.readyToSendCts()))
    { // this case shouldn't happen for BAM / FastPacket
      #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "Processing Burst" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      // CTS after Burst? -> process last Burst!
      if (refc_stream.getBurstNumber() > 0) { // 0 means that no CTS has been sent, the first incoming burst is nr. 1 !
        processStreamDataChunk_ofMatchingClient(&refc_stream, false);
        // don't care for result here!
      }
      #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "Send CTS to get first/next burst!" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      sendCurrentCts (&refc_stream); // will increase the burstCurrent
    }

    /// TimeOut-Checks
    if (refc_stream.timedOut())
    {
      #ifdef DEBUG
        #ifdef NMEA_2000_FAST_PACKET
        if (refc_stream.getStreamType() == StreamFastPacket)
          INTERNAL_DEBUG_DEVICE << "\n *** Fast-Packet-";
        else
        #endif
          INTERNAL_DEBUG_DEVICE << "\n *** (E)TP-";
        INTERNAL_DEBUG_DEVICE << "Stream with SA " << (uint16_t) refc_stream.getIdent().getSa() << " timedOut, so sending out 'connAbort'. AwaitStep was " << (uint16_t) refc_stream.getNextComing() << " ***" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      notifyError(refc_stream.getIdent(), 110);
      connAbortTellClient (/* send Out ConnAbort Msg*/ true, &refc_stream);
      // remove Stream
      i_list_streams = list_streams.erase (i_list_streams);
      continue;
    }

    // END timeEvent every Stream_c
    i_list_streams++; // standard weiterschaltung, im "erase" fall wird "continue" gemacht!
  }
  return true;
} // -X2C



IsoAgLib::iStream_c*
MultiReceive_c::getFinishedJustKeptStream (IsoAgLib::iStream_c* rpc_lastKeptStream)
{
  // If "last==NULL", take the first to get, else wait for last to occur and take next!
  bool b_takeIt = (rpc_lastKeptStream == NULL);

  for (std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin(); i_list_streams != list_streams.end(); i_list_streams++)
  {
    DEF_Stream_c_IMPL* pc_stream = &*i_list_streams;
    if (pc_stream->getStreamingState() == StreamFinishedJustKept)
    { // let's see if we take this kept one..
      if (b_takeIt)
        return pc_stream;
      if (rpc_lastKeptStream == pc_stream)
        b_takeIt=true; // take the next kept one following this...
    }
  }
  return NULL;
}

/// Use to remove a "kept"-stream after it is gotten by "getFinishedJustKeptStream" and processed.
void
MultiReceive_c::removeKeptStream(IsoAgLib::iStream_c* rpc_keptStream)
{
  for (std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin(); i_list_streams != list_streams.end(); i_list_streams++)
  {
    if ((&*i_list_streams) == rpc_keptStream)
    { // delete it. it's a justKept one, as we checked that before!
      if (i_list_streams->getStreamingState() != StreamFinishedJustKept)
        return; // do NOT allow any other streams to be deleted

      list_streams.erase (i_list_streams);
      return;
    }
  }
}



// //////////////////////////////// +X2C Operation 2853 : sendCurrentCts
//! Parameter:
//! @param rpc_stream:
void
MultiReceive_c::sendCurrentCts(DEF_Stream_c_IMPL* rpc_stream)
{ // ~X2C
  // This function actually IS only called if in state "AwaitCtsSend" !

  /** may also be 0, meaning HOLD CONNECTION OPEN, but we can handle multiple streams, can't we? ;-) */

  uint8_t ui8_pkgsToExpect = rpc_stream->expectBurst(Stream_c::sui8_pkgBurst); // we wish e.g. 20 pkgs (as always), but there're only 6 more missing to complete the stream!
  uint8_t pgn, cmdByte;
  uint8_t ui8_next2WriteLo =  ((rpc_stream->getPkgNextToWrite()) & 0xFF);
  uint8_t ui8_next2WriteMid= (((rpc_stream->getPkgNextToWrite()) >> 8) & 0xFF);
  uint8_t ui8_next2WriteHi = (((rpc_stream->getPkgNextToWrite()) >> 16) & 0xFF);

  if (rpc_stream->getStreamType() & StreamEcmdMASK)   cmdByte = 0x15 /* decimal: 21 */;
  else /* -------------------------------------- */ { cmdByte = 0x11 /* decimal: 17 */; ui8_next2WriteMid=ui8_next2WriteHi=0xFF; }
  if (rpc_stream->getStreamType() & StreamEpgnMASK) pgn = ETP_CONN_MANAGE_PGN >> 8;
  else /* -------------------------------------- */ pgn = TP_CONN_MANAGE_PGN >> 8;

  c_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ rpc_stream->getIdent().getSa(), /* src: */ rpc_stream->getIdent().getDa(),
                        cmdByte, ui8_pkgsToExpect, ui8_next2WriteLo, ui8_next2WriteMid, ui8_next2WriteHi, MACRO_BYTEORDER_toLoMidHi(rpc_stream->getIdent().getPgn()));
  __IsoAgLib::getCanInstance4Comm() << c_data;
} // -X2C



// //////////////////////////////// +X2C Operation : sendConnAbort
//! Parameter:
//! @param rpc_stream:
void
MultiReceive_c::sendConnAbort(StreamType_t rt_streamType, IsoAgLib::ReceiveStreamIdentifier_c rc_rsi)
{ // ~X2C
  if (rc_rsi.getDa() == 0xFF)
  { // NEVER answer to a packet that was sent to GLOBAL ADDRESS 0xFF !
    return;
  }

  uint8_t pgn = (rt_streamType & StreamEpgnMASK) ? (ETP_CONN_MANAGE_PGN >> 8) : (TP_CONN_MANAGE_PGN >> 8);

  c_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ rc_rsi.getSa(), /* src: */ rc_rsi.getDa(),
                        0xFF /* decimal: 255 */, 0xFF,0xFF,0xFF,0xFF, MACRO_BYTEORDER_toLoMidHi(rc_rsi.getPgn()));
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Sending out an ConnAbort!" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  __IsoAgLib::getCanInstance4Comm() << c_data;
} // -X2C


void
MultiReceive_c::connAbortTellClient(bool rb_sendConnAbort, Stream_c* rpc_stream)
{
  if (rb_sendConnAbort)
    sendConnAbort(rpc_stream->getStreamType(), rpc_stream->getIdent());

  if (rpc_stream->getIdent().getDa() != 0xFF)
  {
    // search Client and tell about connAbort
    IsoAgLib::MultiReceiveClient_c* pc_clientFound = getClient(rpc_stream->getIdent());
    if (pc_clientFound) {
      pc_clientFound->reactOnAbort (rpc_stream);
    }
  } // else: // NEVER promote aborts from BROADCASTS, because there's no partial parsing possible, so no need to inform the user...
}


// //////////////////////////////// +X2C Operation 2853 : connAbortTellClientRemoveStream
//! Parameter:
//! @param rb_sendConnAbort: send out a connAbort-Msg? (Do this only for receiver-initiated aborts)
//! @param rpc_stream:
//! Will send out connAbort (if requested), inform the client via "reactOnAbort" and close/remove the stream from list
void
MultiReceive_c::connAbortTellClientRemoveStream(bool rb_sendConnAbort, Stream_c* rpc_stream)
{ // ~X2C
  connAbortTellClient (rb_sendConnAbort, rpc_stream);

  removeStream(rpc_stream);
} // -X2C



// //////////////////////////////// +X2C Operation : sendEndOfMessageAck
//! Parameter:
//! @param rpc_stream:
void
MultiReceive_c::sendEndOfMessageAck(DEF_Stream_c_IMPL* rpc_stream)
{ // ~X2C
  // NO Check here, this function IS called on purpose and WILL send EoMAck and CLOSE the Stream!
  uint8_t pgn;
  if (rpc_stream->getStreamType() & StreamEpgnMASK) pgn = ETP_CONN_MANAGE_PGN >> 8;
  else /* -------------------------------------- */ pgn = TP_CONN_MANAGE_PGN >> 8;

  if (rpc_stream->getStreamType() & StreamEcmdMASK) {
    c_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ rpc_stream->getIdent().getSa(), /* src: */ rpc_stream->getIdent().getDa(),
                          0x17 /* decimal: 23 */, MACRO_BYTEORDER_toLoMidMidHi(rpc_stream->getByteTotalSize()), MACRO_BYTEORDER_toLoMidHi(rpc_stream->getIdent().getPgn()));
  } else {
    c_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ rpc_stream->getIdent().getSa(), /* src: */ rpc_stream->getIdent().getDa(),
                          0x13 /* decimal: 19 */, MACRO_BYTEORDER_toLoHi(rpc_stream->getByteTotalSize()), rpc_stream->getPkgTotalSize(), 0xFF /* reserved */, MACRO_BYTEORDER_toLoMidHi(rpc_stream->getIdent().getPgn()));
  }
  __IsoAgLib::getCanInstance4Comm() << c_data;
} // -X2C



void
MultiReceive_c::singletonInit()
{
  setAlreadyClosed(); // so init() will perform once!
  init();
}


// //////////////////////////////// +X2C Operation 2855 : init
void
MultiReceive_c::init()
{ // ~X2C
  if ( checkAlreadyClosed() ) {
    // clear state of b_alreadyClosed, so that close() is called one time
    clearAlreadyClosed();
    // register in Scheduler_c to get timeEvents
    __IsoAgLib::getSchedulerInstance4Comm().registerClient( this );
    // register to get ISO monitor list changes
    __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );

  }

} // -X2C


// //////////////////////////////// +X2C Operation : close
/** every subsystem of IsoAgLib has explicit function for controlled shutdown */
void
MultiReceive_c::close( void )
{ // ~X2C
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    __IsoAgLib::getSchedulerInstance4Comm().unregisterClient( this );

    list_streams.clear();
    list_clients.clear();

  }
} // -X2C


//  Operation: getClient
//! Parameter:
//! @param rc_streamIdent:
//! @return NULL for "doesn't exist", otherwise valid "MultiReceiveClient_c*"
IsoAgLib::MultiReceiveClient_c*
MultiReceive_c::getClient(IsoAgLib::ReceiveStreamIdentifier_c rc_streamIdent)
{
  for (std::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = list_clients.begin();
       i_list_clients != list_clients.end();
       i_list_clients++)
  {
    if (rc_streamIdent.matchDaPgn (i_list_clients->ui8_clientAddress, i_list_clients->ui32_pgn))
      return i_list_clients->pc_client;
  }
  return NULL;
}



bool
MultiReceive_c::anyMultiReceiveClientRegisteredForThisDa (uint8_t ui8_da)
{
  if (ui8_da == 0xFF) return true;
  /** @todo extend this function, so it checks if any of the clients want BAMs! */
  for (std::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = list_clients.begin();
       i_list_clients != list_clients.end();
       i_list_clients++)
  {
    if (ui8_da == i_list_clients->ui8_clientAddress)
      return true;
  }
  return false;
}



/**
  deliver reference to data pkg as reference to CANPkgExt_c
  to implement the base virtual function correct
*/
__IsoAgLib::CANPkgExt_c&
MultiReceive_c::dataBase()
{
  return c_data;
}


//! return 0x00-0xFF: first byte of the stream!
//! return 0x100: first byte not yet known!
//! return 0x101: not a valid index!
uint16_t
MultiReceive_c::getStreamFirstByte (uint32_t ui32_index) const
{
  uint32_t ui32_curIndex=0;
  if (ui32_index < list_streams.size()) {
    for (std::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = list_streams.begin(); pc_iter != list_streams.end(); pc_iter++)
    {
      if (ui32_curIndex == ui32_index) {
        if ((*pc_iter).getByteAlreadyReceived() > 0)
          return (*pc_iter).getFirstByte();
        else
          return 0x100; // first byte not yet known!
      }
      ui32_curIndex++;
    }
  }
  return 0x101; // wrong index
}


bool
MultiReceive_c::isAtLeastOneWithFirstByte(uint8_t firstByte)
{
//  INTERNAL_DEBUG_DEVICE << "\nisAtLeastOneWithFirstByte dez:" << (uint32_t) firstByte << ": ";
  for (std::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = list_streams.begin(); pc_iter != list_streams.end(); pc_iter++) {
    if ((*pc_iter).getByteAlreadyReceived() > 0)  {
      if ((*pc_iter).getFirstByte() == firstByte) {
        return true;
      }
    }
  }
  return false;
}

//! giving the data in 1/1000. This implementation works for pools up to 4 MB, which should be enough ;)
uint32_t
MultiReceive_c::getStreamCompletion1000 (uint32_t ui32_index, bool b_checkFirstByte, uint8_t ui8_returnNullIfThisIsFirstByte) const
{
  uint32_t ui32_curIndex=0;
  if (ui32_index < list_streams.size()) {
    // retrieve completion in 1/10..100%
    for (std::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = list_streams.begin(); pc_iter != list_streams.end(); pc_iter++)
    {
      if (ui32_curIndex == ui32_index) {
        if ((b_checkFirstByte) && ((*pc_iter).getFirstByte() != ui8_returnNullIfThisIsFirstByte))
          return 0;
        // over 4 MB clipping ;) shouldn't occur anyway...
        if ((*pc_iter).getByteAlreadyReceived() > 0x400000 /*(4*1024*1024)*/)
          return 1000;
        else
          return (pc_iter->getByteAlreadyReceived() * 1000) / pc_iter->getByteTotalSize();
      }
      ui32_curIndex++;
    }
  }
  return 0; // wrong index or (unlikely) not "found" in list...
}

//! giving the data in 1/1000. This implementation works for pools up to 4 MB, which should be enough ;)
uint32_t
MultiReceive_c::getMaxStreamCompletion1000 (bool b_checkFirstByte, uint8_t ui8_returnNullIfThisIsFirstByte) const
{
  uint32_t ui32_maxStreamCompletion1000=0;
  uint32_t ui32_currentCompletion1000;
  // retrieve completion in 1/10..100%
  for (std::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = list_streams.begin(); pc_iter != list_streams.end(); pc_iter++)
  {
    if ((b_checkFirstByte) && ((*pc_iter).getFirstByte() != ui8_returnNullIfThisIsFirstByte))
      ui32_currentCompletion1000=0; // don't care for this stream
    else if ((*pc_iter).getByteAlreadyReceived() > 0x400000 /*(4*1024*1024)*/) // over 4 MB clipping ;) shouldn't occur anyway...
      return 1000; // is already max ;)
    else
      ui32_currentCompletion1000 = (pc_iter->getByteAlreadyReceived() * 1000) / pc_iter->getByteTotalSize();

    if (ui32_currentCompletion1000 > ui32_maxStreamCompletion1000)
      ui32_maxStreamCompletion1000 = ui32_currentCompletion1000;
  }
  return ui32_maxStreamCompletion1000;
}



/** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
  * @param refc_devKey const reference to the item which ISOItem_c state is changed
  * @param rpc_newItem pointer to the currently corresponding ISOItem_c
    */
void
MultiReceive_c::reactOnMonitorListAdd( const __IsoAgLib::DevKey_c& refc_devKey, const __IsoAgLib::ISOItem_c* rpc_newItem )
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "reactOnMonitorListAdd() handles CLAIM of ISOItem_c for device with DevClass: " << int(refc_devKey.getDevClass())
      << ", Instance: " << int(refc_devKey.getDevClassInst()) << ", and manufacturer ID: " << int(refc_devKey.getConstName().manufCode())
      << "NOW use SA: " << int(rpc_newItem->nr()) << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_NEWLINE
      << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  if ( getSystemMgmtInstance4Comm().existLocalMemberDevKey(refc_devKey) )
  { // lcoal ISOItem_c has finished adr claim
    uint32_t ui32_nr = rpc_newItem->nr();

    /*** Filter Registration Start ***/
    __IsoAgLib::FilterBox_c* refFB;
    MACRO_insertFilterIfNotYetExists_mask1FFFF00_setRef(TP_CONN_MANAGE_PGN,ui32_nr,false,refFB)
    MACRO_insertFilterIfNotYetExists_mask1FFFF00_useRef(TP_DATA_TRANSFER_PGN,ui32_nr,false,refFB)
    MACRO_insertFilterIfNotYetExists_mask1FFFF00_setRef(ETP_CONN_MANAGE_PGN,ui32_nr,false,refFB)
    MACRO_insertFilterIfNotYetExists_mask1FFFF00_useRef(ETP_DATA_TRANSFER_PGN,ui32_nr,true,refFB)
    /*** Filter Registration End ***/
  }
  for (std::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = list_clients.begin();
       i_list_clients != list_clients.end();
       i_list_clients++)
  {
    if (i_list_clients->ui8_clientAddress == 0xFE)
    { // it's a mrcw that was set to hold, so maybe this addr-claim is for it?
      if (i_list_clients->c_devKey == refc_devKey)
      { // yes, it's that DEVKEY that lost its SA
        i_list_clients->ui8_clientAddress = rpc_newItem->nr();
      }
    }
  }
}

/** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
  * @param refc_devKey const reference to the item which ISOItem_c state is changed
  * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
  */
void
MultiReceive_c::reactOnMonitorListRemove( const __IsoAgLib::DevKey_c&
                                          refc_devKey
                                          , uint8_t rui8_oldSa )
{
#ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "reactOnMonitorListRemove() handles LOSS of ISOItem_c for device with DevClass: " << int(refc_devKey.getDevClass())
      << ", Instance: " << int(refc_devKey.getDevClassInst()) << ", and manufacturer ID: " << int(refc_devKey.getConstName().manufCode())
      << " and PREVIOUSLY used SA: " << int(rui8_oldSa) << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_NEWLINE
      << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
  if ( getSystemMgmtInstance4Comm().existLocalMemberDevKey(refc_devKey) )
  { // lcoal ISOItem_c has lost SA
    uint32_t ui32_nr = rui8_oldSa;
    MACRO_deleteFilterIfExists_mask1FFFF00(ETP_DATA_TRANSFER_PGN,ui32_nr)
    MACRO_deleteFilterIfExists_mask1FFFF00(ETP_CONN_MANAGE_PGN,ui32_nr)
    MACRO_deleteFilterIfExists_mask1FFFF00(TP_DATA_TRANSFER_PGN,ui32_nr)
    MACRO_deleteFilterIfExists_mask1FFFF00(TP_CONN_MANAGE_PGN,ui32_nr)
  }

  // Notify all registered clients
  for (std::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = list_clients.begin();
       i_list_clients != list_clients.end();
       i_list_clients++)
  {
    if (rui8_oldSa == i_list_clients->ui8_clientAddress) {
      i_list_clients->ui8_clientAddress = 0xFE;
    }
  }

  // Abort all running streams, because we do NOT save the devKey in the stream and it would be out of sync probably anyway!
  for (std::list<DEF_Stream_c_IMPL>::iterator i_list_streams = list_streams.begin();
       i_list_streams != list_streams.end();)
  {
    /** @todo What to do with the kept streams?? */
    if ((i_list_streams->getIdent().getDa() == rui8_oldSa) || (i_list_streams->getIdent().getSa() == rui8_oldSa))
    {
      connAbortTellClient(true, &(*i_list_streams));
      i_list_streams = list_streams.erase(i_list_streams);
    }
    else
    {
      i_list_streams++;
    }
  }
}



} // end namespace __IsoAgLib
