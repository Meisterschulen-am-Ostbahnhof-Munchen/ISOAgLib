/*
  ivtobjectellipse_c.h

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
#ifndef IVTOBJECTELLIPSE_C_H
#define IVTOBJECTELLIPSE_C_H

#include "impl/vtobjectellipse_c.h"



namespace IsoAgLib {

class iVtObjectEllipse_c : public __IsoAgLib::vtObjectEllipse_c
{
public:
  static uint16_t objectType();
  iVtObjectEllipse_c(const iVtObjectEllipse_s* vtObjectellipseSROM , int ai_multitonInst);
  const iVtObjectEllipse_s& get_vtObjectEllipse_a();
  void setLineAttributes(iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setEllipseType(uint8_t newEllipseType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setStartAngle(uint8_t newStartAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setEndAngle(uint8_t newEndAngle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setFillAttributes(iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectEllipse_c::updateObjectType(); }
  */
  
  uint16_t updateLineAttributes(bool b_SendRequest=false);
  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint8_t updateEllipseType(bool b_SendRequest=false);
  uint8_t updateStartAngle(bool b_SendRequest=false);
  uint8_t updateEndAngle(bool b_SendRequest=false);
  uint16_t updateFillAttributes(bool b_SendRequest=false);
#endif
  uint16_t getObjectType() const override;
};

} // IsoAgLib

#endif

