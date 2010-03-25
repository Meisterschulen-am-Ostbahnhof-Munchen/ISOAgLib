/*
  cancustomer_c.h: header file for basic object for customer
    relationship to one or more FilterBox_c (-> CAN IO) instances

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef CAN_CUSTOMER_H
#define CAN_CUSTOMER_H

#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{
  // forward declarations
  class ReceiveStreamIdentifier_c;
  class Stream_c;


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
    <!--@param apc_box pointer to the FilterBox_c instances which received the telegram (i.e. which has the telegram in its buffer)-->
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processMsg();

  /**
    process a message -> the specialized/derived version of this virtual
    function can be called during processing of received CAN telegrams in CanIo_c::processMsg
    <!--@param apc_box pointer to the FilterBox_c instances which received the telegram (i.e. which has the telegram in its buffer)-->
    @see __IsoAgLib::CanIo_c::processMsg
  */
  virtual bool processInvalidMsg() { return false; }

  virtual bool isNetworkMgmt() const { return false; }


/// MULTI-PACKET (TP/ETP) METHODS
/// /////////////////////////////

  /// Operation: reactOnStreamStart
  /// <!--@param ac_ident
  /// @param aui32_totalLen-->
  virtual bool reactOnStreamStart (const ReceiveStreamIdentifier_c& /*ac_ident*/, uint32_t /*aui32_totalLen*/) { return false; }

  //  Operation: reactOnAbort
  virtual void reactOnAbort (Stream_c& /*arc_stream*/) {}

  /// Operation: processPartStreamDataChunk
  ///
  //! <!--@param arc_stream: stream that is to be processed
  //! @param ab_isFirstChunk: is it the first chunk? do we have to perform some decision action?
  //! @param ab_isLastChunk: this also indicates that the MultiPacket-Message has been acknowledged via "End Of Message Acknowledge"!-->
  //! @return only of interest when "ab_isLastChunk==true":
  //!           true -> keep the stream (for later processing)
  //!           false -> don't keep the stream, let it be finished/removed right away.
  virtual bool processPartStreamDataChunk (Stream_c& /*apc_stream*/, bool /*ab_isFirstChunk*/, bool /*ab_isLastChunk*/) { return false; }

  virtual void notificationOnMultiReceiveError (const ReceiveStreamIdentifier_c& /*ac_streamIdent*/, uint8_t /*aui8_multiReceiveError*/, bool /*ab_isGlobal*/) {} // needs not to be overwritten


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
