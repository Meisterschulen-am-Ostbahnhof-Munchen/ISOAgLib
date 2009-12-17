/*
  hal_can_interface.cpp: CAN interface between BIOS/OS and IsoAgLib to
    concentrate CAN handling abstraction within one module

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "../errcodes.h"
#include "../typedef.h"
#include "hal_can_interface.h"

#include <IsoAgLib/driver/can/impl/canpkg_c.h>

#include <IsoAgLib/hal/generic_utils/can/write_central_fifo.h>
#include <IsoAgLib/hal/generic_utils/can/canfifo_c.h>

#include <commercial_BIOS/bios_ees/hal/hal_can.h>

namespace __HAL {

  //! statically allocated message structure
  static hal_n::can_c::msg_t sc_sendMsg;

  /**
    send a message via a MsgObj;
    CanPkg_c (or derived object) must provide (virtual)
    functions:
    * MASK_TYPE ident() -> deliver ident value
    * __IsoAgLib::Ident_c::identType_t identType() -> deliver type of ident
    * void getData(MASK_TYPE& rt_ident, uint8_t& rui8_identType,
                   uint8_t& rb_dlcTarget, uint8_t* pb_dataTarget)
      -> put DLC in referenced r_dlc and insert data in uint8_t string pb_data
    @param aui8_busNr number of the BUS to config
    @param aui8_msgobjNr number of the MsgObj to config
    @param apc_data pointer to CanPkg_c instance with data to send
    @return HAL_NO_ERR == no error;
            HAL_CONFIG_ERR == BUS not initialised, MsgObj is no send object
            HAL_NOACT_ERR == BUS OFF
            HAL_OVERFLOW_ERR == send buffer overflowed
            HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
  int16_t can_useMsgobjSend( uint8_t aui8_busNr, uint8_t /* aui8_msgobjNr */, __IsoAgLib::CanPkg_c* apc_data ) {
    apc_data->getData( sc_sendMsg.t_id.ui32_id, sc_sendMsg.t_id.ui8_xTd, sc_sendMsg.ui8_dlc, sc_sendMsg.t_canData.arrui8_data );
    return hal_n::can_c::instance().sendMsg( aui8_busNr, sc_sendMsg );
  }

  /**
   * Write Can Message Data to central can FIFO
   */
  int16_t iWriteCentralCanFifo( uint8_t bBus, hal_n::can_c::msg_t* ap_msg ) {
    HAL::iFifoWrite( bBus, -1, ap_msg->t_id.ui32_id, static_cast<void*>( ap_msg ) );
    return 0;
  }

  /**
   * Retrive Can data from BIOS format
   */
  void getIrqData( void* inputData, HAL::fifoData_s* destination, uint8_t /* aui8_extended */ ) {

    destination->dwId = static_cast<hal_n::can_c::msg_t*>( inputData )->t_id.ui32_id;
    destination->bXtd = static_cast<hal_n::can_c::msg_t*>( inputData )->t_id.ui8_xTd;
    destination->bDlc = static_cast<hal_n::can_c::msg_t*>( inputData )->ui8_dlc;

    memcpy( destination->abData, static_cast<hal_n::can_c::msg_t*>( inputData )->t_canData.arrui8_data, destination->bDlc );
  }

} // end of namespace
// eof
