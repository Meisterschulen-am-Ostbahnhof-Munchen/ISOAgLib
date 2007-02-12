/***************************************************************************
             errcodes.h  - header for the DjBios1
                           error codes
                -------------------                                         
    begin    : 12 Jun 2006                                           
    copyright: (C) 2006 Joel T. Morton (DICKEY-john Corp.)
    email    : jmorton@dickey-john:com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef __DJ1_ERRCODES_H__
#define __DJ1_ERRCODES_H__

namespace HAL 
{
//   /* include BIOS errcode definition */
//   extern "C" 
//   {
//     #include <commercial_BIOS/bios_DjBios1/DjBios1.h>
//   }

  /* --- map BIOS error codes to target independend HAL codes --- */
  #define HAL_NO_ERR             0       // C_NO_ERR
  #define HAL_BUSY_ERR           0xFFFF  // C_BUSY
  #define HAL_CONFIG_ERR         0xFFFE  // C_CONFIG
  #define HAL_RANGE_ERR          0xFFFD  // C_RANGE
  #define HAL_NOACT_ERR          0xFFFC  // C_NOACT
  #define HAL_WARN_ERR           0xFFFB  // C_WARN
  #define HAL_OVERFLOW_ERR       0xFFFA  // C_OVERFLOW
  #define HAL_RS232_RECEIVE_ERR  0xFFF9  // C_RD_WR
  #define HAL_UNKNOWN_ERR        0xFFF8  // C_UNKNOWN_ERR

  #define HAL_DEFAULT            0xFFF8  // C_DEFAULT
  #define HAL_RD_WR              0xFFF6  // C_RD_WR
  #define HAL_COM                0xFFF5  // C_COM
  #define HAL_CHECKSUM           0xFFF4  // C_CHECKSUM

  #define HAL_WD_DATA_CHANGED    0xFFF2  // 1

  #define HAL_DIGOUT_OPEN        0xFFF1  //  (int16_t)(-(20 * 0x0001))
  #define HAL_DIGOUT_SHORTCUT    0xFFF0  //  (int16_t)(-(21 * 0x0001))
  #define HAL_DIGOUT_OVERTEMP    0xFFEF  //  (int16_t)(-(22 * 0x0001))
  #define HAL_DIGOUT_UNDERVOLT   0xFFEE  //  (int16_t)(-(23 * 0x0001))
  #define HAL_DIGOUT_OVERVOLT    0xFFED  //  (int16_t)(-(24 * 0x0001))

  #define EE_READY   HAL_NO_ERR
} // end namespace HAL
#endif

/***************************************************************************
             errcodes.h  - header for the DjBios1
                           error codes
                -------------------                                         
    begin    : 12 Jun 2006                                           
    copyright: (C) 2006 Joel T. Morton (DICKEY-john Corp.)
    email    : jmorton@dickey-john:com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef __DJ1_ERRCODES_H__
#define __DJ1_ERRCODES_H__

namespace HAL 
{
//   /* include BIOS errcode definition */
//   extern "C" 
//   {
//     #include <commercial_BIOS/bios_DjBios1/DjBios1.h>
//   }

  /* --- map BIOS error codes to target independend HAL codes --- */
  #define HAL_NO_ERR             0       // C_NO_ERR
  #define HAL_BUSY_ERR           0xFFFF  // C_BUSY
  #define HAL_CONFIG_ERR         0xFFFE  // C_CONFIG
  #define HAL_RANGE_ERR          0xFFFD  // C_RANGE
  #define HAL_NOACT_ERR          0xFFFC  // C_NOACT
  #define HAL_WARN_ERR           0xFFFB  // C_WARN
  #define HAL_OVERFLOW_ERR       0xFFFA  // C_OVERFLOW
  #define HAL_RS232_RECEIVE_ERR  0xFFF9  // C_RD_WR
  #define HAL_UNKNOWN_ERR        0xFFF8  // C_UNKNOWN_ERR

  #define HAL_DEFAULT            0xFFF8  // C_DEFAULT
  #define HAL_RD_WR              0xFFF6  // C_RD_WR
  #define HAL_COM                0xFFF5  // C_COM
  #define HAL_CHECKSUM           0xFFF4  // C_CHECKSUM

  #define HAL_WD_DATA_CHANGED    0xFFF2  // 1

  #define HAL_DIGOUT_OPEN        0xFFF1  //  (int16_t)(-(20 * 0x0001))
  #define HAL_DIGOUT_SHORTCUT    0xFFF0  //  (int16_t)(-(21 * 0x0001))
  #define HAL_DIGOUT_OVERTEMP    0xFFEF  //  (int16_t)(-(22 * 0x0001))
  #define HAL_DIGOUT_UNDERVOLT   0xFFEE  //  (int16_t)(-(23 * 0x0001))
  #define HAL_DIGOUT_OVERVOLT    0xFFED  //  (int16_t)(-(24 * 0x0001))

  #define EE_READY   HAL_NO_ERR
} // end namespace HAL
#endif

