/***************************************************************************
                          dinstopmanager_c.h - object which helps managing
                                            DIN9684 stop commands
                             -------------------
    begin                : Fri Apr 07 2000
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
#ifndef STOP_MANAGER_H
#define STOP_MANAGER_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  manage running stop commands;
  f.e. to send stop release exactly 5 times
  per previous stop command
  @author Dipl.-Inform. Achim Spangler
*/
class DINStopManager_c {
public:
  /**
    constructor for DINStopManager_c instance which sets item to inactive
    @param rui8_empf optional number of stopped member
    @param rb_toStop optional stop state
  */
  DINStopManager_c(uint8_t rui8_empf = 0xF, bool rb_toStop = false);
  /**
    copy constructor
    @param rrefc_src source DINStopManager_c instance
  */
  DINStopManager_c(const DINStopManager_c& rrefc_src);
  /** default destructor which has nothing to do */
  ~DINStopManager_c();
  /**
    check if this stop item is active
    @return true -> this DINStopManager_c is active
  */
  bool active(){return (toStop() < 6);};
  /**
    init a stop state with rb_toStop == true; else init stopRelease
    @param rui8_empf number of member which should be stopped
    @param rb_toStop state of stop command (on or off)
  */
  void setStop(uint8_t rui8_empf, bool rb_toStop = true)
  {
    setEmpf(rui8_empf);
    if (rb_toStop) setToStop(0);
    else setToStop(1);
  };
  /** notify a sent stop command - important for stopRelease counting */
  void notifySend() { if (toStop() > 0) setToStop(toStop()+1);  };
  /**
    deliver actual stop command
    @return command to use for Scheduler_c SystemMgmt_c msg
  */
  uint8_t command()const{return ((toStop() == 0)? 0xF : 0);};
  /**
    deliver target empf for stop command
    @return number of member which is addressed by this stop command
  */
  uint8_t empf()const{return data.ui8_empf;};

private:
  // private methods
  /**
    set b_toStop value within bitfield
    @param rb_toStop value to set for data.b_toStop
  */
  void setToStop(uint8_t rb_toStop) {data.b_toStop = rb_toStop;};
  /**
    get b_toStop value
    @return b_toStop value
  */
  uint8_t toStop()const{return data.b_toStop;};
  /**
    set ui8_empf address
    @param rui8_empf emf value
  */
  void setEmpf(uint8_t rui8_empf){data.ui8_empf = rui8_empf;};

  // Private attributes
  struct {
    /**
      <0xF stop comman for specific member
      ==0xF global system stop command
    */
    uint8_t ui8_empf : 4;
    /**
      0== stop member ui8_empf;
      [1;5] send release command;
      >5 this item is inactive -> can delte in vector
    */
    uint8_t b_toStop : 4;
  } data ;
};

}
#endif





