#ifndef COMPONENT_TCCLIENT_H
#define COMPONENT_TCCLIENT_H

// IsoAgLib
#include <IsoAgLib/typedef.h>

// IsoAgLib (DFFF-specific)
#include <IsoAgLib/comm/Part7_ProcessData/processdatachangehandler_c.h>


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

      void init();

      /* iDataSource_c */
      virtual void handleNewWorkStateData( bool ab_on );
      virtual void handleNewSensorAData( int32_t ai32_newData );
      virtual void handleNewSensorBData( int32_t ai32_newData );

      /* IsoAgLib::ProcessDataChangeHandler_c */
	  virtual bool processDefaultLoggingStart( IsoAgLib::EventSource_c ac_src, int32_t ai32_val, const IsoAgLib::iIsoName_c& ac_callerIsoName );
  };

} /* namespace IsoAgLibTutorialDataSource */

#endif
/* eof */
