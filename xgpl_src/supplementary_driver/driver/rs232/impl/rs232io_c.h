/***************************************************************************
                          rs232io_c.h  -  header for RS232IO_c object
                                         for serial communication (Rs232)
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef RS232_IO_H
#define RS232_IO_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/util/impl/singleton.h>

#if defined(__TSW_CPP__) && !defined(__TSW_CPP_70__) && !defined(__TSW_CPP_756__)
  #include <stl/_string.h>
#else
  #include <string>
#endif
#include <supplementary_driver/hal/rs232.h>

/** define based on system type the support of several RS232 channels */
#if  defined(SYSTEM_C2C) || defined(SYSTEM_PC)
	#define USE_RS232_CHANNEL
	#define RS232_CHANNEL_PARAM_SINGLE ui8_channel
	#define RS232_CHANNEL_PARAM_LAST , ui8_channel
#else
	#define RS232_CHANNEL_PARAM_SINGLE
	#define RS232_CHANNEL_PARAM_LAST
#endif


namespace IsoAgLib { class iRS232IO_c;};

namespace __IsoAgLib {
class RS232IO_c;
typedef RS232_SINGLETON(RS232IO_c) SingletonRS232_c;
/**
  object for serial communication via RS232 device;
  the interface is initialized during constructor call;
  and is closed during destructor -> automatic;
  stream input and output operators ease the data communication
  @short object for serial communication via RS232 device.
  @author Dipl.-Inform. Achim Spangler
*/
class RS232IO_c : public SingletonRS232_c {
public:

  /**
    enum type for setting data bit, stop bit and parity within one parameter:
    databitParityStopbit {7,8}_{N,O,E}_{1,2}
  */
  enum t_dataMode {_7_E_1 = 0x11, _7_E_2 = 0x12, _7_O_1 = 0x31, _7_O_2 = 0x32, _8_E_1 = 0x21, _8_E_2 = 0x22,
        _8_O_1 = 0x41, _8_O_2 = 0x42, _8_N_1 = 0x51, _8_N_2 = 0x52};

  /** destructor has nothing to destruct */
  ~RS232IO_c();

  // ++++++++++++++++++++++++++++++++++++
  // ++++ RS232 managing operations ++++
  // ++++++++++++++++++++++++++++++++++++

  /**
    init function which initialises the BIOS RS232
    @param rui16_baudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
    @param ren_dataMode data mode setting of {7,8}_{N,O,E}_{1,2}
    @param rb_xonXoff use XON/XOFF sw handshake (true, false)
    @param rui16_sndPuf sending puffer size
    @param rui16_recPuf recieving puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffers
        * Err_c::range one of the configuration vals is not in allowed ranges
  */
  bool init(uint16_t rui16_baudrate = CONFIG_RS232_DEFAULT_BAUDRATE,
          t_dataMode ren_dataMode = CONFIG_RS232_DEFAULT_DATA_MODE,
          bool rb_xonXoff = CONFIG_RS232_DEFAULT_XON_XOFF,
          uint16_t rui16_sndPuf = CONFIG_RS232_DEFAULT_SND_PUF_SIZE, uint16_t rui16_recPuf = CONFIG_RS232_DEFAULT_REC_PUF_SIZE
					#ifdef USE_RS232_CHANNEL
					,uint8_t rui8_channel = 0
					#endif
					);
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void ){};

  /**
    set the baudrate to a new value
    @param rui16_baudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}

    possible errors:
        * Err_c::range the wanted baudrate is not allowed
    @return true -> setting successful
  */
  bool setBaudrate(uint16_t rui16_baudrate);

  /**
    deliver the actual baudrate
    @return RS232 baudrate
  */
  uint16_t baudrate()const{return ui16_baudrate;};

  /**
    set send puffer size
    @param rui16_sndPuf sending puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffer
    @return true -> allocating of puffer successful
  */
  bool setSndPufferSize(uint16_t rui16_pufferSize);
  /**
    deliver the actual send puffer size
    @return send puffer size
  */
  uint16_t sndPufferSize()const{return ui16_sndPuf;};
  /**
    clear the send puffer without send of actual data in puffer
  */
  void clearSndPuffer()const{HAL::clearRs232TxBuffer(RS232_CHANNEL_PARAM_SINGLE);};
  /**
    set receive puffer size
    @param rui16_recPuf receiving puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffer
    @return true -> allocating of puffer successful
  */
  bool setRecPufferSize(uint16_t rui16_pufferSize);
  /**
    deliver the actual receive puffer size
    @return receive puffer size
  */
  uint16_t rec_pufferSize()const{return ui16_recPuf;};

	#ifdef USE_RS232_CHANNEL
	/** get the channel */
	uint8_t getChannel() const { return ui8_channel;};
	#endif
  /**
    clear the receive puffer without reading of actual data in puffer
  */
  void clearRecPuffer()const{HAL::clearRs232RxBuffer(RS232_CHANNEL_PARAM_SINGLE);};
  /**
    check if the receive puffer is empty
    @return true -> receive puffer is empty
  */
  bool eof()const{return (HAL::getRs232RxBufCount(RS232_CHANNEL_PARAM_SINGLE) == 0)?true:false;};
  /**
    deliver the count of data uint8_t in receive puffer
    @return amount of data bytes in receive puffer
  */
  uint16_t rec_pufferCnt()const{return HAL::getRs232RxBufCount(RS232_CHANNEL_PARAM_SINGLE);};

  /* ******************************** */
  /* iostream related output operator */
  /* ********    sending     ******** */
  /* ******************************** */

  /**
    send data uint8_t string with given length on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rpb_data pointer to data string
    @param rui8_len length of data string
  */
  void send(const uint8_t* rpData, uint8_t rui8_len);
  /**
    send string on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rrefc_data sent data string
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(const std::basic_string<char>& rrefc_data);
  /**
    send NULL terminated string on RS232 (terminating NULL isn't sent)

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rpc_data pointer to NULL terminated string to send
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(const char *const rpc_data);
  /**
    send single uint8_t on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rb_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(uint8_t rb_data);
  /**
    send single int8_t on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rc_data sent data char
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(int8_t rc_data);
  /**
    send uint16_t (2byte unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rui16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(uint16_t rui16_data);
  /**
    send int16_t (2 uint8_t unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(int16_t ri16_data);
  /**
    send uint32_t (4 uint8_t unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rui32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(uint32_t rui32_data);
  /**
    send int32_t (4 uint8_t signed) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(int32_t ri32_data);
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    send float value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(float rf_data);
  #endif

  /* ******************************** */
  /* iostream related output operator */
  /* ********    receiving     ******** */
  /* ******************************** */

  /**
    receive data uint8_t string with given length on RS232

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param pb_data pointer to data string
    @param rui8_len length of data string
  */
  void receive(uint8_t* pData, uint8_t rui8_len);
  /**
    receive whitespace (or puffer end) terminated string on RS232
    @param refc_data reference to data string for receive
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(std::basic_string<char>& refc_data);
  /**
    receive '\n' (or puffer end) terminated string on RS232
    @param pb_data pointer to string to receive
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(uint8_t* pb_data);
  /**
    receive single uint8_t on RS232 (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param b_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(uint8_t& b_data);
  /**
    receive single int8_t on RS232 (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param c_data received data char
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(int8_t& c_data);
  /**
    receive uint16_t (2byte unsigned) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param ui16_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(uint16_t& ui16_data);
  /**
    receive int16_t (2 uint8_t unsigned) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param i16_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(int16_t& i16_data);
 /**
    receive uint32_t (4 uint8_t unsigned) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param ui32_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(uint32_t& ui32_data);
  /**
    receive int32_t (4 uint8_t signed) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param i32_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(int32_t& i32_data);
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    receive float value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param f_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(float& f_data);
  #endif

private: //Private methods
  friend class RS232_SINGLETON( RS232IO_c );
  friend class IsoAgLib::iRS232IO_c;
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of RS232IO_c within application
    */
  RS232IO_c( void ) {init();};
  /**
    read a token (eat whitespace in front of, end stop before next whitespace or at end og buf)
  */
  void readToken();

private:
  // Private attributes
  uint16_t ui16_baudrate;
  t_dataMode en_dataMode;
  bool b_xon_xoff;
  uint16_t ui16_sndPuf;
  uint16_t ui16_recPuf;
  char pc_token[15];
	#ifdef USE_RS232_CHANNEL
	/** new: define channel to use for RS232 */
	uint8_t ui8_channel;
	#endif
};


#if defined( RS232_INSTANCE_CNT ) && ( RS232_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique RS232IO_c singleton instance
    * if more than one RS232 channel is used for IsoAgLib, an index must be given to select the wanted channel
    */
  RS232IO_c& getRs232Instance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique RS232IO_c singleton instance */
  RS232IO_c& getRs232Instance( void );
#endif

}

#endif
