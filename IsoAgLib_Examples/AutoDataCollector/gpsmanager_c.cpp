/***************************************************************************
                          gpsmanager_c.cpp  -  description
                             -------------------
    begin                : Tue Jul 18 2000
    copyright            : (C) 2000 by Dipl.-Inform. Achim Spangler
    email                : spangler@tec.agrar.tu-muenchen.de
    type                 : Source
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

#include "gpsmanager_c.h"
#include <supplementary_driver/driver/rs232/irs232io_c.h>
#include <IsoAgLib/comm/Process/igps_c.h>
#include <IsoAgLib/comm/Base/itimeposgps_c.h>

GpsManager_c::GpsManager_c()
{
  b_activated = false;
}
GpsManager_c::~GpsManager_c()
{
  b_activated = false;
}

/**
  activate
*/
void GpsManager_c::activate()
{
  b_activated = true;
}

/** deactivate */
void GpsManager_c::deactivate()
{
  b_activated = false;
}


/** write header with literal description of
process data */
void GpsManager_c::writeHeader()
{
  IsoAgLib::getIrs232Instance() << "date;time;lon;E/W;lat;N/S;alt;gpsstate;";
}

/** write informations of according member (DEV CLASS/INST, name)
    and all remote process data of ppc_data
    to RS232
*/
void GpsManager_c::writeData()
{
  IsoAgLib::iGPS_c& c_gps = IsoAgLib::getIGpsInstance();
  IsoAgLib::iTimePosGPS_c& c_timePosGps = IsoAgLib::getITimePosGpsInstance();
  IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();

// Neu Ehrl: array (char position[20] ist zu klein -> Erhöhung von 20 auf 30!!
//  char position[20];
  char position[30];
  float f_temp_pos;
  char sign;

  // date
  c_rs232 << (int16_t)c_timePosGps.day() << "." << (int16_t)c_timePosGps.month() << "." << (int16_t)c_timePosGps.year();
  // time
  c_rs232 << ";" << (int)c_gps.hour() << ":" << (int)c_gps.minute() << ":" << (int)c_gps.second() << ";";
  // longitude
  f_temp_pos = c_gps.longitude();
  if (f_temp_pos < 0)
  {
    f_temp_pos *= -1;
    sign = 'W';
  }
  else
  {
    sign = 'E';
  }
  sprintf(position, "%#11.8f;%c;", f_temp_pos, sign);
  // change use float format to german
  *(strstr(position, ".")) = ',';

  c_rs232 << position;

  // latitude
  f_temp_pos = c_gps.latitude();
  if (f_temp_pos < 0)
  {
    f_temp_pos *= -1;
    sign = 'S';
  }
  else
  {
    sign = 'N';
  }
  sprintf(position, "%#11.8f;%c;", f_temp_pos, sign);
  // change use float format to german
  *(strstr(position, ".")) = ',';
  c_rs232 << position;

  // altitude
  sprintf(position, "%#8.3f;", c_gps.altitude());
  // change use float format to german
  *(strstr(position, ".")) = ',';
  c_rs232 << position;
  // hdop
//  c_rs232 << c_gps.hdop();
  // rec-mode
  switch (c_gps.rec_mode())
  {
    case IsoAgLib::iGPS_c::noGps:
      c_rs232 << "0;";
      break;
    case IsoAgLib::iGPS_c::gps:
      c_rs232 << "1;";
      break;
    case IsoAgLib::iGPS_c::dgps:
      c_rs232 << "2;";
      break;
    case IsoAgLib::iGPS_c::rtkgps:
      c_rs232 << "3;";
      break;
  }
}
