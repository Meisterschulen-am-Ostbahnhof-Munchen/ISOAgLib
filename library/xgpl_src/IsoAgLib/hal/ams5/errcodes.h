// ---------------------------------------------------------------------------------------------
/** \file      errcodes.h
    \brief     header file for AMS5 (M32C/87)
               error codes
    \version   1.00
    \date      10.07.2007
    \author    Stefan Klueh
    \author    copyright (c) 2007 GESAS GmbH
*/
// ---------------------------------------------------------------------------------------------

#ifndef __AMS5_ERRCODES_H__
   #define __AMS5_ERRCODES_H__

   namespace HAL
   {
      /* --- map BIOS error codes to target independent HAL codes --- */
#ifndef HAL_NO_ERR
      #define HAL_NO_ERR         (int16_t)( (0 * 0x0001))
#endif
#ifndef HAL_UNKNOWN_ERR
      #define HAL_UNKNOWN_ERR    (int16_t)(-(1 * 0x0001))
#endif
#ifndef HAL_WARN_ERR
      #define HAL_WARN_ERR       (int16_t)(-(2 * 0x0001))
#endif
#ifndef HAL_DEFAULT
      #define HAL_DEFAULT        (int16_t)(-(3 * 0x0001))
#endif
#ifndef HAL_BUSY_ERR
      #define HAL_BUSY_ERR       (int16_t)(-(4 * 0x0001))
#endif
#ifndef HAL_RANGE_ERR
      #define HAL_RANGE_ERR      (int16_t)(-(5 * 0x0001))
#endif
#ifndef HAL_OVERFLOW_ERR
      #define HAL_OVERFLOW_ERR   (int16_t)(-(6 * 0x0001))
#endif
#ifndef HAL_RD_WR
      #define HAL_RD_WR          (int16_t)(-(7 * 0x0001))
#endif
#ifndef HAL_NOACT_ERR
      #define HAL_NOACT_ERR      (int16_t)(-(8 * 0x0001))
#endif
#ifndef HAL_COM
      #define HAL_COM            (int16_t)(-(9 * 0x0001))
#endif
#ifndef HAL_CONFIG_ERR
      #define HAL_CONFIG_ERR     (int16_t)(-(10* 0x0001))
#endif
#ifndef HAL_CHECKSUM
      #define HAL_CHECKSUM       (int16_t)(-(11* 0x0001))
#endif
#ifndef HAL_WD_DATA_CHANGED
      #define HAL_WD_DATA_CHANGED   1
#endif
   } // end namespace HAL

   namespace AMSBIOS
   {
#ifndef BIOS_NO_ERR
      #define BIOS_NO_ERR         (int16_t)( (0 * 0x0001))
#endif
#ifndef BIOS_UNKNOWN_ERR
      #define BIOS_UNKNOWN_ERR    (int16_t)(-(1 * 0x0001))
#endif
#ifndef BIOS_WARN_ERR
      #define BIOS_WARN_ERR       (int16_t)(-(2 * 0x0001))
#endif
#ifndef BIOS_DEFAULT
      #define BIOS_DEFAULT        (int16_t)(-(3 * 0x0001))
#endif
#ifndef BIOS_BUSY_ERR
      #define BIOS_BUSY_ERR       (int16_t)(-(4 * 0x0001))
#endif
#ifndef BIOS_RANGE_ERR
      #define BIOS_RANGE_ERR      (int16_t)(-(5 * 0x0001))
#endif
#ifndef BIOS_OVERFLOW_ERR
      #define BIOS_OVERFLOW_ERR   (int16_t)(-(6 * 0x0001))
#endif
#ifndef BIOS_RD_WR_ERR
      #define BIOS_RD_WR_ERR      (int16_t)(-(7 * 0x0001))
#endif
#ifndef BIOS_NOACT_ERR
      #define BIOS_NOACT_ERR      (int16_t)(-(8 * 0x0001))
#endif
#ifndef BIOS_COM
      #define BIOS_COM            (int16_t)(-(9 * 0x0001))
#endif
#ifndef BIOS_CONFIG_ERR
      #define BIOS_CONFIG_ERR     (int16_t)(-(10* 0x0001))
#endif
#ifndef BIOS_CHECKSUM
      #define BIOS_CHECKSUM       (int16_t)(-(11* 0x0001))
#endif
#ifndef BIOS_WD_DATA_CHANGED
      #define BIOS_WD_DATA_CHANGED   1
#endif
#ifndef HAL_DIGOUT_OPEN
      #define HAL_DIGOUT_OPEN      (int16_t)(-(20 * 0x0001))
#endif
#ifndef HAL_DIGOUT_SHORTCUT
      #define HAL_DIGOUT_SHORTCUT  (int16_t)(-(21 * 0x0001))
#endif
#ifndef HAL_DIGOUT_OVERTEMP
      #define HAL_DIGOUT_OVERTEMP  (int16_t)(-(22 * 0x0001))
#endif
#ifndef HAL_DIGOUT_UNDERVOLT
      #define HAL_DIGOUT_UNDERVOLT (int16_t)(-(23 * 0x0001))
#endif
#ifndef HAL_DIGOUT_OVERVOLT
      #define HAL_DIGOUT_OVERVOLT  (int16_t)(-(24 * 0x0001))
#endif
#ifndef HAL_NEW_SEND_DELAY
      #define HAL_NEW_SEND_DELAY   (int16_t)(-(25 * 0x0001))
#endif
   } // end namespace AMSBIOS



#endif

