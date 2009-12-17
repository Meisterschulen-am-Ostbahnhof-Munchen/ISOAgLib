/*
  errcodes.h: header for the PSEUDO BIOS error codes

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __ERRCODES_H
#define __ERRCODES_H__

#include <hal_can.h>

#define HAL_NO_ERR          hal_n::ec_noError
#define HAL_UNKNOWN_ERR     0xF0
#define HAL_WARN_ERR        0xF1
#define HAL_DEFAULT         0xF2
#define HAL_BUSY_ERR        0xF3
#define HAL_RANGE_ERR       hal_n::ec_range
#define HAL_OVERFLOW_ERR    hal_n::ec_overflow
#define HAL_RD_WR           0xF4
#define HAL_NOACT_ERR       hal_n::ec_noAction
#define HAL_COM             0xF5
#define HAL_CONFIG_ERR      0xF6
#define HAL_CHECKSUM        0xF7
#define HAL_WD_DATA_CHANGED 0xF8

#endif
