#ifndef COMPONENT_TCCLIENT_H
#define COMPONENT_TCCLIENT_H

#ifndef USE_DYNAMIC_TCCLIENT

namespace IsoAgLib
{ // forward declarations
  class iIdentItem_c;
}

// IsoAgLib
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatahandler_c.h>

#include <map>
#include <bitset>

namespace IsoAgLibTutorialSectionControl {

  class TutorialSectionControlTc_c : public IsoAgLib::iProcDataHandler_c
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
      TutorialSectionControlTc_c();
      ~TutorialSectionControlTc_c();

      void init( IsoAgLib::iIdentItem_c &arc_identItem );
      
      /* IsoAgLib::iProcDataHandler_c */
      /* IsoAgLib::iProcDataHandler_c */
      virtual void processSetpointSet( IsoAgLib::iProcData_c& procdata, int32_t, IsoAgLib::ProcData::remoteType_t, bool );
      virtual void processDefaultLoggingStart( IsoAgLib::ProcData::remoteType_t ecuType ) {}
      virtual void processTcConnected( IsoAgLib::ProcData::remoteType_t a_ecuType, const IsoAgLib::iIsoName_c& tcname) {}

      virtual void processTaskStarted( IsoAgLib::ProcData::remoteType_t ecuType ) {}
      virtual void processTaskStopped( IsoAgLib::ProcData::remoteType_t ecuType ) {}


    private:
      void resetSectionStatus();
      void updateSectionStatus( );

    private:
      typedef std::map<IsoAgLib::iProcData_c*,SetPointIndex>   MapProcDataLocal;
      typedef std::pair<IsoAgLib::iProcData_c*,SetPointIndex>  PairProcDataLocal;
      typedef MapProcDataLocal::iterator                       IterProcDataLocal;
     
      MapProcDataLocal m_mapProcDataSetPoint;

      // condensedWorkState received from TC
      std::bitset<32> vec_condensedWorkStateTaskController;
      // condensedWorkState to be sent to the TC
      std::bitset<32> vec_condensedWorkStateCurrent;

      // current section status
      std::bitset<scui8_numberOfSection> vec_sectionStatusCurrent;
      // wished status sent by the TaskController
      std::bitset<scui8_numberOfSection> vec_sectionStatusTaskController;

  };

} /* namespace IsoAgLibTutorialSectionControl */

#endif
/* eof */

#endif

