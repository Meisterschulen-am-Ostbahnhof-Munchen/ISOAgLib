/***************************************************************************
                          gps_manager.h  -  description
                             -------------------
    begin                : Tue Jul 18 2000
    copyright            : (C) 2000 by Dipl.-Inform. Achim Spangler
    email                : spangler@tec.agrar.tu-muenchen.de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * The "LBS Library" is an object oriented program library to serve as a   *
 * software layer between application specific program and communication   *
 * protocol details. By providing simple function calls for jobs like      *
 * starting a measuring program for a process data value on a remote ECU,  *
 * the main program has not to deal with single CAN telegram formatting.   *
 * This way communication problems between ECU's which use this library    *
 * should be prevented.                                                    *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999  Dipl.-Inform. Achim Spangler                        *
 *                                                                         *
 * This library is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Lesser General Public License for more details.                         *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public License*
 * along with this library; if not, write to the Free Software Foundation, *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 ***************************************************************************/

#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <IsoAgLib/driver/system/isystem_c.h>

/**manage process data for GPS (sent by Fieldsar ComUnit
  *@author Dipl.-Inform. Achim Spangler
  */

class GpsManager_c {
public:
	GpsManager_c();
	virtual ~GpsManager_c();

  /**
    activate
  */
  void activate();
  /** deactivate */
  void deactivate();
  /** test if this instance is activated */
  bool activated()const{return b_activated;};
  /** write header with literal description of
      process data
  */
  void writeHeader();
  /** write informations of according member (GETY, POS, name)
      and all remote process data of ppc_data
      to RS232
  */
  void writeData();
private:
  /** flag to control if this instance is activated */
  bool b_activated;
};

#endif
