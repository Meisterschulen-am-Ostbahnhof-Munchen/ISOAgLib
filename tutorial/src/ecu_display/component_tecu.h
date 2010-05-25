#ifndef __COMPONENT_TECU_H
#define __COMPONENT_TECU_H

#include "component_vtclient.h"

#include <IsoAgLib/scheduler/ischedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

#include <vector>


namespace IsoAgLibTutorialDisplay {

  class TutorialDisplayTecu_c: public IsoAgLib::iSchedulerTask_c {
    public:
      TutorialDisplayTecu_c();
      ~TutorialDisplayTecu_c();

      void init( IsoAgLib::iIdentItem_c* ap_ident );
      void registerDisplay( iDisplay_c* ap_display );
      void unRegisterDisplay( iDisplay_c* ap_display );

      /* iSchedulerTask_c */
      virtual bool timeEvent( void );
      virtual void close( void );
      virtual const char* getTaskName() const;

    private:
      struct MoveCtn_s {
        MoveCtn_s() : speedReal( 0 ), speedTheor( 0 ), distReal( 0 ), distTheor( 0 ) {}
        int16_t speedReal;
        int16_t speedTheor;
        uint32_t distReal;
        uint32_t distTheor;
        bool operator!=( const MoveCtn_s& x ) {
          return (( x.speedReal != speedReal ) || ( x.speedTheor != speedTheor ) || ( x.distReal != distReal ) || ( x.distTheor != distTheor ) );
        }
      };

      struct PtoCtn_s {
        PtoCtn_s() : ptoFront( 0 ), ptoFrontEngaged( false ), ptoRear( 0 ), ptoRearEngaged( false ) {}
        int32_t ptoFront;
        bool ptoFrontEngaged;
        int32_t ptoRear;
        bool ptoRearEngaged;
        bool operator!=( const PtoCtn_s& x ) {
          return (( x.ptoFront != ptoFront ) || ( x.ptoFrontEngaged != ptoFrontEngaged ) || ( x.ptoRear != ptoRear ) || ( x.ptoRearEngaged != ptoRearEngaged ) );
        }
      };

      struct GeneralCtn_s {
        GeneralCtn_s() : keySwitch( false ), maxPowerTime( 0 ) {}
        bool keySwitch;
        uint8_t maxPowerTime;
        bool operator!=( const GeneralCtn_s& x ) {
          return (( x.keySwitch != keySwitch ) || ( x.maxPowerTime != maxPowerTime ) );
        }
      };

      MoveCtn_s ms_lastMoveData;
      PtoCtn_s ms_lastPtoData;
      GeneralCtn_s ms_lastGeneralData;

      void checkMoveUpdate( bool ab_force = false );
      void sendVtMoveValues( const MoveCtn_s& ac_m );

      void sendVtPtoValues( const PtoCtn_s& c );
      void checkPtoUpdate( bool ab_force = false );

      void sendVtGeneralValues( const GeneralCtn_s& c );
      void checkGeneralUpdate( bool ab_force = false );

      std::vector<iDisplay_c*> mvec_display;
      uint8_t mui8_countdownMaintainPower;
  };
}
#endif
