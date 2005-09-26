/***************************************************************************
                          stream_c.cpp -
                             -------------------
    class                : ::Stream_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:39:02 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id: stream_c.cpp 1258 2005-06-07 10:14:39Z wodok $

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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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

#include "stream_c.h"
#include "multireceive_c.h"

// IsoAgLib
#include <IsoAgLib/hal/system.h>

#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif


// helper macros
#define MACRO_minimum(a,b) (a < b) ? a : b


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {



// helper consts
uint8_t Stream_c::sui8_pkgBurst=0x10;



Stream_c::Stream_c (StreamType_t rt_streamType, IsoAgLib::ReceiveStreamIdentifier_c rc_rsi, uint32_t rui32_msgSize, bool rb_skipCtsAwait)
  : iStream_c()
  , c_ident (rc_rsi)
  , t_streamState (StreamRunning)
  , t_awaitStep (AwaitCtsSend) // so next timeEvent will send out the CTS!
  , ui32_byteTotalSize (rui32_msgSize)
  , ui32_byteAlreadyReceived (0)
  , ui32_pkgNextToWrite (1)
  , ui32_pkgTotalSize ((rui32_msgSize + 6) / 7)
  , ui32_burstCurrent (0) // so we know that it's the first burst when calling the processBurst from the client
  , ui8_streamFirstByte (0) // meaning: not yet identified!! (when you check it, it's already set!)
  , i32_timeoutLimit (sci32_timeNever)
  , t_streamType (rt_streamType)
 // ui8_pkgRemainingInBurst     // will be set in "expectBurst(wishingPkgs)", don't care here as t_awaitStep == awaitCtsSend!!
{
  #ifdef NMEA_2000_FAST_PACKET
  if (rt_streamType == StreamFastPacket)
  { // other calculation for FastPacket, as the first package only has 6 netto data bytes AND first package begins with frame count 0
    ui32_pkgNextToWrite = 0;
    ui32_pkgTotalSize = (rui32_msgSize + 7) / 7;
  }

  if ((rt_streamType == StreamFastPacket) ||
  #else
  if (
  #endif
   (rc_rsi.getDa() == 0xFF) || rb_skipCtsAwait)
  { // if it's FastPacket or BAM, then directly expect data to be sent! --- or if we directly wanna expect data (for fake streams..)
    expectBurst (255); // We're expecting one big burst directly now without CTS/DPO stuff!
  }
};


Stream_c::~Stream_c()
{
}


void
Stream_c::awaitNextStep (NextComing_t rt_awaitStep, int32_t ri32_timeOut)
{
  t_awaitStep = rt_awaitStep;
  i32_timeoutLimit = HAL::getTime()+ri32_timeOut;
}



/**
  will be directly called in sendCurrentCts
  (anyway, be sure to only call if in the right state)
  */
uint8_t
Stream_c::expectBurst(uint8_t wishingPkgs)
{
  #ifdef NMEA_2000_FAST_PACKET
  if (t_streamType == StreamFastPacket)
  {
    awaitNextStep (AwaitData, sci32_timeOutFP);
    ui8_pkgRemainingInBurst = ui32_pkgTotalSize; // for Fast-Packet there's only one burst!
  }
  else
  #endif
  { // Await after is CTS has timeout value of "sci32_timeOutT2=1250; // cts -> data(TP)/dpo(ETP)"
    if (t_streamType & StreamEcmdMASK) awaitNextStep (AwaitDpo,  sci32_timeOutT2);
    else /* ----------------------- */ awaitNextStep (AwaitData, (getIdent().getDa() == 0xFF) ? sci32_timeOutT1 /* BAM */
                                                                                              : sci32_timeOutT2 /* dest-adr. */);
    ui8_pkgRemainingInBurst = MACRO_minimum((ui32_pkgTotalSize - (ui32_pkgNextToWrite - 1)), wishingPkgs); // how many pkgs are missing at all? is it more than wished?
  }

  // increase ui32_burstCurrent, the expected Burst is a next new one (of course)...
  ui32_burstCurrent++;

  return ui8_pkgRemainingInBurst;
}



// //////////////////////////////// +X2C Operation 814 : handleDataPacket
//! Parameter:
//! @param pui8_offsetOfData: the offset given in the 8 byte data can pkg!
bool
Stream_c::handleDataPacket (uint8_t* rui8_data)
{ // ~X2C
  // expecting data at all?
  if (t_awaitStep != AwaitData) {
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "t_awaitStep != AwaitData! --- t_awaitStep ==" << t_awaitStep << " \n";
    #endif
    return false;
  }

  bool b_pkgNumberWrong=false;
  switch (t_streamType)
  {
    case StreamSpgnEcmdINVALID: // shouldn't occur, but catch that case anyway. (so break left out intentionally!)
    case StreamEpgnEcmd:   if ((rui8_data[0] + ui32_dataPageOffset) != ui32_pkgNextToWrite) b_pkgNumberWrong=true; break;
    case StreamSpgnScmd:
    case StreamEpgnScmd:   if ((rui8_data[0] /* no DPO for TP!! */) != ui32_pkgNextToWrite) b_pkgNumberWrong=true; break;
    #ifdef NMEA_2000_FAST_PACKET
    case StreamFastPacket: if ((rui8_data[0] & (0x1FU)) != ui32_pkgNextToWrite) b_pkgNumberWrong=true; break;
    #endif
  }

  if (b_pkgNumberWrong) {
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "wrong pkg-number! ";
    #endif
    return false;
  }

  #ifdef NMEA_2000_FAST_PACKET
  if ((t_streamType == StreamFastPacket) && (ui32_pkgNextToWrite == 0))
  { // special FastPacket first-frame handling
    insertFirst6Bytes (rui8_data+2);
    ui32_byteAlreadyReceived += 6;
  }
  else
  #endif
  {
    insert7Bytes (rui8_data+1);
    ui32_byteAlreadyReceived += 7;
  }

  if (ui32_byteAlreadyReceived > ui32_byteTotalSize) ui32_byteAlreadyReceived = ui32_byteTotalSize; // cut any padded (0xFF) bytes...

  #ifdef DEBUG
    INTERNAL_DEBUG_DEVICE << "#" << ui32_pkgNextToWrite << " ";
  #endif

/// <<UPDATE_ALL>> Pkg counting stuff
  ui32_pkgNextToWrite++;
  ui8_pkgRemainingInBurst--;

  if (ui8_pkgRemainingInBurst == 0) {
    // End? or CTS for more?
    if (
        #ifdef NMEA_2000_FAST_PACKET
        (t_streamType == StreamFastPacket) ||
        #endif
        (ui32_pkgNextToWrite > ui32_pkgTotalSize))
    {
      // ---END--- was last packet! So
      awaitNextStep (AwaitNothing, sci32_timeNever); // no timeOut on own Send-Awaits
      t_streamState = StreamFinished;
    } else {
      // ---CTS--- go for more!
      awaitNextStep (AwaitCtsSend, sci32_timeNever); // no timeOut on own Send-Awaits
    }
    // (A complete / The last) chunk is received, handling will be done after this function returns
  } else {
    awaitNextStep (AwaitData, sci32_timeOutT1); // state "AwaitData" was already set until now, but it's about the timeOut!
  }

  return true;
} // -X2C




// //////////////////////////////// +X2C Operation 2861 : setDataPageOffset
//! Parameter:
//! @param rui32_dataPageOffset:
bool
Stream_c::setDataPageOffset(uint32_t rui32_dataPageOffset)
{ // ~X2C
  if (t_awaitStep == AwaitDpo) {
    ui32_dataPageOffset = rui32_dataPageOffset;
    awaitNextStep (AwaitData, sci32_timeOutT5);
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "DPO ";
    #endif
    return true; // was awaited!
  } else {
    #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "\n\n DPO was not awaited at this state (" << (uint16_t) t_awaitStep << "), please handle as error! \n ";
    #endif
    return false; // DPO was not awaited at this state, please handle as error!
  }
} // -X2C



bool
Stream_c::timedOut()
{
  bool b_result=false;

  if (i32_timeoutLimit != sci32_timeNever) {
    if (HAL::getTime() > i32_timeoutLimit) b_result = true;
  }

  return b_result;
}

} // end namespace __IsoAgLib
