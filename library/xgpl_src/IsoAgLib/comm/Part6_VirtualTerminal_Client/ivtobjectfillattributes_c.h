/*
  ivtobjectfillattributes_c.h

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
#ifndef IVTOBJECTFILLATTRIBUTES_C_H
#define IVTOBJECTFILLATTRIBUTES_C_H

#include "impl/vtobjectfillattributes_c.h"
#ifdef CONFIG_USE_VTOBJECT_fillattributes

#include "ivtobjectpicturegraphic_c.h"


namespace IsoAgLib {

class iVtObjectFillAttributes_c : public __IsoAgLib::vtObjectFillAttributes_c
{
private:
    iVtObjectFillAttributes_c() = delete;
public:


    iVtObjectFillAttributes_c(
    		iVtClientObjectPool_c* pool,
    		ObjectID ID = autoID,
    		FillType fillType = fillColour,
    		Colour fillColour = RED,
    		iVtObjectPictureGraphic_c *fillPatternObject = nullptr);


   ~iVtObjectFillAttributes_c() override;
  static uint16_t objectType();
  void setFillType(FillType newFillType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFillColour(Colour newFillColour, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;

#ifdef CONFIG_USE_VTOBJECT_picturegraphic
  void setFillPattern(iVtObjectPictureGraphic_c* newFillPatternObject, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setFillAttributes(FillType newFillType, Colour newFillColour, iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
#endif


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectFillAttributes_c::updateObjectType(); }
  */

  FillType updateFillType(bool b_SendRequest=false) override;
  Colour updateFillColour(bool b_SendRequest=false) override;
  iVtObjectPictureGraphic_c* updateFillPattern(bool b_SendRequest=false) override;
#endif
  uint16_t getObjectType() const override;
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_fillattributes

#endif //IVTOBJECTFILLATTRIBUTES_C_H
