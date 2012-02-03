/*
  vtobjectauxiliaryfunction2_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTAUXILIARYFUNCTION2_C_H
#define VTOBJECTAUXILIARYFUNCTION2_C_H
#include <IsoAgLib/hal/hal_typedef.h>

#ifdef USE_VTOBJECT_auxiliaryfunction2
// includes
#include "vtobject_c.h"
#include "vttypes.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Class : vtObjectAuxiliaryFunction2_c
class vtObjectAuxiliaryFunction2_c : public vtObject_c
{
public:

  // store assigned AUX inputs and preferred assigned AUX inputs in this structure
  struct AssignedInput_s
  {
    AssignedInput_s()
      : mc_inputIsoName(),
        mui16_inputUid(0xFFFF)
    {}

    bool operator==(const AssignedInput_s& arc_ref) const
    {
      if ( (mc_inputIsoName != arc_ref.mc_inputIsoName) ||
           (mui16_inputUid != arc_ref.mui16_inputUid) )
        return false;
      else
        return true;
    }

    IsoName_c mc_inputIsoName;
    uint16_t mui16_inputUid;
  };

  struct PreferredAssignedInput_s
  {
    PreferredAssignedInput_s()
      : mc_inputIsoName(),
        mui16_inputModelIdentificationCode(0xFFFF),
        mui16_inputUid(0xFFFF)
    {}

    bool operator==(const PreferredAssignedInput_s& arc_ref) const
    {
      if ( (mc_inputIsoName != arc_ref.mc_inputIsoName) ||
           (mui16_inputModelIdentificationCode != arc_ref.mui16_inputModelIdentificationCode) ||
           (mui16_inputUid != arc_ref.mui16_inputUid) )
        return false;
      else
        return true;
    }

    IsoName_c mc_inputIsoName;
    uint16_t mui16_inputModelIdentificationCode;
    uint16_t mui16_inputUid;
  };

  //  Operation: stream
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  //  Operation: init
  //! @param vtObjectAuxiliaryFunction2SROM:
  //! @param b_initPointer:
  void init(const iVtObjectAuxiliaryFunction2_s* vtObjectAuxiliaryFunction2SROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  {vtObject_c::init((iVtObject_s *)vtObjectAuxiliaryFunction2SROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);}

  //  Operation: get_vtObjectAuxiliaryFunction2_a
  iVtObjectAuxiliaryFunction2_s* get_vtObjectAuxiliaryFunction2_a() {return (iVtObjectAuxiliaryFunction2_s *)&(get_vtObject_a());}

  //  Operation: vtObjectAuxiliaryFunction2_c
  vtObjectAuxiliaryFunction2_c();

  ~vtObjectAuxiliaryFunction2_c() {}

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: setOriginSKM
  //! @param b_SKM:
  void setOriginSKM(bool b_SKM);

  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 31; }
  */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/) {};
#endif

  void getAssignedInput(IsoName_c& arc_isoName, uint16_t& arui16_inputUid) const;

  void getPreferredAssignedInput(IsoName_c& arc_isoName, uint16_t& arui16_inputModelIdentificationCode, uint16_t& arui16_inputUid) const;

  //! @return TRUE, if values are changed
  bool setAssignedInput(const IsoName_c& arc_isoName, uint16_t aui16_inputUid);

  //! @return TRUE, if values are changed
  bool unassignInputIfIsoNameMatches(const IsoName_c& arc_isoName);

  //! @return TRUE, if values are changed
  bool setPreferredAssignedInput(const IsoName_c& arc_isoName, uint16_t aui16_inputModelIdentificationCode, uint16_t aui16_inputUid);

  //! @return TRUE, if AUX2 input is assigned
  bool isAssigned() const;

  //! @return TRUE, if preferred assignment available
  bool hasPreferredAssigment() const;

  void setMatchingPreferredAssignedInputReady(bool a_isReady) { m_matchingPreferredAssignedInputReady = a_isReady; }
  bool getMatchingPreferredAssignedInputReady() { return m_matchingPreferredAssignedInputReady; }

private:

  AssignedInput_s ms_assignedInput;

  PreferredAssignedInput_s ms_preferredAssignedInput;

  bool m_matchingPreferredAssignedInputReady;

};

} // end of namespace __IsoAgLib

#endif
#endif
