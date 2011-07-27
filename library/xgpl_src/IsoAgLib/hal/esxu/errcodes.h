/*
  errcodes.h: header for the ESXu error codes

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __ESXu_ERRCODES_H__
#define __ESXu_ERRCODES_H__

namespace HAL {
  /* include BIOS errcode definition */
  extern "C" {
    #include <commercial_BIOS/bios_esxu/mos10err.h>
  }
  /* --- map BIOS error codes to target independend HAL codes --- */
  #define HAL_NO_ERR            C_NO_ERR
  #define HAL_BUSY_ERR          C_BUSY
  #define HAL_CONFIG_ERR        C_CONFIG
  #define HAL_RANGE_ERR         C_RANGE
  #define HAL_NOACT_ERR         C_NOACT
  #define HAL_WARN_ERR          C_WARN
  #define HAL_OVERFLOW_ERR      C_OVERFLOW
  #define HAL_RS232_RECEIVE_ERR C_RD_WR
  #define HAL_UNKNOWN_ERR       C_UNKNOWN_ERR

  #define HAL_DEFAULT           C_DEFAULT
  #define HAL_RD_WR             C_RD_WR
  #define HAL_COM               C_COM
  #define HAL_CHECKSUM          C_CHECKSUM

  #define HAL_WD_DATA_CHANGED   1

  #define HAL_DIGOUT_OPEN        (int16_t)(-(20 * 0x0001))
  #define HAL_DIGOUT_SHORTCUT    (int16_t)(-(21 * 0x0001))
  #define HAL_DIGOUT_OVERTEMP    (int16_t)(-(22 * 0x0001))
  #define HAL_DIGOUT_UNDERVOLT   (int16_t)(-(23 * 0x0001))
  #define HAL_DIGOUT_OVERVOLT    (int16_t)(-(24 * 0x0001))

  #ifdef USE_CAN_SEND_DELAY_MEASUREMENT
  #define HAL_NEW_SEND_DELAY     (int16_t)(-(25 * 0x0001))
  #endif


} // end namespace __HAL
#endif

