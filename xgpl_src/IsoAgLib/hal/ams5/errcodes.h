/***************************************************************************
    errcodes.h  - header for error codes
    -------------------
    date                 : 18.06.2007
    copyright            : (C) 2007 GESAS GmbH
    email                : stefan.klueh@gesas:de
 ***************************************************************************/

#ifndef __AMS5_ERRCODES_H__
   #define __AMS5_ERRCODES_H__
   
   namespace HAL
   {
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
   } // end namespace HAL

   namespace AMSBIOS
   {
      #define BIOS_NO_ERR         (int16_t)( (0 * 0x0001))
      #define BIOS_UNKNOWN_ERR    (int16_t)(-(1 * 0x0001))
      #define BIOS_WARN_ERR       (int16_t)(-(2 * 0x0001))
      #define BIOS_DEFAULT        (int16_t)(-(3 * 0x0001))
      #define BIOS_BUSY_ERR       (int16_t)(-(4 * 0x0001))
      #define BIOS_RANGE_ERR      (int16_t)(-(5 * 0x0001))
      #define BIOS_OVERFLOW_ERR   (int16_t)(-(6 * 0x0001))
      #define BIOS_RD_WR_ERR      (int16_t)(-(7 * 0x0001))
      #define BIOS_NOACT_ERR      (int16_t)(-(8 * 0x0001))
      #define BIOS_COM            (int16_t)(-(9 * 0x0001))
      #define BIOS_CONFIG_ERR     (int16_t)(-(10* 0x0001))
      #define BIOS_CHECKSUM       (int16_t)(-(11* 0x0001))
   
      #define BIOS_WD_DATA_CHANGED   1

#define HAL_DIGOUT_OPEN      (int16_t)(-(20 * 0x0001))
#define HAL_DIGOUT_SHORTCUT  (int16_t)(-(21 * 0x0001))
#define HAL_DIGOUT_OVERTEMP  (int16_t)(-(22 * 0x0001))
#define HAL_DIGOUT_UNDERVOLT (int16_t)(-(23 * 0x0001))
#define HAL_DIGOUT_OVERVOLT  (int16_t)(-(24 * 0x0001))
#define HAL_NEW_SEND_DELAY   (int16_t)(-(25 * 0x0001))

   } // end namespace AMSBIOS



#endif

