/***************************************************************************
                          igps_c.h  - special class to handle GPS
                                         informations
                             -------------------
    begin                : Mon Jul 30 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef IGPS_H
#define IGPS_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/gps_c.h"


// Begin Namespace IsoAgLib::iProcess_c
namespace IsoAgLib {

/**
  central class to manage GPS informations which
  can be received as Partner Process Data from Fieldstar
  or as Service to Broadcast Process Data according to
  LBS+: longitude [degree], latitude [degree], altitude [m],
        hdop [%], receiveMode [noGps, gps, dgps],
        hour, minute, second (if no GPS time available Base_c time is
        delivered)

  @author Dipl.-Inform. Achim Spangler
*/
class iGPS_c : private __IsoAgLib::GPS_c  {
public:
  enum rec_mode_t { noGps = 0, gps = 1, dgps = 2, rtkgps = 3};
  /** initialisation for GPS_c  */
  void init( void ) { GPS_c::init();};

  /** deliver longitude [degree] as float */
  float longitude()const{return GPS_c::longitude();};
  /** deliver latitude [degree] as float */
  float latitude()const{return GPS_c::latitude();};
  /** deliver altitude [m] as float */
  float altitude()const{return GPS_c::altitude();};
  /** deliver hdop [%] as float */
  float hdop()const{return GPS_c::hdop();};
  /** deliver receiver mode: noGps, gps, dgps */
  rec_mode_t rec_mode()const{return (rec_mode_t)(GPS_c::rec_mode());};

  /** receive time hour */
  uint8_t hour()const{return GPS_c::hour();};
  /** receive time minute */
  uint8_t minute()const{return GPS_c::minute();};
  /** receive time second */
  uint8_t second()const{return GPS_c::second();};
 private:
  /** allow getIGpsInstance() access to shielded base class.
      otherwise __IsoAgLib::getGpsInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iGPS_c& getIGpsInstance( uint8_t rui8_instance );
  #else
  friend iGPS_c& getIGpsInstance( void );
  #endif
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique GPS_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iGPS_c& getIGpsInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iGPS_c&>(__IsoAgLib::getGpsInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique GPS_c singleton instance */
  inline iGPS_c& getIGpsInstance( void )
  { return static_cast<iGPS_c&>(__IsoAgLib::getGpsInstance());};
#endif

} // end of namespace IsoAgLib::iProcess_c
#endif
