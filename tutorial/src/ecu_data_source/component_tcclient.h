#ifndef COMPONENT_TCCLIENT_H
#define COMPONENT_TCCLIENT_H

namespace IsoAgLib
{ // forward declarations
  class iIdentItem_c;
}

// IsoAgLib
#include <IsoAgLib/isoaglib_config.h>

// IsoAgLib
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdatahandler_c.h>


namespace IsoAgLibTutorialDataSource {

  /** General DataSource handler */
  class iDataSource_c {
    public:
      virtual void handleNewWorkStateData( bool ab_on ) = 0;
      virtual void handleNewSensorAData( int32_t ai32_newData ) = 0;
      virtual void handleNewSensorBData( int32_t ai32_newData ) = 0;
  };

  // DFFF-specific: also derive from ProcessDataChangeHandler_c to receive DFFF-requests
  class TutorialDataSource_c : public iDataSource_c, public IsoAgLib::iProcDataHandler_c {
    public:
      TutorialDataSource_c();
      ~TutorialDataSource_c();

      void init( IsoAgLib::iIdentItem_c &arc_identItem );

      /* iDataSource_c */
      virtual void handleNewWorkStateData( bool ab_on );
      virtual void handleNewSensorAData( int32_t ai32_newData );
      virtual void handleNewSensorBData( int32_t ai32_newData );

      /* IsoAgLib::ProcessDataChangeHandler_c */
      virtual void processDefaultLoggingStart( IsoAgLib::ProcData::remoteType_t a_ecuType );
      virtual void processSetpointSet( IsoAgLib::iProcData_c&, int32_t, IsoAgLib::ProcData::remoteType_t a_ecuType, bool ) {}
      virtual void processTcStatusMessage( bool, IsoAgLib::ProcData::remoteType_t a_ecuType ) {}
  };

} /* namespace IsoAgLibTutorialDataSource */

#endif
/* eof */
