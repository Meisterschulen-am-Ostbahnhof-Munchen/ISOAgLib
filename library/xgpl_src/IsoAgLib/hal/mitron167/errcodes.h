/*
  errcodes.h: header for the ESX error codes

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __MITRON167_ERRCODES_H__
#define __MITRON167_ERRCODES_H__

namespace HAL {
  /* --- map BIOS error codes to target independent HAL codes --- */
   #define HAL_NO_ERR         (int16_t)( (0 * 0x0001))
   #define HAL_UNKNOWN_ERR    (int16_t)(-(1 * 0x0001))
   #define HAL_WARN_ERR       (int16_t)(-(2 * 0x0001))
   #define HAL_DEFAULT        (int16_t)(-(3 * 0x0001))
   #define HAL_BUSY_ERR       (int16_t)(-(4 * 0x0001))
   #define HAL_RANGE_ERR      (int16_t)(-(5 * 0x0001))
   #define HAL_OVERFLOW_ERR   (int16_t)(-(6 * 0x0001))
   #define HAL_RD_WR          (int16_t)(-(7 * 0x0001))
   #define HAL_NOACT_ERR      (int16_t)(-(8 * 0x0001))
   #define HAL_COM            (int16_t)(-(9 * 0x0001))
   #define HAL_CONFIG_ERR     (int16_t)(-(10* 0x0001))
   #define HAL_CHECKSUM       (int16_t)(-(11* 0x0001))

   #define HAL_WD_DATA_CHANGED   1
} // end namespace __HAL
#endif

