/*
  ivtobject_c.h

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
#ifndef IVTOBJECT_C_H
#define IVTOBJECT_C_H

#include <cstdint>
#include "ivttypes.h"
#include "iColour.h"
#include "iFillType.h"
#include "iObjectID.h"
#include "iOptions.h"
#include "iJustification.h"
#include "iLineDirection.h"
#include "iLineSuppression.h"
#include "iFont.h"
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/multiton.h>
#include <IsoAgLib/util/iassert.h>

const uint8_t OO_CAPACITY = 100;

namespace IsoAgLib {


// https://www.geeksforgeeks.org/pure-virtual-functions-and-abstract-classes/
//

class iVtObject_c : public virtual AbstractClientBase {
public:


	  /**
	   *
	   * iVtObject_s is the mother of all objects. it defines the ObjectID
	   *
	   *
	   *
	   */
  struct iVtObject_s {
	ObjectID ID;
    static ObjectID nextID;
    explicit iVtObject_s() = delete;
    explicit iVtObject_s(ObjectID ID);
    virtual ~iVtObject_s() = 0;
  };

  // the "virtual" here is really important, avoid https://en.wikipedia.org/wiki/Multiple_inheritance#The_diamond_problem
  /**
   * iVtObjectLanguages_s is a List of Languages.
   * currentyl only supported on the WS Object
   *
   *
   *
   */
  struct iVtObjectLanguages_s : virtual iVtObject_s {
	  uint8_t numberOfLanguagesToFollow_size;
	  uint8_t numberOfLanguagesToFollow;
	  const repeat_vtLanguage_s* languagesToFollow;
	  explicit iVtObjectLanguages_s() = delete;
	  explicit iVtObjectLanguages_s(
		ObjectID ID);
	  explicit iVtObjectLanguages_s(
		ObjectID ID,
		uint8_t size);
  };

protected:

  // the "virtual" here is really important, avoid https://en.wikipedia.org/wiki/Multiple_inheritance#The_diamond_problem
  /**
   *
   *
   * iVtObjectObject_s is a Object than contain other Objects.
   *
   *
   *
   */
  struct iVtObjectObject_s : virtual iVtObject_s {
	uint8_t numberOfObjectsToFollow_size;
    uint8_t numberOfObjectsToFollow;
    repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
    void Append(iVtObject_c* const vtObject, int16_t x, int16_t y);
    explicit iVtObjectObject_s() = delete;
    explicit iVtObjectObject_s(
    		ObjectID ID);
    explicit iVtObjectObject_s(
    		ObjectID ID,
			uint8_t size);
  };

  // the "virtual" here is really important, avoid https://en.wikipedia.org/wiki/Multiple_inheritance#The_diamond_problem
  /**
   *
   *
   * iVtObjectwMacro_s is a Object than contain Macros.
   *
   *
   *
   *
   */
  struct iVtObjectwMacro_s : virtual iVtObject_s {
		uint8_t numberOfMacrosToFollow_size;
	    uint8_t numberOfMacrosToFollow;
	    const repeat_event_iVtObjectMacro_s* macrosToFollow;
	    explicit iVtObjectwMacro_s(
			ObjectID ID);
		explicit iVtObjectwMacro_s(
			ObjectID ID,
			uint8_t size);
  };




  struct iVtObjectString_s : virtual iVtObject_s {
	  uint16_t length;
	  char* value; /* size length+1 (0-termination intern!) */
	  explicit iVtObjectString_s(
		ObjectID ID,
		char *value)
	  : iVtObject_s(ID)
	  , value(value)
		{
	      if (value != nullptr)
	    	  length = std::strlen(value);
	      else
	    	  length = 0;
		}
  };



  struct iVtObjectMask_s : iVtObjectObject_s, iVtObjectwMacro_s {
	  explicit iVtObjectMask_s(
		ObjectID ID)
	  : iVtObject_s(ID)
	  , iVtObjectObject_s(ID)
	  , iVtObjectwMacro_s(ID)
	  {}
  };



// METHODS
public:
  // Constructor
  iVtObject_c();

  ~iVtObject_c() override = 0; //pure virtual

  //  Operation: getID
  virtual ObjectID getID() const = 0; //pure virtual

  //! *** API-function ***
  //! Per default, all objects are uploaded.
  //! This function inhibts objects from being uploaded
  //! to the VT. Only use this function if you know what
  //! you're doing and if there's no other way.
  //! This can be useful for omitting e.g. AuxFunction
  //! objects that should not be available due to configuration.
  void omitFromUpload();

  virtual void setOriginSKM(bool b_SKM);

#ifdef CONFIG_USE_VTOBJECT_button
  virtual void setOriginBTN(iVtObjectButton_c* p_btn);
#endif //CONFIG_USE_VTOBJECT_button

  void setClientID (uint8_t ui8_clientID);

  /** return object type as described in the standard.
      please note that the upper byte may be used for proprietary objects.
   */
  virtual ObjectType getObjectType() const = 0;

protected:
#ifdef CONFIG_USE_VTOBJECT_button
  iVtObjectButton_c* p_parentButtonObject;
#endif //CONFIG_USE_VTOBJECT_button

  struct {
    uint8_t flags:5;
    uint8_t clientId:3; // when changing, adapt the assertion in "setClientID(..)"!
  } s_properties;

private:
  // non-copy/assign-able
  iVtObject_c( const iVtObject_c& );
  iVtObject_c& operator=( const iVtObject_c& );
};

inline
void
iVtObject_c::omitFromUpload()
{
  s_properties.flags = uint8_t(s_properties.flags | FLAG_OMIT_OBJECT);
}


inline
void
iVtObject_c::setClientID( uint8_t ui8_clientID )
{
  isoaglib_header_assert( ui8_clientID < 8 ); // 3 bits only, see struct s_clientId
  s_properties.clientId = (ui8_clientID & 0x07); // "&" to avoid -Wconversion warning
}


} // IsoAgLib

#endif //IVTOBJECT_C_H
