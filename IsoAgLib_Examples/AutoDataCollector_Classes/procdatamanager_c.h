/***************************************************************************
                          procdatamanager_c.h  -  description
                             -------------------
    begin                : Sun Jul 16 2000
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

#ifndef PROCDATAMANAGER_H
#define PROCDATAMANAGER_H

#include <IsoAgLib/comm/Process/Remote/Std/iprocdataremote_c.h>
#include <IsoAgLib/comm/SystemMgmt/iidentitem_c.h>

// using namespace IsoAgLib;

/**Object for basic DEVCLASS dependent handling
of process data for writing by Task Controller
  *@author Dipl.-Inform. Achim Spangler
  */
class ProcDataManager_c {
public:
  ProcDataManager_c(uint8_t rui8_cnt = 0);
  ProcDataManager_c(const ProcDataManager_c& rrefc_src);
  virtual ~ProcDataManager_c();
  /**
    activate with creating the needed ProcessData
    @param rpc_monitor pointer to member_item of data delivering member
  */

  //! This example is NOT YET converted to IsoAgLib 2.0
  //! so please don't wonder why it doesn't compile
#warning "This example is NOT YET converted to IsoAgLib 2.0 so please don't wonder why it doesn't compile."

  void activate(IsoAgLib::iDINItem_c* rpc_monitor);
  /** deactivate with deleting the created ProcessDatas */
  void deactivate();
  /** test if this instance is activated */
  bool activated()const{return b_activated;};

  /** write informations of according member (DEV CLASS/INST, name)
      and all remote process data of pc_data
      to RS232
  */
  virtual void writeData();
  /** start all (default) or a specific remote process data */
  void start(int ri_ind = -1);
  /** stop all (default) or a specific remote process data */
  void stop(int ri_ind = -1);
  /** reset val of all (default) or a specific remote process data */
  void resetVal(int ri_ind = -1);
  /** write header of all data columns */
  virtual void writeHeader();
  /** perform periodic actions */
  virtual void timeEvent(long rl_time = -1);
  /** set count of process data */
  void set_proc_cnt(uint8_t rui8_cnt) {ui8_cnt = rui8_cnt;};
  /** get count of process data */
  uint8_t proc_cnt()const {return ui8_cnt;};
  /**
    deliver remote isoName of this recording set
    @return c_remoteISOName
  */
  const IsoAgLib::iISOName_c& isoName() const { return c_remoteISOName;};
protected: // Protected attributes
  /** set activated mode of this instance  */
  void setActivated(bool rb_state = true){b_activated = rb_state;};
  /** pointer to Member_Item of which the
      remote data is recorded */
  IsoAgLib::iDINItem_c *pc_monitor;
  /** pointer to array of pointers to remote process data entries */
  IsoAgLib::iProcDataRemote_c *pc_data;
  /** flag to control if this instance is activated */
  bool b_activated;
  /** amount of managed process data */
  uint8_t ui8_cnt;
  /** isoName of remote item */
  IsoAgLib::iISOName_c c_remoteISOName;
};

#endif
