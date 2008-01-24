/***************************************************************************
                          defaultrecordconfig_c.h  -  description
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

#ifndef DEFAULT_RECORD_CONFIG_H
#define DEFAULT_RECORD_CONFIG_H

#include <IsoAgLib/typedef.h>

/**
	Read default Recording Defaults from EEPROM
	and provide them for simple access
	during DevClass_Flex_Manager initialisation for specific
	device
  *@author Dipl.-Inform. Achim Spangler
*/
class DefaultRecordConfig_c {
public:
	/**
		constructor which initialises all data and can call init to read
		config data from EEPROM (if eeprom adress is given)
		<!--@param apc_lbs pointer to LBS_Lib::nLBS::iLBS (optional)
		@param apc_system pointer to LBS_Lib::nHwSystem::iSystem (optional)-->
		@param aui16_eepromAdr adress in EEPROM where config data is stored
	*/
	DefaultRecordConfig_c( uint16_t aui16_eepromAdr = 0 );
	/**
		copy Constructor
		@param acrc_src reference to source instance
	*/
	DefaultRecordConfig_c(const DefaultRecordConfig_c& acrc_src);
	/**
		destructor
	*/
	~DefaultRecordConfig_c();
	/**
		initialise with reading the config data from EEPROM in flags
		@param aui16_eepromAdr adress in EEPROM where config data begins
	*/
  void init(uint16_t aui16_eepromAdr);

	/**
		deliver information whether accumulated data to time and distance should
		be recorded
		@return true -> accumulated data to time and distance should be recorded
	*/
	bool accumulatedTimeDist() const {return b_accumulatedTimeDist;};
	/**
		deliver information whether diagnose data should be recorded
		@return true -> diagnose data should be recorded
	*/
	bool diagnoseData() const {return b_diagnoseData;};
	/**
		deliver time distance between data recording in [msec.] (typical 1000 msec.)
		@return time distance between different recordings
	*/
	uint16_t recordingRate() const {return ui16_recordingRate;};

	/**
		deliver WERT of process data for time informations
		@return wert 0x9 or 0xA for time data
	*/
	uint8_t timeWert() const {return ui8_timeWert;};
	/**
		deliver WERT/INST of process data for main working state
		@return wert_inst for main working state
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
		deliver information whether measuring programs should be used to
		get every sec. a new value (use alternatively single requests for Fieldstar systems)
		@return 0=no measure prog and no single measurement val request,
						1=no measure prog but send single measurement val requests,
		        2=only for identified IMIs (DEVCLASS different from 4:seed, 5:fertilizer, 6:spreader
		                                    or DEVCLASSINST 4:"Aufsattelposition", 7:"Anhaengeposition2"
																				or LBS-Name beginning with "IMI")
					  3=always
	*/
	uint8_t useMeasureProgs() const {return ui8_useMeasureProgs;};

	/**
		deliver information whether fuel consumption should be recorded
		@return true -> record fuel consumption
	*/
	bool fuelConsumption() const {return b_fuelConsumption;};
	/**
		deliver information whether draft force should be recorded
		@return true -> 0: record no draft force
		                1: record draft force [kN]
										2: record draft force [kN], sensor left/right, sensor_u_bat
	*/
	uint8_t draftForce() const {return ui8_draftForce;};
	/**
		deliver the amount of device specific recording data sets
		@return amount of device specific recording data sets
	*/
	uint8_t deviceSpecificConfigCnt() const { return ui8_deviceSpecificConfigCnt;};
private:
	/** Startadress of config data in EEPROM */
	uint16_t ui16_eepromAdr;

	/** decide if time-distance statistics should be recorded */
	bool b_accumulatedTimeDist;
	/** decide if diagnostic data should be recorded (heap, runtime, CAN baudrate) */
	bool b_diagnoseData;
	/** time between data recordings [msec] - typical 1000 msec. */
	uint16_t ui16_recordingRate;
	/** process data WERT for time informations: 0x9 | 0xA */
	uint8_t ui8_timeWert;
	/** wert_inst code for main working state */
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
	/** record fuel consumption */
	bool b_fuelConsumption;
	/** record draft force which can be gathered from EHR sensors
	    0:no , 1:draft force [kN], 2: additional uncalculated sensor values
	 */
	uint8_t ui8_draftForce;
	/** amount of device specific recording data sets */
	uint8_t ui8_deviceSpecificConfigCnt;
};

#endif
