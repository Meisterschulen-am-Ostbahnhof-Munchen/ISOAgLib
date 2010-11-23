/*
  rs232io_c.cpp:
    source for RS232IO_c object for serial communication (Rs232)

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <stdio.h>

// ISOAgLib
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>

#include "rs232io_c.h"

#if (defined(__TSW_CPP__) && !defined(__TSW_CPP_70__) && !defined(__TSW_CPP_756__))  || defined( __TMS320C2000__ )
  #define isspace(_c) ((_c)&(0x1|0x4|'\r'|'\n'))
#else
  #include <cctype>
#endif


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** C-style function, to get access to the unique RS232IO_c singleton instance
 * if more than one RS232 channel is used for IsoAgLib, an index must be given to select the wanted channel
 */
RS232IO_c &getRs232Instance( uint8_t aui8_instance )
{
  MACRO_MULTITON_GET_INSTANCE_BODY(RS232IO_c, RS232_INSTANCE_CNT, aui8_instance);
}

/*******************************************/
/** definition of public element functions */
/*******************************************/

RS232IO_c::RS232IO_c( void )
  : ui32_baudrate (BAUDERATE_CONTRUCTOR_DEFAULT_VALUE)
  , en_dataMode (_8_N_1)
  , b_xon_xoff (false)
  , ui16_sndBuf (0)
  , ui16_recBuf (0)
  , ui8_channel (CHANNEL_CONTRUCTOR_DEFAULT_VALUE)
{};


void
RS232IO_c::close()
{
  if ( ui8_channel != CHANNEL_CONTRUCTOR_DEFAULT_VALUE ) HAL::close_rs232( ui8_channel );
}


bool
RS232IO_c::init(
    uint32_t aui32_baudrate,
    t_dataMode ren_dataMode,
    bool ab_xonXoff,
    uint16_t aui16_sndBuf,
    uint16_t aui16_recBuf,
    uint8_t aui8_channel)
{
  bool b_result;
  // check the configuration informations
  bool b_baudAllowed = false,
       b_dataModeAllowed = false;

  // check if aui32_baudrate is one of the allowed settings
  uint32_t pi32_allowed[] = HAL_RS232_BAUDRATE_LIST;
  for (uint8_t ui8_index = 0; ui8_index < HAL_RS232_BITRATE_CNT; ui8_index++)
  {
    if (pi32_allowed[ui8_index] == aui32_baudrate)
    { // given baudrate is in allowed list at position ui8_index
      b_baudAllowed = true; // store success state
      break; // exit search loop
    }
  }

  uint8_t b_dataParityVal = (ren_dataMode >> 4);
  uint8_t b_stopBit = (ren_dataMode & 0xF);
  if ((b_dataParityVal > 0) && (b_dataParityVal < 6))
  { // setting for data bit and parity is correct
    // now check if stop bis are set to 1 or 2
    b_dataModeAllowed = ((b_stopBit == 1) || (b_stopBit == 2));
  }
  // stop RS232 interface if configured before
  if ( ui32_baudrate != BAUDERATE_CONTRUCTOR_DEFAULT_VALUE )
  { // no more initial value
    if ( ui8_channel != CHANNEL_CONTRUCTOR_DEFAULT_VALUE )
      HAL::close_rs232( ui8_channel );
  }

  // set error state if one of b_baudAllowed and b_dataModeAllowed is false
  // and init hardware if everything is accepted
  if ( ((b_baudAllowed) && (b_dataModeAllowed))
     &&(HAL::init_rs232(aui32_baudrate, b_dataParityVal, b_stopBit, ab_xonXoff, aui8_channel) == HAL_NO_ERR)
      )
  { // o.k.
    // store configuration values
    en_dataMode = ren_dataMode;
    b_xon_xoff = ab_xonXoff;
    ui16_sndBuf = aui16_sndBuf;
    ui16_recBuf = aui16_recBuf;

    b_result = true;
    // now init buffers
    if (HAL::configRs232TxObj(ui16_sndBuf, NULL, NULL, aui8_channel) != HAL_NO_ERR) b_result = false;
    if (HAL::configRs232RxObj(ui16_recBuf, NULL, aui8_channel) != HAL_NO_ERR) b_result = false;

    if (!b_result)
      getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Rs232 );
    else
    {
      // Only here do we store the values, as these two fields help us know whether the device is already initialized
      ui32_baudrate = aui32_baudrate;
      ui8_channel = aui8_channel;
    }
  }
  else
  { //wrong values given
    b_result = false;
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Rs232 ); // something is still wrong
  }

  return b_result;
}


/**
  set the baudrate to a new value
  @param aui32_baudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}

  possible errors:
      * Err_c::range one of the configuration vals is not in allowed ranges
  @return true -> setting successful
*/
bool RS232IO_c::setBaudrate(uint32_t aui32_baudrate)
{
  // check the configuration informations
  bool b_baudAllowed = false;

  // check if aui32_baudrate is one of the allowed settings
  uint32_t pi32_allowed[] = HAL_RS232_BAUDRATE_LIST;
  for (uint8_t ui8_index = 0; ui8_index < HAL_RS232_BITRATE_CNT; ui8_index++)
  {
    if (pi32_allowed[ui8_index] == aui32_baudrate)
    { // given baudrate is in allowed list at position ui8_index
      b_baudAllowed = true; // store success state
      break; // exit search loop
    }
  }

  // now set the baudrate if allowed
  if ((b_baudAllowed) && (HAL::setRs232Baudrate(aui32_baudrate, ui8_channel) == HAL_NO_ERR))
  { // everything o.k.
    ui32_baudrate = aui32_baudrate;
  }
  else
  { // wrong setting
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Rs232 );
  }
  return b_baudAllowed;
}

/**
  set send buffer size
  @param aui16_bufferSize sending buffer size

  possible errors:
      * Err_c::badAlloc not enough memory for allocating the buffer
  @return true -> allocating of buffer successful
*/
bool RS232IO_c::setSndBufferSize(uint16_t aui16_bufferSize)
{
  if (HAL::configRs232TxObj(aui16_bufferSize, NULL, NULL, ui8_channel) == HAL_NO_ERR)
  { // send buffer successful created
    ui16_sndBuf = aui16_bufferSize;
    return true;
  }
  else
  {
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Rs232 );
    return false;
  }
}
/**
  set receive buffer size
  @param aui16_bufferSize receiving buffer size

  possible errors:
      * Err_c::badAlloc not enough memory for allocating the buffer
  @return true -> allocating of buffer successful
*/
bool RS232IO_c::setRecBufferSize(uint16_t aui16_bufferSize)
{
  if (HAL::configRs232RxObj(aui16_bufferSize, NULL, ui8_channel) == HAL_NO_ERR)
  { // receive buffer successful created
    ui16_recBuf = aui16_bufferSize;
    return true;
  }
  else
  {
    getILibErrInstance().registerError( iLibErr_c::BadAlloc, iLibErr_c::Rs232 );

    return false;
  }

}


  /* ******************************** */
  /* iostream related output operator */
  /* ********    sending     ******** */
  /* ******************************** */

  /**
    send data uint8_t string with given length on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param apb_data pointer to data string
    @param aui8_len length of data string
  */
  void RS232IO_c::send(const uint8_t* apb_data, uint8_t aui8_len)
  {
    if (! isInitialized() ) return;

    uint16_t ui16_maxSendItemSize;
    uint16_t ui16_startSendPos = 0,
            ui16_restLen = aui8_len;  

    while ( ui16_restLen > 0 )
    { // send max item

      ui16_maxSendItemSize = ui16_sndBuf - HAL::getRs232TxBufCount(ui8_channel);    

      // restrict actual max item size to waiting chars to send
      if ( ui16_maxSendItemSize > ui16_restLen ) ui16_maxSendItemSize = ui16_restLen;
      // send actual item
      if (HAL::put_rs232NChar((apb_data + ui16_startSendPos), ui16_maxSendItemSize, ui8_channel) != HAL_NO_ERR)
      {
        getILibErrInstance().registerError( iLibErr_c::Rs232Overflow, iLibErr_c::Rs232 );
      }
      // update ui8_restLen
      ui16_restLen -= ui16_maxSendItemSize;
      // update start send pos
      ui16_startSendPos += ui16_maxSendItemSize;
      // triger watchdog
      HAL::wdTriggern();
    }
  }

  /**
    send string on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param acrc_data sent data string
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(const STL_NAMESPACE::string& acrc_data)
  {
    send(((uint8_t*)(acrc_data.c_str())), (uint8_t)acrc_data.size());
    return *this;
  }

  /**
    send NULL terminated string on RS232 (terminating NULL isn't sent)

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param apc_data pointer to NULL terminated string to send
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(const char *const apc_data)
  {
    send( (uint8_t*)(apc_data), (uint8_t)strlen( apc_data ) );
    return *this;
  }

  /**
    send single uint8_t on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param ab_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(uint8_t ab_data)
  {
	if (! isInitialized() ) return *this;
    if (HAL::put_rs232Char(ab_data, ui8_channel) != HAL_NO_ERR)
    {
      getILibErrInstance().registerError( iLibErr_c::Rs232Overflow, iLibErr_c::Rs232 );
    }
    return *this;
  }

  /**
    send single int8_t on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param ac_data sent data char
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(int8_t ac_data)
  {
    int16_t i16_val (ac_data);
    char pc_data[5];
    // sprintf print value as text to uint8_t string and terminate it with '\0'
    sprintf(pc_data, "%hi", i16_val);

    return RS232IO_c::operator<<(pc_data);
  }


  /**
    send uint16_t (2byte unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param aui16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(uint16_t aui16_data)
  {
    char pc_data[10];
    // sprintf print value as text to uint8_t string and terminate it with '\0'
    sprintf(pc_data, "%hu", aui16_data);

    return operator<<(pc_data);
  }

  /**
    send int16_t (2 uint8_t unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param ai16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(int16_t ai16_data)
  {
    char pc_data[10];
    // sprintf print value as text to uint8_t string and terminate it with '\0'
    sprintf(pc_data, "%hi", ai16_data);

    return operator<<(pc_data);
  }

  /**
    send uint32_t (4 uint8_t unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param aui32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(uint32_t aui32_data)
  {
    char pc_data[20];
    // sprintf print value as text to uint8_t string and terminate it with '\0'
    sprintf(pc_data, "%lu", aui32_data);

    return operator<<(pc_data);
  }

  /**
    send int32_t (4 uint8_t signed) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer buffer overflow during send
    @param ai32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& RS232IO_c::operator<<(int32_t ai32_data)
  {
    char pc_data[20];
    #ifdef SYSTEM_PC
    // sprintf print value as text to uint8_t string and terminate it with '\0'
    // variant for 32Bit CPU
    sprintf(pc_data, "%d", ai32_data);
    #else
    // sprintf print value as text to uint8_t string and terminate it with '\0'
    // variant for 16Bit CPU where int has size of int16_t --> only long int has 32 Bit
    sprintf(pc_data, "%ld", ai32_data);
    #endif

    return operator<<(pc_data);
  }
#ifdef USE_FLOAT_DATA_TYPE
/**
  send float value as text on RS232

  possible errors:
      * Err_c::rs232_overflow send buffer buffer overflow during send
  @param ai32_data sent data byte
  @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
*/
RS232IO_c& RS232IO_c::operator<<(float af_data)
{
  char pc_data[20];
  // sprintf print value as text to uint8_t string and terminate it with '\0'
  sprintf(pc_data, "%f", af_data);
  // change use float format to german
  *(strstr((char*)pc_data, ".")) = ',';

  return operator<<(pc_data);
}
#endif


/* ******************************** */
/* iostream related output operator */
/* ********   receiving    ******** */
/* ******************************** */

/**
  receive data uint8_t string with given length on RS232

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param pb_data pointer to data string
  @param aui16_len length of data string
*/
void RS232IO_c::receive(uint8_t* pb_data, uint16_t aui16_len)
{
	if (! isInitialized()) return;


  uint8_t* pb_writer = pb_data;
  uint16_t ui16_ind = 0;
  for (; ui16_ind < aui16_len; ui16_ind++)
  {
    if (eof())
    { // error - buffer empty before reading wanted count of data
      getILibErrInstance().registerError( iLibErr_c::Rs232Underflow, iLibErr_c::Rs232 );
      break;
    }
    HAL::getRs232Char(pb_writer, ui8_channel);
    pb_writer++;
  }
}

/** read a line to the next apperance of '\n'.
    read nothing if the delimiter isn't found.
  @param pui8_data    pointer to buffer for writing the data
  @param ui8_lastChar terminating char for read
  @return HAL_NOACT_ERR -> nothing copied as delimiter not found;
          HAL_NO_ERR -> delimiter found; text before delimiter copied; delimiter removed
*/
int16_t RS232IO_c::getLine( uint8_t* pui8_data, uint8_t ui8_lastChar )
{
  return HAL::getRs232String( pui8_data, ui8_lastChar, ui8_channel);
}

/**
  receive whitespace (or buffer end) terminated string on RS232
  @param rc_data reference to data string for receive
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(STL_NAMESPACE::string& rc_data)
{
  uint8_t b_data;
//  char pc_tempArray[50];

  // first eat white space (including \t,\n) - stop if buffer is empty
  for (HAL::getRs232Char(&b_data, ui8_channel); (((b_data == ' ' ) || (b_data == '\t' )) && (!eof()));
       HAL::getRs232Char(&b_data, ui8_channel));

  // if buffer is empty exit
  if (eof())
  {
    rc_data = "";
    return *this;
  }

  // now b_data is a not whitespace byte
//  rc_data = b_data; // store it
  while ( !eof() )
  {
    HAL::getRs232Char(&b_data, ui8_channel);
    //strncat(pc_tempArray, (char *)&b_data, 1);
    if ((b_data == ' ' ) || (b_data == '\t' )) break;
    else rc_data.push_back( b_data );
//      following line caused assertion
//    rc_data += b_data;
  }
  return *this;
}

/**
  read the received RS232 string into a deque.
  read until the end of the buffer.
  @param rc_data reference to data deque for receive
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
RS232IO_c& RS232IO_c::operator>>(STL_NAMESPACE::deque<char>& rc_data)
{
  uint8_t b_data;

  while ( !eof() )
  {
    HAL::getRs232Char(&b_data, ui8_channel);
    rc_data.push_back( b_data );
  }
  return *this;
}



/**
  receive '\n' (or buffer end) terminated string on RS232
  @param pb_data pointer to string to receive
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(uint8_t* pb_data)
{
  HAL::getRs232String(pb_data, '\n', ui8_channel);
  return *this;
}

/**
  receive single uint8_t on RS232 (ignore white spaces)

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param b_data received data byte
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(uint8_t& b_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  uint16_t ui16_val;
  sscanf(pc_token, "%hu", &ui16_val);
  b_data = ui16_val;
  return *this;
}

/**
  receive single int8_t on RS232 (ignore white spaces)

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param c_data received data char
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(int8_t& c_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  int16_t i16_val;
  sscanf(pc_token, "%hi", &i16_val);
  c_data = i16_val;

  return *this;
}

/**
  receive uint16_t (2byte unsigned) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param ui16_data received data byte
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(uint16_t& ui16_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  sscanf(pc_token, "%hu", &ui16_data);
  return *this;
}

/**
  receive int16_t (2 uint8_t unsigned) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param i16_data received data byte
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(int16_t& i16_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  sscanf(pc_token, "%hi", &i16_data);
  return *this;
}

/**
  receive uint32_t (4 uint8_t unsigned) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param ui32_data received data byte
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(uint32_t& ui32_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  #ifdef SYSTEM_PC
  // variant for 32Bit CPU
  sscanf(pc_token, "%u", &ui32_data);
  #else
  // variant for 16Bit CPU where int has size of int16_t --> only long int has 32 Bit
  sscanf(pc_token, "%lu", &ui32_data);
  #endif
  return *this;
}


/**
  receive int32_t (4 uint8_t signed) value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param i32_data received data byte
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(int32_t& i32_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  #ifdef SYSTEM_PC
  // variant for 32Bit CPU
  sscanf(pc_token, "%i", &i32_data);
  #else
  // variant for 16Bit CPU where int has size of int16_t --> only long int has 32 Bit
  sscanf(pc_token, "%li", &i32_data);
  #endif
  return *this;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  receive float value as text on RS232
  (ignore white spaces)

  possible errors:
      * Err_c::rs232_underflow receive buffer underflow during receive
  @param f_data received data byte
  @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
*/
RS232IO_c& RS232IO_c::operator>>(float& f_data)
{
  readToken(); // it set rs232_underflow error if no data is read
  sscanf(pc_token, "%f", &f_data);
  return *this;
}
#endif

/**
  read a token (eat whitespace in front of, end stop before next whitespace or at end og buf)

  possible errors:
      * Err_c::rs232_underflow no data other than whitespace in buffer to read in token
*/
void RS232IO_c::readToken()
{
  uint8_t b_data = 0;
  uint8_t ui8_ind = 0;

  // first eat white space (including \t,\n) - stop if buffer is empty
  for (HAL::getRs232Char(&b_data, ui8_channel); (isspace(b_data) && (eof()));
       HAL::getRs232Char(&b_data, ui8_channel));


  // if buffer is empty, overwrite old data, set rs232_underflow error and exit
  if (eof())
  { // overwrite old token data with '\0'
    for (ui8_ind = 0; ui8_ind < 12; ui8_ind++)pc_token[ui8_ind] = '\0';
    getILibErrInstance().registerError( iLibErr_c::Rs232Underflow, iLibErr_c::Rs232 );
    return;
  }

  // now b_data is a not whitespace byte
  pc_token[0] = b_data; // store it
  for (ui8_ind = 1; ((ui8_ind < 12) && (eof())); ui8_ind++)
  {
    HAL::getRs232Char(&b_data, ui8_channel);
    if (isspace(b_data))break; // break loop on whitespace
    pc_token[ui8_ind] = b_data;
  }

  // set lasting positions in pc_token to '\0'
  for (;(ui8_ind < 12); ui8_ind++) pc_token[ui8_ind] = '\0';
}

} // end of namespace __IsoAgLib
