/*
  vtobjectpicturegraphic_c.h

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
#ifndef VTOBJECTPICTUREGRAPHIC_C_H
#define VTOBJECTPICTUREGRAPHIC_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_picturegraphic

#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectPictureGraphic_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectPictureGraphic_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectPictureGraphic_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectPictureGraphic_c(iVtObjectPictureGraphic_s* vtObjectPictureGraphicSROM , int ai_multitonInst);



  uint32_t fitTerminal() const;

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  void setTransparencyColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

  /// The following modification functions will only take affect on updating the object pool!
  /// USE THEM WITH CARE!!!
  void setRawData0 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF);
  void setRawData1 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF);
  void setRawData2 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF);
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 20; }
  */

  uint16_t updateWidth(bool b_SendRequest=false);
  uint8_t updateOptions(bool b_SendRequest=false);
  uint8_t updateTransparencyColour(bool b_SendRequest=false);
  /** these attributes are in parentheses in the spec, so commented out here
  uint16_t updateActualWidth(bool b_SendRequest=false);
  uint16_t updateActualHeight(bool b_SendRequest=false);
  uint8_t updateFormat(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} //__IsoAgLib

#endif //CONFIG_USE_VTOBJECT_picturegraphic

#endif //VTOBJECTPICTUREGRAPHIC_C_H
