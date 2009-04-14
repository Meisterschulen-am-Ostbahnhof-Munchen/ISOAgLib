#ifndef __COMPONENT_TECU_PROP_CAN
#define __COMPONENT_TECU_PROP_CAN


#include <IsoAgLib/driver/can/icancustomer_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>

#include "component_internal_can_protocol.h"

namespace IsoAgLibTutorialTecu {

  class TutorialTecu_c : public IsoAgLib::iCanCustomer_c {
    public:
      TutorialTecu_c();
      ~TutorialTecu_c();

      void init( IsoAgLib::iIdentItem_c* ap_ident );
      
      /* iCanCustomer_c */
      bool processMsg();
      IsoAgLib::iCanPkgExt_c& iDataBase();

    private:

      IsoAgLibTutorial::TutorialInternalPkg_c mc_internalData;
  };
} /* namespace IsoAgLibTutorialTecu */
#endif
/* eof tutorialTecu_c.h */
