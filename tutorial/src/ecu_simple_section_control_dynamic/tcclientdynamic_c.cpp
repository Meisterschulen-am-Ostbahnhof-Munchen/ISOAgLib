#include <IsoAgLib/driver/system/isystem_c.h>
//#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/proc_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatalocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocess_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/process_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>

#include "tcclientdynamic_c.h"

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/ddidefinition.h>

// Object and element ID for device pool defintion
// The object IDs are arbitrary with the exception of the DVC (objectID = 0)

tcClientDynamic_c::tcClientDynamic_c() 
: mi32_effectiveTimeMs(0)
, m_sections(8)
,  vec_condensedWorkStateTaskController( ),
  vec_condensedWorkStateCurrent( ),
  vec_sectionStatusCurrent( 0xFF ), // default sections status ON
  vec_sectionStatusTaskController( 0xFF )
{
	/// Assume metric units initially.  The scale and unit strings will change when language command updated
	currentSettings.uArea = 0;
	currentSettings.uDistance = 0;
	currentSettings.uForce = 0;
	currentSettings.uMass = 0;
	currentSettings.uPressure = 0;
	currentSettings.uTemperature = 0;
	currentSettings.uUnitsSystem = 0;

  // set up m_mapProcDataSetPoint content
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_sectionControlState,IndexSectionControlState));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_setpointVolumeRate,IndexSetpointVolumeRate));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection01,IndexWorkStateSection01));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection02,IndexWorkStateSection02));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection03,IndexWorkStateSection03));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection04,IndexWorkStateSection04));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection05,IndexWorkStateSection05));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection06,IndexWorkStateSection06));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection07,IndexWorkStateSection07));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_workStateSection08,IndexWorkStateSection08));
  m_mapProcDataSetPoint.insert(PairProcDataLocal(&c_condensedWorkState,IndexCondensedWorkState));

  std::cout << "CONSTRUCTOR" << std::endl;

  arr_workStateProcData[0] = &c_workStateSection01;
  arr_workStateProcData[1] = &c_workStateSection02;
  arr_workStateProcData[2] = &c_workStateSection03;
  arr_workStateProcData[3] = &c_workStateSection04;
  arr_workStateProcData[4] = &c_workStateSection05;
  arr_workStateProcData[5] = &c_workStateSection06;
  arr_workStateProcData[6] = &c_workStateSection07;
  arr_workStateProcData[7] = &c_workStateSection08;
}

void 
tcClientDynamic_c::init()
{
	IsoAgLib::getISchedulerInstance().registerClient(this);
	changeRetriggerTime(IsoAgLib::iSystem_c::getTime() + c_schedulerPeriod,c_schedulerPeriod);
}


void tcClientDynamic_c::close()
{
	IsoAgLib::getISchedulerInstance().unregisterClient(this);
}

void tcClientDynamic_c::setSections(uint8_t sections)
{
	if ( m_sections == sections )
		return;

	m_sections = sections;

	// machine's geometry changed, clear up the original pool and reconstruct a new pool
	m_DevicePool.clear();
	initDevicePool();
}


// initDevicePool()
//	Initial construct of DDOP and reconstruct of DDOP when geometry changed
void
tcClientDynamic_c::initDevicePool()
{
  std::cout << "Start creating device description" << std::endl;
  // Create the main device
	DeviceObjectDvc_c dvcObj(0);
	dvcObj.WsmName(m_myIdent->isoName());
	dvcObj.Version("V1.00");
	dvcObj.StructureLabel("TC00001");
	dvcObj.Designator("OSBSprayer");

	m_DevicePool.Add(dvcObj);

  // Create Main DET
  DeviceObjectDet_c rootDET( 0xfffe, 0, 0, 1/*device*/, "Sprayer Interface");
  // create tree
  DeviceObjectDet_c* pDetObj = m_DevicePool.getDetObject(m_DevicePool.Add(rootDET));

  //Populate with DPT and DPD

// Create the DPD entries
// Parameters are (DpdObjectID, DDI, Properties, LogMethod, Designator, DvpReference)
//	Properties  bit 0 - belongs to default set
//				      bit 1 - settable
//	Method		  bit 0 - time interval
//				      bit 1 - distance interval
//				      bit 2 - threshold limits
//				      bit 3 - on change
//				      bit 4 - total (accumulator)

  // main DET
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpt_c(0xfff2, 135, 0,"Y Offset")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpt_c(0xfff1, 136, 0,"Z Offset")));

	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffeb, 134, 0, 8, "X Offset")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffea, 67,  0, 8, "Actual Width")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffe9, 70,  0, 8, "Max Width")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffe8, 1,   2, 9, "Setpoint Volume Rate")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffe7, 2,   1, 9, "Actual Volume Rate")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffe6, 141, 0, 9, "Work State")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffe5, 160, 2, 8, "Section Control State")));
	pDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffe4, 161, 2, 9, "Condensed Work State")));

  // Create Sections DET
  for (int counter = 0; counter < 8; ++counter)
  {
    //char buffer[10];
    //sprintf(buffer,"Section 0%d", 1+counter);
    DeviceObjectDet_c new_section(0xfffd - counter, 0xfffe, 1 + counter, 4/*section*/,"Section");
    uint16_t sectionid = m_DevicePool.Add(new_section);
    pDetObj->AddChild(sectionid);

    DeviceObjectDet_c* pMiddleSectionDetObj = m_DevicePool.getDetObject(sectionid);
	  pMiddleSectionDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpt_c(0xffe3 - 7*counter, 134, 0,                          "offsetX")));
	  pMiddleSectionDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpt_c(0xffe2 - 7*counter, 135, -12000 + (counter * 3000),  "offsetY")));
	  pMiddleSectionDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpt_c(0xffe1 - 7*counter, 136, 0,                          "offsetZ")));
	  pMiddleSectionDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpt_c(0xffe0 - 7*counter, 67,  3000,                       "actual width")));
	  pMiddleSectionDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpt_c(0xffdf - 7*counter, 70,  3000,                       "maximal width")));
    pMiddleSectionDetObj->AddChild(m_DevicePool.Add(DeviceObjectDpd_c(0xffdd - 7*counter, 141,	2,  9,                     "WorkState")));
  }
}

void 
tcClientDynamic_c::InitDpdAndRegisterDDOP(IsoAgLib::iIdentItem_c* myIdent)
{
	DdopBase_c::init(myIdent);
	IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().registerDevicePool(m_myIdent, 0, 0, false);

	//initDevicePool();

	// Init all DPDs
	const IsoAgLib::iIsoName_c& isoName = myIdent->isoName();
  // DPD initialisation
  c_xOffset.init(0x86, 0, isoName, false, 8);
  c_actualWidth.init(0x43, 0, isoName, false, 8);
  c_maxWidth.init(0x46, 0, isoName, false, 8);
  c_setpointVolumeRate.init(0x1, 0, isoName, true, 9);
  c_actualVolumeRate.init(0x2, 0, isoName, false, 9);
  c_workState.init(0x8d, 0, isoName, false, 9);
  c_sectionControlState.init(0xa0, 0, isoName, true, 8);
  c_condensedWorkState.init(0xa1, 0, isoName, true, 9);
  c_workStateSection01.init(0x8d, 1, isoName, true, 9);
  c_workStateSection02.init(0x8d, 2, isoName, true, 9);
  c_workStateSection03.init(0x8d, 3, isoName, true, 9);
  c_workStateSection04.init(0x8d, 4, isoName, true, 9);
  c_workStateSection05.init(0x8d, 5, isoName, true, 9);
  c_workStateSection06.init(0x8d, 6, isoName, true, 9);
  c_workStateSection07.init(0x8d, 7, isoName, true, 9);
  c_workStateSection08.init(0x8d, 8, isoName, true, 9);
	
	IsoAgLib::getIProcessInstance().setProcessDataChangeHandler(this);

  // register all setpoints
  for (IterProcDataLocal iter = m_mapProcDataSetPoint.begin(); iter != m_mapProcDataSetPoint.end();++iter) {
    iter->first->setProcessDataChangeHandler(this);
  }

  // setup init value
  c_actualVolumeRate.setMasterMeasurementVal(200); // Section Control coverage is working only if c_actualVolumeRate != 0
  c_workState.setMasterMeasurementVal(true); // main work state must be ON

  c_setpointVolumeRate.setMasterMeasurementVal(200);
  c_sectionControlState.setMasterMeasurementVal(0);

  c_actualWidth.setMasterMeasurementVal(24000);
  c_maxWidth.setMasterMeasurementVal(24000);

  c_xOffset.setMasterMeasurementVal(-500);

  // init section status
  resetSectionStatus();
  updateSectionStatus( );
}

//When system unit changed, update DDOP via partial pool updating
void
tcClientDynamic_c::updateUnitSettings(const localSettings_s& settings)
{
	DeviceObjectDvc_c* dvcObj = m_DevicePool.getDvcObject(0);
	if (dvcObj)
		dvcObj->Localization(settings);
}

bool 
tcClientDynamic_c::timeEvent()
{
  timeEventDevicePool();

	return true;
}

/* In DeviceDescription.xml, all ProcessDataCombination's is_setpoint attribute is false.
   DPD variable (e.g. c_workState) will init iProcDataLocal_c's b_isSetpoint to false,
   this derived function will be called upon receiving any PD messages from TC.
*/
bool 
tcClientDynamic_c::processMeasurementReset(IsoAgLib::EventSource_c rc_src, uint16_t /*rui16_ddi*/, int32_t ai32_val , const IsoAgLib::iIsoName_c& /* rc_callerIsoName */ )
{
	IsoAgLib::iProcDataLocal_c* procData = rc_src.makeIProcDataLocal();
  uint16_t ddi = procData->DDI();
	uint16_t element = procData->element();

	if ( ddi == c_workState.DDI() && element== c_workState.element() )
	{
		/// TC commanded me to set work state, just echo the same value back 
		c_workState.setMasterMeasurementVal(ai32_val);
		return true;
	}
	else  if ( ddi == c_actualVolumeRate.DDI()  && element == c_actualVolumeRate.element() )
	{
		/// echo back setpoint. TC needs it for control display and data logging
		c_actualVolumeRate.setMasterMeasurementVal(ai32_val); 

		/// Add a small jiter to actual application rate and send it back to TC for display and data logging
		int32_t actual =  (int32_t) ((double)(ai32_val) * ( 1 + 0.01* (double)((rand()%10)) ));
		c_actualVolumeRate.setMasterMeasurementVal(actual);
		return true;
	}
	
	return false;
}

///	Derived function will be called upon receiving start default logging command from TC
bool 
tcClientDynamic_c::processDefaultLoggingStart(IsoAgLib::EventSource_c /* ac_src */, uint16_t /*rui16_ddi*/, int32_t /* ai32_val */, const IsoAgLib::iIsoName_c& /* ac_callerIsoName */)
{
	c_workState.startDataLogging(IsoAgLib::Proc_c::OnChange, 1);
	c_actualVolumeRate.startDataLogging(IsoAgLib::Proc_c::TimeProp, 1000);

	return true;
}

///Derived method will be called when task status changed
bool 
tcClientDynamic_c::processTcStatusMessage(bool ab_taskRunning , const IsoAgLib::iIsoName_c& /* ac_callerISOName  */)
{
	static bool bTaskStatus = false;
	if ( bTaskStatus != ab_taskRunning)
	{
		if ( ab_taskRunning )
		{
			//Nothing to do here
		}
		else
		{	
			timeEvent(); //update totals before stop counting
		}
		bTaskStatus = ab_taskRunning;
	}
	return true;
}

void
tcClientDynamic_c::updateEarlierAndLatestInterval(){
  setEarlierInterval(0);
  setLatestInterval( getTimePeriod() / 2) ;
}

void
tcClientDynamic_c::updateSectionStatus( )
{
  for (uint8_t ui8_sectionId = 0; ui8_sectionId < scui8_numberOfSection; ++ui8_sectionId) {
    // 1.1 recompute current section status
    vec_sectionStatusCurrent[ui8_sectionId] = vec_sectionStatusTaskController[ui8_sectionId];
    // 1.2 convert to CondensedWorkState (2 bits per section)
    vec_condensedWorkStateCurrent[2*ui8_sectionId] = vec_sectionStatusTaskController[ui8_sectionId];
    vec_condensedWorkStateCurrent[2*ui8_sectionId + 1] = 0;
    // 2. set workstate. For now both setMasterMeasurementVal and setSetpointMasterVal call are required.
    arr_workStateProcData[ui8_sectionId]->setMasterMeasurementVal(static_cast<int32_t>(vec_sectionStatusCurrent[ui8_sectionId]));
  }

  // 3. set condensed workstate. For now both setMasterMeasurementVal and setSetpointMasterVal call are required.
  c_condensedWorkState.setMasterMeasurementVal(static_cast<int32_t>(vec_condensedWorkStateCurrent.to_ulong()));
}

bool
tcClientDynamic_c::processSetpointSet(
  IsoAgLib::EventSource_c ac_src, uint16_t ddi, int32_t ai32_val, const IsoAgLib::iIsoName_c& ac_callerISOName, bool /* ab_change*/ )
{
  //if (!ab_change) return false;

  std::cout << ac_src.makeIProcDataLocal()->DDI() << " " << ddi << std::endl;

  IterProcDataLocal iter = m_mapProcDataSetPoint.find(ac_src.makeIProcDataLocal());
  if (iter != m_mapProcDataSetPoint.end())
  {
    switch (iter->second)
    {
    case IndexSectionControlState: // GS2 Swath control
    {
      // send response to TC
      c_sectionControlState.setMasterMeasurementVal(ai32_val);
      c_sectionControlState.sendMasterMeasurementVal(ac_callerISOName); // force sending the message (see ISOBUS-11)
      break;
    }
    case IndexWorkStateSection01:
    case IndexWorkStateSection02:
    case IndexWorkStateSection03:
    case IndexWorkStateSection04:
    case IndexWorkStateSection05:
    case IndexWorkStateSection06:
    case IndexWorkStateSection07:
    case IndexWorkStateSection08:
    {
      uint16_t index = static_cast<uint16_t>(iter->second) - static_cast<uint16_t>(IndexWorkStateSection01);
      if (vec_sectionStatusTaskController[index] != (ai32_val & 0x1)) {
        vec_sectionStatusTaskController[index] = ai32_val & 0x1;
        
        updateSectionStatus( );
      }
      break;
    }
    case IndexCondensedWorkState:
      if (vec_condensedWorkStateTaskController.to_ulong() != static_cast<uint32_t>(ai32_val)) {
        vec_condensedWorkStateTaskController = ai32_val;

        // convert CondensedWorkState (2 bits per section) to our status array (1 bit per section)
        for (uint8_t sectionId = 0; sectionId < scui8_numberOfSection; ++sectionId) {
          // section is ON when condensedWorkState status is '01'
          vec_sectionStatusTaskController[sectionId] = vec_condensedWorkStateTaskController[sectionId*2] & !vec_condensedWorkStateTaskController[sectionId*2 + 1];
        }
        
        std::cout << "vec_condensedWorkStateTaskController " << vec_condensedWorkStateTaskController.to_string() << std::endl;
        std::cout << "vec_sectionStatusTaskController      " << vec_sectionStatusTaskController.to_string() << std::endl;

        updateSectionStatus( );
      }
      break;
    case IndexSetpointVolumeRate: // not processed in this tutorial
      break;

    }
  }
  return false;
}


void tcClientDynamic_c::resetSectionStatus() {
  // set all 16 sections in condensedWorkState as not available
  vec_condensedWorkStateTaskController.set();
  vec_condensedWorkStateCurrent.set();

  for (uint8_t sectionId = 0; sectionId < scui8_numberOfSection; ++sectionId) {
    // set sections from our system to disabled/off ( 00 )
    vec_condensedWorkStateCurrent[sectionId*2 + 0] = 0;
    vec_condensedWorkStateCurrent[sectionId*2 + 1] = 0;
  }
}


tcClientDynamic_c& getTcClientDynamic (void)
{
	static tcClientDynamic_c c_tcClientDynamic_c;
	return c_tcClientDynamic_c;
};


