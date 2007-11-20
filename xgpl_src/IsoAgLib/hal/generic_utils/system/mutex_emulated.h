/***************************************************************************
                          mutex:emulated.h - definition of Hardware Abstraction
                                     Layer for central system functions
                                     for debug/simulation
                                     system on PC
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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


#ifndef _HAL_MUTEX_EMULATED_H_
#define _HAL_MUTEX_EMULATED_H_


namespace HAL
{

#ifdef USE_MUTUAL_EXCLUSION
/**
 * Class for a mutual exclusive use of a resource
 * @short Class for a mutual exclusive use of a resource.
*/

class ExclusiveAccess_c {
public:

  ExclusiveAccess_c() {// init the Mutex
                        mi_exclusiveAccess = 0; //unlocked
                      }

  ~ExclusiveAccess_c() {// destroy the Mutex
                            }

  int  waitAcquireAccess() {
                              while(mi_exclusiveAccess == 1) //if locked, wait until unlocked
                              {
                                wdTriggern();
                               }
                              // we reachonly here, if unlocked
                              mi_exclusiveAccess = 1;

                              return 0;
                            }

  int  tryAcquireAccess(){
                            if(mi_exclusiveAccess == 0) //if unlocked , lock it
                            {
                                mi_exclusiveAccess = 1;
                                return 0;
                            }
                            else //if locked return
                             return 1; //was locked
}

  int releaseAccess(){ mi_exclusiveAccess = 0; return 0;}


private:

  volatile int mi_exclusiveAccess; // 1 = locked, 0 = unlocked

};//end class ExclusiveAccess_c

#endif





}

#endif


