/***************************************************************************
                             dinmaskuploadstructures_t.h -
                               data strucutures and defines for IsoAgLib+
                               terminal mask upload
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#ifndef DIN_MASKUPLOAD_STRUCTURES_H
#define DIN_MASKUPLOAD_STRUCTURES_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/igetypos_c.h>
#include <IsoAgLib/typedef.h>

/** the class DINMaskUpload_c uses a constant C-Array
    of pointers to store the registered mask definitions
    - define the size of the array
  */
#define REGISTRABLE_MASK_CNT 4

// Defines for the different syncronisation informations
// define syncronisation settings for old style Fieldstar Upload
/** WERT/INST setting for process data with software version */
#define FS_OLD_SW_VERSION        0xF3
/** INST setting for process data with required version of Fieldstar Terminal */
#define FS_OLD_REQ_TERM_VERSION  0x0C
/** INST setting for process data with terminal project version of Fieldstar Terminal mask pool */
#define FS_OLD_PROJECT_VERSION   0x0E
/** INST setting for process data with boot code version of Fieldstar Terminal mask pool */
#define FS_OLD_BOOT_CODE_VERSION 0x0F

// define syncronisation for IsoAgLib+
/** ZAEHLNUM setting for IsoAgLib+ system status: 1==mask available for upload */
#define LBS_PLUS_SYSTEM_STATUS          2
/** ZAEHLNUM setting for version of generic driver */
#define LBS_PLUS_GENERIC_DRIVER_VERSION 3
/** ZAEHLNUM setting for required version of the connected terminal of specified type */
#define LBS_PLUS_REQUIRED_VERSION       3
/** ZAEHLNUM setting for version of the contained terminal project */
#define LBS_PLUS_PROJECT_VERSION        4
/** ZAEHLNUM setting for generation date of the terminal project - must correspond to the date definition in the mask pool */
#define LBS_PLUS_PROJECT_DATE           5
/** ZAEHLNUM setting for project status */
#define LBS_PLUS_PROJECT_STATUS         6
#define LBS_PLUS_PROJECT_IDENT_NO       7
#define LBS_PLUS_PROJECT_SIGNATURE      8

#define LBS_PLUS_PROJECTOR_VERSION      9
#define LBS_PLUS_LSK_VERSION            9
#define LBS_PLUS_PROJECTOR_DATE         10
#define LBS_PLUS_LSK_DATE               10

#define LBS_PLUS_PROJECT_START          11
#define LBS_PLUS_PROJECT_VISISTART      13
#define LBS_PLUS_PROJECT_VISISIZE       14

namespace IsoAgLib {
/**
  decide which terminal type has to be uploaded
*/
enum terminalType_t {FendtVario = 1, AgrocomACT = 2, FieldstarPlus = 10, FieldstarOld = 11};

/** definition of a syncronisation tupel for IsoAgLib+ Mask */
typedef struct {
  /** value for syncronisation */
  int32_t i32_val;
  /** identification number of this sync val (ZAEHLNUM val for IsoAgLib+) */
  uint8_t ui8_ind;
} t_syncTupel;

/** definition of a LBS+ terminal mask */
typedef struct
{
  /** pointer to DEVKEY of local member which wants to send mask */
  iDevKey_c* pc_localDevKey;

  /** DEV_KEY of temrinal */
  iDevKey_c c_devKey;
  /** 7byte name string of terminal */
  uint8_t pb_termName[8];
  /**
    decide which terminal type has to be uploaded
    (FendtVario, AgrocomACT, FieldstarPlus, FieldstarOld)
  */
  terminalType_t en_terminalType;
  /**  pointer with configuration information which can ba requested with cmd code 3 */
  HUGE_MEM uint8_t* phb_configData;
  /** size of configuration data */
  uint16_t ui16_configSize;
  /** HUGE data pointer to binary image of mask */
  HUGE_MEM uint8_t* phb_maskData;
  /** size of the mask binary */
  uint32_t ui32_maskSize;
  /** amont of sync tupels for this specific terminal */
  uint8_t ui8_syncCnt;
  /**
    Array of syncronisation numbers (with 0 for all unused numbers):
    1. version number of ECU operating system
      (specific for Varioterminal; if unused set to 0)
      OR
      required version of terminal (Fieldstar LBS+)
    10. project size (created by PC-Tool; for Vario, Fieldstar and ACT)
    2. project version of menu (for all LBS+ and Fieldstar terminals)
    13. version of non-reprogrammable part of ECU software (Fieldstar OLD)
    14. main program version of ECU (Fieldstar OLD)

    3. project date
      (bit0..4 day, bit5..8 month, bit9..19 year, bit20..31 reserved to 0; for Vario and ACT)
    4. project state
      (bit0..3 64k block cnt, bit4..9 menu page cnt, bit10..29 reserved to 0,
       bit30 1==visible info available, bit31 1==joystick allowed; for Vario)
    5. ident no of menu (Vario)
    6. 32bit unsigned checksum over menue (created by PC-Tool; for Vario and ACT)
    7. version no of PC Mask Generator (for Vario and ACT)
    8. release date of PC Mask Generator (for Vario and ACT)
    9. start adress of menu image in ECU (Vario)
    11. start adress of visibility information in ECU (Vario)
    12. size of visibility information in ECU (Vario)
  */
  t_syncTupel* pt_syncArray;
} t_maskDefinition;

};

#endif

