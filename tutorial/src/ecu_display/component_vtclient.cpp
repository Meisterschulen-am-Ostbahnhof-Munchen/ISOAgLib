#include "tutorial_settings.h"
#include "component_vtclient.h"

#include "component_vtclient_pool/tutorialDisplay_derived-cpp.h"

extern IsoAgLibTutorialDisplay::TutorialDisplay_c* p_display;


/*
 * Normally the application class could directly derive from "iDisplay_c",
 * but in this tutorial we would have to use multiple inheritance to do so,
 * which is bad on some exotic embedded compilers, so we let the events be
 * forwarded from the "static tutorialPool" to our "TutorialDisplay_c" "p_display".
 */
static IsoAgLibTutorialDisplay::iDisplayImplementation_c sc_display;


/*
 * Tutorial Display implementation
 * -------------------------------
 *
 * Here the main work is done of this component
 *
 * */
IsoAgLibTutorialDisplay::TutorialDisplay_c::TutorialDisplay_c()
  : mp_srcHandle( 0 )
  , mui_activeMaskId( 0xFFFF ) // use 0xFFFF as default because it's not a valid Mask-ID.
{}


IsoAgLibTutorialDisplay::TutorialDisplay_c::~TutorialDisplay_c()
{}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::init( IsoAgLib::iIdentItem_c& ar_ident )
{
  mp_srcHandle = IsoAgLib::getIisoTerminalInstance().initAndRegisterIsoObjectPool( ar_ident, *this, NULL );
}


IsoAgLibTutorialDisplay::iDisplay_c*
IsoAgLibTutorialDisplay::TutorialDisplay_c::getDisplayHandler()
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
IsoAgLibTutorialDisplay::TutorialDisplay_c::eventKeyCode( uint8_t , uint16_t , uint16_t , uint8_t , bool )
{
  // This feature is not used in this simple ECU.
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::eventStringValue( uint16_t , uint8_t , StreamInput_c& , uint8_t, bool , bool )
{
  // This feature is not used in this simple ECU.
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::eventNumericValue( uint16_t, uint8_t, uint32_t )
{
  // This feature is not used in this simple ECU.
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::eventObjectPoolUploadedSuccessfully( bool /* ab_wasLanguageUpdate */, int8_t /* ai8_languageIndex */, uint16_t /* aui16_languageCode */ )
{
  clearValues();
  setMetaInfo();
  iVtObjectwsTutorialDisplay.changeActiveMask( &iVtObjectdmAbout, false, false ); /* make sure about the mask */
  maskChanged( iVtObjectIDdmAbout ); // initialize the stored current mask
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::eventEnterSafeState()
{
  // This feature is not used in the simple ECU
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::eventVtStatusMsg()
{
  if ( 0 != mp_srcHandle ) {
    uint16_t amId = mp_srcHandle->getVtServerInst().getVtState()->dataAlarmMask;
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
IsoAgLibTutorialDisplay::TutorialDisplay_c::maskChanged( uint16_t aui_activeMaskId )
{
  // keep track of the current mask to avoid unnecessary updates to objects in hidden masks.
  mui_activeMaskId = aui_activeMaskId;
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::clearValues()
{
  /// @todo SOON-81: Doesn't make too much sense because the active mask doesn't match!
  handleNewPosValues( 0, 0 );
  handleNewTimeValues( 0, 0, 0 );
  handleNewGeneralValues( false, 0 );
  handleNewMoveValues( 0, 0, 0, 0 );
  handleNewPto( 0, false, 0, false );
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::setMetaInfo()
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

void IsoAgLibTutorialDisplay::iDisplayImplementation_c::handleNewPosValues( uint32_t aui_lat, uint32_t aui_lon ) { p_display->handleNewPosValues( aui_lat, aui_lon); } 
void IsoAgLibTutorialDisplay::iDisplayImplementation_c::handleNewTimeValues( uint8_t aui_h, uint8_t aui_m, uint8_t aui_s ) { p_display->handleNewTimeValues( aui_h, aui_m, aui_s ); }
void IsoAgLibTutorialDisplay::iDisplayImplementation_c::handleNewGeneralValues( bool ab_keySwitch, uint8_t aui_maxPower ) { p_display->handleNewGeneralValues( ab_keySwitch, aui_maxPower ); }
void IsoAgLibTutorialDisplay::iDisplayImplementation_c::handleNewMoveValues( int16_t ai_speedReal, int16_t ai_speedTheor, uint32_t aui_distReal, uint32_t aui_distTheor ) { p_display->handleNewMoveValues( ai_speedReal, ai_speedTheor, aui_distReal, aui_distTheor ); }
void IsoAgLibTutorialDisplay::iDisplayImplementation_c::handleNewPto( int32_t ai_ptoFront, bool ab_frontEngaged, int32_t ai_ptoRear, bool ab_rearEngaged ) { p_display->handleNewPto( ai_ptoFront, ab_frontEngaged, ai_ptoRear, ab_rearEngaged ); }


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::handleNewPosValues( uint32_t aui_lat, uint32_t aui_lon )
{
  if ( mui_activeMaskId == iVtObjectIDdmTimePos ) {
    iVtObjectnvLat.setValue( aui_lat, false, true );
    iVtObjectnvLon.setValue( aui_lon, false, true );
  }
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::handleNewTimeValues( uint8_t aui_h, uint8_t aui_m, uint8_t aui_s )
{
  if ( mui_activeMaskId == iVtObjectIDdmAbout ) {
    iVtObjectnvHr.setValue( aui_h, false, true );
    iVtObjectnvMin.setValue( aui_m, false, true );
    iVtObjectnvSec.setValue( aui_s, false, true );
  }
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::handleNewGeneralValues( bool ab_keySwitch, uint8_t aui_maxPower )
{
  if ( mui_activeMaskId == iVtObjectIDdmTecu ) {
    iVtObjectopKeySwitch.setValue(( ab_keySwitch ? &iVtObjectelStateGreen : &iVtObjectelStateRed ) , false, true );
    iVtObjectnvMaxPowerTime.setValue( aui_maxPower, false, true );
  }
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::handleNewMoveValues( int16_t ai_speedReal, int16_t ai_speedTheor, uint32_t aui_distReal, uint32_t aui_distTheor )
{
  if ( mui_activeMaskId == iVtObjectIDdmTecu ) {
    iVtObjectnvSpeedReal.setValue( ai_speedReal, false, true );
    iVtObjectnvSpeedTheor.setValue( ai_speedTheor, false, true );
    iVtObjectnvDistReal.setValue( aui_distReal / 1000, false, true );
    iVtObjectnvDistTheor.setValue( aui_distTheor / 1000, false, true );
  }
}


void
IsoAgLibTutorialDisplay::TutorialDisplay_c::handleNewPto( int32_t ai_ptoFront, bool ab_frontEngaged, int32_t ai_ptoRear, bool ab_rearEngaged )
{
  if ( mui_activeMaskId == iVtObjectIDdmTecu ) {
    iVtObjectnvPtoFront.setValue( ai_ptoFront, false, true );
    iVtObjectopPtoFrontEngaged.setValue( ab_frontEngaged ? &iVtObjectelStateGreen : &iVtObjectelStateRed, false, true );
    iVtObjectnvPtoRear.setValue( ai_ptoRear, false, true );
    iVtObjectopPtoRearEngaged.setValue( ab_rearEngaged ? &iVtObjectelStateGreen : &iVtObjectelStateRed, false, true );
  }
}

/* eof tutorialDisplay_c */
