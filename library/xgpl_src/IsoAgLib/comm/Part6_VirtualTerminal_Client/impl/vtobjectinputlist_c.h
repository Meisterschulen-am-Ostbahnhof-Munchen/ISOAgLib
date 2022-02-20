/*
  vtobjectinputlist_c.h

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
#ifndef VTOBJECTINPUTLIST_C_H
#define VTOBJECTINPUTLIST_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_inputlist

#include <memory> // PImpl
#include "vtobject_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectInputList_c : public vtObject_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectInputList_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectInputList_s> vtObject_a;

	vtObjectInputList_c() = delete;
	vtObjectInputList_c(iVtObjectInputList_s* vtObjectInputListSROM , multiton ai_multitonInst);

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  ~vtObjectInputList_c() override;
  vtObjectInputList_c(
		    multiton ai_multitonInst,
		    IsoAgLib::ObjectID ID,
  		    uint16_t width,
			uint16_t height,
			IsoAgLib::iVtObjectNumberVariable_c *variableReference,
			uint8_t value,
			IsoAgLib::iVtObjectInputListOptions options);

  void Append(iVtObject_c* const vtObject);


    virtual IsoAgLib::iVtObject_c* getListItem(uint8_t xth);

    virtual uint8_t getNumberOfListItems();
  uint32_t fitTerminal() const;

    virtual void setValue(uint8_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);

    virtual void setItem(uint8_t aui8_index, IsoAgLib::iVtObject_c* apc_object, bool b_enableReplaceOfCmd=false);

    virtual // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setVariableReference(IsoAgLib::iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

    virtual void setOptions(IsoAgLib::iVtObjectInputListOptions newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
    virtual bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
    virtual IsoAgLib::Enabled getEnabled();
    virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
   /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 10; }
  */

   virtual uint16_t updateWidth(bool b_SendRequest=false);

        virtual uint16_t updateHeight(bool b_SendRequest=false);

        virtual IsoAgLib::iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false);

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false);

  uint8_t updateOptions(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
private:
  void updateEnable(IsoAgLib::Enabled aui8_enOrDis);
};

} // __IsoAgLib

#endif // CONFIG_USE_VTOBJECT_inputlist

#endif // VTOBJECTINPUTLIST_C_H
