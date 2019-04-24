/*
  vtobjectauxiliaryinput2_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTAUXILIARYINPUT2_C_H
#define VTOBJECTAUXILIARYINPUT2_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef USE_VTOBJECT_auxiliaryinput2

#include "vtobject_c.h"
#include "vttypes.h"

namespace __IsoAgLib {

class vtObjectAuxiliaryInput2_c : public vtObject_c
{
public:
  enum StateForLearnMode_en
  {
    StateForLearnMode_CalculateInternally,
    StateForLearnMode_SetActive,
    StateForLearnMode_SetInactive
  };

#ifdef CONFIG_VT_CLIENT_AUX2INPUTS_FORCE_OFF_STATE_BETWEEN_ON_MESSAGES
  enum StateForNextMessage_en
  {
    StateForNextMessageInactive,
    StateForNextMessageOff,
    StateForNextMessageOn
  };
#endif

  // store information about AUX input
  struct InputState_s
  {
    InputState_s()
      : mb_enabled(false), mi32_timeStampLastStateMsg(0)
    {}

    bool mb_enabled;
    ecutime_t mi32_timeStampLastStateMsg;
  };

  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  void init(const iVtObjectAuxiliaryInput2_s* vtObjectAuxiliaryInput2SROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA);

  iVtObjectAuxiliaryInput2_s* get_vtObjectAuxiliaryInput2_a() { return (iVtObjectAuxiliaryInput2_s *)&(get_vtObject_a()); }

  vtObjectAuxiliaryInput2_c();

  uint32_t fitTerminal() const;

  void setOriginSKM(bool b_SKM);

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 32; }
  */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/) {};
#endif

  void setInputStateEnabled(bool ab_enabled);
  void setTimeStampLastStateMsg();

  /**
    when first status message is sent after setValue the non latching boolean objects have to send a different value1
    for signaling "held" position
  */
  void updateValueForHeldPositionAfterFirstMsg();

  bool getInputStateEnabled() const { return m_inputState.mb_enabled; }

  bool sendNextStatusAsSoonAsPossible() const { return mb_valueChangeToHandle; }

  bool highUpdateRateActive() const { return mb_highStatusUpdateRate; }

  ecutime_t getTimeStampLastStateMsg() const  { return m_inputState.mi32_timeStampLastStateMsg; }

  uint16_t getValue1() const { return mui16_value1; }
  uint16_t getValue2() const { return mui16_value2; }

  bool getInputActivatedInLearnMode() const { return mb_inputActivatedInLearnMode; }

  /**
    set new value and trigger AUX2 input state message
    @paramter aen_stateForLearnMode
       - default value: the active state is calculated from aui16_value1 != 0
       - StateForLearnMode_SetActive: set state to activated
       - StateForLearnMode_SetInactive: set state to not activated
    @return true if range of value matches the expected range for this function type
  */
  bool setValue(uint16_t aui16_value1, uint16_t aui16_value2, StateForLearnMode_en aen_stateForLearnMode);

  uint8_t getFunctionType() { return (getValue8(MACRO_getStructOffset(get_vtObjectAuxiliaryInput2_a(), functionAttributes), 0 /* ui16_structLen */) & 0x1F); }

private:

  enum FunctionType_en 
  {
    FunctionType_LatchingBoolean                             = 0,
    FunctionType_Analog                                      = 1,
    FunctionType_NonLatchingBoolean                          = 2,
    FunctionType_AnalogReturnTo50                            = 3,
    FunctionType_AnalogReturnTo0                             = 4,
    FunctionType_BothLatchingDualBoolean                     = 5,
    FunctionType_BothNonLatchingDualBoolean                  = 6,
    FunctionType_LatchingDualBooleanUp                       = 7,
    FunctionType_LatchingDualBooleanDown                     = 8,
    FunctionType_CombinedAnalogReturnTo50DualLatchingBoolean = 9,
    FunctionType_CombinedAnalogDualLatchingBoolean           = 10,
    FunctionType_NonLatchingQuadBoolean                      = 11,
    FunctionType_AnalogQuad                                  = 12,
    FunctionType_AnalogReturnTo50Quad                        = 13,
    FunctionType_Encoder                                     = 14
  };

  InputState_s m_inputState;

  uint16_t mui16_value1;
  uint16_t mui16_value2;

#ifdef CONFIG_VT_CLIENT_AUX2INPUTS_FORCE_OFF_STATE_BETWEEN_ON_MESSAGES
  StateForNextMessage_en m_nextMessageState;
#endif

  // set this value to true when the input is "activated" via setValue()
  bool mb_inputActivatedInLearnMode;

  // non latching booleans have to send status at higher update rate when pressed
  bool mb_highStatusUpdateRate;

  // when mui16_value1 or mui16_value2 change:
  // - send changed values as soon as possible
  bool mb_valueChangeToHandle;

};

} // __IsoAgLib

#endif

#endif
