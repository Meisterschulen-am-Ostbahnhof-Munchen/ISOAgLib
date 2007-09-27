/** @page MultiMsgPage Send of data streams of more than 8 Data Byte with Multi-Message Protocol
  * The ISO 11783 protocol uses the a protocol extension to send data streams ( units )
  * of more than 8 Data Bytes, which don't fit into one CAN packet.
  * This is especially important for the interaction with graphic user interfaces - terminals.
  * Here the complete description of all user interface components ( mask pool ) results in
  * data streams of several 100 kByte.
  * The IsoAgLib provides simple but capable API functions to send such data streams for each protocol
  * type. The application can check in both cases for the current state of the data upload, so that
  * the application can react on the complete upload, failed transaction or other events.
  *
  * As the ISO_Terminal class provides the capability to adapt the GUI definition to the capabilities
  * of the terminal, the upload mechanism needs a mechanism to calculate the dynamic parts in small
  * pieces without heavy usage of the heap - it wouldn't be acceptable to allocate the complete
  * dynamic mask pool in the heap.
  *
  * The receive of multimessage data streams isn't implemented by the IsoAgLib - at the moment.
  * In case, this is needed later, this functionality would be integrated in this directory.
  *
  * The upload of multi message data streams is normally not relevant for the application, as only
  * the ISO_Terminal use this servcice internally.
  *
  * The IsoAgLib places on each call of __IsoAgLib::Scheduler_c::timeEvent() and the succeding call of
  * __IsoAgLib::MultiSend_c::timeEvent() a bunch of CAN messages into the buffer of the CAN driver.
  * The correct time distance between the CAN messages is realized by setting the minimum send pause between
  * CAN messages with the call of IsoAgLib::iCanIo_c::setSendpause(). If this function is not supported by
  * the CAN implementation of the HAL ( HAL::can_configMsgobjSendpause() ), the CAN messages of each timeEvent()
  * call are sent as one burst of data. As long as the receiver can puffer the messages of one burst, the
  * data transfere shouldn't be disturbed. The application can control the size of the data bunch by setting
  * the time interval between the __IsoAgLib::Scheduler_c::timeEvent() calls - the IsoAgLib calculates based on the
  * time distance the amount of CAN messages to send till the next timeEvent() call.
  * If the current time delay between CAN messages should be 4 msec., and the timeEvent() function is called
  * every 40 msec, the function __IsoAgLib::MultiSend_c::timeEvent() will place up to 10 CAN messages into
  * the CAN send puffer for send. This amount is limited by the available free space in the send puffer of the
  * CAN driver.
  *
  * @section MultiMsgIsoSimpleExamle Example for start of simple ISO Send
  * \code
  * uint8_t ui8_sendAdr             = 0x2;
  * uint8_t ui8_receiveAdr          = 0x0;
  * const uint8_t *rpui8_pool       = 0x1234;   ///< adress in ROM where pool is stored
  * const int32_t ri32_dataSize     = 300*1024; ///< example size of 300 kByte
  * const int32_t ci32_pgn          = 0x00E700LU ///< PGN defined in isoaglib_config.h as ECU_TO_VT_PGN
  * sendSuccess_t t_sendState;                   ///< IsoAgLib will store current send state here
  *
  * bool b_isTargetWanted           = true;     ///< select target or broadcast
  *
  * if ( b_isTargetWanted ) {
  *   if ( IsoAgLib::getIMultiSendInstance().sendIsoTarget(ui8_sendAdr, ui8_receiveAdr, rpui8_pool, ri32_dataSize, ci32_pgn, &t_sendState ) ) {
  *     // fine - upload will start soon ;-))
  *   }
  * }
  * else {
  *   // broadcast protocol defines state independent send of complete data stream - without reaction on send problems - so no send success state is defined
  *   if ( IsoAgLib::getIMultiSendInstance().sendIsoBroadcast(ui8_sendAdr, ui8_receiveAdr, rpui8_pool, ri32_dataSize, ci32_pgn ) ) {
  *     // fine - upload will start soon ;-))
  *   }
  * }
  * \endcode
  *
  * @section MultiMsgIsoStreamerExamle Example for start of ISO Send with help of MultiSendStreamer_c
  * \code
  * /** derive app specific class as source for dynamic data
  *     - implement the functions in seperate source module not described in this example
  * */
  * class MyDynamicDataSource_c : public IsoAgLib::MultiSendStreamer_c {
  *   /** place next data to send direct into send puffer of pointed
  *       stream send package - MultiSendStreamer_c will send this
  *       puffer afterwards */
  *       main method as long as no resend of older part is needed */
  *   virtual void setDataNextStreamPart (MultiSendPkg_c* mspData, uint8_t bytes);
  *   /** set cache for data source to stream start */
  *   virtual void resetDataNextStreamPart();
  *   /** save current send position in data source - neeed for resend on send problem */
  *   virtual void saveDataNextStreamPart ();
  *   /** reactivate previously stored data source position - used for resend on problem */
  *   virtual void restoreDataNextStreamPart ();
  *   /** calculate the size of the data source */
  *   virtual uint32_t getStreamSize ();
  * }
  *
  * uint8_t ui8_sendAdr             = 0x2;
  * uint8_t ui8_receiveAdr          = 0x0;
  * MyDynamicDataSource_c c_dataSrc;             ///< instance of app-specific data source class
  * const int32_t ci32_pgn          = 0x00E700LU ///< PGN defined in isoaglib_config.h as ECU_TO_VT_PGN
  * sendSuccess_t t_sendState;                   ///< IsoAgLib will store current send state here
  * if ( IsoAgLib::getIMultiSendInstance().sendIsoTarget(ui8_sendAdr, ui8_receiveAdr, &c_dataSrc, ci32_pgn, &t_sendState ) ) {
  *   // fine - upload will start soon ;-))
  * }
  * \endcode
  */