#ifndef COMPONENT_TCCLIENT_H
#define COMPONENT_TCCLIENT_H

// IsoAgLib
#include <IsoAgLib/typedef.h>


namespace IsoAgLibTutorialDataSource {

  /** General Display handler */
  class iDataSource_c {
    public:
      virtual void handleNewWorkStateData( bool ab_on ) = 0;
      virtual void handleNewSensorAData( int32_t ai32_newData ) = 0;
      virtual void handleNewSensorBData( int32_t ai32_newData ) = 0;
  };

  class TutorialDataSource_c : public iDataSource_c {
    public:
      TutorialDataSource_c();
      ~TutorialDataSource_c();

      void init();

      /* iDataSource_c */
      virtual void handleNewWorkStateData( bool ab_on );
      virtual void handleNewSensorAData( int32_t ai32_newData );
      virtual void handleNewSensorBData( int32_t ai32_newData );
  };

} /* namespace IsoAgLibTutorialDataSource */

#endif
/* eof */
