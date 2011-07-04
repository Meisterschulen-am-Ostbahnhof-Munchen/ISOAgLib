/*
  errcodes.h: header for the P1MC error codes

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __P1MC_ERRCODES_H__
#define __P1MC_ERRCODES_H__

namespace HAL {
  /* include BIOS errcode definition */
  /* --- map BIOS error codes to target independent HAL codes --- */
  #define HAL_NO_ERR            1
  #define HAL_BUSY_ERR          2
  #define HAL_CONFIG_ERR        3
  #define HAL_RANGE_ERR         4
  #define HAL_NOACT_ERR         5
  #define HAL_WARN_ERR          6
  #define HAL_OVERFLOW_ERR      7
  #define HAL_RS232_RECEIVE_ERR 8
  #define HAL_UNKNOWN_ERR       9

  #define HAL_DEFAULT           10
  #define HAL_RD_WR             11
  #define HAL_COM               12
  #define HAL_CHECKSUM          13

  #define HAL_NEW_SEND_DELAY    14

  #define HAL_WD_DATA_CHANGED   15

  #define HAL_DIGOUT_OPEN        (int16_t)(-(20 * 0x0001))
  #define HAL_DIGOUT_SHORTCUT    (int16_t)(-(21 * 0x0001))
  #define HAL_DIGOUT_OVERTEMP    (int16_t)(-(22 * 0x0001))
  #define HAL_DIGOUT_UNDERVOLT   (int16_t)(-(23 * 0x0001))
  #define HAL_DIGOUT_OVERVOLT    (int16_t)(-(24 * 0x0001))

  #define EE_READY 0
} // end namespace __HAL
#endif

