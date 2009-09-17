#ifndef COMPONENT_SIMULATEDSOURCE_H
#define COMPONENT_SIMULATEDSOURCE_H

#include "component_tcclient.h"

// IsoAgLib
#include <IsoAgLib/scheduler/ischedulertask_c.h>


namespace IsoAgLibTutorialDataSource {

  class TutorialDataSourceSimulation_c: public IsoAgLib::iSchedulerTask_c {
    public:
      TutorialDataSourceSimulation_c();
      ~TutorialDataSourceSimulation_c();

      void init();

      void registerDataSource( iDataSource_c& ar_dataSource );
      void unRegisterDataSource( iDataSource_c& ar_dataSource );

      /* iSchedulerTask_c */
      virtual bool timeEvent( void );
      virtual void close( void );
      virtual const char* getTaskName() const;

    private:
      int32_t mi32_simulatedSensorA;
      int32_t mi32_simulatedSensorB;
      bool mb_simulatedWorkState;

      std::vector<iDataSource_c*> mvec_dataSource;
  };
}
#endif
