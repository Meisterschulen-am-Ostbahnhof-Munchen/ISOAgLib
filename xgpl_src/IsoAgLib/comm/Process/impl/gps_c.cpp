/***************************************************************************
                          lbsGps.cc - special class to handle GPS
                                       informations
                             -------------------
    begin                : Mon Jul 30 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "gps_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Base/impl/base_c.h>
#include <IsoAgLib/comm/Process/impl/process_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>

#ifdef USE_DIN_9684
  #include <IsoAgLib/comm/SystemMgmt/DIN9684/impl/dinitem_c.h>
#endif

namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique GPS_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  GPS_c& getGpsInstance( uint8_t rui8_instance ) { return GPS_c::instance( rui8_instance );};
#else
  /** C-style function, to get access to the unique GPS_c singleton instance */
  GPS_c& getGpsInstance( void ) { return GPS_c::instance();};
#endif

static const GetyPos_c gc_fieldstarGtp(0x1, 0x3); // DIN-GTP: 0xB

/** initialise element which can't be done during construct */
void GPS_c::init()
{ // clear state of b_alreadyClosed, so that close() is called one time
  clearAlreadyClosed();
  // first register in Scheduler_c
  getSchedulerInstance4Comm().registerClient( this );

  i32_lastFilterBoxTime = 0;

  f_longitude = 0.0F;
  f_latitude = 0.0F;
  f_altitude = 0.0F;
  f_hdop = 0.0F;
  en_recMode = noGps;
  ui8_fieldstarNr = 0xFF;

  b_isGpsTime = 0;
  b_hour = b_minute = b_second = b_gpsSecond = 0;


  pc_data = &(getProcessInstance4Comm().data());

  // create filter to receive service to broadcast member messages from LBS+
  if (!getCanInstance4Comm().existFilter((uint16_t)0x7FF,(uint16_t)0x3F3))
    getCanInstance4Comm().insertFilter(*this, 0x7FF,0x3F3, false);
  if (!getCanInstance4Comm().existFilter((uint16_t)0x700,(uint16_t)0x500))
    getCanInstance4Comm().insertFilter(*this, 0x700,0x500, true);
}
/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void GPS_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/** default destructor which has nothing to do */
GPS_c::~GPS_c(){
  close();
}


/**
  perform periodical actions
  @return true -> all planned activities performed in allowed time
*/
bool GPS_c::timeEvent( void )
{
  if ( Scheduler_c::getAvailableExecTime() == 0 ) return false;
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();

  #ifdef USE_DIN_9684
  // if local active member exist - check every second if
  // filters for targeted or partner process data should be created
  if (((i32_time - i32_lastFilterBoxTime) > 3000)
   &&(getSystemMgmtInstance4Comm().existActiveLocalDinMember())
     )
  {
    DINMonitor_c& c_din_monitor = getDinMonitorInstance4Comm();
    CANIO_c& c_can_io = getCanInstance4Comm();
    i32_lastFilterBoxTime = i32_time;
    uint16_t ui16_filter;
    if (c_din_monitor.existDinMemberGtp(gc_fieldstarGtp))
    {
      uint8_t ui8_fieldstarNrTemp = c_din_monitor.dinMemberGtp(gc_fieldstarGtp).nr();
      if (ui8_fieldstarNr != ui8_fieldstarNrTemp)
      { // fieldstar changed number -> delete first old filter
        ui16_filter = ( 0x500 | ui8_fieldstarNr);
        if ((ui8_fieldstarNr != 0xFF) &&(c_can_io.existFilter((uint16_t)0x70F,ui16_filter, Ident_c::StandardIdent)))
        {
          c_can_io.deleteFilter((uint16_t)0x70F,ui16_filter, Ident_c::StandardIdent);
        }

        // set actual fieldstar no
        ui8_fieldstarNr = ui8_fieldstarNrTemp;
        // create receive filters to receive messages from fieldstar terminal
        ui16_filter = (0x500 | ui8_fieldstarNr);
        if (!c_can_io.existFilter((uint16_t)0x70F,ui16_filter, Ident_c::StandardIdent))
            c_can_io.insertFilter(*this, (uint16_t)0x70F,ui16_filter, true, Ident_c::StandardIdent);
      } // end of create new filter on fieldstar nr change
    } // end fieldstar exist as member
    else
    { // no fieldstar as member exist
      if (ui8_fieldstarNr != 0xFF)
      {  // previously fieldstar existed -> filter was created
        ui16_filter = (0x500 | ui8_fieldstarNr);
        if (c_can_io.existFilter((uint16_t)0x70F,ui16_filter, Ident_c::StandardIdent))
          c_can_io.deleteFilter((uint16_t)0x70F,ui16_filter, Ident_c::StandardIdent);
        b_isGpsTime = 0;
        ui8_fieldstarNr = 0xFF;
      } // end delete old unused filter
    } // end else
  } // end filter check every second
  #endif

  return true;
};

/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
*/
bool GPS_c::processMsg(){
  int32_t i32_tempVal = data().dataRawCmdLong();
  #ifdef USE_BASE
	Base_c& c_lbsBase = getBaseInstance4Comm();
	#endif
  bool b_result = false;

  #ifdef USE_DIN_9684
  if ( (data().pri() == 5)
    && (data().lis() == 3)
    && (data().wert() == 5)
    && (data().gety() <= 1)
     )
  { // partner group process data from fieldstar with GPS information
    b_result = true;
    switch (data().inst())
    {
      case 0: // longitude
        // degree
        f_longitude = (float)(i32_tempVal >> 24);
        // minute -> / 60
        f_longitude += (static_cast<float>((i32_tempVal >> 16) & 0xFF)/60.0F);
        // 1/1.0000 minute -> x / 60.0000
        f_longitude += (static_cast<float>(i32_tempVal & 0x7FFF)/600000.0F);
        // sign
        if ((i32_tempVal & 0x8000) > 0)f_longitude *= -1.0F;
        break;
      case 1: // latitude
        // degree
        f_latitude = (float)(i32_tempVal >> 24);
        // minute -> / 60
        f_latitude += (static_cast<float>((i32_tempVal >> 16) & 0xFF)/60.0F);
        // 1/10000 minute -> x / 600000
        f_latitude += (static_cast<float>(i32_tempVal & 0x7FFF)/600000.0F);
        // sign
        if ((i32_tempVal & 0x8000) > 0)f_latitude *= -1.0F;
        break;
      case 2: // altitude
        int2Float(&i32_tempVal, &f_altitude);
        break;
      case 3: // pos-fix-time
        // first check if new second is different from last
        if (b_gpsSecond != (i32_tempVal & 0xFF))
        { // change of second -> valid time
          b_isGpsTime = 3;
          b_second = (i32_tempVal & 0xFF);
          b_gpsSecond = b_second;
          b_minute = ((i32_tempVal >> 8) & 0xFF);
          // add +1h because GPS time is in UTC
          b_hour = ((i32_tempVal >> 16) & 0xFF) + 1;
					#ifdef USE_BASE
          // consider summer time
          uint8_t b_day = c_lbsBase.day(),
          b_month = c_lbsBase.month();
          if ( ( (b_month > 3) && (b_month < 10) )
            || ( (b_month == 10) && (b_day < 25) )
             )
          { // its summertime
            b_hour += 1;
          }
          else
          { // use in march and october after day 25 the base_c.hour becuse it could be
            // summer or winter time (fieldstar GPS data doesn't tell real day of week
            // -> no exact test possible) -> base_c.hour is more reliyable
            if ( (b_day >= 25) && ( (b_month == 3)  || (b_month == 10) ) )
            {
              b_hour = c_lbsBase.hour();
            }
          }
					#endif
        }
        else
        { // no second change -> decrement validity of time
          if (b_isGpsTime > 0) b_isGpsTime -= 1;
        }
				#ifdef USE_BASE
        if ( (b_isGpsTime == 0) || (b_hour > 24) || (b_minute > 60) || (b_second > 60) )
        { // if time note valid because of no sec change or because of invalid settings use
          // Base_c
          b_hour = c_lbsBase.hour();
          b_minute = c_lbsBase.minute();
          b_second = c_lbsBase.second();
        }
        #endif
        break;
      case 5: // hdop
        int2Float(&i32_tempVal, &f_hdop);
        break;
      case 7: // receiver-mode
        if (((i32_tempVal >> 8) & 0xFF) == 0x2)en_recMode = gps;
        else en_recMode = dgps;
        break;
    }
    if (en_recMode == noGps) en_recMode = gps;
  }
  else
  { // no partner process data from fieldstar with GPS data
    if ( (data().pri() == 3)
      && (data().send() == 3)
      && (data().lis() == 0)
      && (data().gety() == 0)
      && (data().empf() == 0xF)
       )
    { // LBS+ GPS information as process data msg from service to broadcast member
      b_result = true;
      switch ((data().wert() << 4) | data().inst())
      {
        case 0x80: // latitude
          f_latitude = data().dataFloat();
          break;
        case 0x81: // longitude
          f_longitude = data().dataFloat();
          break;
        case 0x82: // altitude
          f_altitude = data().dataFloat();
          break;
        case 0x90: // receiver-mode
          switch (i32_tempVal)
          {
            case 0:
              en_recMode = noGps;
              break;
            case 1:
              en_recMode = gps;
              break;
            case 2:
              en_recMode = dgps;
              break;
          }
          break;
        case 0x92: // hdop
          f_hdop = data().dataFloat();
          break;
        case 0xA0: // pos-fix-time
          // first check if new second is different from last
          if (b_gpsSecond != (i32_tempVal % 60))
          { // change of second -> valid time
            b_isGpsTime = 3;
            b_second = (i32_tempVal % 60);
            b_gpsSecond = b_second;
            b_minute = ((i32_tempVal / 60) % 60);
            b_hour = (i32_tempVal / 3600);

            // consider summer time
						#ifdef USE_BASE
            uint8_t b_day = c_lbsBase.day(),
            b_month = c_lbsBase.month();
            if ( ( (b_month > 3) && (b_month < 10) )
              || ( (b_month == 10) && (b_day < 25) )
               )
            { // its summertime
              b_hour += 1;
            }
            else
            { // use in march and october after day 25 the base_c.hour becuse it could be
              // summer or winter time (fieldstar GPS data doesn't tell real day of week
              // -> no exact test possible) -> base_c.hour is more reliyable
              if ( (b_day >= 25) && ( (b_month == 3)  || (b_month == 10) ) )
              {
                b_hour = c_lbsBase.hour();
              }
            }
            #endif
          }
          else
          { // no second change -> no valid time
            if (b_isGpsTime > 0) b_isGpsTime -= 1;
          }
          if ( (b_isGpsTime == 0) || (b_hour > 24) || (b_minute > 60) || (b_second > 60) )
          { // if time note valid because of no sec change or because of invalid settings use
            // Base_c
						#ifdef USE_BASE
            b_hour = c_lbsBase.hour();
            b_minute = c_lbsBase.minute();
            b_second = c_lbsBase.second();
            #endif
          }
          break;
      }
      if (en_recMode == noGps) en_recMode = gps;
    } // sercvice process data from GPS service
    else
    { // another type of process data -> let Process_c process it
      getProcessInstance4Comm().processMsg();
    }
  } // else for no partner process data from fieldstar with GPS data
  #endif // USE_DIN_9684

  return b_result;
}

/**
  deliver reference to data pkg as reference to CANPkgExt
  to implement the base virtual function correct
*/
CANPkgExt_c& GPS_c::dataBase()
{
  return *pc_data;
}

} // end of namespace __IsoAgLib
