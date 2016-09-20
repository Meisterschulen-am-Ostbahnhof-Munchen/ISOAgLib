/*
  vtobjectauxiliaryfunction2_c.cpp

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "vtobjectauxiliaryfunction2_c.h"

#ifdef USE_VTOBJECT_auxiliaryfunction2
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "vtclient_c.h"

#include <algorithm>

namespace __IsoAgLib {

int16_t
vtObjectAuxiliaryFunction2_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectAuxiliaryFunction2_a
#define MACRO_vtObjectTypeS iVtObjectAuxiliaryFunction2_s
  MACRO_streamLocalVars;
  MACRO_scaleLocalVars;
  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 31; // Object Type = Auxiliary Function 2
    destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObjectAuxiliaryFunction2_a->backgroundColour, this, IsoAgLib::BackgroundColour);
    destMemory [4] = vtObjectAuxiliaryFunction2_a->functionType;
    destMemory [5] = vtObjectAuxiliaryFunction2_a->numberOfObjectsToFollow;
    sourceOffset += 6;
    curBytes += 6;
  }
  
  MACRO_streamObjectXYcenteredInSoftKey(6);
  
  return curBytes;
}


vtObjectAuxiliaryFunction2_c::vtObjectAuxiliaryFunction2_c()
{}

uint32_t
vtObjectAuxiliaryFunction2_c::fitTerminal() const
{
  MACRO_localVars;
  return 6+vtObjectAuxiliaryFunction2_a->numberOfObjectsToFollow*6;
}

bool
vtObjectAuxiliaryFunction2_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObjectAuxiliaryFunction2_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectAuxiliaryFunction2_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectAuxiliaryFunction2_a(), objectsToFollow), sizeof(iVtObjectAuxiliaryFunction2_s), b_enableReplaceOfCmd);
}

bool
vtObjectAuxiliaryFunction2_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  MACRO_localVars;
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObjectAuxiliaryFunction2_a->numberOfObjectsToFollow, const_cast<IsoAgLib::repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *> (vtObjectAuxiliaryFunction2_a->objectsToFollow), MACRO_getStructOffset(get_vtObjectAuxiliaryFunction2_a(), objectsToFollow), sizeof(iVtObjectAuxiliaryFunction2_s), b_enableReplaceOfCmd);
}

void
vtObjectAuxiliaryFunction2_c::overrideFunctionType_onlyAtStartup(uint8_t newValue)
{
  isoaglib_assert( vtObject_a != NULL );

  saveValue8 (MACRO_getStructOffset(get_vtObjectAuxiliaryFunction2_a(), functionType), sizeof(iVtObjectAuxiliaryFunction2_s), newValue);
}


void
vtObjectAuxiliaryFunction2_c::setOriginSKM(bool /* b_SKM */)
{
  MACRO_localVars;
  for (int i=0; i<vtObjectAuxiliaryFunction2_a->numberOfObjectsToFollow; i++) {
    vtObjectAuxiliaryFunction2_a->objectsToFollow[i].vtObject->setOriginSKM (true);
  }
}


bool
vtObjectAuxiliaryFunction2_c::setAssignedInput(const IsoAgLib::iIsoName_c& arc_isoName, uint16_t a_modelIdentificationCode, uint16_t aui16_inputUid, bool a_preferredAssignment)
{
  const IsoAgLib::iAux2InputData assignedInputBackup = ms_assignedInput;
  IsoAgLib::iAux2InputData new_input(arc_isoName, a_modelIdentificationCode, aui16_inputUid, false /* fromUserInput */);

  if(a_preferredAssignment)
  {
    removePreferredAssignedInputCandidate(ms_assignedInput);
  }
  
  if (aui16_inputUid != 0xFFFF)
  { // store assignment
    if(a_preferredAssignment)
    {
      addPreferredAssignedInputCandidate(new_input);
    }
    ms_assignedInput.name = arc_isoName;
    ms_assignedInput.modelIdentificationCode = a_modelIdentificationCode;
  }
  else
  { // remove assignment
    ms_assignedInput.name = IsoAgLib::iIsoName_c::iIsoNameUnspecified();
    ms_assignedInput.modelIdentificationCode = 0xFFFF;
  }

  ms_assignedInput.uid = aui16_inputUid;

  // true if changed
  return !(assignedInputBackup == ms_assignedInput);
}

bool
vtObjectAuxiliaryFunction2_c::unassignAfterTimeout(const IsoAgLib::iIsoName_c& arc_isoName)
{
  // reset any matching for preferred assignment for this arc_isoName
  for(STL_NAMESPACE::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
      iter != ml_preferredAssignedInputCandidate.end();
      ++iter)
  {
    if(arc_isoName == iter->name)
    {
      iter->preferredAssignmentMatched = false;
    }
  }
    
  if (arc_isoName != ms_assignedInput.name)
    return false; 
  else
    return setAssignedInput(arc_isoName, ms_assignedInput.modelIdentificationCode, 0xFFFF, false /* a_preferredAssignment */); // unassign;
}

void
vtObjectAuxiliaryFunction2_c::addPreferredAssignedInputCandidate(const IsoAgLib::iAux2InputData& a_ref_input)
{
  ml_preferredAssignedInputCandidate.push_back(a_ref_input);

  if(ml_preferredAssignedInputCandidate.size() > CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION)
  {
    bool erased = false;
    // delete oldes entry (no preset)
    for(STL_NAMESPACE::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
      iter != ml_preferredAssignedInputCandidate.end();
      ++iter)
    {
      if(!iter->preserve)
      {
        ml_preferredAssignedInputCandidate.erase(iter);
        erased = true;
        break;
      }
    }

    if(!erased)
    {
       ml_preferredAssignedInputCandidate.pop_front(); 
    }
  }

}

void vtObjectAuxiliaryFunction2_c::removePreferredAssignedInputCandidate(const IsoAgLib::iAux2InputData& a_ref_input)
{
  STL_NAMESPACE::list<IsoAgLib::iAux2InputData>::iterator iter = STL_NAMESPACE::find(ml_preferredAssignedInputCandidate.begin(), ml_preferredAssignedInputCandidate.end(), a_ref_input);
  if(iter != ml_preferredAssignedInputCandidate.end())
  {
    ml_preferredAssignedInputCandidate.erase(iter);
  }
}

bool
vtObjectAuxiliaryFunction2_c::matchPreferredAssignedInput(const IsoAgLib::iIsoName_c& arc_isoName,
                                                          uint16_t aui16_inputModelIdentificationCode)
{
  STL_NAMESPACE::list<IsoAgLib::iAux2InputData>::iterator iter_fuzzy = ml_preferredAssignedInputCandidate.end();
  for(STL_NAMESPACE::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
      iter != ml_preferredAssignedInputCandidate.end();
      ++iter)
  {
    if(aui16_inputModelIdentificationCode != iter->modelIdentificationCode)
      continue;
      
    if( iter->name == arc_isoName )
    {
      iter->preferredAssignmentMatched = true;
      return true;
    }

    if( iter->name.isEqualRegardingNonInstFields( arc_isoName ) )
    {
      iter_fuzzy = iter;
    }    
  }

  if(iter_fuzzy != ml_preferredAssignedInputCandidate.end())
  {
    iter_fuzzy->name = arc_isoName;
    // use this input from now on as preferred input
    iter_fuzzy->preferredAssignmentMatched = true;
    return true;
  }
  
  return false;
}

bool vtObjectAuxiliaryFunction2_c::getMatchingPreferredAssignedInputReady()
{
  for(STL_NAMESPACE::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
      iter != ml_preferredAssignedInputCandidate.end();
      ++iter)
  {
    if(iter->preferredAssignmentMatched)
    {
      return true;
    }
  }
  return false;
}

bool
vtObjectAuxiliaryFunction2_c::getPreferredAssignedInput(IsoAgLib::iIsoName_c& arc_isoName, uint16_t& arui16_inputModelIdentificationCode, uint16_t& arui16_inputUid) const
{
{
  for(STL_NAMESPACE::list<IsoAgLib::iAux2InputData>::const_iterator iter = ml_preferredAssignedInputCandidate.begin();
      iter != ml_preferredAssignedInputCandidate.end();
      ++iter)
  {
      if(iter->preferredAssignmentMatched)
      {
          arc_isoName = iter->name;
          arui16_inputModelIdentificationCode = iter->modelIdentificationCode;
          arui16_inputUid = iter->uid;
          return true;
      }
    }
  }
  return false;
}

bool
vtObjectAuxiliaryFunction2_c::isAssigned() const
{
  return (0xFFFF != ms_assignedInput.uid);
}

void
vtObjectAuxiliaryFunction2_c::getAssignedInput(IsoAgLib::iIsoName_c& arc_isoName, uint16_t& arui16_inputUid) const
{
  arc_isoName = ms_assignedInput.name;
  arui16_inputUid = ms_assignedInput.uid;
}


bool
vtObjectAuxiliaryFunction2_c::hasPreferredAssigment() const
{
  return !ml_preferredAssignedInputCandidate.empty();
}


} // __IsoAgLib
#endif
