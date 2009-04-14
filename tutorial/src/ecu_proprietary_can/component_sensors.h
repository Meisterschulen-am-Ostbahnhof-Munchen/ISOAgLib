#ifndef __COMPONENT_SENSORS_H
#define __COMPONENT_SENSORS_H 

#include "component_internal_can_protocol.h"

namespace IsoAgLibTutorialPropCan {

  class iPropCanSensor_c {
    public:
      virtual bool getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg ) = 0;
  };

  class SpeedSensor_c : public iPropCanSensor_c {
    public:
      SpeedSensor_c();
      ~SpeedSensor_c();
      bool getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg );
    private:
      uint16_t mui_speed;
      bool mb_updown;
      static const uint16_t mscui_step = 1;
      static const uint16_t mscui_minSpeed = 50;
      static const uint16_t mscui_maxSpeed = 300;
  };

  class DistanceSensor_c : public iPropCanSensor_c {
    public:
      DistanceSensor_c();
      ~DistanceSensor_c();
      bool getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg );
    private:
      uint32_t mui_distance;
  };

  class PtoSensor_c : public iPropCanSensor_c {
    public:
      typedef enum {
        modeFront = 0,
        modeRear,
      } PtoMode_e;

      PtoSensor_c();
      ~PtoSensor_c();
      bool getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg );
      void setMode( PtoMode_e a_mode );
    private:
      PtoMode_e m_mode;
      uint16_t mui_rpm;
      bool mb_updown;
      static const uint16_t mscui_step = 1;
      static const uint16_t mscui_minRpm = 540;
      static const uint16_t mscui_maxRpm = 1080;
  };

  class PowerSensor_c : public iPropCanSensor_c {
    public:
      PowerSensor_c();
      ~PowerSensor_c();
      bool getData( IsoAgLibTutorial::TutorialInternalPkg_c& ar_pkg );
    private:
      uint8_t mui_keySwitch;
  };
}
#endif
