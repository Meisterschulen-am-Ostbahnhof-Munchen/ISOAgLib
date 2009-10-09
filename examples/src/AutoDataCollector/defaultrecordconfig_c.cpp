/***************************************************************************
                          defaultrecordconfig_c.cpp  -  description
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

#include "defaultrecordconfig_c.h"
#include <IsoAgLib/driver/eeprom/ieepromio_c.h>
#include <IsoAgLib/driver/system/isystem_c.h>

/**
	constructor which initialises all data and can call init to read
	config data from EEPROM (if eeprom adress is given)
	@param aui16_eepromAdr adress in EEPROM where config data is stored
*/
DefaultRecordConfig_c::DefaultRecordConfig_c(uint16_t aui16_eepromAdr)
{ // init vars
	ui16_eepromAdr = aui16_eepromAdr;

	b_accumulatedTimeDist = false;
	b_diagnoseData = false;
	ui16_recordingRate = 1000;
	ui8_timeWert = 0;
	ui8_workWertInst = 0;
	ui8_applrateRecording = 0;
	b_transportDummyWidth = false;
	b_transportWorkDist = false;
	ui8_useMeasureProgs = 0;
	b_fuelConsumption = false;
	ui8_draftForce = 0;
	ui8_deviceSpecificConfigCnt = 0;

	// check if pointers and eepromd are valid -> call init
	if (ui16_eepromAdr != 0)
	{ // valid data -> call init
		init(ui16_eepromAdr);
	}
}
/**
	copy Constructor
	@param acrc_src reference to source instance
*/
DefaultRecordConfig_c::DefaultRecordConfig_c(const DefaultRecordConfig_c& acrc_src)
{
	ui16_eepromAdr = acrc_src.ui16_eepromAdr;

	b_accumulatedTimeDist = acrc_src.b_accumulatedTimeDist;
	b_diagnoseData = acrc_src.b_diagnoseData;
	ui16_recordingRate = acrc_src.ui16_recordingRate;
	ui8_timeWert = acrc_src.ui8_timeWert;
	ui8_workWertInst = acrc_src.ui8_workWertInst;
	ui8_applrateRecording = acrc_src.ui8_applrateRecording;
	b_transportDummyWidth = acrc_src.b_transportDummyWidth;
	b_transportWorkDist = acrc_src.b_transportWorkDist;
	ui8_useMeasureProgs = acrc_src.ui8_useMeasureProgs;
	b_fuelConsumption = acrc_src.b_fuelConsumption;
	ui8_draftForce = acrc_src.ui8_draftForce;
	ui8_deviceSpecificConfigCnt = acrc_src.ui8_deviceSpecificConfigCnt;
}
/**
	destructor
*/
DefaultRecordConfig_c::~DefaultRecordConfig_c()
{
	ui16_eepromAdr = 0;
}

/**
	initialise with reading the config data from EEPROM in flags
	@param aui16_eepromAdr adress in EEPROM where config data begins
*/
void DefaultRecordConfig_c::init(uint16_t aui16_eepromAdr)
{ // set data
	ui16_eepromAdr = aui16_eepromAdr;
	IsoAgLib::iSystem_c::triggerWd();
	// exit if data not valid
	if (ui16_eepromAdr == 0)
	{ // wrong data
		return;
	}

	// now read data
	uint8_t ui8_data;
	IsoAgLib::iEepromIo_c& c_eeprom = IsoAgLib::getIeepromInstance();
	c_eeprom.setg(ui16_eepromAdr);

	c_eeprom >> ui8_data;
	b_accumulatedTimeDist = (ui8_data == 1);

	c_eeprom >> ui8_data;
	b_diagnoseData = (ui8_data == 1);

	c_eeprom >> ui16_recordingRate;

	c_eeprom >> ui8_data;
	ui8_timeWert = ui8_data;

	c_eeprom >> ui8_data;
	ui8_workWertInst = ui8_data;


	c_eeprom >> ui8_data;
	ui8_applrateRecording = ui8_data;

	c_eeprom >> ui8_data;
	b_transportDummyWidth = (ui8_data == 1);

	c_eeprom >> ui8_data;
	b_transportWorkDist = (ui8_data == 1);

	c_eeprom >> ui8_data;
	ui8_useMeasureProgs = ui8_data;

	c_eeprom >> ui8_data;
	b_fuelConsumption = (ui8_data == 1);

	c_eeprom >> ui8_data;
	ui8_draftForce = ui8_data;

	c_eeprom >> ui8_data;
	ui8_deviceSpecificConfigCnt = ui8_data;
}
