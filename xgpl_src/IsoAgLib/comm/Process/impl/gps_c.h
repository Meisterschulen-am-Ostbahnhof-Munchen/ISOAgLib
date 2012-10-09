/***************************************************************************
                             gps_c.h - special class to handle GPS
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

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#ifndef GPS_H
#define GPS_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include "processpkg_c.h"

#ifdef PC_OS_Linux
  #include <bits/stl_function.h>
#else
  #include <functional>
#endif

namespace IsoAgLib { class iGPS_c;};
// Begin Namespace IsoAgLib_GPS
namespace __IsoAgLib {
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
class GPS_c : public SINGLETON_DERIVED(GPS_c, ElementBase_c) {
private:
public:
  enum rec_mode_t { noGps = 0, gps = 1, dgps = 2, rtkgps = 3};

  /** initialisation for GPS_c  */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );

  /** default destructor which has nothing to do */
  virtual ~GPS_c();

  /** deliver longitude [degree] as float */
  float longitude()const{return f_longitude;};
  /** deliver latitude [degree] as float */
  float latitude()const{return f_latitude;};
  /** deliver altitude [m] as float */
  float altitude()const{return f_altitude;};
  /** deliver hdop [%] as float */
  float hdop()const{return f_hdop;};
  /** deliver receiver mode: noGps, gps, dgps */
  rec_mode_t rec_mode()const{return en_recMode;};

  /** receive time hour */
  uint8_t hour()const{return b_hour;};
  /** receive time minute */
  uint8_t minute()const{return b_minute;};
  /** receive time second */
  uint8_t second()const{return b_second;};
  /**
    deliver reference to data pkg
    @return reference to ProcessPkg_c which handles CAN I/O of process data
  */
  ProcessPkg_c& data(){return *pc_data;};
  /**
    deliver reference to data pkg as reference to CANPkgExt
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase();

  /**
    start processing of a process msg
    ignore all invalid messages where SEND is not of a member with claimed address,
    or where EMPF isn't valid

    possible errors:
      * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  */
  bool processMsg();

  /**
    perform periodical actions
    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );
private: // Private methods

  /**
    HIDDEN constructor for a GPS_c object instance
    NEVER instantiate a variable of type Process_c within application
    only access GPS_c via getGpsInstance() or getGpsInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  GPS_c() {};
  /**
    initialize directly after the singleton instance is created.
    this is called from singleton.h and should NOT be called from the user again.
    users please use init(...) instead.
  */
  void singletonInit() { init(); };

private: // Private attributes
  friend class SINGLETON_DERIVED(GPS_c, ElementBase_c);
  friend class IsoAgLib::iGPS_c;
  /** msg object for CAN I/O */
  ProcessPkg_c* pc_data;

  /** last timestamp with FilterBox_c check */
  int32_t i32_lastFilterBoxTime;


  /** longitude position */
  float f_longitude;
  /** latitude position */
  float f_latitude;
  /** altitude [m]*/
  float f_altitude;
  /** hdop */
  float f_hdop;
  /** receive mode: 0=no, 1=gps, 2=dgps */
  rec_mode_t en_recMode;
  /** GPS hour */
  uint8_t b_hour;
  /** GPS minute */
  uint8_t b_minute;
  /** GPS second */
  uint8_t b_second;
  /** valid PGS time: count down from 3 to zero for every received
    GPS time without change on second */
  uint8_t b_isGpsTime;
  /** member number of the fieldstar terminal */
  uint8_t ui8_fieldstarNr;
  /** second of GPS time to detect GPS time failure */
  uint8_t b_gpsSecond;
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique GPS_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  GPS_c& getGpsInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique Process_c singleton instance */
  GPS_c& getGpsInstance( void );
#endif

}
#endif
