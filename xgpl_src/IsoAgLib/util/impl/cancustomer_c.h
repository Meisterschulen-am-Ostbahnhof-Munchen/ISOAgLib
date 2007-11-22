/***************************************************************************
                          cancustomer_c.h  -  header file for basic object
                                            for customer relationship to one
                                            or more FilterBox_c (-> CAN IO) instances
                             -------------------
    begin                : Tue Aug 3 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef CAN_CUSTOMER_H
#define CAN_CUSTOMER_H

#include <IsoAgLib/driver/can/impl/ident_c.h>
#include "canpkgext_c.h"


namespace IsoAgLib
{ // forward declarations
  class ReceiveStreamIdentifier_c;
  class iStream_c;
}


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** Base class for CAN customer classes
  -> FilterBox_c calls member function
    processMsg() to initiate processing of new received CAN telegram,
    which suits the configured filter/mask combination
  ==> all CAN handling object, which uses the services of FilterBox_c, CAN_IO
      should be derived from this class
  @see __IsoAgLib::CAN_IO
  @see __IsoAgLib::FilterBox
  *@author Dipl.-Inform. Achim Spangler
  */
class CanCustomer_c  {
public:

  /**
    virtual function which delivers a pointer to the CANCustomer
    specific CanPkgExt_c instance
  */
  virtual CanPkgExt_c& dataBase();

  /** virtual destructor */
  virtual ~CanCustomer_c();


/// SINGLE-PACKET METHODS
/// /////////////////////

  /**
    process a message -> the specialized/derived version of this virtual
    function is called during processing of received CAN telegrams in CanIo_c::processMsg
    @param apc_box pointer to the FilterBox_c instances which received the telegram (i.e. which has the telegram in its puffer)
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processMsg();

  /**
    process a message -> the specialized/derived version of this virtual
    function can be called during processing of received CAN telegrams in CanIo_c::processMsg
    @param apc_box pointer to the FilterBox_c instances which received the telegram (i.e. which has the telegram in its puffer)
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processInvalidMsg() { return false; }

  virtual bool isNetworkMgmt() const { return false; }


/// MULTI-PACKET (TP/ETP) METHODS
/// /////////////////////////////

  //  Operation: reactOnStreamStart
  //! Parameter:
  //! @param ac_ident:
  //! @param aui32_totalLen:
  virtual bool reactOnStreamStart (const IsoAgLib::ReceiveStreamIdentifier_c& /*ac_ident*/, uint32_t /*aui32_totalLen*/) { return false; }

  //  Operation: reactOnAbort
  virtual void reactOnAbort (IsoAgLib::iStream_c& /*arc_stream*/) {}

  //  Operation: processPartStreamDataChunk
  //! Parameter:
  //! @param arc_stream: stream that is to be processed
  //! @param ab_isFirstChunk: is it the first chunk? do we have to perform some decision action?
  //! @param ab_isLastChunk: this also indicates that the MultiPacket-Message has been acknowledged via "End Of Message Acknowledge"!
  //! @return .... keep stuff @todo COMMENT describe here
  virtual bool processPartStreamDataChunk (IsoAgLib::iStream_c& /*apc_stream*/, bool /*ab_isFirstChunk*/, bool /*ab_isLastChunk*/) { return false; }

  virtual void notificationOnMultiReceiveError (const IsoAgLib::ReceiveStreamIdentifier_c& /*ac_streamIdent*/, uint8_t /*aui8_multiReceiveError*/, bool /*ab_isGlobal*/) {} // needs not to be overwritten


#if defined(ALLOW_PROPRIETARY_MESSAGES_ON_STANDARD_PROTOCOL_CHANNEL)
  /** this virtual function can be used to detect CAnCustomer_c derived CAN message handlers, which
      want to send/receive proprietary messages on a CanIo_c instance, which is used to transport
      standardized protocol ISO 11783.
      Any proprietary derived CAN message handler should overload this function to return true in this function.
  */
  virtual bool isProprietaryMessageOnStandardizedCan() const { return false; }
#endif
};

/** this typedef is only for some time to provide backward compatibility at API level */
typedef CanCustomer_c CANCustomer_c;


} // end of namespace __IsoAgLib
#endif //CAN_CUSTOMER_H
