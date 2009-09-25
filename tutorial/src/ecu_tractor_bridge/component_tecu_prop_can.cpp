#include "component_tecu_prop_can.h"
#include "tutorial_settings.h"

#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracgeneral_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracmove_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/itracpto_c.h>

IsoAgLibTutorialTecu::TutorialTecu_c::TutorialTecu_c() { }

IsoAgLibTutorialTecu::TutorialTecu_c::~TutorialTecu_c() { }

void IsoAgLibTutorialTecu::TutorialTecu_c::init( IsoAgLib::iIdentItem_c* ap_ident ) {
  /* initialize tractor classes */
  IsoAgLib::getITracGeneralInstance().config( &(ap_ident->isoName()), IsoAgLib::IdentModeTractor );
  IsoAgLib::getITracMoveInstance().config( &(ap_ident->isoName()), IsoAgLib::IdentModeTractor );
  IsoAgLib::getITracPtoInstance().config( &(ap_ident->isoName()), IsoAgLib::IdentModeTractor );

  /* reset tractor data to initial values */
  IsoAgLib::getITracGeneralInstance().setKeySwitch( IsoAgLib::IsoActiveFlag_t( 0 ) );
  IsoAgLib::getITracGeneralInstance().setMaxPowerTime( 0 );

  IsoAgLib::getITracMoveInstance().setSpeedReal( 0 );
  IsoAgLib::getITracMoveInstance().setSpeedTheor( 0 );
  IsoAgLib::getITracMoveInstance().setDistReal( 0 );
  IsoAgLib::getITracMoveInstance().setDistTheor( 0 );

  IsoAgLib::getITracPtoInstance().setPtoFront( 0 );
  IsoAgLib::getITracPtoInstance().setPtoFrontEngaged( IsoAgLib::IsoActiveFlag_t( 0 ) );
  IsoAgLib::getITracPtoInstance().setPtoRear( 0 );
  IsoAgLib::getITracPtoInstance().setPtoRearEngaged( IsoAgLib::IsoActiveFlag_t( 0 ) );
}

IsoAgLib::iCanPkgExt_c& IsoAgLibTutorialTecu::TutorialTecu_c::iDataBase() {
  return mc_internalData;
}

bool IsoAgLibTutorialTecu::TutorialTecu_c::processMsg() {

  IsoAgLib::iTracGeneral_c& c_tracGeneral = IsoAgLib::getITracGeneralInstance();
  IsoAgLib::iTracMove_c& c_tracMove = IsoAgLib::getITracMoveInstance();
  IsoAgLib::iTracPTO_c& c_tracPto = IsoAgLib::getITracPtoInstance();

  switch ( mc_internalData.ident() ) {
    case IsoAgLibTutorial::InternalSpeed:
      c_tracMove.setSpeedReal( mc_internalData.mu_tracData.Speed.real );
      c_tracMove.setSpeedTheor( mc_internalData.mu_tracData.Speed.theor );
      return true;
    case IsoAgLibTutorial::InternalDistance:
      c_tracMove.setDistReal( mc_internalData.mu_tracData.Distance.real );
      c_tracMove.setDistTheor( mc_internalData.mu_tracData.Distance.theor );
      return true;
    case IsoAgLibTutorial::InternalPtoFront:
      c_tracPto.setPtoFront( mc_internalData.mu_tracData.PtoFront.front );
      c_tracPto.setPtoFrontEngaged( IsoAgLib::IsoActiveFlag_t( mc_internalData.mu_tracData.PtoFront.engaged ) );
      return true;
    case IsoAgLibTutorial::InternalPtoRear:
      c_tracPto.setPtoRear(  mc_internalData.mu_tracData.PtoRear.rear );
      c_tracPto.setPtoRearEngaged( IsoAgLib::IsoActiveFlag_t( mc_internalData.mu_tracData.PtoFront.engaged ) );
      return true;
    case IsoAgLibTutorial::InternalPower:
      c_tracGeneral.setKeySwitch( IsoAgLib::IsoActiveFlag_t(mc_internalData.mu_tracData.Power.keySwitch ) );
      c_tracGeneral.setMaxPowerTime( mc_internalData.mu_tracData.Power.powerMaxTime );
      return true;
  }

  return true;
}
