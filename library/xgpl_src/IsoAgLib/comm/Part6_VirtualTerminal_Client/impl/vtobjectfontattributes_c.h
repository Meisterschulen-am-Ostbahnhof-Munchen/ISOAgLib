/*
  vtobjectfontattributes_c.h

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
#ifndef VTOBJECTFONTATTRIBUTES_C_H
#define VTOBJECTFONTATTRIBUTES_C_H


#include <memory> // PImpl
#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectFontAttributes_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectFontAttributes_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectFontAttributes_s> vtObject_a;


public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;


	vtObjectFontAttributes_c(
			int ai_multitonInst,
			IsoAgLib::ObjectID ID,
			IsoAgLib::Colour fontColour,
			IsoAgLib::Font fontSize,
			uint8_t fontType, // always =0 ISO_LATIN_1
			uint8_t fontStyle);

  vtObjectFontAttributes_c(iVtObjectFontAttributes_s* vtObjectFontAttributesSROM , int ai_multitonInst);
  virtual ~vtObjectFontAttributes_c() = default;
  uint32_t fitTerminal() const;
  uint16_t getScaledWidthHeight();

     // //////////////////////////////////
  // All special Attribute-Set methods
    virtual void setFontColour(IsoAgLib::Colour newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setFontSize(IsoAgLib::Font newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setFontType(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setFontStyle(uint8_t newValue,  bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    void setFontAttributes(IsoAgLib::Colour newFontColour, IsoAgLib::Font newFontSize, uint8_t newFontType, uint8_t newFontStyle, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES

        virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 23; }
  */

  uint8_t updateFontColour(bool b_SendRequest=false);
  virtual uint8_t updateFontSize(bool b_SendRequest=false);
  virtual uint8_t updateFontType(bool b_SendRequest=false);
  virtual uint8_t updateFontStyle(bool b_SendRequest=false);
  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif

private:

  static uint8_t marr_font2PixelDimensionTableW [15];
  static uint8_t marr_font2PixelDimensionTableH [15];

  //! Calculate font size and cache in mui8_fontSizeScaled
  void calcScaledFontDimension() const;

  //! Chached font size calculated by calcScaledFontDimension()
  mutable uint8_t mui8_fontSizeScaled;

};

} // __IsoAgLib

#endif

