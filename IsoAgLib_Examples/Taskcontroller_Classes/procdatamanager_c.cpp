/***************************************************************************
                          procdatamanager.cc  -  description
                             -------------------
    begin                : Sun Jul 16 2000
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

#include "procdatamanager_c.h"
#include <supplementary_driver/driver/rs232/irs232io_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>
#include <IsoAgLib/comm/Process/proc_c.h>
#include <IsoAgLib/comm/Process/Remote/Std/iprocdataremote_c.h>

ProcDataManager_c::ProcDataManager_c( uint8_t rui8_cnt)
{
  ui8_cnt = rui8_cnt;
  pc_data = NULL;
  b_activated = false;
	c_remoteGtp.setUnspecified();
}
ProcDataManager_c::~ProcDataManager_c()
{
}

ProcDataManager_c::ProcDataManager_c(const ProcDataManager_c& rrefc_src)
{
  pc_monitor = rrefc_src.pc_monitor;
  pc_data = rrefc_src.pc_data;
  ui8_cnt = rrefc_src.ui8_cnt;
	c_remoteGtp = rrefc_src.c_remoteGtp;
	// don't copy actation state - start with deactivated
  b_activated = false;
}

/**
  activate with creating the needed ProcessData
  @param rpc_monitor pointer to member_item of data delivering member
*/
void ProcDataManager_c::activate(IsoAgLib::iDINItem_c* rpc_monitor)
{
  pc_monitor = rpc_monitor;
	c_remoteGtp = rpc_monitor->gtp();
  setActivated(true);
}

/** deactivate with deleting the created ProcessDatas */
void ProcDataManager_c::deactivate()
{
  setActivated(false);
}

/** perform periodic actions */
void ProcDataManager_c::timeEvent(long /* rl_time */ )
{return;}

/** write informations of according member (GETY, POS, name)
    and all remote process data of pc_data
    to RS232
*/
void ProcDataManager_c::writeData()
{
  char temp_name[15];
	IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();
  memmove(temp_name, (char*)(pc_monitor->name()), 8);
  memmove(temp_name+8, ":\0\0\0", 3);
  c_rs232 << (int16_t)(pc_monitor->gtp().getGety()) << "; " << (int16_t)(pc_monitor->gtp().getPos())
          << "; " << (int16_t)(pc_monitor->nr()) << "; " << temp_name;
  for (uint8_t b_ind = 0; b_ind < ui8_cnt; b_ind++)
  {
    c_rs232 << "; " << (pc_data[b_ind]).prog().val();
  }
}
/** start all (default) or a specific remote process data */
void ProcDataManager_c::start(int ri_ind){
  if ((ri_ind >= 0) && ( !((pc_data[ri_ind]).prog().started()) ))
  {
		(pc_data[ri_ind]).prog().addSubprog(IsoAgLib::Proc_c::TimeProp, 1000);
		(pc_data[ri_ind]).prog().start(IsoAgLib::Proc_c::Target, Proc_c::TimeProp, Proc_c::DoVal);
  }
  else
  {
    for (uint8_t b_ind = 0; b_ind < ui8_cnt; b_ind++)
    {
      if (!((pc_data[b_ind]).prog().started()))
      {
        (pc_data[b_ind]).prog().addSubprog(IsoAgLib::Proc_c::TimeProp, 1000);
		    (pc_data[b_ind]).prog().start(IsoAgLib::Proc_c::Target, Proc_c::TimeProp, Proc_c::DoVal);
		  }
		}
  }
}

/** stop all (default) or a specific remote process data */
void ProcDataManager_c::stop(int ri_ind){
  if ((ri_ind >= 0) && ( (pc_data[ri_ind]).prog().started() ))
  {
		(pc_data[ri_ind]).prog().stop();
  }
  else
  {
    for (uint8_t b_ind = 0; b_ind < ui8_cnt; b_ind++)
    {
      if ((pc_data[b_ind]).prog().started())
      {
		    (pc_data[b_ind]).prog().stop();
		  }
		}
  }
}

/** reset val of all (default) or a specific remote process data */
void ProcDataManager_c::resetVal(int ri_ind){
  if (ri_ind >= 0)
  {
		(pc_data[ri_ind]).prog().resetVal();
  }
  else
  {
    for (uint8_t b_ind = 0; b_ind < ui8_cnt; b_ind++)
    {
      (pc_data[b_ind]).prog().resetVal();
		}
  }
}

/** write header of all data columns */
void ProcDataManager_c::writeHeader(){
	IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();
  c_rs232 << "GETY; POS; Adresse; Name";
  for (uint8_t b_ind = 0; b_ind < ui8_cnt; b_ind++)
  { // write for each process data "LIS:GETY:WERT:INST:ZAEHLNUM"
    c_rs232 << ";" << (int16_t)(pc_data[b_ind]).lis() << ":"
              << (int16_t)(pc_data[b_ind]).gety() << ":"
              << (int16_t)(pc_data[b_ind]).wert() << ":"
              << (int16_t)(pc_data[b_ind]).inst() << ":"
              << (int16_t)(pc_data[b_ind]).zaehlnum() ;
  }
}
