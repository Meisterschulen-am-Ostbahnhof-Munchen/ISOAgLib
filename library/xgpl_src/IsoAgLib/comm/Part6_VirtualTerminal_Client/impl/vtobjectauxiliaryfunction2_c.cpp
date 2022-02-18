/*
  vtobjectauxiliaryfunction2_c.cpp

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

#include "vtobjectauxiliaryfunction2_c.h"

#ifdef CONFIG_USE_VTOBJECT_auxiliaryfunction2
#include "../ivtobjectfontattributes_c.h"
#include "../ivtobjectbutton_c.h"
#include "vtclient_c.h"

#include <algorithm>

namespace __IsoAgLib {


struct vtObjectAuxiliaryFunction2_c::iVtObjectAuxiliaryFunction2_s: iVtObjectObject_s {
	IsoAgLib::Colour backgroundColour;
	uint8_t functionAttributes;
	iVtObjectAuxiliaryFunction2_s(
			IsoAgLib::ObjectID ID,
			IsoAgLib::Colour backgroundColour,
			uint8_t functionAttributes)
    : iVtObject_s(ID)
	, iVtObjectObject_s(ID)
	, backgroundColour(backgroundColour)
	, functionAttributes(functionAttributes)
	{
	}


};


int16_t
vtObjectAuxiliaryFunction2_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
  uint16_t curBytes=0; /* current bytes written */;
  MACRO_scaleLocalVars;
  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 31; // Object Type = Auxiliary Function 2
    destMemory [3] = __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (vtObject_a->backgroundColour, this, IsoAgLib::BackgroundColour);
    destMemory [4] = vtObject_a->functionAttributes;
    destMemory [5] = vtObject_a->numberOfObjectsToFollow;
    sourceOffset += 6;
    curBytes += 6;
  }
  
  MACRO_streamObjectXYcenteredInSoftKey(6);
  
  return curBytes;
}


IsoAgLib::ObjectID vtObjectAuxiliaryFunction2_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectAuxiliaryFunction2_c::fitTerminal() const
{
  return 6+vtObject_a->numberOfObjectsToFollow*6;
}

bool
vtObjectAuxiliaryFunction2_c::moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildLocation (apc_childObject, dx, dy, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow);
}

bool
vtObjectAuxiliaryFunction2_c::setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t x, int16_t y, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  return genericChangeChildPosition (apc_childObject, x, y, b_updateObject, vtObject_a->numberOfObjectsToFollow, vtObject_a->objectsToFollow, b_enableReplaceOfCmd, SoftKeyOffset);
}

void
vtObjectAuxiliaryFunction2_c::overrideFunctionType_onlyAtStartup(uint8_t newValue)
{
  isoaglib_assert( vtObject_a != NULL );
  vtObject_a->functionAttributes = newValue;
}


void
vtObjectAuxiliaryFunction2_c::setOriginSKM(bool /* b_SKM */)
{
  for (int i=0; i<vtObject_a->numberOfObjectsToFollow; i++) {
    vtObject_a->objectsToFollow[i].vtObject->setOriginSKM (true);
  }
}


bool
vtObjectAuxiliaryFunction2_c::setAssignedInput(const IsoAgLib::iIsoName_c& arc_isoName, uint16_t a_modelIdentificationCode, uint16_t aui16_inputUid, bool a_preferredAssignment)
{
  const IsoAgLib::iAux2InputData assignedInputBackup = ms_assignedInput;
  IsoAgLib::iAux2InputData new_input(arc_isoName, a_modelIdentificationCode, aui16_inputUid, false /* fromUserInput */);

  if(a_preferredAssignment)
  {
    // remove/add for same input changes the list order 
    // => currently used input will won't be dropped at check agains CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION
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
  for(std::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
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
  // avoid any possible duplicates in list
  removePreferredAssignedInputCandidate(a_ref_input);

  ml_preferredAssignedInputCandidate.push_back(a_ref_input);

  if(ml_preferredAssignedInputCandidate.size() > CONFIG_MAX_AUX2_PREFERRED_ASSIGNMENT_PER_FUNCTION)
  {
    bool erased = false;
    // delete oldes entry (no preset)
    for(std::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
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
  std::list<IsoAgLib::iAux2InputData>::iterator iter = std::find(ml_preferredAssignedInputCandidate.begin(), ml_preferredAssignedInputCandidate.end(), a_ref_input);
  if(iter != ml_preferredAssignedInputCandidate.end())
  {
    ml_preferredAssignedInputCandidate.erase(iter);
  }
}

bool
vtObjectAuxiliaryFunction2_c::matchPreferredAssignedInput(const IsoAgLib::iIsoName_c& arc_isoName,
                                                          uint16_t aui16_inputModelIdentificationCode)
{
  std::list<IsoAgLib::iAux2InputData>::iterator iter_fuzzy = ml_preferredAssignedInputCandidate.end();
  for(std::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
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

    if( iter->name.isFunctionallyIdentical( arc_isoName ) )
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
  for(std::list<IsoAgLib::iAux2InputData>::iterator iter = ml_preferredAssignedInputCandidate.begin();
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
  for(std::list<IsoAgLib::iAux2InputData>::const_iterator iter = ml_preferredAssignedInputCandidate.begin();
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


bool
vtObjectAuxiliaryFunction2_c::clearPreferredAssignments()
{
  const bool clearedSomething = !ml_preferredAssignedInputCandidate.empty();
  
  ml_preferredAssignedInputCandidate.clear();

  return clearedSomething;
}

    vtObjectAuxiliaryFunction2_c::vtObjectAuxiliaryFunction2_c(
    		vtObjectAuxiliaryFunction2_c::iVtObjectAuxiliaryFunction2_s *vtObjectAuxiliaryFunction2SROM,
            multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectAuxiliaryFunction2SROM)
    {}

    vtObjectAuxiliaryFunction2_c::~vtObjectAuxiliaryFunction2_c() = default;


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    void vtObjectAuxiliaryFunction2_c::saveReceivedAttribute(uint8_t, uint8_t *) {}
#endif
    const std::list<IsoAgLib::iAux2InputData> &vtObjectAuxiliaryFunction2_c::getRefPreferredAssignmentCandidates() const { return ml_preferredAssignedInputCandidate; }

    uint8_t vtObjectAuxiliaryFunction2_c::getFunctionType() {
    	return vtObject_a->functionAttributes;
    }


} // __IsoAgLib
#endif
