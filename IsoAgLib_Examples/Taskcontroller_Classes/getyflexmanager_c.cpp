/***************************************************************************
                          getyflexmanager_c.cpp  -  description
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

#include "getyflexmanager_c.h"
#include <Application_Config/eeprom_adr.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>
#include <IsoAgLib/driver/eeprom/ieepromio_c.h>

GetyFlexManager_c::GetyFlexManager_c(DefaultRecordConfig_c* rpc_defaultRecordConfig, uint16_t rui16_eepromOffsetAdr)
  : ProcDataManager_c(7), c_flexibleHeader()
{
  pc_data = (IsoAgLib::iProcDataRemote_c*)_pc_proc;
	pc_defaultRecordConfig = rpc_defaultRecordConfig;

	configField.posApplrateX = 0;
	configField.requestSingleVals = 0;

	configField.timeWert = 0xA;

	configField.applrateXHa = 1;
	configField.applrateXMin = 0;
	configField.applrateX = 0;
	configField.applrateTankX = 0;

	configField.transportDummyWidth = 0;
	configField.recordWorkDist = 0;
	configField.fieldstarSend = 0;
	configField.recordAsGety = 0;
	configField.workWertInst = 0x10;
	configField.useMeasureProgs = 1;
	configField.timeDistGety = 0xF;

	// set proc_cnt to 1 (working state) unless more info found
	set_proc_cnt(1);
}
GetyFlexManager_c::GetyFlexManager_c(const GetyFlexManager_c& rrefc_src)
 : ProcDataManager_c(rrefc_src)
{
    pc_defaultRecordConfig = rrefc_src.pc_defaultRecordConfig;
    configField.posApplrateX = rrefc_src.configField.posApplrateX;
		configField.requestSingleVals = rrefc_src.configField.requestSingleVals;
    // don't copy proc data pointer array - start with deactivated
    _pc_proc = NULL;

		ui16_eepromOffsetAdr = rrefc_src.ui16_eepromOffsetAdr;

		configField.timeWert = rrefc_src.configField.timeWert;

		configField.applrateXHa = rrefc_src.configField.applrateXHa;
		configField.applrateXMin = rrefc_src.configField.applrateXMin;
		configField.applrateX = rrefc_src.configField.applrateX;
		configField.applrateTankX = rrefc_src.configField.applrateTankX;

		configField.transportDummyWidth = rrefc_src.configField.transportDummyWidth;
		configField.recordWorkDist = rrefc_src.configField.recordWorkDist;
		configField.fieldstarSend = rrefc_src.configField.fieldstarSend;
		configField.recordAsGety = rrefc_src.configField.recordAsGety;
 		configField.workWertInst = rrefc_src.configField.workWertInst;
		configField.useMeasureProgs = rrefc_src.configField.useMeasureProgs;
		configField.timeDistGety = rrefc_src.configField.timeDistGety;

		c_flexibleHeader = rrefc_src.c_flexibleHeader;
}

GetyFlexManager_c::~GetyFlexManager_c()
{
  if (activated()) deactivate();
}

void GetyFlexManager_c::init(DefaultRecordConfig_c* rpc_defaultRecordConfig)
{
	pc_defaultRecordConfig = rpc_defaultRecordConfig;
}

/**
  activate with creating the needed ProcessData
  @param rpc_monitor pointer to member_item of data delivering member
  @param rpc_localGtp pointer to local member GTP for sending of commands
*/
void GetyFlexManager_c::activate(IsoAgLib::iDINItem_c* rpc_monitor, IsoAgLib::iGetyPos_c* rpc_localGtp)
{
  ProcDataManager_c::activate(rpc_monitor);
	// read specific device config from EEPROM
	SpecificRecordConfig_c c_specificRecordConfig(ADR_TASK_CONTROLLER_SPECIFIC_CONFIG,
		pc_defaultRecordConfig, rpc_monitor);
	// set local config settings
	configField.timeWert = c_specificRecordConfig.timeWert();

	configField.applrateXHa = c_specificRecordConfig.applrateXHa();
	configField.applrateXMin = c_specificRecordConfig.applrateXMin();
	configField.applrateX = c_specificRecordConfig.applrateX();
	configField.applrateTankX = c_specificRecordConfig.applrateTankX();

	configField.transportDummyWidth = c_specificRecordConfig.transportDummyWidth();
	configField.recordWorkDist = c_specificRecordConfig.transportWorkDist();
	configField.fieldstarSend = c_specificRecordConfig.fieldstarSend();
	configField.recordAsGety = c_specificRecordConfig.recordAsGtp().getGety();
	configField.workWertInst = c_specificRecordConfig.workWertInst();
	configField.useMeasureProgs = c_specificRecordConfig.useMeasureProgs();
	configField.requestSingleVals = c_specificRecordConfig.sendRequest();
	configField.timeDistGety = c_specificRecordConfig.timeDistGety();

	uint8_t b_calc_proc_cnt = 1; // at least work state

	// don't start measuring prog for width for transport
	if (rpc_monitor->gety() != 11) b_calc_proc_cnt += 1;

	if (configField.applrateXHa == 1) b_calc_proc_cnt += 1;
	if (configField.applrateXMin == 1) b_calc_proc_cnt += 1;
	if (configField.applrateX == 1) b_calc_proc_cnt += 1;
	if (configField.applrateTankX == 1) b_calc_proc_cnt += 1;

	// if time-distance values should be recorded
	// -> (wholeTime, wholeDist, workTime, workDist, workArea) => max 5 columns
	if (pc_defaultRecordConfig->accumulatedTimeDist())
	{
		b_calc_proc_cnt += 3;
		if (configField.recordWorkDist == 1) b_calc_proc_cnt += 1;
		// don't start measuring prog for work area for transport
		if (rpc_monitor->gety() != 11) b_calc_proc_cnt += 1; // work area
	}

	// reserve space for device specific proc data
	b_calc_proc_cnt += c_specificRecordConfig.procDataCnt();

	// now set proc_cnt to b_calc_proc_cnt
	set_proc_cnt(b_calc_proc_cnt);
	_pc_proc = new IsoAgLib::iProcDataRemote_c [proc_cnt()];
	pc_data = _pc_proc;

	c_flexibleHeader = "";


	IsoAgLib::iGetyPos_c c_remoteWorkDataGtp = rpc_monitor->gtp(),
				c_remoteTimedistDataGtp = rpc_monitor->gtp();
	uint8_t	ui8_remoteTimedistDataGety = c_specificRecordConfig.timeDistGety();

	if (c_specificRecordConfig.fieldstarSend())
	{ // use data sent from pseudo virtTerm to pseudoTaskCon (and vice versa)
		// which is used from Jacobi (Mueller) for adressing the fieldstar
		c_remoteWorkDataGtp = 0xB;
		c_remoteTimedistDataGtp = 0xA;
	}

  // create remote process data instances and store pointers in ppc_data of ProcDataManager_c
	uint8_t b_proc_ind = 0;
  // 1. Activation PROC-Column: WIDTH ( Dummy or Measurement ) -> IND==0
	// don't start measuring prog for work width for transport
	if (rpc_monitor->gety() != 11)
	{	// remote process data for "working width" [mm] (LIS=0, GETY=2, WERT=3, INST=1)
		pc_data[b_proc_ind].init(0, c_remoteWorkDataGtp, 0x3, 0x1, 0xFF, 2, c_remoteWorkDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;
	}

  // 2. Activation PROC-Column: STATE -> IND==1/0
	// remote process data for "on/off" dependent on configField.workWertInst [0/0x64] (LIS=0, GETY=2, WERT=1, INST=0)
	pc_data[b_proc_ind].init(0, c_remoteWorkDataGtp, (configField.workWertInst >> 4),
				(configField.workWertInst & 0xF), 0xFF, 2, c_remoteWorkDataGtp, rpc_localGtp);
	if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
	b_proc_ind++;

	// mostly unused proc data with 3,4 or 5 proc data
	if (pc_defaultRecordConfig->accumulatedTimeDist())
	{ // remote process data for "whole time" [sec] (LIS=0, GETY=2, WERT=0xA, INST=0)
		uint8_t bTime_inst = (configField.timeWert == 0xA)?0:0xB;
		IsoAgLib::iGetyPos_c c_specRemoteTimeDistGtp = c_remoteTimedistDataGtp;
		c_specRemoteTimeDistGtp.setGety( ui8_remoteTimedistDataGety );

		pc_data[b_proc_ind].init(0, c_specRemoteTimeDistGtp, configField.timeWert,
					bTime_inst, 0xFF, 2, c_remoteTimedistDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;

		// remote process data for "whole distance" [m] (LIS=0, GETY=2, WERT=8, INST=1)
		pc_data[b_proc_ind].init(0, c_specRemoteTimeDistGtp, 0x8, 0x1, 0xFF,
					2, c_remoteTimedistDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;

		// remote process data for "work time" [sec] (LIS=0, GETY=2, WERT=0xA, INST=7)
		bTime_inst = (configField.timeWert == 0xA)?7:5;
		pc_data[b_proc_ind].init(0, c_specRemoteTimeDistGtp, configField.timeWert,
					bTime_inst, 0xFF, 2, c_remoteTimedistDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;

		if (configField.recordWorkDist == 1)
		{	// remote process data for "working distance" [m] (LIS=0, GETY=2, WERT=8, INST=4)// remote process data for "working distance" [m] (LIS=0, GETY=2, WERT=8, INST=4)
			pc_data[b_proc_ind].init(0, c_specRemoteTimeDistGtp, 0x8, 0x4, 0xFF,
						2, c_remoteTimedistDataGtp, rpc_localGtp);
			if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
			b_proc_ind++;
		}

		// don't start measuring prog for work area for transport
		if (rpc_monitor->gety() != 11)
		{	// remote process data for "working area" [m2] (LIS=0, GETY=2, WERT=8, INST=0)
			pc_data[b_proc_ind].init(0, c_specRemoteTimeDistGtp, 0x8, 0x0, 0xFF,
						2, c_remoteWorkDataGtp, rpc_localGtp);
			if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
			b_proc_ind++;
		}
	}

	if (configField.applrateXHa == 1)
	{ // record x/ha -> "appl rate per ha" (LIS=0, GETY=2, WERT=5, INST=0)
		pc_data[b_proc_ind].init(0, c_remoteWorkDataGtp, 0x5, 0x0, 0xFF, 2, c_remoteWorkDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;
	}
	if (configField.applrateXMin == 1)
	{ // record x/ha -> "appl rate per minute" (LIS=0, GETY=2, WERT=5, INST=1)
		pc_data[b_proc_ind].init(0, c_remoteWorkDataGtp, 0x5, 0x1, 0xFF, 2, c_remoteWorkDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;
	}
	if (configField.applrateX == 1)
	{ // record x/ha -> "application total" (LIS=0, GETY=2, WERT=5, INST=2)
		pc_data[b_proc_ind].init(0, c_remoteWorkDataGtp, 0x5, 0x2, 0xFF, 2, c_remoteWorkDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		configField.posApplrateX = b_proc_ind;
		b_proc_ind++;
	}
	if (configField.applrateTankX == 1)
	{ // record x/ha -> "applicable volumen in tank" (LIS=0, GETY=2, WERT=5, INST=3)
		pc_data[b_proc_ind].init(0, c_remoteWorkDataGtp, 0x5, 0x3, 0xFF, 2, c_remoteWorkDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;
	}

	uint8_t ui8_flexWertInst, ui8_flexLis, ui8_flexWert, ui8_flexInst;
	for (uint8_t ui8_specProcInd = 0; ui8_specProcInd < c_specificRecordConfig.procDataCnt(); ui8_specProcInd++)
	{ // now create the specific proc data for this device
		// append column seperator in flexible header
		ui8_flexWertInst = c_specificRecordConfig.procDataIndWertinst(ui8_specProcInd);
		ui8_flexWert = (ui8_flexWertInst >> 4);
		ui8_flexInst = (ui8_flexWertInst & 0xF);
		c_flexibleHeader += (const char*)c_specificRecordConfig.procDataIndHeader(ui8_specProcInd);
		c_flexibleHeader += ";";
		ui8_flexLis = c_specificRecordConfig.procDataIndLis(ui8_specProcInd);

		pc_data[b_proc_ind].init(ui8_flexLis, c_remoteWorkDataGtp, ui8_flexWert, ui8_flexInst, 0xFF,
					2, c_remoteWorkDataGtp, rpc_localGtp);
		if (!configField.useMeasureProgs) pc_data[b_proc_ind].prog().receiveForeignMeasurement();
		b_proc_ind++;
	}

	// now start recording
	// only start measurement programs, if requested in config
	// ( value 3 == start measurement prog )
	if (configField.useMeasureProgs) start();
}

/** deactivate with deleting the created ProcessDatas */
void GetyFlexManager_c::deactivate()
{
  if (activated())
  {
	  // first stop recording
		// only stop measurement programs, if requested in config
		// ( value 3 == start/stop measurement prog )
		if (configField.useMeasureProgs) stop();
		// set deactivated state with base function
	  ProcDataManager_c::deactivate();
	  // delete all proces data entries
		delete(_pc_proc);
		_pc_proc = NULL;
  }
}


/** write header with literal description of
process data */
void GetyFlexManager_c::writeHeader()
{
	IsoAgLib::iSystem_c::triggerWd();
	// First Header Column: Name
	IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();
  c_rs232 << "iname;";

  // 1. Header PROC-Column: WIDTH ( Dummy or Measurement ) -> IND==0
  if (recordWidth())
  {
	  c_rs232 << "iwow;";
  }
  // 2. Header PROC-Column: STATE -> IND==1/0
  c_rs232 << "istate;";

	// mostly unused proc data with 3,4 or 5 proc data and 7,8 or 9 columns
	if (pc_defaultRecordConfig->accumulatedTimeDist())
  { // write header for area_type dependent recording
	  c_rs232 << "wholeTime mounting [sec]; wholeDist mounting [m];";
	  c_rs232 << "wholeTime transport [sec]; wholeDist transport [m];";
	  c_rs232 << "wholeTime field [sec]; wholeDist field [m];";
	  c_rs232 << "workTime field [sec];";

	  if (configField.recordWorkDist == 1)
	  {
	  	c_rs232 << "workDist field [m];";
	  }
	  if (recordWidth())
	  {
	  	c_rs232 << "workArea field [m^2];";
	  }
  }

  // 3. Main Header PROC-Column: AppRate_X_Ha -> IND==2_3 or 1_2
	// always print these two columns - use placeholder "0" if no data
  if (configField.applrateXHa == 1)
  {
	  c_rs232 << "iapvols;iapvola;";
  }
	// always print this column - use placeholder "0" if no data
	// ( set EEPROM config of value "applrate_recording" for global config
	//   and "appl_rec" for device specific config to bitmask with at least
	//   0x2 set, to get applrateXMin active,
	//   IMPORTANT: device specific config overwrites global config!!! )
  if (configField.applrateXMin == 1)
  {
    c_rs232 << "iapvolpmin;";
  }

	// ( set EEPROM config of value "applrate_recording" for global config
	//   and "appl_rec" for device specific config to bitmask with at least
	//   0x4 set, to get applrateX active,
	//   IMPORTANT: device specific config overwrites global config!!! )
  if (configField.applrateX == 1)
  {
    c_rs232 << "Applicated Volume [l];";
  }
	// ( set EEPROM config of value "applrate_recording" for global config
	//   and "appl_rec" for device specific config to bitmask with at least
	//   0x8 set, to get applrateTankX active,
	//   IMPORTANT: device specific config overwrites global config!!! )
  if (configField.applrateTankX == 1)
  {
    c_rs232 << "Tank Vol [l];";
  }

  if ( c_flexibleHeader.empty() )
	{ // no individual data
		c_rs232 << "ivar1;ivar2;";
	}
	else
	{ // write individual data
		IsoAgLib::iSystem_c::triggerWd();
		c_rs232 << c_flexibleHeader;
	}
}

/** NEW: 14-10-2004
	check if a single measurement request should be sent on every local
	value access.
	@param rb_procInd index of process data to check
*/
bool GetyFlexManager_c::sendRequest( uint8_t rb_procInd )const
{ // never send value request, if globally not wanted
	if ( ! sendRequest() ) return false;

	uint16_t w_valueAge = ( IsoAgLib::iSystem_c::getTime() - ( (pc_data[rb_procInd]).prog().lastMeasurementReceive() ) );
	if (  w_valueAge < 1000 ) return false;
	else return true;
}

/** write informations of according member (GETY, POS, name)
    and all remote process data of ppc_data
    to RS232
*/
void GetyFlexManager_c::writeData()
{

  char temp_name[15];
//	uint8_t b_output_gety,
//			b_output_pos;
	static bool b_false_float = false;
	pc_monitor->getPureAsciiName((int8_t*)temp_name, 14);
//	b_output_gety = configField.recordAsGety;
//	b_output_pos = pc_monitor->pos();
	uint8_t b_ppc_data_ind = 0;


  memmove(temp_name+12, "\0\0\0", 3);
	IsoAgLib::iRS232IO_c& c_rs232 = IsoAgLib::getIrs232Instance();

	// this process might last longer -> trigger watchdog
 IsoAgLib::iSystem_c::triggerWd();

//  c_rs232 << (INT_2_BYTE)(b_output_gety) << "; " << (INT_2_BYTE)(b_output_pos)
//            << "; " << (INT_2_BYTE)(pc_monitor->nr()) << "; " << temp_name << "; ";

  // First Data Column: Name
	c_rs232 << temp_name << ";";

  // check if no new values are received -> send stop/start
	if ( (configField.useMeasureProgs)
		&& (IsoAgLib::iSystem_c::getTime() - (pc_data[b_ppc_data_ind]).prog().lastMeasurementReceive() > 3000)
		 )
	{ // last working state received > 3sec ago
		stop();
		start();
	}

  // now write values
	b_ppc_data_ind = 0;
  // 1. Data PROC-Column: WIDTH ( Dummy or Measurement ) -> IND==0
  if (pc_monitor->gety() != 11)
  { // work width value for devices other than transport
		uint16_t ui16_WorkWidth = (pc_data[b_ppc_data_ind]).prog().val( sendRequest(b_ppc_data_ind) );
  	c_rs232 << ui16_WorkWidth << ";";
		b_ppc_data_ind++;
  }
	else
	{ // output dummy width if wanted
		if (configField.transportDummyWidth == 1) c_rs232 << "0;";
	}

  // 2. Data PROC-Column: STATE -> IND==1/0
	// write area independent values: work_state
	c_rs232 <<  (pc_data[b_ppc_data_ind]).prog().val(sendRequest(b_ppc_data_ind)) << ";";
	b_ppc_data_ind++;

	// mostly unused proc data with 3,4 or 5 proc data and 7,8 or 9 columns
	#if 0
	if (pc_defaultRecordConfig->accumulatedTimeDist())
	{ // parts for mounting
		c_rs232 << wholeTimeMounting()<< ";" << wholeDistMounting()<< ";";
		// parts for transport
		c_rs232 << wholeTimeTransport() << ";" << wholeDistTransport() << ";" ;
		// parts for field
		c_rs232 << wholeTimeField() << ";" << wholeDistField() << ";" ;
		c_rs232 << workTimeField() << ";";
		// previous data is derived from wholeTime, wholeDist and workTime
		b_ppc_data_ind += 3;

		if (configField.recordWorkDist == 1)
		{ // record workDist
			int32_t i32_WorkDist = workDistField();

			if (b_false_float)
			{ // some implements send workDist as float - but don't indicate them as a float type
				// ==> reading of them as integer causes nonsense values
			#if (defined(SYSTEM_ESX) || defined(SYSTEM_IMI) || defined(SYSTEM_PM167))
				uint8_t pb_tmp[4];
				// change first and last two bytes on PC for float access
				memmove(pb_tmp, ((uint8_t*)(&i32_WorkDist))+2, 2);
				memmove(pb_tmp+2, ((uint8_t*)(&i32_WorkDist)), 2);
				i32_WorkDist = *(static_cast<const float*>(static_cast<const void*>(pb_tmp)));
			#elif defined(SYSTEM_PC)
				i32_WorkDist = *(static_cast<const float*>(static_cast<const void*>(((uint8_t*)(&i32_WorkDist)))));
			#endif
			}
			c_rs232 << i32_WorkDist << ";";
			// is derived from 1 real proc data
			b_ppc_data_ind++;
		}
		// Decide on how to write workArea
		if (pc_monitor->gety() != 11)
		{ // normal case - implement with real working width ( i.e. no transport )
			c_rs232 << workAreaField() << ";";
			// previous item workArea is derived from 1 proc data
			b_ppc_data_ind++;
		}
		else
		{ // record workDist and workArea for transport if configured
			if (configField.transportDummyWidth == 1)
			{ // transport device is configured to use dummy width for compatible column definition for all types of implements
				c_rs232 << "0;";
				// previous items: state, wholeTime, wholeDist, workTime, work width
				// -> application info or device specific info start at 5
			}
		}
	}
	#endif

  // 3. Main Data PROC-Column: AppRate_X_Ha -> IND==2_3 or 1_2
	if (configField.applrateXHa == 1)
	{ // setpoint and measured appl rate
		int32_t l_measure_rate = (pc_data[b_ppc_data_ind]).prog().val(sendRequest(b_ppc_data_ind)),
				l_setpoint_rate = (pc_data[b_ppc_data_ind]).setpoint().master().exact();
		b_false_float = false;
		if (labs(l_measure_rate - l_setpoint_rate) > 0x10000)
		{ // maybe the ECU send a value as float without setting the D flag for float
			int32_t l_temp_measure;
		#if (defined(SYSTEM_ESX) || defined(SYSTEM_IMI) || defined(SYSTEM_PM167))
			uint8_t pb_tmp[4];
			// change first and last two bytes on PC for float access
			memmove(pb_tmp, ((uint8_t*)(&l_measure_rate))+2, 2);
			memmove(pb_tmp+2, ((uint8_t*)(&l_measure_rate)), 2);
			l_temp_measure = *(static_cast<const float*>(static_cast<const void*>(pb_tmp)));
		#elif defined(SYSTEM_PC)
			l_temp_measure = *(static_cast<const float*>(static_cast<const void*>(((uint8_t*)(&l_measure_rate)))));
		#endif
			// check if the difference between setp and measure gets smaller
			if (labs(l_temp_measure - l_setpoint_rate) < labs(l_measure_rate - l_setpoint_rate))
			{
				l_measure_rate = l_temp_measure;
				b_false_float = true;
			}
		}
		c_rs232 <<  l_setpoint_rate << ";";
		c_rs232 <<  l_measure_rate << ";";
		b_ppc_data_ind++;
	}
	else
	{ // if no appl rate available, write only a dummy for setpoint and real val
		c_rs232 << "0;0;";
	}

	if (configField.applrateXMin == 1)
	{ // applrateXMin is wanted -> write value from process data
		c_rs232 << (pc_data[b_ppc_data_ind]).prog().val(sendRequest(b_ppc_data_ind)) << ";";
		b_ppc_data_ind++;
	}
	else
	{ // if no appl rate per minute available, write only a dummy
		c_rs232 << "0;";
	}


	// print now:
	// * applrateX (if configured)
	// * applrateTankX (if configured)
	// * device specific process data (e.g. "SectionState of TeeJet)

	if ( b_ppc_data_ind >= proc_cnt() )
	{ // write now: 2 dummies for ivar1 and ivar2 as no other data is coming
		c_rs232 << "0;0;";
	}
	else
	{ // real device specific data is coming
  IsoAgLib::iSystem_c::triggerWd();
		for (; b_ppc_data_ind < proc_cnt(); b_ppc_data_ind++)
		{ // use loop to print out lasting values
			c_rs232 << (pc_data[b_ppc_data_ind]).prog().val(sendRequest(b_ppc_data_ind)) << ";" ;
		}
	}
}
