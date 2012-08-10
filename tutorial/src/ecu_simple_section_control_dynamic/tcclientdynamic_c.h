#ifndef TCCLIENT_DYNAMIC_C_H
#define TCCLIENT_DYNAMIC_C_H

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/scheduler/ischedulertask_c.h>
//#include <IsoAgLib/typedef.h>

#include <IsoAgLib/comm/Part10_TaskController_Client/proc_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatalocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocess_c.h>

#include <IsoAgLib/comm/Part10_TaskController_Client/impl/ddopbase_c.h>

#include <map>
#include <bitset>

/* Note: Brian Wei, 12/10/2010
	This project is created to demonstrate how to design a TC client with dynamic DDOP
	
	Dynamic DDOP generation steps:
		1. Derive the class from DdopBase_c and implement the abstract method initDevicePool() to init the device pools
		2. Call base class's timeEventDevicePool() method periodically (such as from this class's timeEvent() ) 
		   to allow DdopBase_c's state machine to run.
		3. Whenever machine geometry changed, such as nubmer of sections changed (setSection function in this class),
		   clear the existing DDOP and call initDevicePool() again to reconstruct the pool
				m_DevicePool.clear();
				initDevicePool();
		    This project have a very simple GUI that allows user to choose either 1 section or 3 sections.
			Change of sections will	trigger update and reload of DDOP to TC
		4. When system setting changes, update the DVP object and the DDOP will be reloaded automatically

	Process Data Communication Steps (Same as Static DDOP approach):
		1. Initialize DPD and set up process data change handler
		2. Derive three virtual methods so IsoAgLib state machine will route message process to your code
			virtual bool processDefaultLoggingStart(...)  //Called when TC sends default logging cmd
			virtual bool processTcStatusMessage(...)	//Called when TC status changed
			virtual bool processMeasurementReset(...)	//Other TC value command

		3. Set and Get DPD's value in application:
			Application code set value of a process data and query its value with the following two methods:
			ProcDataLocal_c:: setMasterMeasurementVal(uint32_t) //set value and IsoAgLib will send it to TC whenever it is requested or measurement threshold  reached
			int32_t ProcDataLocal_c:: masterMeasurementVal()	//query the measurement value from IsoAgLib

		4. Everything else is handled by IsoAgLib state machine automatically. 
*/	



class tcClientDynamic_c : public IsoAgLib::iSchedulerTask_c,
						              public IsoAgLib::ProcessDataChangeHandler_c,
						              public DdopBase_c
{
private:
  static const uint8_t scui8_numberOfSection = 8;
  enum SetPointIndex
  {
    IndexSectionControlState,
    IndexSetpointVolumeRate,
    IndexWorkStateSection01,
    IndexWorkStateSection02,
    IndexWorkStateSection03,
    IndexWorkStateSection04,
    IndexWorkStateSection05,
    IndexWorkStateSection06,
    IndexWorkStateSection07,
    IndexWorkStateSection08,
    IndexCondensedWorkState,
  };

public:
	void init(void);
	tcClientDynamic_c();
	virtual ~tcClientDynamic_c(){};


  //virtual bool processMeasurementReset(IsoAgLib::EventSource_c rc_src, uint16_t rui16_ddi, int32_t ri32_val, const IsoAgLib::iIsoName_c& rc_callerIsoName );
  //virtual bool processDefaultLoggingStart(IsoAgLib::EventSource_c ac_src, uint16_t rui16_ddi,int32_t ai32_val,const IsoAgLib::iIsoName_c& ac_callerIsoName);
  //virtual bool processSetpointSet( IsoAgLib::EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const IsoAgLib::iIsoName_c& ac_callerISOName, bool ab_change ) { return true; }
  //virtual bool processMeasurementUpdate( IsoAgLib::EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const IsoAgLib::iIsoName_c& ac_callerISOName, bool ab_change ) { return true; }
  //virtual bool processSetpointResponse( IsoAgLib::EventSource_c ac_src, uint16_t rui16_ddi, int32_t ai32_val, const IsoAgLib::iIsoName_c& ac_callerISOName ) { return true; }
  //virtual bool processTcStatusMessage(bool ab_taskRunning, const IsoAgLib::iIsoName_c& ac_callerISOName );
  virtual bool processDefaultLoggingStart( IsoAgLib::EventSource_c, int32_t, const IsoAgLib::iIsoName_c& ) { return false; }
  virtual bool processSetpointSet( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c&, bool );
  virtual bool processMeasurementReset( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c& );
  virtual bool processMeasurementUpdate( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c&, bool ) { return false; }
  virtual bool processSetpointResponse( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c& ) { return false; }
  virtual bool processDefaultLoggingStart( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c& );
  virtual bool processTcStatusMessage( bool, const IsoAgLib::iIsoName_c& );

/// IsoAgLib::iSchedulerTask_c implementation
	void close();
	bool timeEvent();
	const char* getTaskName() const	{ return "tcClientDynamic_c"; }
	void singletonInit(){init();};

	void InitDpdAndRegisterDDOP(IsoAgLib::iIdentItem_c* myIdent);

//	Called when a language/unit settings PGN is received from the VT.
//	Update the DVP entries in the device pool to match the terminal settings
	void updateUnitSettings(const localSettings_s& settings);

//  Called when number of sections changed to update DDOP dynamically
	void setSections(uint8_t); 

  virtual void updateEarlierAndLatestInterval();

private:
  void resetSectionStatus();
  void updateSectionStatus( );

private:
	void initDevicePool();

	localSettings_s	currentSettings;

	const static uint32_t c_schedulerPeriod = 200;

	//DPD definitions
  IsoAgLib::iProcDataLocal_c c_xOffset;
  IsoAgLib::iProcDataLocal_c c_actualWidth;
  IsoAgLib::iProcDataLocal_c c_maxWidth;
  IsoAgLib::iProcDataLocal_c c_setpointVolumeRate;
  IsoAgLib::iProcDataLocal_c c_actualVolumeRate;
  IsoAgLib::iProcDataLocal_c c_workState;
  IsoAgLib::iProcDataLocal_c c_sectionControlState;
  IsoAgLib::iProcDataLocal_c c_condensedWorkState;
  IsoAgLib::iProcDataLocal_c c_workStateSection01;
  IsoAgLib::iProcDataLocal_c c_workStateSection02;
  IsoAgLib::iProcDataLocal_c c_workStateSection03;
  IsoAgLib::iProcDataLocal_c c_workStateSection04;
  IsoAgLib::iProcDataLocal_c c_workStateSection05;
  IsoAgLib::iProcDataLocal_c c_workStateSection06;
  IsoAgLib::iProcDataLocal_c c_workStateSection07;
  IsoAgLib::iProcDataLocal_c c_workStateSection08;

	//@todo: if 3 sections are configured, work states needs to be defined and initialized
	//		 for left and right section too
	
	uint32_t mi32_effectiveTimeMs;	

	uint8_t m_sections;

  private:
    typedef std::map<IsoAgLib::iProcDataLocal_c*,SetPointIndex>   MapProcDataLocal;
    typedef std::pair<IsoAgLib::iProcDataLocal_c*,SetPointIndex>  PairProcDataLocal;
    typedef MapProcDataLocal::iterator                            IterProcDataLocal;
   
    MapProcDataLocal m_mapProcDataSetPoint;
    IsoAgLib::iProcDataLocal_c* arr_workStateProcData[8]; 

    // condensedWorkState received from TC
    std::bitset<32> vec_condensedWorkStateTaskController;
    // condensedWorkState to be sent to the TC
    std::bitset<32> vec_condensedWorkStateCurrent;

    // current section status
    std::bitset<scui8_numberOfSection> vec_sectionStatusCurrent;
    // wished status sent by the TaskController
    std::bitset<scui8_numberOfSection> vec_sectionStatusTaskController;
};

tcClientDynamic_c& getTcClientDynamic(void);

#endif
