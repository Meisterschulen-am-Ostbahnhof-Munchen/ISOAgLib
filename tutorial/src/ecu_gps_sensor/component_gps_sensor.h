#ifndef __COMPONENT_GPS_SENSOR_H
#define __COMPONENT_GPS_SENSOR_H

#include <IsoAgLib/scheduler/ischedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

#define DEF_MSEC_PER_SEC 1000
#define DEF_SEC_PER_MIN 60
#define DEF_MIN_PER_H 60
#define DEF_H_PER_D 24

namespace IsoAgLibTutorialGpsSensor {

  struct LatLon_s {
    uint32_t lat;
    uint32_t lon;
  };

  class TutorialGpsSensor_c : public IsoAgLib::iSchedulerTask_c {
    public:
      TutorialGpsSensor_c();
      ~TutorialGpsSensor_c();

      void init( IsoAgLib::iIdentItem_c* ap_ident );

      /* iSchedulerTask_c */
      virtual bool timeEvent( void );
      virtual void close( void );
      virtual const char* getTaskName() const;
      void updateEarlierAndLatestInterval();

    private:
      uint16_t mui_pos;
      bool updateGpsPos();
      bool updateGpsTime();
  };

} /* namespace IsoAgLibTutorialGpsSensor */
#endif
