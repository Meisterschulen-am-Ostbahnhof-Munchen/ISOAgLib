/***************************************************************************
                          hal_can_interface.h - CAN interface between
                                        BIOS/OS and IsoAgLib to concentrate
                                        CAN handling abstraction within
                                        one module
                             -------------------
    begin                : 04/20/09 11:32:46 AM CEST
    copyright            : (C) 2009 OSB-AG
    email                : m.wodok@osb-ag:de
 ***************************************************************************/

#ifndef _HAL_EES_CAN_INTERFACE_H_
#define _HAL_EES_CAN_INTERFACE_H_

namespace __IsoAgLib {
  class Ident_c;
  class CanPkg_c;
}

namespace __HAL {

  /**
    send a message via a MsgObj;
    CanPkg_c (or derived object) must provide (virtual)
    functions:
    * Ident_c& getIdent() -> deliver ident of msg to send
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
  int16_t can_useMsgobjSend( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, __IsoAgLib::CanPkg_c* apc_data );

}
#endif
// eof
