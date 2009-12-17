/*
  multireceive_c.cpp: Implementation of ISO 11783-Part 3
    (Extended-)Transport-Protocol and NMEA 2000 Fast-Packet Protocol

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "multireceive_c.h"

// IsoAgLib
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>


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
  MultiReceive_c& getMultiReceiveInstance( uint8_t aui8_instance ) { return MultiReceive_c::instance( aui8_instance );};
#else
  /** C-style function, to get access to the unique MultiReceive_c singleton instance */
  MultiReceive_c& getMultiReceiveInstance( void ) { return MultiReceive_c::instance();};
#endif



MultiReceiveClientWrapper_s::MultiReceiveClientWrapper_s( CanCustomer_c& arc_client,
                                                          const IsoName_c& acrc_isoNameClient,
                                                          uint32_t aui32_pgn,
                                                          uint32_t aui32_pgnMask,
                                                          bool ab_alsoBroadcast,
                                                          bool ab_alsoGlobalErrors
                                                          #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                                                          ,bool ab_isFastPacket
                                                          #endif
                                                          SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA
                                                        )
  : SINGLETON_PARENT_CONSTRUCTOR mpc_client(&arc_client)
  , mc_isoName (acrc_isoNameClient)
  , mui32_pgn(aui32_pgn)
  , mui32_pgnMask(aui32_pgnMask)
  , mb_alsoBroadcast (ab_alsoBroadcast)
  , mb_alsoGlobalErrors (ab_alsoGlobalErrors)
  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  , mb_isFastPacket (ab_isFastPacket) // means the PGN has to be "insertFilter"/"removeFilter"ed
  #endif
{
  if (__IsoAgLib::getIsoMonitorInstance4Comm().existIsoMemberISOName(acrc_isoNameClient, true)) // it needs to have claimed an address
    mui8_cachedClientAddress = __IsoAgLib::getIsoMonitorInstance4Comm().isoMemberISOName(acrc_isoNameClient).nr();
  else
  {
    mui8_cachedClientAddress = 0xFE;
    if (!mb_alsoBroadcast) //should really not happen ;-)
      getILibErrInstance().registerError( iLibErr_c::Inconsistency, iLibErr_c::System );
  }
}


// c'tor
MultiReceive_c::MultiReceive_c()
  : mlist_streams()
  , mlist_clients()
  , mi32_ctsSendDelayOneStream (CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM) // data -> cts
  , mi32_ctsSendDelayMoreStreams (CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS) // data -> cts
  , mi32_retriggerDelayForFirstCts (100)
  , mi32_timePeriodForActiveStreams (100)
{
}

// d'tor
MultiReceive_c::~MultiReceive_c()
{
  close();
}


void
MultiReceive_c::notifyError (const IsoAgLib::ReceiveStreamIdentifier_c &acrc_streamIdent, MultiReceive_c::TransferError_e a_transferErrorCode)
{
  if (acrc_streamIdent.getDa() == 0xFF)
  { // BAM
    for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = mlist_clients.begin(); i_list_clients != mlist_clients.end(); i_list_clients++)
    { // // inform all clients that want Broadcast-TP-Messages
      MultiReceiveClientWrapper_s& curClientWrapper = *i_list_clients;
      if (curClientWrapper.mb_alsoBroadcast) {
          notifyCanCustomerOfTransferError(
              *curClientWrapper.mpc_client,
              acrc_streamIdent,
              a_transferErrorCode,
              false);
      }
    }
  }
  else
  { // really destin specific
    if (getClient(acrc_streamIdent))
    {
      notifyCanCustomerOfTransferError(
          *getClient(acrc_streamIdent),
          acrc_streamIdent,
          a_transferErrorCode,
          false);
    }
    else
    {
      // global notify for clients who want notification on global errors (which noone else can take ;-))
      for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = mlist_clients.begin(); i_list_clients != mlist_clients.end(); i_list_clients++)
      { // // inform all clients that want Broadcast-TP-Messages
        MultiReceiveClientWrapper_s& curClientWrapper = *i_list_clients;
        if (curClientWrapper.mb_alsoGlobalErrors) {
          notifyCanCustomerOfTransferError(
              *curClientWrapper.mpc_client,
              acrc_streamIdent,
              a_transferErrorCode,
              true);
        }
      }
    }
  }
}



// //////////////////////////////// +X2C Operation 193 : processMsg
//! @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
bool
MultiReceive_c::processMsg()
{ // ~X2C
  #define MACRO_Define_t_streamType_and_checkInvalid \
    mt_streamType = (StreamType_t) (((b_eCmd) ? 0x1:0) + ((b_ePgn) ? 0x2:0)); \
    if (mt_streamType == StreamSpgnEcmdINVALID) { \
      /* this type is invalid - using Extended commands on Standard-TP PGN */ \
      /* answer with ConnAbort! */ \
      notifyError(c_tmpRSI, TransferErrorExtendedCommandOnStandardPgn); \
      sendConnAbort (mt_streamType, c_tmpRSI); \
      return true; /* no other CAN-Customer should be interested in that one */\
    }

  bool b_ePgn=false;
  bool b_eCmd=false;
  StreamType_t mt_streamType; // will be set before used, see MACRO_t_streamType_checkInvalid

  // Always get sure that the can-pkg from "data()" is NOT written to unless it's FULLY PARSED!
  const uint8_t cui8_pgnFormat = data().isoPf();
  const uint8_t cui8_dataByte0 = data().getUint8Data(0);
  const uint8_t cui8_da = data().isoPs();
  const uint8_t cui8_sa = data().isoSa();
  const uint32_t mui32_pgn =  uint32_t (data().getUint8Data(5)) |
                            (uint32_t (data().getUint8Data(6)) << 8) |
                            (uint32_t (data().getUint8Data(7)) << 16);

  // build the RSI here as we need it downwards normally...
  IsoAgLib::ReceiveStreamIdentifier_c c_tmpRSI (mui32_pgn, cui8_da, data().getISONameForDA().toConstIisoName_c(),
                                                          cui8_sa, data().getISONameForSA().toConstIisoName_c());

  /** @todo SOON-178 Do we need to check this anyway? What do we need to check here and what not? Well, it works for the good-case so far... */
  if (anyMultiReceiveClientRegisteredForThisDa (cui8_da))
  { // we ONLY care for this can-pkg at all, as it's NOT directed to any of the registered clients!

    switch (cui8_pgnFormat) {
      case MACRO_pgnFormatOfPGN(ETP_CONN_MANAGE_PGN):
        b_ePgn = true; // break left out intentionally!
      case MACRO_pgnFormatOfPGN(TP_CONN_MANAGE_PGN):
        #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "{CM: " << data().time() << "} ";
        #endif

      { // to allow local variables
        switch (cui8_dataByte0)
        {

          case 0x14: // decimal 20
            b_eCmd=true; // break left out intentionally

          case 0x10: // decimal 16
            /////////////////////////
            /// RTS (Request To Send)
            MACRO_Define_t_streamType_and_checkInvalid

            { // to allow local variables!

              // RTS from an SA that has already a Stream running? // regardless of PGN
              Stream_c* pc_streamFound = getStream (cui8_sa, cui8_da
              #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
              , /* Fast-Packet:*/ false
              #endif
              );

              if (pc_streamFound != NULL) {
                // abort, already running stream is interrupted by RTS
                notifyError(c_tmpRSI, TransferErrorAlreadyRunningStream);
                connAbortTellClientRemoveStream (true /* send connAbort-Msg */, pc_streamFound);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** ConnectionAbort due to Already-Running-Stream! (RTS in between) ***" << (int) data().isoSa() << " " << (int)cui8_da << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }
              /** @todo SOON-178 Maybe close old stream and open a new one nevertheless right here, right now? */

              // otherwise it is a new stream, but before check from the client if he can take it (size is okay)

              // Get Message Size
              uint32_t ui32_msgSize;
              if (cui8_dataByte0 == 0x14) ui32_msgSize = data().getUint32Data(1);
              else /* 0x10, decimal 16 */ ui32_msgSize = data().getUint16Data(1);

              // Calculate Number of Packets (only for TP-#Pkg-Check!)
              const uint32_t cui32_numPkg = (ui32_msgSize + 6) / 7;
              // check for TP-RTS if pkg-count matches the calculated AND if size > 0
              if (((cui8_dataByte0 == 0x10) && (data().getUint8Data(3) != cui32_numPkg))
                || (ui32_msgSize < 9))
              { // This handles both
                notifyError(c_tmpRSI, TransferErrorWrongPackageAmountOrMessageSize);
                sendConnAbort (mt_streamType, c_tmpRSI);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** ConnectionAbort due to (Wrong Pkg Amount || msgSize < 9) ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }

              // First of all, is there a client registered that handles those PGNs via (E)TP-Messages?
              CanCustomer_c* pc_clientFound = getClient(c_tmpRSI);
              if (pc_clientFound == NULL)
              { // There's no client registered to take this PGN->thisAddress! */
                notifyError(c_tmpRSI, TransferErrorPgnNotRequestedToReceive);
                sendConnAbort (mt_streamType, c_tmpRSI);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** ConnectionAbort due to PGN requested that the MR-Client has not registered to receive ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }

              // Send the Request To Send (RTS) to the client - Does he give us a Clear To Send (CTS) ?
              if (!pc_clientFound->reactOnStreamStart (c_tmpRSI, ui32_msgSize)) {
                // Client rejects this stream!
                notifyError(c_tmpRSI, TransferErrorClientRejectingStream);
                sendConnAbort (mt_streamType, c_tmpRSI);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** ConnectionAbort due to Client Rejecting the stream ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }
              // else: Client accepts this stream, so create a representation of the stream NOW -
              // - further handling is done in "timeEvent()" now!*/
              createStream(mt_streamType, c_tmpRSI, ui32_msgSize);
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
              #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
              , /* Fast-Packet: */ false
              #endif
              );

              if (pc_streamFound == NULL)
              {
                notifyError(c_tmpRSI, TransferErrorDpoForUnknownOrUnopenedStream);
                sendConnAbort (mt_streamType, c_tmpRSI); // according to Brad: ConnAbort
                #ifdef DEBUG
                  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_NEWLINE << "DPO for an unknown/unopened stream!!" << INTERNAL_DEBUG_DEVICE_ENDL ;
                #endif
                return true; // all DPOs are not of interest for MultiSend or other CAN-Customers!
              }

              if (!b_ePgn)
              {
                notifyError(c_tmpRSI, TransferErrorDpoForAStandardTpStream);
                connAbortTellClientRemoveStream (true /* send connAbort-Msg */, pc_streamFound); // according to Brad: ConnAbort
                #ifdef DEBUG
                  INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << INTERNAL_DEBUG_DEVICE_NEWLINE << "DPO for a Standard-TP stream!!" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all DPOs are not of interest for MultiSend or other CAN-Customers!
              }

              // try to set the DPO in this stream - if it's not allowed right now (or timedout), this DPO was not correct
              if (! (pc_streamFound->setDataPageOffset ( uint32_t(data().getUint8Data(2)) |
                                                        (uint32_t(data().getUint8Data(3)) << 8) |
                                                        (uint32_t(data().getUint8Data(4)) << 16))))
              { // DPO not awaited now!
                notifyError(c_tmpRSI, TransferErrorDpoNotAwaitedNow);
                connAbortTellClientRemoveStream (true /* send connAbort-Msg */, pc_streamFound);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** ConnectionAbort due to DPO at wrong 'mt_awaitStep' - was NOT AwaitDpo ***" << INTERNAL_DEBUG_DEVICE_ENDL;
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
              if (cui8_da != 0xFF)
              { // we do NOT take BAMs that are NOT directed to the GLOBAL (255) address
                notifyError(c_tmpRSI, TransferErrorBamToNonGlobalAddress);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** BAM to NON-GLOBAL address "<< (uint16_t) cui8_da <<" ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all BAMs are not of interest for MultiSend or other CAN-Customers!
              }

              // From now on it is assured that BAM is directed to 0xFF (255)

              // BAM from an SA that has already a Stream running?
              Stream_c* pc_streamFound = getStream (data().isoSa(), 255 /* 0xFF, BAM is always to GLOBAL */
              #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
              , false
              #endif
              );
              if (pc_streamFound != NULL) {
                // abort already running stream is interrupted by BAM
                notifyError(c_tmpRSI, TransferErrorBamInBetweenAlreadyRunningStream);
                connAbortTellClientRemoveStream (false /* do NOT send ConnAbort msg */, pc_streamFound);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** ConnectionAbort due to Already-Running-Stream! (BAM in between) ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                // return true;
                // ^^^ do NOT return, if the old BAM is "aborted" due to this BAM, try with this BAM now...
              }

              const uint32_t cui32_msgSize = data().getUint16Data(1);
              const uint32_t cui32_numPkg = (cui32_msgSize + 6) / 7;
              // check for TP-RTS if pkg-count matches the calculated AND if size > 0
              if ((data().getUint8Data(3) != cui32_numPkg) || (cui32_msgSize < 9))
              { // This handles both
                notifyError(c_tmpRSI, TransferErrorBamNotTakenWrongPkgNumberOrMessageSize);
                #ifdef DEBUG
                INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** BAM not taken due to (Wrong Pkg Number || msgSize < 9) ***" << INTERNAL_DEBUG_DEVICE_ENDL;
                #endif
                return true; // all RTSes are not of interest for MultiSend or other CAN-Customers!
              }

              // NO client checks as in RTS-case above, as it's for ALL clients, they HAVE to take it =)

              // "Stream_c"'s constructor will set awaitStep to "awaitData" and timeOut to 250ms!
              createStream(mt_streamType, c_tmpRSI, cui32_msgSize);
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
              #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
              , false
              #endif
              );

              if (pc_streamFound) {
                notifyError (c_tmpRSI, TransferErrorUnknownSafety);
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
            mt_streamType = (StreamType_t) ((b_ePgn) ? 0x2:0); // only care for ePgn or not for ConnAbort
            notifyError(c_tmpRSI, TransferErrorUnknownOrInvalidCommandWithTpPgn);
            sendConnAbort (mt_streamType, c_tmpRSI);
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
          Stream_c* pc_streamFound = getStream (cui8_sa, cui8_da
          #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
          , false
          #endif
          );
          if (pc_streamFound == NULL) {
            // There's no stream running for this multi-packet-DATA!, this [DATA] MAY BE for MultiSend, so simply return false!
            #ifdef DEBUG
            INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** (E)TP.DATA, but no open stream! ignoring that... ***" << INTERNAL_DEBUG_DEVICE_ENDL;
            #endif
            notifyError(c_tmpRSI, TransferErrorNoStreamRunningForMultiPacketData);
            return false;
          }
          // From this point on the SA/DA pair matches, so that we can return true
          if (!(pc_streamFound->handleDataPacket(data().getDataUnionConst()))) {
            // Stream was not in state of receiving DATA right now, connection abort, inform Client and close Stream!
            if (cui8_da == 0xFF)
            {
              notifyError(c_tmpRSI, TransferErrorBamSequenceError);
              #ifdef DEBUG
              INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** BAM sequence error ***" << INTERNAL_DEBUG_DEVICE_ENDL;
              #endif
            } else {
              notifyError(c_tmpRSI, TransferErrorWrongSequenceNumber);
              #ifdef DEBUG
              INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** ConnectionAbort due to (E)TP.DATA, but wrong sequence number, see msg before! ***" << INTERNAL_DEBUG_DEVICE_ENDL;
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
        #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
        break; // let's see if it's a fast-packet registered pgn
        #else
        return false; // PGN not managed here, so return false so that other CAN-Customers will "processMsg" them!
        #endif
    }
  } // end if directed to known dest (this check works NOT for fast packet!!!)
  #ifndef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  // This point should NOT be reached anyway! all "case" statements
  getILibErrInstance().registerError( iLibErr_c::Inconsistency, iLibErr_c::System );
  return false;
  #else
  // Check if it's registered for fast-packet receive
  for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator pc_iter = mlist_clients.begin(); pc_iter != mlist_clients.end(); pc_iter++)
  { // is it fast-packet, and is it this pgn?
    /** @todo SOON-178 determine when to set the PS field of the pgn to "aui8_cachedClientAddress" */
    if ((pc_iter->mb_isFastPacket) && (pc_iter->mui32_pgn == data().isoPgn()))
    {
      uint8_t ui8_counterFrame = data().getUint8Data (0) & 0x1F;
      //#ifdef DEBUG
      //uint8_t ui8_counterSequence = (data().getUint8Data (0) >> 5) & 0x7;
      //#endif

      /** @todo SOON-178 determine if Fast-Packet is always addressed to GLOBAL 255, 0xFF. Note on 11/2007: FP can also be destination specific! */
      Stream_c* pc_streamFound = getStream(data().isoSa(), 0xFF, /* Fast-Packet: */ true);
      if (!pc_streamFound)
      { // stream not there. create a new one if it's the first frame
        if (ui8_counterFrame == 0)
        { // First Frame => okay, create new Stream!
          /** @todo SOON-178 check for range of 0..223 */
          /** @todo SOON-178 determine when to set the PS field of the pgn to "aui8_cachedClientAddress" */
          IsoAgLib::ReceiveStreamIdentifier_c c_fpRSI (data().isoPgn(), 0xFF /* Ps is destin adr in the (E)TP-PGNs*/, IsoAgLib::iIsoName_c::iIsoNameUnspecified(),
                                                                        data().isoSa(),                               data().getISONameForSA().toConstIisoName_c());
          pc_streamFound = createStream (StreamFastPacket, c_fpRSI, data().getUint8Data (1));
        }
        else
        { // else no stream open and wrong packeted number comes in.
          notifyError(c_tmpRSI, TransferErrorFastPacketFrameButNoOpenStream);
          #ifdef DEBUG
          INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** FastPacket-Frame "<<(uint16_t)ui8_counterFrame<<", but no open stream! ignoring that... ***" << INTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          return true;
        }
      } // else stream there, so pass on data, just like for first frame

      if (!(pc_streamFound->handleDataPacket(data().getDataUnionConst()))) {
        // Stream was not in state of receiving DATA right now, connection abort, inform Client and close Stream!
        notifyError(c_tmpRSI, TransferErrorFastpacketSequenceError);
        /// Do NOT tell client on Abort of something it doesn't know about.
        //connAbortTellClientRemoveStream (false /* no ConnAbort to GlobalAddress */, pc_streamFound);
        removeStream (pc_streamFound);
        #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** FastPacket sequence error ***" << INTERNAL_DEBUG_DEVICE_ENDL;
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
MultiReceive_c::registerClient(CanCustomer_c& arc_client, const IsoName_c& acrc_isoName,
                               uint32_t aui32_pgn, uint32_t aui32_pgnMask,
                               bool ab_alsoBroadcast, bool ab_alsoGlobalErrors
                               #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                               , bool ab_isFastPacket
                               #endif
                              )
{ // ~X2C
  STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator list_clients_i = mlist_clients.begin();
  // Already in list?
  while (list_clients_i != mlist_clients.end()) {
    MultiReceiveClientWrapper_s iterMRCW = *list_clients_i;
    if ((iterMRCW.mpc_client == (&arc_client)) && (iterMRCW.mc_isoName == acrc_isoName) && (iterMRCW.mui32_pgn == aui32_pgn)
    #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
      && (iterMRCW.mb_isFastPacket == ab_isFastPacket)
    #endif
    )
      return; // Already in list!!
    list_clients_i++;
  }
  // Not already in list, so insert!
  mlist_clients.push_back (MultiReceiveClientWrapper_s (arc_client, acrc_isoName, aui32_pgn, aui32_pgnMask, ab_alsoBroadcast, ab_alsoGlobalErrors
                                                       #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                                                       , ab_isFastPacket
                                                       #endif
                                                       SINGLETON_VEC_KEY_WITH_COMMA
                                                      )
                         );

  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  /// Fast-Packet additions
  if (ab_isFastPacket)
  {
    const uint32_t ui32_mask = (0x3FFFF00UL);
    const uint32_t ui32_filter = (static_cast<MASK_TYPE>(aui32_pgn << 8));
    /** @todo SOON-178 determine when to set the PS field of the pgn to "aui8_cachedClientAddress" */
    if (!__IsoAgLib::getCanInstance4Comm().existFilter(*this, ui32_mask, ui32_filter, __IsoAgLib::Ident_c::ExtendedIdent))
    { /* create FilterBox */
      __IsoAgLib::getCanInstance4Comm().insertStandardIsoFilter(*this,(aui32_pgn),true);
    }
  }
  #endif
} // -X2C



//  Operation: deregisterClient
//! Will kick all the MR-Clients registered by a CanCustomer_c.
//! Mainly to be used when shutting down a class-instance
//! that just wants to notify MR that it's gone!
void
MultiReceive_c::deregisterClient (CanCustomer_c& arc_client)
{
  // first of all remove all streams that are for this client!
  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator pc_iter = mlist_streams.begin(); pc_iter != mlist_streams.end(); )
  {
    // do also erase "kept" streams!!
    if (getClient (pc_iter->getIdent()) == &arc_client)
    { // remove stream (do not call any callbacks, as deregister is likely called in the client's destructor
      pc_iter = mlist_streams.erase (pc_iter);
    } else {
      pc_iter++;
    }
  }

  // then remove all MultiReceiveClientWrappers for this client
  for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator pc_iter = mlist_clients.begin(); pc_iter != mlist_clients.end(); )
  {
    if (pc_iter->mpc_client == &arc_client)
    { // remove MultiReceiveClientWrapper_s
      #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
      /// Fast-Packet additions
      if (pc_iter->mb_isFastPacket) {
        const uint32_t cui32_mask = (0x3FFFF00UL);
        const uint32_t cui32_filter = (static_cast<MASK_TYPE>(pc_iter->mui32_pgn << 8));
        /** @todo SOON-178 determine when to set the PS field of the pgn to "aui8_cachedClientAddress" */
        __IsoAgLib::getCanInstance4Comm().deleteFilter( *this, cui32_mask, cui32_filter, __IsoAgLib::Ident_c::ExtendedIdent);
      }
      #endif
      pc_iter = mlist_clients.erase (pc_iter);
    } else {
      pc_iter++;
    }
  }
}


// Operation: deregisterClient
void
MultiReceive_c::deregisterClient(CanCustomer_c& arc_client, const IsoName_c& acrc_isoName,
                                 uint32_t aui32_pgn, uint32_t aui32_pgnMask)
{
  // first of all remove all streams that are for this client with this filter/mask/isoname tuple
  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator pc_iter = mlist_streams.begin(); pc_iter != mlist_streams.end(); )
  {
    // do also erase "kept" streams!!
    STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = mlist_clients.begin();
    while (i_list_clients != mlist_clients.end())
    {
      if (pc_iter->getIdent().matchDaPgnWithMask (i_list_clients->mui8_cachedClientAddress, i_list_clients->mui32_pgn, i_list_clients->mui32_pgnMask))
        break;
      i_list_clients++;
    }

    if (i_list_clients != mlist_clients.end())
    {
      if ( (i_list_clients->mpc_client == &arc_client)
        && (i_list_clients->mc_isoName == acrc_isoName)
        && (i_list_clients->mui32_pgn == aui32_pgn)
        && (i_list_clients->mui32_pgnMask == aui32_pgnMask)
         )
      { // remove stream (do not call any callbacks, as deregister is likely called in the client's destructor
        pc_iter = mlist_streams.erase (pc_iter);
      } else {
        pc_iter++;
      }
    }
  }

  // then remove all MultiReceiveClientWrappers for this client
  for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator pc_iter = mlist_clients.begin(); pc_iter != mlist_clients.end(); )
  {
    if ( (pc_iter->mpc_client == &arc_client)
      && (pc_iter->mc_isoName == acrc_isoName)
      && (pc_iter->mui32_pgn == aui32_pgn)
      && (pc_iter->mui32_pgnMask == aui32_pgnMask)
       )
    { // remove MultiReceiveClientWrapper_s
      #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
      /// Fast-Packet additions
      if (pc_iter->mb_isFastPacket) {
        const uint32_t cui32_mask = (0x3FFFF00UL);
        const uint32_t cui32_filter = (static_cast<MASK_TYPE>(pc_iter->mui32_pgn << 8));
        /** @todo SOON-178 determine when to set the PS field of the pgn to "aui8_cachedClientAddress" */
        __IsoAgLib::getCanInstance4Comm().deleteFilter( *this, cui32_mask, cui32_filter, __IsoAgLib::Ident_c::ExtendedIdent);
      }
      #endif
      pc_iter = mlist_clients.erase (pc_iter);
    } else {
      pc_iter++;
    }
  }
}

// //////////////////////////////// +X2C Operation 845 : createStream
//! Parameter:
//! @pre ONLY CALL THIS IF YOU KNOW THAT THERE'S NOT SUCH A STREAM ALREADY IN LIST!
//! @param at_streamType
//! @param ac_streamIdent
//! @param aui32_msgSize
Stream_c*
MultiReceive_c::createStream(StreamType_t at_streamType, IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent, uint32_t aui32_msgSize)
{ // ~X2C
/**
  // check if not already there?? not any more!
  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin();
       i_list_streams != mlist_streams.end();
       i_list_streams++) {
    DEF_Stream_c_IMPL* curStream = &*i_list_streams;
    if (curStream->getIdent() == ac_streamIdent) // .matchRSI (ac_streamIdent)
      return NULL;
  }
*/
  // Not there, so create!
  mlist_streams.push_back (DEF_Stream_c_IMPL (at_streamType, ac_streamIdent, aui32_msgSize SINGLETON_VEC_KEY_WITH_COMMA));
  mlist_streams.back().immediateInitAfterConstruction();

  // notify the Scheduler that we want to a 100ms timeEvent now (as we have at least one stream!)
  // this is yet to optimize because we can detect exactly how long to sleep, etc.etc.
  /// THIS IS ALWAYS CALLED FROM ::processMsg, so we can't use setTimePeriod() here
  __IsoAgLib::getSchedulerInstance4Comm().changeRetriggerTimeAndResort(this, HAL::getTime()+mi32_retriggerDelayForFirstCts, mi32_timePeriodForActiveStreams);

  return &mlist_streams.back();
} // -X2C



// //////////////////////////////// +X2C Operation 849 : getStream (the public one)
//! Parameter:
//! @param ac_streamIdent
Stream_c*
MultiReceive_c::getStream(IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent
                          #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                          , bool ab_fastPacket
                          #endif
                          )
{ // ~X2C
  STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin();
  while (i_list_streams != mlist_streams.end())
  {
    DEF_Stream_c_IMPL& curStream = *i_list_streams;
    #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
    if ((curStream.getIdent() == ac_streamIdent) && (ab_fastPacket == (curStream.getStreamType() == StreamFastPacket))) // .matchRSI (ac_streamIdent) and either "ab_fastPacket == false and Stream not FP" or "ab_fastPacket == true and Stream is FP"!
    #else
    if (curStream.getIdent() == ac_streamIdent) // .matchRSI (ac_streamIdent)
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


/// Operation: getStream (private!)
///
/// @param aui8_sa source address
/// @param aui8_da destination address
/// @return NULL for "doesn't exist", otherwise valid "DEF_Stream_c_IMPL*"
Stream_c* MultiReceive_c::getStream(uint8_t aui8_sa, uint8_t aui8_da
                                    #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                                    , bool ab_fastPacket
                                    #endif
                                    )
{
  STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin();
  while (i_list_streams != mlist_streams.end()) {
    DEF_Stream_c_IMPL& curStream = *i_list_streams;
    #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
    if ((curStream.getIdent().matchSaDa (aui8_sa, aui8_da)) && (ab_fastPacket == (curStream.getStreamType() == StreamFastPacket)))
    #else
    if (curStream.getIdent().matchSaDa (aui8_sa, aui8_da))
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
          i_list_streams = mlist_streams.erase (i_list_streams);
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
//! @param apc_stream
//! Only removes the stream from list, won't call any clients or alike...
void
MultiReceive_c::removeStream(Stream_c* apc_stream)
{ // ~X2C
  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin();
       i_list_streams != mlist_streams.end();
       i_list_streams++) {
    if (apc_stream == (&*i_list_streams))
    { // also let "kept" streams be erased!
      mlist_streams.erase (i_list_streams);
      return;
    }
  }
} // -X2C



// //////////////////////////////// +X2C Operation : processStreamDataChunk_ofMatchingClient
//! Parameter:
bool /* keep it? */
MultiReceive_c::processStreamDataChunk_ofMatchingClient(Stream_c& arc_stream, bool b_lastChunk)
{
  bool b_firstChunk = false;
  if (arc_stream.getBurstNumber() == 1) {
    // For first chunk processing, fill in the "mui8_streamFirstByte" field into the stream, so that the Client will now and later know what type of stream it is and how to handle the later Chunks...
    uint8_t ui8_firstByte;
    arc_stream >> ui8_firstByte;
    arc_stream.setFirstByte(ui8_firstByte);
    b_firstChunk = true;
  }

  // There must be a registered client, we can assume that here!
  bool b_keepIt = getClient (arc_stream.getIdent())->processPartStreamDataChunk (arc_stream, b_firstChunk, b_lastChunk);
  if (!b_lastChunk) return false; // result doesn't care if it's not the last chunk
  return b_keepIt;
}


bool
MultiReceive_c::finishStream (DEF_Stream_c_IMPL& arc_stream)
{
  bool b_keepStream = false;
  #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
  const bool cb_isFastPacketStream = (arc_stream.getStreamType() == StreamFastPacket);
  if (cb_isFastPacketStream || (arc_stream.getIdent().getDa() == 0xFF))
  { // FastPacket or BAM
  #else
  if (arc_stream.getIdent().getDa() == 0xFF)
  { // BAM
  #endif
    for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = mlist_clients.begin(); i_list_clients != mlist_clients.end(); i_list_clients++)
    { // // inform all clients that want Broadcast-TP-Messages
      MultiReceiveClientWrapper_s& curClientWrapper = *i_list_clients;
      if (curClientWrapper.mui32_pgn == arc_stream.getIdent().getPgn())
      {
        if (
            #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
            (curClientWrapper.mb_isFastPacket == cb_isFastPacketStream)
         && ( curClientWrapper.mb_alsoBroadcast )
            #else
            curClientWrapper.mb_alsoBroadcast
            #endif
          )
        {
          curClientWrapper.mpc_client->processPartStreamDataChunk (arc_stream, /*firstChunk*/true/*it's only one, don't care*/, /*b_lastChunk*/true); // don't care about result, as BAMs will NOT be kept anyway!
        }
      }
    }
    // and DO NOT continue (==> keep b_keepStream == false), so the stream will be removed, as BAM-streams can NOT be kept!
  }
  else
  { // destination specific
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "Sending End of Message Acknowledge out!" << INTERNAL_DEBUG_DEVICE_ENDL;
    #endif
    sendEndOfMessageAck (&arc_stream);

    if (processStreamDataChunk_ofMatchingClient (arc_stream, true))
    { // keep stream (in "FinishedJustKept" kinda state
      arc_stream.setStreamFinishedJustKept();
      b_keepStream = true;
    }
  }
  return b_keepStream;
}



// //////////////////////////////// +X2C Operation 2851 : timeEvent
bool
MultiReceive_c::timeEvent( void )
{ // ~X2C
  STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin();
  while(i_list_streams != mlist_streams.end())
  {
    DEF_Stream_c_IMPL& rc_stream = *i_list_streams;
    // BEGIN timeEvent every Stream_c

    // Check how to proceed with this Stream
    if (rc_stream.getStreamingState() == StreamFinishedJustKept)
    { // those streams are only stored for later processing, do NOTHING with them!
      i_list_streams++;
      continue;
    }
    else if (rc_stream.getStreamingState() == StreamFinished)
    {
      const bool cb_keepStream = finishStream (rc_stream);

      if (cb_keepStream)
      { // do NOT delete stream then!
        i_list_streams++;
      }
      else
      { // if not "continue"d, remove Stream
        i_list_streams = mlist_streams.erase (i_list_streams);
      }
      continue;
    }
    else if ((rc_stream.getNextComing() == AwaitCtsSend) && (rc_stream.readyToSendCts()))
    { // this case shouldn't happen for BAM / FastPacket
      #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "Processing Burst" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      // CTS after Burst? -> process last Burst!
      if (rc_stream.getBurstNumber() > 0) { // 0 means that no CTS has been sent, the first incoming burst is nr. 1 !
        processStreamDataChunk_ofMatchingClient(rc_stream, false);
        // don't care for result here!
      }
      #ifdef DEBUG
        INTERNAL_DEBUG_DEVICE << "Send CTS to get first/next burst!" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      sendCurrentCts (&rc_stream); // will increase the burstCurrent
    }

    /// TimeOut-Checks
    if (rc_stream.timedOut())
    {
      #ifdef DEBUG
        #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
        if (rc_stream.getStreamType() == StreamFastPacket)
          INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** Fast-Packet-";
        else
        #endif
          INTERNAL_DEBUG_DEVICE << INTERNAL_DEBUG_DEVICE_NEWLINE << "*** (E)TP-";
        INTERNAL_DEBUG_DEVICE << "Stream with SA " << (uint16_t) rc_stream.getIdent().getSa() << " timedOut, so sending out 'connAbort'. AwaitStep was " << (uint16_t) rc_stream.getNextComing() << " ***" << INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      notifyError (rc_stream.getIdent(), TransferErrorStreamTimedOut);
      connAbortTellClient (/* send Out ConnAbort Msg*/ true, &rc_stream);
      // remove Stream
      i_list_streams = mlist_streams.erase (i_list_streams);
      continue;
    }

    // END timeEvent every Stream_c
    i_list_streams++; // standard weiterschaltung, im "erase" fall wird "continue" gemacht!
  }
  if (mlist_streams.begin() == mlist_streams.end())
  { // we have NO Streams - we got NOTHING to do..
    setTimePeriod (5000);
  }
  else
  { // we have at least on Stream in the list...
    setTimePeriod (mi32_timePeriodForActiveStreams);
  }
  return true;
} // -X2C



//! virtual function which allows a scheduler client to define
//! a minimum execution time, that should be saved after this item in the
//! scheduler loop - some tasks might not be able to finish any sensible
//! work in the default min exec time of 10msec
uint16_t MultiReceive_c::getForcedMinExecTime() const
{
  return 30;
}



IsoAgLib::iStream_c*
MultiReceive_c::getFinishedJustKeptStream (IsoAgLib::iStream_c* apc_lastKeptStream)
{
  // If "last==NULL", take the first to get, else wait for last to occur and take next!
  bool b_takeIt = (apc_lastKeptStream == NULL);

  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin(); i_list_streams != mlist_streams.end(); i_list_streams++)
  {
    DEF_Stream_c_IMPL* pc_stream = &*i_list_streams;
    if (pc_stream->getStreamingState() == StreamFinishedJustKept)
    { // let's see if we take this kept one..
      if (b_takeIt)
        return pc_stream;
      if (apc_lastKeptStream == pc_stream)
        b_takeIt=true; // take the next kept one following this...
    }
  }
  return NULL;
}

/// Use to remove a "kept"-stream after it is gotten by "getFinishedJustKeptStream" and processed.
void
MultiReceive_c::removeKeptStream(IsoAgLib::iStream_c* apc_keptStream)
{
  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin(); i_list_streams != mlist_streams.end(); i_list_streams++)
  {
    if ((&*i_list_streams) == apc_keptStream)
    { // delete it. it's a justKept one, as we checked that before!
      if (i_list_streams->getStreamingState() != StreamFinishedJustKept)
        return; // do NOT allow any other streams to be deleted

      mlist_streams.erase (i_list_streams);
      return;
    }
  }
}



// //////////////////////////////// +X2C Operation 2853 : sendCurrentCts
//! Parameter:
//! @param apc_stream
void
MultiReceive_c::sendCurrentCts(DEF_Stream_c_IMPL* apc_stream)
{ // ~X2C
  // This function actually IS only called if in state "AwaitCtsSend" !

  /* may also be 0, meaning HOLD CONNECTION OPEN, but we can handle multiple streams... ;-)
     and we don't want to hold connections open that are very short, so well........... */

  // the following "> 0" check shouldn't be needed because if we reach here, we shouldn't
  uint32_t ui32_allowPackets = (getStreamCount() > 0) ? ((CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS) / getStreamCount()) : 1;
  if (ui32_allowPackets == 0)
  { // Don't allow 0 packets here as this would mean HOLD-CONNECTION OPEN and
    // we'd have to take action and cannot wait for the sender sending...
    ui32_allowPackets = 1;
  }
  if (ui32_allowPackets > CONFIG_MULTI_RECEIVE_MAX_PER_CLIENT_BURST_IN_PACKETS)
  { // limit the number of packets a single sender can send even if we could handle all those packets!
    ui32_allowPackets = CONFIG_MULTI_RECEIVE_MAX_PER_CLIENT_BURST_IN_PACKETS;
  }

  uint8_t ui8_pkgsToExpect = apc_stream->expectBurst (ui32_allowPackets); // we wish e.g. 20 pkgs (as always), but there're only 6 more missing to complete the stream!
  uint8_t pgn, cmdByte;
  uint8_t ui8_next2WriteLo =  ((apc_stream->getPkgNextToWrite()) & 0xFF);
  uint8_t ui8_next2WriteMid= (((apc_stream->getPkgNextToWrite()) >> 8) & 0xFF);
  uint8_t ui8_next2WriteHi = (((apc_stream->getPkgNextToWrite()) >> 16) & 0xFF);

  if (apc_stream->getStreamType() & StreamEcmdMASK)   cmdByte = 0x15 /* decimal: 21 */;
  else /* -------------------------------------- */ { cmdByte = 0x11 /* decimal: 17 */; ui8_next2WriteMid=ui8_next2WriteHi=0xFF; }
  if (apc_stream->getStreamType() & StreamEpgnMASK) pgn = ETP_CONN_MANAGE_PGN >> 8;
  else /* -------------------------------------- */ pgn = TP_CONN_MANAGE_PGN >> 8;

  mc_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ apc_stream->getIdent().getSa(), /* src: */ apc_stream->getIdent().getDa(),
                        cmdByte, ui8_pkgsToExpect, ui8_next2WriteLo, ui8_next2WriteMid, ui8_next2WriteHi, MACRO_BYTEORDER_toLoMidHi(apc_stream->getIdent().getPgn()));
  __IsoAgLib::getCanInstance4Comm() << mc_data;
} // -X2C



// //////////////////////////////// +X2C Operation : sendConnAbort
//! Parameter:
//! @param at_streamType stream type
//! @param ac_rsi receive stream identifier
void
MultiReceive_c::sendConnAbort(StreamType_t at_streamType, IsoAgLib::ReceiveStreamIdentifier_c ac_rsi)
{ // ~X2C
  if (ac_rsi.getDa() == 0xFF)
  { // NEVER answer to a packet that was sent to GLOBAL ADDRESS 0xFF !
    return;
  }

  uint8_t pgn = (at_streamType & StreamEpgnMASK) ? (ETP_CONN_MANAGE_PGN >> 8) : (TP_CONN_MANAGE_PGN >> 8);

  mc_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ ac_rsi.getSa(), /* src: */ ac_rsi.getDa(),
                        0xFF /* decimal: 255 */, 0xFF,0xFF,0xFF,0xFF, MACRO_BYTEORDER_toLoMidHi(ac_rsi.getPgn()));
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "Sending out an ConnAbort!" << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  __IsoAgLib::getCanInstance4Comm() << mc_data;
} // -X2C


void
MultiReceive_c::connAbortTellClient(bool ab_sendConnAbort, Stream_c* apc_stream)
{
  if (ab_sendConnAbort)
    sendConnAbort(apc_stream->getStreamType(), apc_stream->getIdent());

  if (apc_stream->getIdent().getDa() != 0xFF)
  {
    // search Client and tell about connAbort
    CanCustomer_c* pc_clientFound = getClient (apc_stream->getIdent());
    if (pc_clientFound) {
      pc_clientFound->reactOnAbort (*apc_stream);
    }
  } // else: // NEVER promote aborts from BROADCASTS, because there's no partial parsing possible, so no need to inform the user...
}


// //////////////////////////////// +X2C Operation 2853 : connAbortTellClientRemoveStream
//! Parameter:
//! @param ab_sendConnAbort: send out a connAbort-Msg? (Do this only for receiver-initiated aborts)
//! @param apc_stream:
//! Will send out connAbort (if requested), inform the client via "reactOnAbort" and close/remove the stream from list
void
MultiReceive_c::connAbortTellClientRemoveStream(bool ab_sendConnAbort, Stream_c* apc_stream)
{ // ~X2C
  connAbortTellClient (ab_sendConnAbort, apc_stream);

  removeStream(apc_stream);
} // -X2C



// //////////////////////////////// +X2C Operation : sendEndOfMessageAck
//! Parameter:
//! @param apc_stream:
void
MultiReceive_c::sendEndOfMessageAck(DEF_Stream_c_IMPL* apc_stream)
{ // ~X2C
  // NO Check here, this function IS called on purpose and WILL send EoMAck and CLOSE the Stream!
  uint8_t pgn;
  if (apc_stream->getStreamType() & StreamEpgnMASK) pgn = ETP_CONN_MANAGE_PGN >> 8;
  else /* -------------------------------------- */ pgn = TP_CONN_MANAGE_PGN >> 8;

  if (apc_stream->getStreamType() & StreamEcmdMASK) {
    mc_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ apc_stream->getIdent().getSa(), /* src: */ apc_stream->getIdent().getDa(),
                          0x17 /* decimal: 23 */, MACRO_BYTEORDER_toLoMidMidHi(apc_stream->getByteTotalSize()), MACRO_BYTEORDER_toLoMidHi(apc_stream->getIdent().getPgn()));
  } else {
    mc_data.setExtCanPkg8 (scui8_tpPriority, 0, pgn, /* dest: */ apc_stream->getIdent().getSa(), /* src: */ apc_stream->getIdent().getDa(),
                          0x13 /* decimal: 19 */, MACRO_BYTEORDER_toLoHi(apc_stream->getByteTotalSize()), apc_stream->getPkgTotalSize(), 0xFF /* reserved */, MACRO_BYTEORDER_toLoMidHi(apc_stream->getIdent().getPgn()));
  }
  __IsoAgLib::getCanInstance4Comm() << mc_data;
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
    getSchedulerInstance4Comm().registerClient( this );
    // register to get ISO monitor list changes
    getIsoMonitorInstance4Comm().registerSaClaimHandler( this );

    // insert receive filters for broadcasted TP
    getCanInstance4Comm().insertFilter (*this, (0x3FFFF00UL), ( TP_CONN_MANAGE_PGN  |0xFF)<<8, false, __IsoAgLib::Ident_c::ExtendedIdent, 8);
    getCanInstance4Comm().insertFilter (*this, (0x3FFFF00UL), ( TP_DATA_TRANSFER_PGN|0xFF)<<8, false, __IsoAgLib::Ident_c::ExtendedIdent, 8);
    getCanInstance4Comm().insertFilter (*this, (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN  |0xFF)<<8, false, __IsoAgLib::Ident_c::ExtendedIdent, 8);
    getCanInstance4Comm().insertFilter (*this, (0x3FFFF00UL), (ETP_DATA_TRANSFER_PGN|0xFF)<<8, false, __IsoAgLib::Ident_c::ExtendedIdent, 8);
    getCanInstance4Comm().reconfigureMsgObj();

    setTimePeriod (5000); // nothing to do per default!
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
    // deregister in Scheduler_c to get no more timeEvents
    getSchedulerInstance4Comm().unregisterClient( this );
    // deregister to get no more IsoMonitorList changes
    getIsoMonitorInstance4Comm().deregisterSaClaimHandler( this );

    // remove receive filters for broadcasted TP
    getCanInstance4Comm().deleteFilter (*this, (0x3FFFF00UL), ( TP_CONN_MANAGE_PGN  |0xFF)<<8, __IsoAgLib::Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter (*this, (0x3FFFF00UL), ( TP_DATA_TRANSFER_PGN|0xFF)<<8, __IsoAgLib::Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter (*this, (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN  |0xFF)<<8, __IsoAgLib::Ident_c::ExtendedIdent);
    getCanInstance4Comm().deleteFilter (*this, (0x3FFFF00UL), (ETP_DATA_TRANSFER_PGN|0xFF)<<8, __IsoAgLib::Ident_c::ExtendedIdent);

    mlist_streams.clear();
    mlist_clients.clear();

  }
} // -X2C


//  Operation: getClient
//! Parameter:
//! @param ac_streamIdent
//! @return NULL for "doesn't exist", otherwise valid "CanCustomer_c*"
CanCustomer_c*
MultiReceive_c::getClient (IsoAgLib::ReceiveStreamIdentifier_c ac_streamIdent)
{
  for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = mlist_clients.begin();
       i_list_clients != mlist_clients.end();
       i_list_clients++)
  {
    if (ac_streamIdent.matchDaPgnWithMask (i_list_clients->mui8_cachedClientAddress, i_list_clients->mui32_pgn, i_list_clients->mui32_pgnMask))
      return i_list_clients->mpc_client;
  }
  return NULL;
}



bool
MultiReceive_c::anyMultiReceiveClientRegisteredForThisDa (uint8_t ui8_da)
{
  if (ui8_da == 0xFF) return true;
  for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = mlist_clients.begin();
       i_list_clients != mlist_clients.end();
       i_list_clients++)
  {
    if (ui8_da == i_list_clients->mui8_cachedClientAddress)
      return true;
  }
  return false;
}



/**
  deliver reference to data pkg as reference to CanPkgExt_c
  to implement the base virtual function correct
*/
__IsoAgLib::CanPkgExt_c&
MultiReceive_c::dataBase()
{
  return mc_data;
}


//! return 0x00-0xFF: first byte of the stream!
//! return 0x100: first byte not yet known!
//! return 0x101: not a valid index!
uint16_t
MultiReceive_c::getStreamFirstByte (uint32_t ui32_index) const
{
  uint32_t ui32_curIndex=0;
  if (ui32_index < mlist_streams.size()) {
    for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = mlist_streams.begin(); pc_iter != mlist_streams.end(); pc_iter++)
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
  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = mlist_streams.begin(); pc_iter != mlist_streams.end(); pc_iter++) {
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
  if (ui32_index < mlist_streams.size()) {
    // retrieve completion in 1/10..100%
    for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = mlist_streams.begin(); pc_iter != mlist_streams.end(); pc_iter++)
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
  for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::const_iterator pc_iter = mlist_streams.begin(); pc_iter != mlist_streams.end(); pc_iter++)
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



/** this function is called by IsoMonitor_c on addition, state-change and removal of an IsoItem.
 * @param at_action enumeration indicating what happened to this IsoItem. @see IsoItemModification_en / IsoItemModification_t
 * @param acrc_isoItem reference to the (const) IsoItem which is changed (by existance or state)
 */
void
MultiReceive_c::reactOnIsoItemModification (IsoItemModification_t at_action, IsoItem_c const& acrc_isoItem)
{
  switch (at_action)
  {
    case AddToMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has finished adr claim
        getIsoFilterManagerInstance().insertIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), ( TP_CONN_MANAGE_PGN   << 8), &acrc_isoItem.isoName(), NULL, 8), false);
        getIsoFilterManagerInstance().insertIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), ( TP_DATA_TRANSFER_PGN << 8), &acrc_isoItem.isoName(), NULL, 8), false);
        getIsoFilterManagerInstance().insertIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN   << 8), &acrc_isoItem.isoName(), NULL, 8), false);
        getIsoFilterManagerInstance().insertIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), (ETP_DATA_TRANSFER_PGN << 8), &acrc_isoItem.isoName(), NULL, 8), true);
      }
      break;

    case RemoveFromMonitorList:
      if (acrc_isoItem.itemState (IState_c::Local))
      { // local IsoItem_c has gone (i.e. IdentItem has gone, too.
        /// @todo SOON-178 activate the reconfiguration when the second parameter in removeIsoFilter is there finally...
        getIsoFilterManagerInstance().removeIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), ( TP_CONN_MANAGE_PGN   << 8), &acrc_isoItem.isoName(), NULL, 8));
        getIsoFilterManagerInstance().removeIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), ( TP_DATA_TRANSFER_PGN << 8), &acrc_isoItem.isoName(), NULL, 8));
        getIsoFilterManagerInstance().removeIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), (ETP_CONN_MANAGE_PGN   << 8), &acrc_isoItem.isoName(), NULL, 8));
        getIsoFilterManagerInstance().removeIsoFilter (IsoFilter_s (*this, (0x3FFFF00UL), (ETP_DATA_TRANSFER_PGN << 8), &acrc_isoItem.isoName(), NULL, 8));
        /// @todo SOON-178 Maybe clean up some streams and clients?
        /// Shouldn't appear normally anyway, so don't care for right now...
      }
      break;

    default:
      break;
  }

  if ((at_action == AddToMonitorList) || (at_action == ChangedAddress) || (at_action == LostAddress) || (at_action == ReclaimedAddress))
  {
    /// If we're LostAddress, then we automatically have 0xFE now as SA...
    const uint8_t cui8_nr = acrc_isoItem.nr();
    for (STL_NAMESPACE::list<MultiReceiveClientWrapper_s>::iterator i_list_clients = mlist_clients.begin();
        i_list_clients != mlist_clients.end();
        i_list_clients++)
    {
      if (i_list_clients->mc_isoName == acrc_isoItem.isoName())
      { // yes, it's that ISOName that A) (locally) lost its SA before or B) (remotely) just changed it.
        // note: we can receive (E)TPs for remote nodes, too. Needed for sniffing the WS-slave stuff!
        // conclusion: just update it, regardless if it "lost (SA == 0xFF)" its SA before or not...
        i_list_clients->mui8_cachedClientAddress = cui8_nr;
      }
    }
    // Notify all running streams
    for (STL_NAMESPACE::list<DEF_Stream_c_IMPL>::iterator i_list_streams = mlist_streams.begin();
         i_list_streams != mlist_streams.end(); i_list_streams++)
    { // Adapt the SA also for kept streams - the application should only use the isoname anyway!
      const IsoAgLib::ReceiveStreamIdentifier_c& rc_rsi = i_list_streams->getIdent();
    // re-vitalize the Addresses, so that following packets using this address will get processed again...
      if (rc_rsi.getDaIsoName() == acrc_isoItem.isoName().toConstIisoName_c()) rc_rsi.setDa (cui8_nr);
      if (rc_rsi.getSaIsoName() == acrc_isoItem.isoName().toConstIisoName_c()) rc_rsi.setSa (cui8_nr);
    }
  }
}




///  Used for Debugging Tasks in Scheduler_c
const char*
MultiReceive_c::getTaskName() const
{ return "MultiReceive_c"; }



} // end namespace __IsoAgLib
