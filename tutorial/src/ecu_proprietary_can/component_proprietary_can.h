#ifndef COMPONENT_PROPRIETARY_CAN_H
#define COMPONENT_PROPRIETARY_CAN_H

#include "component_sensors.h"
#include "component_internal_can_protocol.h"

#include <IsoAgLib/scheduler/ischedulertask_c.h>

namespace IsoAgLibTutorialPropCan {

  class TutorialPropCan_c : public IsoAgLib::iSchedulerTask_c {
    public:
      TutorialPropCan_c();
      ~TutorialPropCan_c();

      void init();

      void registerSensor( iPropCanSensor_c* ap_sensor );

      virtual bool timeEvent( void );
      virtual void close( void );
      virtual const char* getTaskName() const;
      void updateEarlierAndLatestInterval();

    private:
      IsoAgLibTutorial::TutorialInternalPkg_c mc_internalData;
      std::vector<iPropCanSensor_c*> mvec_sensors;

  };
}
#endif
