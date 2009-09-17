#ifndef COMPONENT_VTCLIENT_H
#define COMPONENT_VTCLIENT_H

#include <IsoAgLib/scheduler/ischedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientservercommunication_c.h>

#include "component_vtclient_pool/tutorialDisplay_derived-h.h"


namespace IsoAgLibTutorialDisplay {

  class TutorialPool_c: public iObjectPool_tutorialDisplay_c {
    public:
  };

  /** General Display handler */
  class iDisplay_c {
    public:
      virtual void handleNewPosValues( uint32_t aui_lat, uint32_t aui_lon ) = 0;
      virtual void handleNewTimeValues( uint8_t aui_h, uint8_t aui_m, uint8_t aui_s ) = 0;
      virtual void handleNewGeneralValues( bool ab_keySwitch, uint8_t aui_maxPower ) = 0;
      virtual void handleNewMoveValues( int16_t ai_speedReal, int16_t ai_speedTheor, uint32_t aui_distReal, uint32_t aui_distTheor ) = 0;
      virtual void handleNewPto( int32_t ai_ptoFront, bool ab_frontEngaged, int32_t ai_ptoRear, bool ab_rearEngaged ) = 0;
  };

  class iDisplayImplementation_c : public iDisplay_c {
    public:
      virtual void handleNewPosValues( uint32_t aui_lat, uint32_t aui_lon );
      virtual void handleNewTimeValues( uint8_t aui_h, uint8_t aui_m, uint8_t aui_s );
      virtual void handleNewGeneralValues( bool ab_keySwitch, uint8_t aui_maxPower );
      virtual void handleNewMoveValues( int16_t ai_speedReal, int16_t ai_speedTheor, uint32_t aui_distReal, uint32_t aui_distTheor );
      virtual void handleNewPto( int32_t ai_ptoFront, bool ab_frontEngaged, int32_t ai_ptoRear, bool ab_rearEngaged );
  };


  class TutorialDisplay_c : public iObjectPool_tutorialDisplay_c {
    public:
      TutorialDisplay_c();
      ~TutorialDisplay_c();

      void init( IsoAgLib::iIdentItem_c& ar_ident );

      //! Initializes run-time MetaInformation for this ECU
      void setMetaInfo();
      //! Notify this component on the current active mask
      void maskChanged( uint16_t aui_activeMaskId );

      iDisplay_c* getDisplayHandler();

      /* iObjectPool_tutorialDisplay_c */
      void eventKeyCode( uint8_t , uint16_t , uint16_t , uint8_t , bool ) { /* not used in this simple tutorial */ }
      void eventStringValue( uint16_t , uint8_t , StreamInput_c& , uint8_t, bool , bool ) { /* not used in this simple tutorial */ }
      void eventNumericValue( uint16_t, uint8_t, uint32_t ) { /* not used in this simple tutorial */ }
      void eventVtStatusMsg();
      void eventObjectPoolUploadedSuccessfully( bool ab_wasLanguageUpdate, int8_t ai8_languageIndex, uint16_t aui16_languageCode );
      void eventEnterSafeState() { /* not used in this simple tutorial */ }

      /* iDisplayImplementation_c */
      virtual void handleNewPosValues( uint32_t aui_lat, uint32_t aui_lon );
      virtual void handleNewTimeValues( uint8_t aui_h, uint8_t aui_m, uint8_t aui_s );
      virtual void handleNewGeneralValues( bool ab_keySwitch, uint8_t aui_maxPower );
      virtual void handleNewMoveValues( int16_t ai_speedReal, int16_t ai_speedTheor, uint32_t aui_distReal, uint32_t aui_distTheor );
      virtual void handleNewPto( int32_t ai_ptoFront, bool ab_frontEngaged, int32_t ai_ptoRear, bool ab_rearEngaged );

    private:
      IsoAgLib::iVtClientServerCommunication_c* mp_srcHandle;

      uint16_t mui_activeMaskId;
  };

} /* namespace IsoAgLibTutorialDisplay */

#endif
/* eof */
