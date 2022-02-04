/*
  vtobjectmeter_c.h

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
#ifndef VTOBJECTMETER_C_H
#define VTOBJECTMETER_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectMeter_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectMeter_c(const iVtObjectMeter_s* vtObjectMeterSROM , int ai_multitonInst);
  iVtObjectMeter_s* get_vtObjectMeter_a();
  vtObjectMeter_c();
  uint32_t fitTerminal() const;

  void setWidth(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setNeedleColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setBorderColour(Colour newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setArcAndTickColour(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setOptions(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setStartAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setEndAngle(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setMin(uint16_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setMax(uint16_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setVariableReference(IsoAgLib::iVtObject_c* newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 17; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint8_t updateNeedleColour(bool b_SendRequest=false);
  uint8_t updateBorderColour(bool b_SendRequest=false);
  uint8_t updateArcAndTickColour(bool b_SendRequest=false);
  uint8_t updateOptions(bool b_SendRequest=false);
  uint8_t updateNumberOfTicks(bool b_SendRequest=false);
  uint8_t updateStartAngle(bool b_SendRequest=false);
  uint8_t updateEndAngle(bool b_SendRequest=false);
  uint16_t updateMinValue(bool b_SendRequest=false);
  uint16_t updateMaxValue(bool b_SendRequest=false);
  uint16_t updateVariableReference(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif


