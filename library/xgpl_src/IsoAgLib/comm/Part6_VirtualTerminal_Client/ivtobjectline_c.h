/*
  ivtobjectline_c.h

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
#ifndef IVTOBJECTLINE_C_H
#define IVTOBJECTLINE_C_H

#include "impl/vtobjectline_c.h"


#ifdef USE_VTOBJECT_line

namespace IsoAgLib {

class iVtObjectLine_c : public __IsoAgLib::vtObjectLine_c
{
public:
  static uint16_t objectType();

  iVtObjectLine_c(
			iVtClientObjectPool_c* pool,
	  		ObjectID ID = autoID,
			iVtObjectLineAttributes_c *lineAttributes = nullptr,
			uint16_t width = 100,
			uint16_t height =100,
			uint8_t lineDirection = 0);



  void setLineAttributes(iVtObjectLineAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setWidth (uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setHeight (uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setLineDirection (uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setEndPoint (uint16_t newWidth, uint16_t newHeight, uint8_t newLineDirection, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLine_c::updateObjectType(); }
  */
  uint16_t updateLineAttributes(bool b_SendRequest=false);
  uint16_t updateWidth(bool b_SendRequest=false);
  uint16_t updateHeight(bool b_SendRequest=false);
  uint8_t updateLineDirection(bool b_SendRequest=false);
#endif
  virtual uint16_t getObjectType() const;
};

} // IsoAgLib

#endif //USE_VTOBJECT_line

#endif //IVTOBJECTLINE_C_H
