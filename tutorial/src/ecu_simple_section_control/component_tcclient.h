#ifndef COMPONENT_TCCLIENT_H
#define COMPONENT_TCCLIENT_H

#ifndef USE_DYNAMIC_TCCLIENT

namespace IsoAgLib
{ // forward declarations
  class iIdentItem_c;
}

// IsoAgLib
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>

#include <map>
#include <bitset>

namespace IsoAgLibTutorialSectionControl {

  class TutorialSectionControlTc_c : public IsoAgLib::ProcessDataChangeHandler_c {

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
      
      /* IsoAgLib::ProcessDataChangeHandler_c */
      virtual bool processDefaultLoggingStart( IsoAgLib::EventSource_c, int32_t, const IsoAgLib::iIsoName_c& ) { return false; }
      virtual bool processSetpointSet( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c&, bool );
      virtual bool processMeasurementReset( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c& ) { return false; }
      virtual bool processMeasurementUpdate( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c&, bool ) { return false; }
      virtual bool processSetpointResponse( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c& ) { return false; }
      virtual bool processDefaultLoggingStart( IsoAgLib::EventSource_c, uint16_t, int32_t, const IsoAgLib::iIsoName_c& ) { return false; }
      virtual bool processTcStatusMessage( bool, const IsoAgLib::iIsoName_c& ) { return false; }

    private:
      void resetSectionStatus();
      void updateSectionStatus( );

    private:
      typedef std::map<IsoAgLib::iProcDataLocal_c*,SetPointIndex>   MapProcDataLocal;
      typedef std::pair<IsoAgLib::iProcDataLocal_c*,SetPointIndex>  PairProcDataLocal;
      typedef MapProcDataLocal::iterator                            IterProcDataLocal;
     
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

