/*
  ivtobject_c.cpp

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
#include "ivtobject_c.h"

#include "impl/vttypes.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {
	// Operation : vtObject_c
	//iVtObject_c::vtObject_c()
	//: flags( 0 ), ui16_bitmaskAttributeAlreadyRequested  (0)
	//{
	//}

	// Operation : setOriginSKM
	//! @param b_SKM: true if this object is included in a softkeymask.
	void
	iVtObject_c::setOriginSKM(bool b_SKM)
	{
	  if (b_SKM) s_properties.flags |= FLAG_ORIGIN_SKM;
	}

	/* Macro ObjID must be 0-255 !! */
	ObjectID iVtObject_c::iVtObject_s::nextID = firstID;

	/// Operation : setOriginBTN
	/// @param p_btn: false if this object is not included in a button.
	void iVtObject_c::setOriginBTN(iVtObjectButton_c* p_btn)
	{
	  if (p_btn) p_parentButtonObject = p_btn;
	}

	iVtObject_c::iVtObject_s::iVtObject_s(
			ObjectID ID)
    {
        this->ID = ID == autoID ? nextID : ID;
        nextID = (ObjectID)((uint16_t)nextID + 1);
    }


	iVtObject_c::iVtObjectObject_s::iVtObjectObject_s(
			ObjectID ID)
	: iVtObjectObject_s(ID, OO_CAPACITY)
	{
	}


    iVtObject_c::iVtObjectObject_s::iVtObjectObject_s(
    		ObjectID ID,
			uint8_t size)
            : iVtObject_s(ID)
    		, numberOfObjectsToFollow_size(size)
            , numberOfObjectsToFollow(0)
            , objectsToFollow(nullptr)
    {
    	objectsToFollow = new repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s[numberOfObjectsToFollow_size];
    }


    void iVtObject_c::iVtObjectObject_s::Append(
    		iVtObject_c* const vtObject,
			int16_t x,
			int16_t y)
    {
        if (numberOfObjectsToFollow <= numberOfObjectsToFollow_size) {
        	objectsToFollow[numberOfObjectsToFollow].vtObject = vtObject;
        	objectsToFollow[numberOfObjectsToFollow].x = x;
        	objectsToFollow[numberOfObjectsToFollow].y = y;
        	numberOfObjectsToFollow++;
        } else {
        	//Exception !!! //TODO
        }
    }


    iVtObject_c::iVtObjectwMacro_s::iVtObjectwMacro_s(
    		ObjectID ID)
    : iVtObjectwMacro_s(ID, OO_CAPACITY)
    {

    }


    iVtObject_c::iVtObjectwMacro_s::iVtObjectwMacro_s(
    		ObjectID ID,
			uint8_t size)
    :iVtObject_s(ID)
    , numberOfMacrosToFollow_size(size)
    , numberOfMacrosToFollow(0)
    , macrosToFollow(nullptr)
    {
    	//TODO implement List like with iVtObjectObject_s
    }

    iVtObject_c::iVtObjectLanguages_s::iVtObjectLanguages_s(
    		ObjectID ID)
    : iVtObjectLanguages_s(ID, OO_CAPACITY)
    {

    }

    iVtObject_c::iVtObjectLanguages_s::iVtObjectLanguages_s(
    		ObjectID ID,
			uint8_t size)
    :iVtObject_s(ID)
    , numberOfLanguagesToFollow_size(size)
    , numberOfLanguagesToFollow(0)
    , languagesToFollow(nullptr)
    {
    	//TODO implement List like with iVtObjectObject_s
    }




} // end of namespace IsoAgLib

