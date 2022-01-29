/*
  vtobjectarchedbargraph_c.h

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
#ifndef VTOBJECTARCHEDBARGRAPH_C_H
#define VTOBJECTARCHEDBARGRAPH_C_H

#include <IsoAgLib/isoaglib_config.h>




#include "vtobject_c.h"
#include "vtclientconnection_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectArchedBarGraph_c : public vtObject_c
{
public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectArchedBarGraph_c(const iVtObjectArchedBarGraph_s* vtObjectArchedBarGraphSROM , int ai_multitonInst);


  iVtObjectArchedBarGraph_s* get_vtObjectArchedBarGraph_a();

  ~vtObjectArchedBarGraph_c() override = default;

  uint32_t fitTerminal() const;

    virtual void setValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true);

    virtual void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setColour(uint8_t newColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setTargetLineColour(uint8_t newTargetLineColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setOptions(uint8_t newOptions, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setBarGraphWidth(uint16_t newBarGraphWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setMinValue(uint16_t newMin, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

    virtual void setMaxValue(uint16_t newMax, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  void setVariableReference(IsoAgLib::iVtObject_c* newVariableReference, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), variableReference) : 0, sizeof(iVtObjectArchedBarGraph_s), 11 /* "Variable Reference" */, newVariableReference, b_enableReplaceOfCmd);
  }

  void setTargetValueVariableReference(IsoAgLib::iVtObject_c* newTargetValueVariableReference, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValueVariableReference) : 0, sizeof(iVtObjectArchedBarGraph_s), 12 /* "Target Value Variable Reference" */, newTargetValueVariableReference, b_enableReplaceOfCmd);
  }

  void setTargetValue(uint16_t newTargetValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectArchedBarGraph_a(), targetValue) : 0, sizeof(iVtObjectArchedBarGraph_s), 13 /* "Target Value" */, newTargetValue, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 19; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint8_t updateColour(bool b_SendRequest=false);
  uint8_t updateTargetLineColour(bool b_SendRequest=false);
  uint8_t updateOptions(bool b_SendRequest=false);
  uint8_t updateStartAngle(bool b_SendRequest=false);
  uint8_t updateEndAngle(bool b_SendRequest=false);
  uint16_t updateBarGraphWidth(bool b_SendRequest=false);
  uint16_t updateMinValue(bool b_SendRequest=false);
  uint16_t updateMaxValue(bool b_SendRequest=false);
  uint16_t updateVariableReference(bool b_SendRequest=false);
  uint16_t updateTargetValueVariableReference(bool b_SendRequest=false);
  uint16_t updateTargetValue(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif


