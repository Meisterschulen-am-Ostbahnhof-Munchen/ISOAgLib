/***************************************************************************
                          devClassflexmanager_c.h  -  description
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

#ifndef DEVCLASS_FLEX_MANAGER_H
#define DEVCLASS_FLEX_MANAGER_H

#include "procdatamanager_c.h"
#include "specificrecordconfig_c.h"

/**manage process data for DEVCLASS 2 (primary soil implement)
  *@author Dipl.-Inform. Achim Spangler
*/
class DevClassFlexManager_c : public ProcDataManager_c  {
public:
  DevClassFlexManager_c(DefaultRecordConfig_c* rpc_defaultRecordConfig = NULL, uint16_t rui16_eepromOffsetAdr = 0xFFFF);
  DevClassFlexManager_c(const DevClassFlexManager_c& rrefc_src);
  virtual ~DevClassFlexManager_c();
  void init(DefaultRecordConfig_c* rpc_defaultRecordConfig);
  bool operator==(const DevClassFlexManager_c& rrefc_comp)const{return (c_remoteDevKey == rrefc_comp.c_remoteDevKey)?true:false;};
  bool operator!=(const DevClassFlexManager_c& rrefc_comp)const{return (c_remoteDevKey != rrefc_comp.c_remoteDevKey)?true:false;};
  bool operator<(const DevClassFlexManager_c& rrefc_comp)const{return (c_remoteDevKey < rrefc_comp.c_remoteDevKey)?true:false;};

  bool operator==(const IsoAgLib::iDevKey_c& rc_comp)const{return (c_remoteDevKey == rc_comp)?true:false;};
  bool operator!=(const IsoAgLib::iDevKey_c& rc_comp)const{return (c_remoteDevKey != rc_comp)?true:false;};
  bool operator<(const IsoAgLib::iDevKey_c& rc_comp)const{return (c_remoteDevKey < rc_comp)?true:false;};
  /**
    activate with creating the needed ProcessData
    @param rpc_monitor pointer to member_item of data delivering member
    @param rpc_localDevKey pointer to local member DEVKEY for sending of commands
  */
  void activate(IsoAgLib::iDINItem_c* rpc_monitor, IsoAgLib::iDevKey_c* rpc_localDevKey);
  /** deactivate with deleting the created ProcessDatas */
  void deactivate();
  /**
    write header with literal description of
    process data
  */
  virtual void writeHeader();
  /** write informations of according member (DEV CLASS/INST, name)
      and all remote process data of ppc_data
      to RS232
  */
  virtual void writeData();


private:
  /**
    check if working width should be recorded
    @return true --> record working width
  */
  bool recordWidth()const
  {return ((pc_monitor->devClass() != 11)||(configField.transportDummyWidth == 1))?true:false;};
  /**
    check if a single measurement request should be sent on every local
    value access
  */
  bool sendRequest()const{return (configField.requestSingleVals == 1)?true:false;};
  /** NEW: 14-10-2004
    check if a single measurement request should be sent on every local
    value access.
    @param rb_procInd index of process data to check
  */
  bool sendRequest( uint8_t rb_procInd )const;
  /**
    deliver the DEVCLASS for time, dist, area values
  */
  uint8_t timeDistDevClass()const{return configField.timeDistDevClass;};

  /** EEPROM adress of offset values */
  uint16_t ui16_eepromOffsetAdr;

  /** internal data string */
  IsoAgLib::iProcDataRemote_c *_pc_proc;

  struct {
    uint8_t timeWert : 4;
    uint8_t recordAsDevClass : 4;

    uint8_t applrateXHa : 1;
    uint8_t applrateXMin : 1;
    uint8_t applrateX : 1;
    uint8_t applrateTankX : 1;
    uint8_t transportDummyWidth : 1;
    uint8_t recordWorkDist : 1;
    uint8_t fieldstarSend : 1;
    uint8_t useMeasureProgs : 1;

    uint8_t workWertInst : 8;

    uint8_t posApplrateX : 4;
    uint8_t timeDistDevClass : 4;

    uint8_t requestSingleVals : 1;
  } configField;

  std::basic_string<char> c_flexibleHeader;
  DefaultRecordConfig_c* pc_defaultRecordConfig;
};

#endif
