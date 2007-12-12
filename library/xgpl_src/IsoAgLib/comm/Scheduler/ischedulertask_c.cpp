/***************************************************************************
                          ischedulertask_c.cpp  -  header for central hardware independent
                                    object which manages all functional
                                    IsoAgLib member objects
                             -------------------
    begin                : Thu Jul 29 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "ischedulertask_c.h"


/// Begin Namespace IsoAgLib
namespace IsoAgLib {

/** Costructor for iSchedulerTask_c */

iSchedulerTask_c::iSchedulerTask_c(int ai_key):mi_key(ai_key)
{
  if (Scheduler_Task_c::checkAlreadyClosed())
  { // avoid another call
    Scheduler_Task_c::clearAlreadyClosed();

    // register in Scheduler_c to get time-events
    __IsoAgLib::getSchedulerInstance4Comm().registerClient(this);
  }
};

/** Destructor for iSchedulerTask_c */
iSchedulerTask_c:: ~iSchedulerTask_c()
{

  if (!Scheduler_Task_c::checkAlreadyClosed())
  { // avoid another call
    Scheduler_Task_c::setAlreadyClosed();

    // deregister in Scheduler_c
    __IsoAgLib::getSchedulerInstance4Comm().unregisterClient(this);
  }
};

}
