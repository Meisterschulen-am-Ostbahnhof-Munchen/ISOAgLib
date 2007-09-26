/***************************************************************************
                             imulti_send_streamer.h - transfer of multi message
                                                      data with help of streamer
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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

#ifndef IMULTI_SEND_STREAMER_C_H
#define IMULTI_SEND_STREAMER_C_H

#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */

namespace __IsoAgLib {
class MultiSendPkg_c;
}


namespace IsoAgLib {


/**
  This class implements the sending of streams that are no

  @author Dipl.-Inform. Achim Spangler
*/
class iMultiSendStreamer_c : public ClientBase
{
public:
  virtual ~iMultiSendStreamer_c(){};

  /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards
    */
  virtual void setDataNextStreamPart (__IsoAgLib::MultiSendPkg_c* mspData, uint8_t bytes)=0;

  #if defined(NMEA_2000_FAST_PACKET)
  /** place next data to send direct into send buffer of pointed
      stream send package - MultiSend_c will send this
      buffer afterwards
   */
  virtual void setDataNextFastPacketStreamPart (__IsoAgLib::MultiSendPkg_c* /*mspData*/, uint8_t /*bytes*/, uint8_t /*rui8_offset*/ = 0 ) {};
  #endif

  /** set cache for data source to stream start */
  virtual void resetDataNextStreamPart()=0;

  /** save current send position in data source - neeed for resend on send problem */
  virtual void saveDataNextStreamPart ()=0;

  /** reactivate previously stored data source position - used for resend on problem */
  virtual void restoreDataNextStreamPart ()=0;

  /** calculate the size of the data source */
  virtual uint32_t getStreamSize ()=0;

  /** get the first byte, which is the COMMAND-byte and should be given back CONST! */
  virtual uint8_t getFirstByte ()=0;
};

} // end namespace IsoAgLib
#endif
