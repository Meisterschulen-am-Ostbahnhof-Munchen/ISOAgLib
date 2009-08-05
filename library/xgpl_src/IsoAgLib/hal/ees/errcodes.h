/***************************************************************************
                          errcodes.h  - header for the PSEUDO BIOS
                                           error codes
                             -------------------
    begin                : 04/20/09 11:32:46 AM CEST
    copyright            : (C) 2009 OSB-AG
    email                : info@osb-ag:de
 ***************************************************************************/

#ifndef __ERRCODES_H
#define __ERRCODES_H__

#include <commercial_BIOS/bios_ees/hal/hal_can.h>

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
