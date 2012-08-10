#ifndef COMPONENT_TCCLIENT_H
#define COMPONENT_TCCLIENT_H

namespace IsoAgLib
{ // forward declarations
  class iIdentItem_c;
}

// IsoAgLib
#include <IsoAgLib/isoaglib_config.h>

// IsoAgLib
#include <IsoAgLib/comm/Part10_TaskController_Client/processdatachangehandler_c.h>


namespace IsoAgLibTutorialDataSource {

  /** General DataSource handler */
  class iDataSource_c {
    public:
      virtual void handleNewWorkStateData( bool ab_on ) = 0;
      virtual void handleNewSensorAData( int32_t ai32_newData ) = 0;
      virtual void handleNewSensorBData( int32_t ai32_newData ) = 0;
  };

  // DFFF-specific: also derive from ProcessDataChangeHandler_c to receive DFFF-requests
  class TutorialDataSource_c : public iDataSource_c, public IsoAgLib::ProcessDataChangeHandler_c {
    public:
      TutorialDataSource_c();
      ~TutorialDataSource_c();

      void init( IsoAgLib::iIdentItem_c &arc_identItem );

      /* iDataSource_c */
      virtual void handleNewWorkStateData( bool ab_on );
      virtual void handleNewSensorAData( int32_t ai32_newData );
      virtual void handleNewSensorBData( int32_t ai32_newData );

      /* IsoAgLib::ProcessDataChangeHandler_c */
      virtual void processDefaultLoggingStart( const IsoAgLib::iIsoName_c& ac_callerIsoName );
      virtual void processSetpointSet( IsoAgLib::EventSource_c, int32_t, const IsoAgLib::iIsoName_c&, bool ) {}
      virtual void processTcStatusMessage( bool, const IsoAgLib::iIsoName_c& ) {}
  };

} /* namespace IsoAgLibTutorialDataSource */

#endif
/* eof */
