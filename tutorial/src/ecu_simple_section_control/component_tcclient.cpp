#ifndef USE_DYNAMIC_TCCLIENT

#include "tutorial_settings.h"
#include "component_tcclient.h"

// IsoAgLib
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocess_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatalocal_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

// device description object pool
#include "component_tcclient_pool/tutorialSimpleSectionControl-func.h"

// debug
#include <IsoAgLib/util/iassert.h>

namespace {

static IsoAgLib::iProcDataLocal_c* arr_workStateProcData[] = {
  &c_workStateSection01,
  &c_workStateSection02,
  &c_workStateSection03,
  &c_workStateSection04,
  &c_workStateSection05,
  &c_workStateSection06,
  &c_workStateSection07,
  &c_workStateSection08,
};

}

IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c::TutorialSectionControlTc_c() :
  vec_condensedWorkStateTaskController( ),
  vec_condensedWorkStateCurrent( ),
  vec_sectionStatusCurrent( 0xFF ), // default sections status ON
  vec_sectionStatusTaskController( 0xFF )
{
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
}


IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c::~TutorialSectionControlTc_c()
{}


void
IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c::init( IsoAgLib::iIdentItem_c &arc_identItem )
{
  const bool cb_registerSuccess =
    IsoAgLib::getIProcessInstance().getDevPropertyHandlerInstance().registerDevicePool(
      &arc_identItem, deviceDescription_en, ui32_arrayLength_en, true);

  (void)cb_registerSuccess;
  isoaglib_assert(cb_registerSuccess);

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


void
IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c::updateSectionStatus( )
{
  for (uint8_t ui8_sectionId = 0; ui8_sectionId < scui8_numberOfSection; ++ui8_sectionId) {
    // 1.1 recompute current section status
    vec_sectionStatusCurrent[ui8_sectionId] = vec_sectionStatusTaskController[ui8_sectionId];
    // 1.2 convert to CondensedWorkState (2 bits per section)
    vec_condensedWorkStateCurrent[2*ui8_sectionId] = vec_sectionStatusTaskController[ui8_sectionId];
    vec_condensedWorkStateCurrent[2*ui8_sectionId + 1] = 0;
    // 2. set workstate. For now both setMasterMeasurementVal and setSetpointMasterVal call are required.
    arr_workStateProcData[ui8_sectionId]->setMasterMeasurementVal(static_cast<int32_t>(vec_sectionStatusCurrent[ui8_sectionId]));
    //arr_workStateProcData[ui8_sectionId]->seexistMaster()tMasterMeasurementVal(static_cast<int32_t>(vec_sectionStatusCurrent[ui8_sectionId]));
  }

  // 3. set condensed workstate. For now both setMasterMeasurementVal and setSetpointMasterVal call are required.
  c_condensedWorkState.setMasterMeasurementVal(static_cast<int32_t>(vec_condensedWorkStateCurrent.to_ulong()));
  //c_condensedWorkState.setSetpointVal(static_cast<int32_t>(vec_condensedWorkStateCurrent.to_ulong()));
}

bool
IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c::processSetpointSet(
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

      std::cout << "IndexSectionControlState " << ai32_val << std::endl;

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


void IsoAgLibTutorialSectionControl::TutorialSectionControlTc_c::resetSectionStatus() {
  // set all 16 sections in condensedWorkState as not available
  vec_condensedWorkStateTaskController.set();
  vec_condensedWorkStateCurrent.set();

  for (uint8_t sectionId = 0; sectionId < scui8_numberOfSection; ++sectionId) {
    // set sections from our system to disabled/off ( 00 )
    vec_condensedWorkStateCurrent[sectionId*2 + 0] = 0;
    vec_condensedWorkStateCurrent[sectionId*2 + 1] = 0;
  }
}

#endif

