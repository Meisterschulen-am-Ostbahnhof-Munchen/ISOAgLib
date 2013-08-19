/*
  errcodes.h: header for the DjBios1 error codes

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __DJ1_ERRCODES_H__
#define __DJ1_ERRCODES_H__

/* --- map BIOS error codes to target independend HAL codes --- */
#define HAL_NO_ERR             0       // C_NO_ERR
#define HAL_BUSY_ERR           0xFFFF  // C_BUSY
#define HAL_CONFIG_ERR         0xFFFE  // C_CONFIG
#define HAL_RANGE_ERR          0xFFFD  // C_RANGE
#define HAL_NOACT_ERR          0xFFFC  // C_NOACT
#define HAL_WARN_ERR           0xFFFB  // C_WARN
#define HAL_OVERFLOW_ERR       0xFFFA  // C_OVERFLOW
#define HAL_UNKNOWN_ERR        0xFFF8  // C_UNKNOWN_ERR

#define HAL_DIGOUT_OPEN        0xFFF1  //  (int16_t)(-(20 * 0x0001))
#define HAL_DIGOUT_SHORTCUT    0xFFF0  //  (int16_t)(-(21 * 0x0001))
#define HAL_DIGOUT_OVERTEMP    0xFFEF  //  (int16_t)(-(22 * 0x0001))
#define HAL_DIGOUT_UNDERVOLT   0xFFEE  //  (int16_t)(-(23 * 0x0001))
#define HAL_DIGOUT_OVERVOLT    0xFFED  //  (int16_t)(-(24 * 0x0001))
#define HAL_NEW_SEND_DELAY     0xFFEC  //  (int16_t)(-(25 * 0x0001))

#endif
