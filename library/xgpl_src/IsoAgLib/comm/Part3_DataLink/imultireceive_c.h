/***************************************************************************
                             imultireceive_c.h - reception of multi message
                                                 data
                             -------------------
    begin                : Fri Feb 01 2005
    copyright            : (C) 2000 - 2006 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate: 2005-10-10 15:35:26 +0200 (Mo, 10 Okt 2005) $
    $LastChangedRevision: 1348 $
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
#ifndef IMULTI_RECEIVE_H
#define IMULTI_RECEIVE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/multireceive_c.h"
#include <IsoAgLib/driver/can/icancustomer_c.h>
#include <IsoAgLib/comm/Scheduler/ischedulertask_c.h>


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  This class implements the various multi message data
  transfer specifications of Fieldstar, LBS+ and ISO 11783
  for reception of more than 8 bytes of data.

  @author Dipl.-Inf. Martin Wodok
*/
class iMultiReceive_c : private __IsoAgLib::MultiReceive_c  {
public:

  //  Operation: init
  void init()
    { MultiReceive_c::init(); }

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close()
    { MultiReceive_c::close(); }

  //  Operation: (de)registerClient
  void registerClient   (IsoAgLib::iCanCustomer_c& arc_client, const iIsoName_c& acrc_isoName,
                         uint32_t aui32_pgn, uint32_t aui32_pgnMask=0x3FFFF,
                         bool ab_alsoBroadcast=false, bool ab_alsoGlobalErrors=false
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , bool ab_isFastPacket=false
                         #endif
                         )
    { MultiReceive_c::registerClient (arc_client, acrc_isoName, aui32_pgn, aui32_pgnMask, ab_alsoBroadcast, ab_alsoGlobalErrors
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , ab_isFastPacket
                         #endif
                         ); }

 void registerClient   (IsoAgLib::iSchedulerTask_c& arc_client, const iIsoName_c& acrc_isoName,
                         uint32_t aui32_pgn, uint32_t aui32_pgnMask=0x3FFFF,
                         bool ab_alsoBroadcast=false, bool ab_alsoGlobalErrors=false
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , bool ab_isFastPacket=false
                         #endif
                         )
    { MultiReceive_c::registerClient (arc_client, acrc_isoName, aui32_pgn, aui32_pgnMask, ab_alsoBroadcast, ab_alsoGlobalErrors
                         #ifdef ENABLE_MULTIPACKET_VARIANT_FAST_PACKET
                         , ab_isFastPacket
                         #endif
                         ); }

  void deregisterClient (IsoAgLib::iCanCustomer_c* apc_client)
    { MultiReceive_c::deregisterClient (*static_cast<__IsoAgLib::CanCustomer_c*>(apc_client)); }


  void deregisterClient (IsoAgLib::iSchedulerTask_c* apc_client)
    { MultiReceive_c::deregisterClient (*static_cast<__IsoAgLib::CanCustomer_c*>(apc_client)); }

  /// Use to remove a "kept"-stream after it is gotten by "getFinishedJustKeptStream" and processed.
  void removeKeptStream (IsoAgLib::iStream_c* apc_keptStream)
    { MultiReceive_c::removeKeptStream (apc_keptStream); }

  //! return 0x00-0xFF: first byte of the stream!
  //! return 0x100: first byte not yet known!
  //! return 0x101: not a valid index!
  uint16_t getStreamFirstByte (uint32_t aui32_index) const
    { return MultiReceive_c::getStreamFirstByte (aui32_index); }

  uint32_t getStreamCount() const
    { return MultiReceive_c::getStreamCount(); }

  uint32_t getStreamCompletion1000 (uint32_t aui32_index, bool ab_checkFirstByte=false, uint8_t aui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const
    { return MultiReceive_c::getStreamCompletion1000 (aui32_index, ab_checkFirstByte, aui8_returnNullIfThisIsFirstByte); }

  uint32_t getMaxStreamCompletion1000 (bool ab_checkFirstByte=false, uint8_t aui8_returnNullIfThisIsFirstByte=0x00 /*don't care if check=false*/) const
    { return getMaxStreamCompletion1000 (ab_checkFirstByte, aui8_returnNullIfThisIsFirstByte); }

  bool isAtLeastOneWithFirstByte (uint8_t aui8_firstByte)
    { return isAtLeastOneWithFirstByte (aui8_firstByte); }

  IsoAgLib::iStream_c* getFinishedJustKeptStream (IsoAgLib::iStream_c* apc_lastKeptStream)
    { return getFinishedJustKeptStream (apc_lastKeptStream); }


  private:
  /** allow getIMultiReceiveInstance() access to shielded base class.
      otherwise __IsoAgLib::getMultiReceiveInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iMultiReceive_c& getIMultiReceiveInstance( uint8_t aui8_instance );
  #else
  friend iMultiReceive_c& getIMultiReceiveInstance( void );
  #endif
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiReceive_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iMultiReceive_c& getIMultiReceiveInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iMultiReceive_c&>(__IsoAgLib::getMultiReceiveInstance(aui8_instance));};
#else
  /** C-style function, to get access to the unique MultiReceive_c singleton instance */
  inline iMultiReceive_c& getIMultiReceiveInstance( void )
  { return static_cast<iMultiReceive_c&>(__IsoAgLib::getMultiReceiveInstance());};
#endif

}
#endif
