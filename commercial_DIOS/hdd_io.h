/***************************************************************************
                          hdd_io.h  -  header for RS232IO_c object
                                         for serial communication (Rs232)
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 by Dipl.-Inform. Achim Spangler
    email                : Achim.Spangler:de
    type                 : Header
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
 * Copyright (C) 1999  Dipl.-Inform. Achim Spangler                        *
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
#ifndef HDD_IO_H
#define HDD_IO_H

// decide if File handle should be flushed or closed
// after sequence of write access
#define FLUSH_AFTER_WRITE_SEQUENCE
//#define CLOSE_AFTER_WRITE_SEQUENCE

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <__IsoAgLib/isoaglib_config.h>
#include <__IsoAgLib/HwBIOS/bios_namespace.h>
#include <IsoAgLib/util/liberr_c.h>

namespace __IsoAgLib {
namespace nHwRS232 {
/* **************************************** */
/* import Shared namespaces for easy access */
/* **************************************** */
#if defined(SYSTEM_ESX) || defined(SYSTEM_IMI) || defined(SYSTEM_PM167)
  #define FAR_MEM far
#elif defined(SYSTEM_PC)
  #define FAR_MEM
#endif

using namespace Shared; // for Err_c and exceptions

/**
  object for seiral communication via Hdd device;
  the interface is initialized during constructor call;
  and is closed during destructor -> automatic;
  stream input and output operators ease the data communication
  @short object for seiral communication via Hdd device.
  @author Dipl.-Inform. Achim Spangler
*/
class RS232IO_c : public virtual Shared::Err_c {
public:

  /**
    enum type for state of the Hdd Object
  */
  enum t_hddState { off_state = 0, init_state = 1, flushingSlot = 2, flushedSlot = 3, cardReading = 4,
      openableState = 0x10, openingState = 0x60, open_state = 0x20, closingState = 0x50, fileStateChanging = 0x40};
  /**  enum type for error state */
  enum t_hddErr { noErr = 0, syncingTan = 1, justSyncedTan = 2, nosyncErr = 4, respErr = 8, fileNotFound = 0x10,
      cardErr = 0x20, noWriteSpaceErr = 0x40, unspecErr = 0x80};
  /**  enum for EOF state for read and write */
  enum t_eof_state {noEof = 0, writeEof = 1, readFileEof = 2, readCallEof = 4};
  /**
    enum type for state of one commend puffer
  */
  enum t_cmdPuffState {inactive = 0, active = 1, flushCmd = 2, SOH_STX = 0x13, CMD = 0x14, ACK = 5, RESPLen = 6, RESPData = 7,
      quickWorkState = 0x10};
  /**
    enum type for management of class global response puffer
  */
  enum t_respPuffState {respInactive = 0, await_STX = 1, await_ACK = 2, await_RESP = 3,
      complete_RESP = 4};
  
  /**
    enum type for file mode
  */
  enum t_filemode { readMode = 0, writeMode = 1, rdwr = 2, create = 4, append = 8};
  
  /**
    enum type for setting data bit, stop bit and parity within one parameter:
    databitParityStopbit {7,8}_{N,O,E}_{1,2}
  */
  enum t_dataMode {_7_E_1 = 0x11, _7_E_2 = 0x12, _7_O_1 = 0x31, _7_O_2 = 0x32, _8_E_1 = 0x21, _8_E_2 = 0x22,
        _8_O_1 = 0x41, _8_O_2 = 0x42, _8_N_1 = 0x51, _8_N_2 = 0x52};

  /**
    default constructor which can set some basic configurations
    @param rui16_baudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
    @param ren_dataMode data mode setting of {7,8}_{N,O,E}_{1,2}
    @param rb_xonXoff use XON/XOFF sw handshake (true, false)
    @param rui16_sndPuf sending puffer size
    @param rui16_recPuf recieving puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffers
        * Err_c::range one of the configuration vals is not in allowed ranges
  */
  RS232IO_c(uint16_t rui16_baudrate = CONFIG_RS232_DEFAULT_BAUDRATE,
          t_dataMode ren_dataMode = CONFIG_HDD_DEFAULT_DATA_MODE,
          bool rb_xonXoff = CONFIG_RS232_DEFAULT_XON_XOFF,  
          uint16_t rui16_sndPuf = CONFIG_RS232_DEFAULT_SND_PUF_SIZE, uint16_t rui16_recPuf = CONFIG_RS232_DEFAULT_REC_PUF_SIZE);
  
  /** destructor has nothing to destruct */
  ~RS232IO_c();

  // ++++++++++++++++++++++++++++++++++++
  // ++++ Hdd managing operations ++++
  // ++++++++++++++++++++++++++++++++++++

  /**
    initialise the Hdd driver
  */
  void init();

  /**
    check if the HDD driver is in error state
    @return true -> HDD is in error state
  */
  bool error()const {return (en_hddErr >= nosyncErr)?true:false;};
  /**
    check if Hdd can take a new cmd into the cmd puffer (e.g. if flush() results in an
    empty cmd puffer)
    @param rb_forceIdle true -> both cmd and send puffer must be idle (optional, default false)
    @return true -> new cmd like open,close can be used
  */
  bool await_newCmd(bool rb_forceIdle = false)const;
  /**
    check if there's text waiting for write to HDD
    @return true -> temporary text is not yet flushed to HDD
  */
  bool waitingTextForFlush()const{return (ui16_tempLen > 0)?true:false;};
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
    @return Hdd baudrate
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
    set receive puffer size
    @param rui16_recPuf receiving puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffer
    @return true -> allocating of puffer successful
  */
  bool setRecPufferSize(uint16_t rui16_pufferSize);

  /**
    control state change of HDD and call timeEventInternControl
    for performing the needed actions
  */
  void timeEvent();
  /**
    let timeEventInternLoop perform actions till
    * quick important part actions are complete (e.g. SOH2STX)
    * incoming response is complete (only for short responses where first part is already received)
    * sendPuffer is inactive and no further flushable cmd is waiting IF rb_forceFinalIdle == TRUE
    @param rb_forceFinalIdle true -> force performing of all waiting cmd's
  */
  void timeEventInternControl(bool rb_forceFinalIdle = false);

  /**
    perform next single step of interaction with HDD
  */
  void timeEventInternLoop();        
  /**
    check if time event is needed for further working on sent cmd
    @return true -> timeEvent should be called
  */
  bool needTimeEvent()const;

  /**
    test if not active puffer is ready to switch to active (e.g. is inactive)
    and switch hddState to errNoresp if not;
    set flushCmd flag for active cmd puffer; switch active cmd puffer;
    @param rb_forceFreeCmdPuff optional force waiting till cmdPuff is
          ready for new cmd (default false)
    @return true -> cmd puffer is now ready for new command
  */
  bool flush(bool rb_forceFreeCmdPuff = false);
  /* ******************************* */
  /* ****** managing of files ****** */
  /* ******************************* */

  /**
    open a file on hdd; after executing this Cmd, the Hdd object switches from closeState to open_state state
    -> writing operations are allowed;
    if this RS232IO_c object is NOT in closeState state, this function returns false
    @param rpb_name name of the file
    @param ren_filemode mode of the file, combination of {readMode, writeMode, create, append}, default is all of them
    @return true -> RS232IO_c switched from closeState to open_state state correct
  */
  bool open(const int8_t* rpb_name, t_filemode ren_filemode = static_cast<t_filemode>(rdwr | create | append));
  /**
    test if file on HDD was not found for open action
    @return true -> wanted file on HDD not found for open
  */
  bool isFileNotFound()const{return ((en_hddErr & fileNotFound) > 0)?true:false;};
  /**
    test if HDD is in open state
    @return true -> file on HDD is open for interaction
  */
  bool isOpen()const{return (en_hddState == open_state)?true:false;};
  /**
    check if EOF for read access is reached
    @return true -> no more data can be read
  */
  bool isReadEof()const{return ((en_eofState & readCallEof) == 0)?true:false;};
  /**
    check if medium is full so that no data can be written
    @return true -> no wirte access possible because no free memory
  */
  bool isFreeSpace()const{return ((en_eofState & writeEof) == 0)?true:false;};
  /**
    close a file on Hdd
    @param rb_flushWriteText true -> flush all stored text to HDD before close of file (optional default)
    @return true -> this object switched from other state to closeState state
  */
  bool close(bool rb_flushWriteText = true);


  /* ******************************** */
  /* iostream related output operator */
  /* ********    sending     ******** */
  /* ******************************** */
  
  /**
    read from a file on Hdd
    @param rui16_len length of data to read
    @return true -> command executed
  */
  bool readFile(uint16_t rui16_len);

  /**
    write data uint8_t string with given length on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rpb_data pointer to data string
    @param rui16_len length of data string
    @return true -> successful inserted
  */
  bool write(const uint8_t *const rpData, uint16_t rui16_len);
  /**
    send string on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rrefc_data sent data string
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(const basic_string<int8_t>& rrefc_data);
  /**
    send NULL terminated string on Hdd (terminating NULL isn't sent)

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rpc_data pointer to NULL terminated string to send
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(const int8_t *const rpc_data);
  /**
    send single uint8_t on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rb_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(uint8_t rb_data);
  /**
    send single int8_t on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rc_data sent data char
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(int8_t rc_data);
  /**
    send uint16_t (2byte unsigned) value as text on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rui16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(uint16_t rui16_data);
  /**
    send int16_t (2 uint8_t unsigned) value as text on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(int16_t ri16_data);
#if defined(SYSTEM_PC)
  /**
    send uint16_t value as text on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rui16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(uint16_t rui16_data);
  /**
    send int16_t value as text on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(int16_t ri16_data);
#endif
  /**
    send dword (4 uint8_t unsigned) value as text on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rui32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(dword rui32_data);
  /**
    send int32_t (4 uint8_t signed) value as text on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(int32_t ri32_data);
  /**
    send float value as text on Hdd

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  RS232IO_c& operator<<(float rf_data);

// read operators
  /*
    read one line
    @param pc_line pointer to uint8_t string for the file text line
    @param rui8_maxLen maximum length to read
    @param rb_delim optional delimiter (default '\n')
    @return reference to RS232_IO
  */
  RS232IO_c& getline(int8_t *pc_linePointer, uint8_t rui8_maxLen, int8_t rc_delim = '\n');
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
    receive dword (4 uint8_t unsigned) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param ui32_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(dword& ui32_data);
  /**
    receive int32_t (4 uint8_t signed) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param i32_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(int32_t& i32_data);
  /**
    receive float value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param f_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  RS232IO_c& operator>>(float& f_data);
private:
  // private methods
  /**
    perform introduction work for write call
    @return true -> write can be continued
  */
  bool writeInternPre();
  /**
    perform pending work for write
  */
  void writeInternPost();
  /**
    if cmd or send puffer must be reset, write text should be stored
    write text in cmd puff is always stored
    @param rb_storeSendPuff true -> force to store write text in send puf also
  */
  void preserveWriteText(bool rb_storeSendPuff);

  /**
    reset the internal buffers for send and
    response; on demand (default) also the cmdPuff is reset
    @param rb_resetCmd also reset cmdPuff (default = true)
  */
  void resetCmdBuffers(bool rb_resetCmd = true);
  /**
    check if file can immediately be used
    (a) file is open or
    (b) can be immediately be opened with free usable cmdPuffer afterwards
    @return true -> file can be used for write and read
  */
  bool fileUsable();
  /**
    deliver the actual send puffer size
    @return send puffer size
  */
  uint16_t sndPufferSize()const{return ui16_sndPuf;};
  /**
    set state to repeat send of cmd due to response error
    @param rb_respErr true -> a response error is registered (default true)
  */
  void triggerCmdRepeat(bool rb_respErr = true);
  /**
    clear the send puffer without send of actual data in puffer
  */
  void clearSndPuffer()const{HAL::clearRs232TxBuffer();};
  /**
    deliver the actual receive puffer size
    @return receive puffer size
  */
  uint16_t rec_pufferSize()const{return ui16_recPuf;};
  /**
    clear the receive puffer without reading of actual data in puffer
  */
  void clearRecPuffer()const{HAL::clearRs232RxBuffer();};
  /**
    check if the receive puffer is empty
    @return true -> receive puffer is empty
  */
  bool eof()const{return (HAL::getRs232RxBufCount() == 0)?true:false;};
  /**
    deliver the count of data uint8_t in send puffer
    @return amount of data bytes in send puffer
  */
  uint16_t sndPufferCnt()const{return HAL::getRs232TxBufCount();};
  /**
    deliver the count of data uint8_t in receive puffer
    @return amount of data bytes in receive puffer
  */
  uint16_t rec_pufferCnt()const{return HAL::getRs232RxBufCount();};
  
  /**
    set a CMD in a cmd puffer
    @param rb_cmd cmd code
  */
  void setCmd(uint8_t rb_cmd){c_puffCmd[3] = rb_cmd;};
  /**
    get the CMD code of a puffer
    @return cmd of wanted puffer
  */
  uint8_t getCmd(){return c_puffCmd[3];};
  /**
    deliver len of cmd puffer (without chksum part)
    @return len of puffer, which must be inserted at start of message
  */
  uint16_t getLen(){return c_puffCmd.size();};
  /**
    deliver stored len of response
    @return len in bytes including the two len data uint8_t and excluding the checksum
  */
  uint16_t getRespLen() const {return (c_puffResp[0] + ((uint16_t)c_puffResp[1] << 8));};
  /**
    validate received response: check len and chksum
    @return true -> stored len is correct AND chksum is correct
  */
  bool validateResponse();
  /**
    prepare one cmd puffer for send: set len and chksum fields
  */
  void preparePuffer();
  /**
    send data uint8_t string with given length from cmd puffer on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rpb_data pointer to data string
    @param rui16_len length of data string
  */
  void send(FAR_MEM uint8_t* rpData, uint16_t rui16_len);
  /**
    insert new cmd in cmd puffer and call timeEventIntern
    to start performation
    @param rb_cmd command
    @param rui16_par optional parameter
  */
  void startSimpleCmd(uint8_t rb_cmd, uint16_t rui16_par = 0xFFFF);
  /**
    send cmd to flushCmd the open file on Hdd drive
  */
  void flushCmdFileHdd();
  /**
    send cmd to flushCmd the open slot on Hdd drive
    -> write all puffer on Hdd drive to disc and close all
       open files
    -> important to call on system stop
  */
  void flushCmdSlotHdd();
  /**
    init cmd sequence by resetting TAN with cmd readVersion
  */
  void readVersionHdd();
  /**
    get card information (above all to gett free space on media)
  */
  void getCardinfo();
  /**
    send start-of-header SOH
  */
  void sendSoh();
  /**
    check if STX is received and change
    en_sendPuffState to CMD for sending
    of cmd as next action
    @return true -> STX received -> cmd can be sent now
  */
  bool soh2stx();
  /**
    check if ACK was received
    @return true -> ACK received -> receive RESP next
  */
  bool checkAck();
  /**
    read len of response (first two bytes)
    @return true -> two uint8_t with len of response are read in response buffer
  */
  bool readRespLen();
  /**
    read the response data in response puffer
    @return true -> complete response received
  */
  bool readRespData();
  /**
    compare TAN of response and TAN of last sent cmd
    @return true -> coresponding TAN
  */
  bool checkRespTan();
  /**
    interprete the received data
  */
  void interpreteRespData();

  /**
    read a token (eat whitespace in front of, end stop before next whitespace or at end og buf)

    possible errors:
        * Err_c::rs232_underflow no data other than whitespace in puffer to read in token
  */
  void readToken();
private:
  // Private attributes
  /** time of last sent cmd */
  int32_t i32_cmdSendTime;
  /** time of last write operation in active cmd puffer */
  int32_t i32_cmdWriteTime;
  /** last time of flushCmding an open file */
  int32_t i32_lastFlushCmdDriveTime;
  /** last time of receive of response */
  int32_t i32_lastCompleteResponse;
  /** send time of SOH (Start Of Header) -> to detect response problem */
  int32_t i32_lastSoh;
  int32_t i32_lastWd;
  /** last time of hdd error */
  int32_t i32_lastRespCardErr;

  uint16_t ui16_baudrate;
  t_dataMode en_dataMode;
  bool b_xon_xoff;
  uint16_t ui16_sndPuf;
  uint16_t ui16_recPuf;
  int8_t pc_token[CONFIG_HDD_MAX_TOKEN_LEN];
  // Hdd specific vars
  /** name of connected file */
  basic_string<int8_t> c_filename;
  /** filemode */
  t_filemode en_filemode;
  /** state of the Hdd object */
  t_hddState en_hddState;
  /** err state of HDD object */
  t_hddErr en_hddErr;
  /** store EOF for write and read actions */
  t_eof_state en_eofState;
  /** state of the cmd puffer */
  t_cmdPuffState en_cmdPuffState;
  /** string for the two cmd strings */
  vector<uint8_t> c_puffCmd;
  FAR_MEM uint8_t *pb_sendPuff;
  uint16_t ui16_sendPuffActSize,
      ui16_sendPuffMaxSize;
  /** state of the sen puffer */
  t_cmdPuffState en_sendPuffState;
  /**
    string for the last received response;
    global for all instances of RS232IO_c -> one common response puffer
  */
  vector<uint8_t> c_puffResp;
  /**
    global flag to control receive of response puffer
  */
  t_respPuffState en_respPuffState;
  /** handle number for connected file for write */
  uint8_t b_writeHandle;
  /** static global for all RS232IO_c instances TAN number */
  uint8_t b_tan;
  /** state of last written data */
  bool b_lastFileWriteFlushed;
  /** length of pb_tempString */
  uint16_t ui16_tempLen;
};


}
} // end of namespace __IsoAgLib

#endif
