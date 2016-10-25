/*
  vtobjectauxiliaryfunction2_c.h

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTAUXILIARYFUNCTION2_C_H
#define VTOBJECTAUXILIARYFUNCTION2_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef USE_VTOBJECT_auxiliaryfunction2

#include "vtobject_c.h"
#include "vttypes.h"

#include <list>

namespace __IsoAgLib {

class Aux2Functions_c;

class vtObjectAuxiliaryFunction2_c : public vtObject_c
{
public:
  vtObjectAuxiliaryFunction2_c();

  void init(const iVtObjectAuxiliaryFunction2_s* vtObjectAuxiliaryFunction2SROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  {vtObject_c::init((iVtObject_s *)vtObjectAuxiliaryFunction2SROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);}

  int16_t stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset);

  iVtObjectAuxiliaryFunction2_s* get_vtObjectAuxiliaryFunction2_a() {return (iVtObjectAuxiliaryFunction2_s *)&(get_vtObject_a());}

  uint32_t fitTerminal() const;
  void setOriginSKM(bool b_SKM);
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void overrideFunctionType_onlyAtStartup(uint8_t newValue);
  bool isAssigned() const;

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 31; }
  */
  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/) {};
#endif

private:

  void getAssignedInput(IsoAgLib::iIsoName_c& arc_isoName, uint16_t& arui16_inputUid) const;
  bool getPreferredAssignedInput(IsoAgLib::iIsoName_c& arc_isoName, uint16_t& arui16_inputModelIdentificationCode, uint16_t& arui16_inputUid) const;
  bool matchPreferredAssignedInput(const IsoAgLib::iIsoName_c& arc_isoName, uint16_t arui16_inputModelIdentificationCode);
  bool setAssignedInput(const IsoAgLib::iIsoName_c& arc_isoName, uint16_t a_modelIdentificationCode, uint16_t aui16_inputUid, bool a_preferredAssignment);
  bool unassignAfterTimeout(const IsoAgLib::iIsoName_c& arc_isoName);
  bool getMatchingPreferredAssignedInputReady();
  void addPreferredAssignedInputCandidate(const IsoAgLib::iAux2InputData& a_ref_input);
  const std::list<IsoAgLib::iAux2InputData>& getRefPreferredAssignmentCandidates() const { return ml_preferredAssignedInputCandidate; }
  void clearPreferredAssignments();
  
private:

  void removePreferredAssignedInputCandidate(const IsoAgLib::iAux2InputData& a_ref_input);
    
  bool hasPreferredAssigment() const;

  IsoAgLib::iAux2InputData ms_assignedInput;

  std::list<IsoAgLib::iAux2InputData> ml_preferredAssignedInputCandidate;

  friend class Aux2Functions_c;
};

}

#endif

#endif
