/***************************************************************************
                          specificrecordconfig_c.h  -  description
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

#ifndef SPECIFIC_RECORD_CONFIG_H
#define SPECIFIC_RECORD_CONFIG_H

#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/comm/SystemMgmt/isystemmgmt_c.h>
#include "defaultrecordconfig_c.h"

#define TAG_NAME_LEN 8
#define TAG_VALUE_LEN 12
#define TAG_TUPLE_LEN 20

/**
  Read default Recording Defaults from EEPROM
  and provide them for simple access
  during DevClass_Flex_Manager initialisation for specific
  device
  *@author Dipl.-Inform. Achim Spangler
*/
class SpecificRecordConfig_c {
public:
  /**
    constructor which initialises all data and can call init to read
    config data from EEPROM (if eeprom adress is given)
    @param rui16_eepromAdr adress in EEPROM where config data is stored
  */
  SpecificRecordConfig_c( uint16_t rui16_eepromAdr = 0, DefaultRecordConfig_c* rpc_defaultConfig = NULL, IsoAgLib::iDINItem_c* rpc_memberItem = NULL);
  /**
    copy Constructor
    @param rrefc_src reference to source instance
  */
  SpecificRecordConfig_c(const SpecificRecordConfig_c& rrefc_src);
  /**
    destructor
  */
  ~SpecificRecordConfig_c();
  /**
    initialise with reading the config data from EEPROM in flags
    @param rui16_eepromAdr adress in EEPROM where config data begins
    @return true-> parameters valid
  */
  bool init( uint16_t rui16_eepromAdr,
            DefaultRecordConfig_c* rpc_defaultConfig, IsoAgLib::iDINItem_c* rpc_memberItem);

  /**
    deliver WERT of process data for time informations
    @return wert 0x9 or 0xA for time data
  */
  uint8_t timeWert() const {return ui8_timeWert;};
  /**
    deliver WERT/INST of process data for working state
    @return WERT/INST of process data for working state
  */
  uint8_t workWertInst() const {return ui8_workWertInst;};
  /**
    deliver data intensity for recording of application rate
    (4:seeder, 5:fertilizer, 6:spreader, 7,8,9:harvest, 10:rain) :
    Bit-Or-Combination of:
    1: x/ha
    2: x/min (not for seeder)
    4: x
    8: tank_x
    @return recording configuration
  */
  uint8_t applrateRecording() const {return ui8_applrateRecording;};
  /**
    check if x/ha should be recorded
    @return true-> record apllication rate per ha
  */
  bool applrateXHa()const {return ((ui8_applrateRecording & 1) != 0)?true:false;};
  /**
    check if x/min should be recorded
    @return true-> record apllication rate per minute
  */
  bool applrateXMin()const {return ((ui8_applrateRecording & 2) != 0)?true:false;};
  /**
    check if x (total) should be recorded
    @return true-> record apllication (total)
  */
  bool applrateX()const {return ((ui8_applrateRecording & 4) != 0)?true:false;};
  /**
    check if x in tank should be recorded
    @return true-> record amount of x in tank
  */
  bool applrateTankX()const {return ((ui8_applrateRecording & 8) != 0)?true:false;};
  /**
    deliver information whether diagnose dummy working width for
    transport should be recorded
    @return true -> for transport the dummy width 0 must be recorded
  */
  bool transportDummyWidth() const {return b_transportDummyWidth;};
  /**
    deliver information whether working distance should be recorded for
    transport
    @return true -> for transport the working distance should be recorded
  */
  bool transportWorkDist() const {return b_transportWorkDist;};
  /**
    deliver information whether measure programs should be used for
    this device
    @return true-> start measuring progs, false-> request everytime a single value
  */
  bool useMeasureProgs() const {return (ui8_useMeasureProgs == 3)?true:false;};
  /**
    deliver information whether measure programs should be used for
    this device
    @return true-> start measuring progs, false-> request everytime a single value
  */
  bool sendRequest() const {return (ui8_useMeasureProgs == 2)?true:false;};
  /** some creative devices doesn't send process data on their own
      -> Fieldstar ComUnit and Terminal sends data for them
      @return true-> normal proc data sent by ComUnit, time_dist sent by Fieldstar Terminal
              false -> actual device send data on its own (default)
  */
  bool fieldstarSend() const {return b_fieldstarSend;};
  /**
    some ECU send time, dist, area values with DEVCLASS 0
    @return specific config for this devClass
  */
  uint8_t timeDistDevClass()const{return ui8_timeDistDevClass;};
  /**
    some devices has a ECU which is implemented for another DEVCLASS than used;
        e.g. TeeJet ECU used for fertiilzer instead of spreader
    @return DEV_KEY which should be used for storing information
  */
  const IsoAgLib::iDevKey_c& recordAsDevKey() const {return c_recordAsDevKey;};
  /**
    deliver the amount of specific process data for this device
    @return amount of specific process data for this device in EEPROM
  */
  uint8_t procDataCnt() const {return ui8_procCnt;};
  /**
    deliver the wert_inst of the rui8_ind'th specific process data of
    this device
    @param rui8_ind number of the wanted specific process data
    @return wert_inst or -1 if proc data wasn't found
  */
  int16_t procDataIndWertinst(uint8_t rui8_ind);
  /**
    deliver the header of the rui8_ind'th specific process data of
    this device
    @param rui8_ind number of the wanted specific process data
    @return pointer to header or NULL if proc data wasn't found
  */
  const uint8_t* procDataIndHeader(uint8_t rui8_ind);
  /**
    deliver the LIS of the rui8_ind'th specific process data of
    this device (default is 0)
    @param rui8_ind number of the wanted specific process data
    @return LIS or the default value 0 if proc data wasn't found
  */
  uint8_t procDataIndLis(uint8_t rui8_ind);
  /**
    deliver the PRI of the rui8_ind'th specific process data of
    this device (default is 2)
    @param rui8_ind number of the wanted specific process data
    @return PRI or the default value 2 if proc data wasn't found
  */
  uint8_t procDataIndPri(uint8_t rui8_ind);

private:
  /**
    private function to read the ident data for one device from EEPROM
    during search
  */
  void readIdentData();

  /**
    utility function to get a tag value for a proc data info
  */
  bool getProcIndString(uint8_t rui8_ind, const char* rpui8_tagName);
  /**
    utility function to find data for a proc data info
  */
  bool findProcInd(int ri_ind);
  /**
    deliver the search end in EEPROM belonging to the
    actual device
  */
  uint16_t get_eeprom_search_end() const {return (ui16_eepromAdr + ui8_configDataCnt * TAG_TUPLE_LEN);};

  /** Startadress of config data in EEPROM */
  uint16_t ui16_eepromAdr;
  /** start adress of specific process data info in EEPROM */
  uint16_t ui16_procEepromAdr;
  uint16_t ui16_cachedProcEepromAdr;
  int16_t i16_cachedProcInd;
  uint8_t pui8_cachedTagName[TAG_NAME_LEN + 1];
  uint8_t pui8_cachedTagData[TAG_VALUE_LEN + 1];

  // obligatory data for each data set
  /** amount of config info for this device */
  uint8_t ui8_configDataCnt;
  /** DEVCLASS of this device (0xFF as wild card) */
  uint8_t ui8_devClass;
  /** device class inst of this device (0xFF as wild card) */
  uint8_t ui8_devClassInst;
  /** amount of chars to compare (important to compare only some of
      leftmost characters */
  uint8_t ui8_nameLen;
  /** name to compare with searched device */
  uint8_t pui8_name[11];
  // optional data
  /** process data WERT for time informations: 0x9 | 0xA */
  uint8_t ui8_timeWert;
  /** WERT/INST for main working state */
  uint8_t ui8_workWertInst;
  /**
  data intensity for recording of application rate
  (4:seeder, 5:fertilizer, 6:spreader, 7,8,9:harvest, 10:rain) :
  Bit-Or-Combination of:
  1: x/ha
  2: x/min (not for seeder)
  4: x
  8: tank_x
  */
  uint8_t ui8_applrateRecording;
  /** decide if the working width should be also outputted for transport (value 0) or not */
  bool b_transportDummyWidth;
  /** decide if the working distance should be recorded for transport */
  bool b_transportWorkDist;
  /** decide if measuring progs should be used to get new values every second */
  uint8_t ui8_useMeasureProgs;
  /** some creative devices doesn't send process data on their own
      -> Fieldstar ComUnit and Terminal sends data for them
  */
  bool b_fieldstarSend;
  /**
    some ECU send time, dist, area values with DEVCLASS 0
    -> specific config for this devClass
  */
  uint8_t ui8_timeDistDevClass;

  /** some devices has a ECU which is implemented for another DEVCLASS than used;
      e.g. TeeJet ECU used for fertiilzer instead of spreader */
  IsoAgLib::iDevKey_c c_recordAsDevKey;
  /** amount of device specific process data */
  uint8_t ui8_procCnt;
};

#endif
