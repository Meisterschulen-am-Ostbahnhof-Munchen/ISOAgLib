/***************************************************************************
                          ibasetypes.h  - types for base data handling
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#ifndef IBASE_TYPES_H
#define IBASE_TYPES_H
/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
//#include <Application_Config/isoaglib_config.h>
#include <IsoAgLib/typedef.h>

namespace IsoAgLib {
  #ifdef USE_ISO_11783
  /** use an enum typedef for the different states of ISO flags */
  typedef enum {
    IsoInactive = 0,    ///< Flag is marked explicit as not active
    IsoActive = 1,      ///< corresponding function is active
    IsoError = 2,       ///< correspnding function is in error state, and thus not useable
    IsoNotAvailable = 3 ///< corresponding function is not available/not implemented in service provider
  } IsoActiveFlag_t;

  /** GPS receive mode */
  typedef enum {
    IsoNoGps            = 0,
    IsoGPS              = 1,
    IsoDGPS             = 2,
    IsoPreciseGps       = 3,
    IsoRtkInt           = 4,
    IsoRtkFloat         = 5,
    IsoEstimatedModePos = 6,
    IsoManualInputPos   = 7,
    IsoSimulatePos      = 8,
    IsoErrorPos         = 14,
    IsoNullPos          = 15
  } IsoGpsRecMode_t ;
  #endif

  /** define the main groups of base data types */
  typedef enum {
    BaseDataNothing  = 0x00, ///< select no base data type
    BaseDataGroup1   = 0x01, ///< real and gear based speed and distance; for ISO: also key_switch_state and max power time
    BaseDataGroup2   = 0x02, ///< front and rear PTO, engine RPM, front and rear hitch information
    BaseDataCalendar = 0x04, ///< calendar data
    BaseDataGroup3   = 0x08, ///< Fendt Vario DIN 9684 specific detailet read link force information
    BaseDataFuel     = 0x10, ///< Fendt Vario DIN 9684 specific fuel consumption and temp info
    BaseDataGps      = 0x20, ///< ISO NMEA 2000 GPS information
    BaseDataGroup12  = 0x03, ///< Combination of Groups 1+2
    BaseDataGroup123 = 0x0B, ///< Combination of Groups 1+2+3
    BaseDataGroup12Cal = 0x07, ///< Combination of Goups 1+2 with Calendar
    BaseDataGroups12CalGps = 0x27, ///< Combination of Groups 1+2 with Calendar and NMEA 2000 GPS
    BaseDataGroupAll = 0x3F  ///< Combination of all possible base data
  } BaseDataGroup_t;

}; // end namespace


#endif

