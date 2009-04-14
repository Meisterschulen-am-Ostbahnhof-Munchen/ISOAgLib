#ifndef __COMPONENT_TIMEPOS_H
#define __COMPONENT_TIMEPOS_H

#include "component_vtclient.h"

#include <vector>

#include <IsoAgLib/scheduler/ischedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

namespace IsoAgLibTutorialDisplay {

  class TutorialDisplayTimePos_c: public IsoAgLib::iSchedulerTask_c {
    public:
      TutorialDisplayTimePos_c();
      ~TutorialDisplayTimePos_c();

      void init( IsoAgLib::iIdentItem_c* ap_ident );

      void registerDisplay( iDisplay_c* ap_display );
      void unRegisterDisplay( iDisplay_c* ap_display );

      /* iSchedulerTask_c */
      virtual bool timeEvent( void );
      virtual void close( void );
      virtual const char* getTaskName() const;

    private:

      struct PosCtn_s {
        PosCtn_s() : lat( 0 ), lon( 0 ) {}
        int32_t lat;
        int32_t lon;
        bool operator!=( const PosCtn_s& x ) {
          return (( x.lat != lat ) || ( x.lon != lon ) );
        }
      };

      PosCtn_s ms_lastPos;

      struct TimeCtn_s {
        TimeCtn_s() : h( 0 ), m( 0 ), s( 0 )  {}
        uint8_t h;
        uint8_t m;
        uint8_t s;
        bool operator!=( const TimeCtn_s& x ) {
          return (( x.h != h ) || ( x.m != m ) || ( x.s != s ) );
        }
      };

      TimeCtn_s ms_lastTime;

      void checkPosUpdate( bool ab_force = false );
      void sendVtPosValues( const PosCtn_s& c );
      void checkTimeUpdate( bool ab_force = false );
      void sendVtTimeValues( const TimeCtn_s& c );


      std::vector<iDisplay_c*> mvec_display;
  };

}
#endif
