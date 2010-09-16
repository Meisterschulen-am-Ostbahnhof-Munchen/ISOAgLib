/** \example component_vtclient.cpp
 * This is the VT-Client-component used in the Display ECU of the Tutorial ECU Network.
 *
 * It handles complete communication with a Virtual Terminal including
 * receiving events from it and updating of values on it.
 */

#include "tutorial_settings.h"
#include "component_vtclient.h"

#include "component_vtclient_pool/tutorialDisplay_derived-cpp.h"

extern IsoAgLibTutorialDisplay::TutorialDisplay_c *p_display;

namespace {

/*
 * Normally the application class could directly derive from "iDisplay_c",
 * but in this tutorial we would have to use multiple inheritance to do so,
 * which is bad on some exotic embedded compilers, so we let the events be
 * forwarded from the "static tutorialPool" to our "TutorialDisplay_c" "p_display".
 */
IsoAgLibTutorialDisplay::iDisplayImplementation_c sc_display;

inline uint8_t depth1Colour(uint8_t aui8_colorValue)
{
  switch (aui8_colorValue) {
  case 19: return 15;
  case 20: return 9;
  case 40: return 10;
  case 42: return 3;
  case 209: return 8;
  case 222: return 1;
  case 229: return 7;
  default:;    
  }
  return aui8_colorValue;
}

inline uint8_t depth0Colour(uint8_t aui8_colorValue)
{
  switch (aui8_colorValue) {
  case 3: return 1;
  default:;    
  }
  return aui8_colorValue;
}

} //namespace

namespace IsoAgLibTutorialDisplay {

/*
 * Tutorial Display implementation
 * -------------------------------
 *
 * Here the main work is done of this component
 *
 * */
TutorialDisplay_c::TutorialDisplay_c()
  : mp_srcHandle( 0 )
  , mp_srcIdent( 0 )
  , mui_activeMaskId( 0xFFFF ) // use 0xFFFF as default because it's not a valid Mask-ID.
{}


TutorialDisplay_c::~TutorialDisplay_c()
{
  if (mp_srcIdent)
  { // registered, so we need to deregister
    IsoAgLib::getIisoTerminalInstance().deregisterIsoObjectPool( *mp_srcIdent );
  }
}


void
TutorialDisplay_c::init( IsoAgLib::iIdentItem_c& ar_ident )
{
  mp_srcHandle = IsoAgLib::getIisoTerminalInstance().initAndRegisterIsoObjectPool( ar_ident, *this, NULL );
  if (mp_srcHandle)
  { // registering succeeded, store IdentItem for deregistration
    mp_srcIdent = &ar_ident;
  }
}


iDisplay_c*
TutorialDisplay_c::getDisplayHandler()
{
  return &sc_display;
}


/* 
 * TutorialDisplay VT-Client needed implementation
 * -----------------------------------------------
 *
 * Here the incoming events from the VT-Server are handled.
 *
 */

void
TutorialDisplay_c::eventObjectPoolUploadedSuccessfully( bool /* ab_wasLanguageUpdate */, int8_t /* ai8_languageIndex */, uint16_t /* aui16_languageCode */ )
{
  setMetaInfo();
  iVtObjectwsTutorialDisplay.changeActiveMask( &iVtObjectdmAbout, false, false ); /* make sure about the mask */
  maskChanged( iVtObjectIDdmAbout ); // initialize the stored current mask
}


void
TutorialDisplay_c::eventVtStatusMsg()
{
  //! This function is needed to keep track of the active mask
  //! as it is changed via Macros in this somponent's objectpool.
  if ( 0 != mp_srcHandle ) {
    const uint16_t amId = mp_srcHandle->getVtServerInst().getVtState()->dataAlarmMask;
    if ( amId != mui_activeMaskId )
    { // a (macro-triggered) mask change has been detected
      maskChanged( amId );
    }
  }
}


/* 
 * TutorialDisplay VT-Client own implementation
 * --------------------------------------------
 *
 * Here the application specific logic is handled.
 *
 */

void
TutorialDisplay_c::maskChanged( uint16_t aui_activeMaskId )
{
  // keep track of the current mask to avoid unnecessary updates to objects in hidden masks.
  mui_activeMaskId = aui_activeMaskId;
}


void
TutorialDisplay_c::setMetaInfo()
{
  iVtObjectosIsoaglibVersion.setValueCopy( IsoAgLibTutorial::scp_isoaglibVersion, true );
  iVtObjectosTutorialVersion.setValueCopy( IsoAgLibTutorial::scp_tutorialVersion, true );
  iVtObjectosBuildDate.setValueCopy( __DATE__ , true );
}


/*
 * Tutorial Display iDisplay implementation
 * ----------------------------------------
 *
 * Here the incoming value updates are handled.
 *
 * */

void iDisplayImplementation_c::handleNewPosValues( uint32_t aui_lat, uint32_t aui_lon ) { p_display->handleNewPosValues( aui_lat, aui_lon); } 
void iDisplayImplementation_c::handleNewTimeValues( uint8_t aui_h, uint8_t aui_m, uint8_t aui_s ) { p_display->handleNewTimeValues( aui_h, aui_m, aui_s ); }
void iDisplayImplementation_c::handleNewGeneralValues( bool ab_keySwitch, uint8_t aui_maxPower ) { p_display->handleNewGeneralValues( ab_keySwitch, aui_maxPower ); }
void iDisplayImplementation_c::handleNewMoveValues( int16_t ai_speedReal, int16_t ai_speedTheor, uint32_t aui_distReal, uint32_t aui_distTheor ) { p_display->handleNewMoveValues( ai_speedReal, ai_speedTheor, aui_distReal, aui_distTheor ); }
void iDisplayImplementation_c::handleNewPto( int32_t ai_ptoFront, bool ab_frontEngaged, int32_t ai_ptoRear, bool ab_rearEngaged ) { p_display->handleNewPto( ai_ptoFront, ab_frontEngaged, ai_ptoRear, ab_rearEngaged ); }


void
TutorialDisplay_c::handleNewPosValues( uint32_t aui_lat, uint32_t aui_lon )
{
  if ( mui_activeMaskId == iVtObjectIDdmTimePos ) {
    iVtObjectnvLat.setValue( aui_lat, false, true );
    iVtObjectnvLon.setValue( aui_lon, false, true );
  }
}


void
TutorialDisplay_c::handleNewTimeValues( uint8_t aui_h, uint8_t aui_m, uint8_t aui_s )
{
  if ( mui_activeMaskId == iVtObjectIDdmAbout ) {
    iVtObjectnvHr.setValue( aui_h, false, true );
    iVtObjectnvMin.setValue( aui_m, false, true );
    iVtObjectnvSec.setValue( aui_s, false, true );
  }
}


void
TutorialDisplay_c::handleNewGeneralValues( bool ab_keySwitch, uint8_t aui_maxPower )
{
  if ( mui_activeMaskId == iVtObjectIDdmTecu ) {
    iVtObjectopKeySwitch.setValue(( ab_keySwitch ? &iVtObjectelStateGreen : &iVtObjectelStateRed ) , false, true );
    iVtObjectnvMaxPowerTime.setValue( aui_maxPower, false, true );
  }
}


void
TutorialDisplay_c::handleNewMoveValues( int16_t ai_speedReal, int16_t ai_speedTheor, uint32_t aui_distReal, uint32_t aui_distTheor )
{
  if ( mui_activeMaskId == iVtObjectIDdmTecu ) {
    iVtObjectnvSpeedReal.setValue( ai_speedReal, false, true );
    iVtObjectnvSpeedTheor.setValue( ai_speedTheor, false, true );
    iVtObjectnvDistReal.setValue( aui_distReal / 1000, false, true );
    iVtObjectnvDistTheor.setValue( aui_distTheor / 1000, false, true );
  }
}


void
TutorialDisplay_c::handleNewPto( int32_t ai_ptoFront, bool ab_frontEngaged, int32_t ai_ptoRear, bool ab_rearEngaged )
{
  if ( mui_activeMaskId == iVtObjectIDdmTecu ) {
    iVtObjectnvPtoFront.setValue( ai_ptoFront, false, true );
    iVtObjectopPtoFrontEngaged.setValue( ab_frontEngaged ? &iVtObjectelStateGreen : &iVtObjectelStateRed, false, true );
    iVtObjectnvPtoRear.setValue( ai_ptoRear, false, true );
    iVtObjectopPtoRearEngaged.setValue( ab_rearEngaged ? &iVtObjectelStateGreen : &iVtObjectelStateRed, false, true );
  }
}


/* Convert "bad" colors before handing over to default color
 * mapping. */
uint8_t TutorialDisplay_c::convertColour(
    uint8_t aui8_colorValue,
    uint8_t aui8_colorDepth,
    IsoAgLib::iVtObject_c *p_obj,
    IsoAgLib::e_vtColour ae_whichColour)
{
  uint8_t const cui8_convertedColorStage1 = aui8_colorDepth > 1 ?
    aui8_colorValue :
    depth1Colour(aui8_colorValue);

  uint8_t const cui8_convertedColorStage2 = aui8_colorDepth > 0 ?
    cui8_convertedColorStage1 :
    depth0Colour(cui8_convertedColorStage1);

  return convertColourDefault(
      cui8_convertedColorStage2,
      aui8_colorDepth,
      p_obj,
      ae_whichColour);
}

} //namespace IsoAgLibTutorialDisplay

/* eof tutorialDisplay_c */
