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

#include <stdint.h>
#include "ivttypes.h"
#include "iColour.h"
#include "iObjectID.h"
#include "iFont.h"
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/multiton.h>
#include <IsoAgLib/util/iassert.h>

const uint8_t OO_CAPACITY = 100;

namespace IsoAgLib {



class iVtObject_c : public ClientBase
{
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
    virtual ~iVtObject_s() = default;
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
	  const IsoAgLib::repeat_vtLanguage_s* languagesToFollow;
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
	  explicit iVtObjectString_s(
		ObjectID ID)
	  :iVtObject_s(ID)
	  {}
  };



  struct iVtObjectMask_s : iVtObjectObject_s, iVtObjectwMacro_s {
	  explicit iVtObjectMask_s(
		ObjectID ID)
	  : iVtObject_s(ID)
	  , iVtObjectObject_s(ID)
	  , iVtObjectwMacro_s(ID)
	  {}
  };

protected:




















	struct iVtObjectPolygon_s: iVtObjectwMacro_s {
		uint16_t width;
		uint16_t height;
		iVtObjectLineAttributes_c *lineAttributes;
		iVtObjectFillAttributes_c *fillAttributes;
		uint8_t polygonType;
		uint8_t numberOfPoints;
		const repeat_x_y_s *pointsToFollow;
		iVtObjectPolygon_s(
				ObjectID ID,
				uint16_t width,
				uint16_t height,
				iVtObjectLineAttributes_c *lineAttributes,
				iVtObjectFillAttributes_c *fillAttributes,
				uint8_t polygonType,
				uint8_t numberOfPoints,
				const repeat_x_y_s *pointsToFollow)
	    : iVtObject_s(ID)
		, iVtObjectwMacro_s(ID)
		, width(width)
		, height(height)
		, lineAttributes(lineAttributes)
		, fillAttributes(fillAttributes)
		, polygonType(polygonType)
		, numberOfPoints(numberOfPoints)
		, pointsToFollow(pointsToFollow)
		{
		}


	};

	struct iVtObjectRectangle_s: iVtObjectwMacro_s {
		iVtObjectLineAttributes_c *lineAttributes;
		uint16_t width;
		uint16_t height;
		uint8_t lineSuppression;
		iVtObjectFillAttributes_c *fillAttributes;
		iVtObjectRectangle_s(
				ObjectID ID,
				iVtObjectLineAttributes_c *lineAttributes,
				uint16_t width,
				uint16_t height,
				uint8_t lineSuppression,
				iVtObjectFillAttributes_c *fillAttributes)
	    : iVtObject_s(ID)
		, iVtObjectwMacro_s(ID)
		, lineAttributes(lineAttributes)
		, width(width)
		, height(height)
		, lineSuppression(lineSuppression)
		, fillAttributes(fillAttributes)
		{
		}


	};

	struct iVtObjectSoftKeyMask_s: iVtObjectObject_s, iVtObjectwMacro_s {
		Colour backgroundColour;
		iVtObjectSoftKeyMask_s(
				ObjectID ID,
				Colour backgroundColour)
	    : iVtObject_s(ID)
		, iVtObjectObject_s(ID)
		, iVtObjectwMacro_s(ID)
		, backgroundColour(backgroundColour)
		{
		}


	};

	struct iVtObjectAuxiliaryInput_s: iVtObjectObject_s {
		Colour backgroundColour;
		uint8_t functionType;
		uint8_t inputId;
		iVtObjectAuxiliaryInput_s(
				ObjectID ID,
				Colour backgroundColour,
				uint8_t functionType,
				uint8_t inputId)
	    : iVtObject_s(ID)
		, iVtObjectObject_s(ID)
		, backgroundColour(backgroundColour)
		, functionType(functionType)
		, inputId(inputId)
		{
		}


	};

	struct iVtObjectAuxiliaryFunction_s: iVtObjectObject_s {
		Colour backgroundColour;
		uint8_t functionType;
		iVtObjectAuxiliaryFunction_s(
				ObjectID ID,
				Colour backgroundColour,
				uint8_t functionType)
		: iVtObject_s(ID)
	    , iVtObjectObject_s(ID)
		, backgroundColour(backgroundColour)
		, functionType(functionType)
		{
		}


	};

	struct iVtObjectGraphicsContext_s: iVtObject_s {
		uint16_t viewportWidth;
		uint16_t viewportHeight;
		int16_t viewportX;       //!< Upper left corner of the viewport
		int16_t viewportY;
		uint16_t canvasWidth;
		uint16_t canvasHeight;
		float viewportZoom;    //!< Zoom value [-32.0F ... 32.0F]
		int16_t cursorX;
		int16_t cursorY;
		uint8_t foregroundColour;
		uint8_t backgroundColour;
		iVtObjectFontAttributes_c *fontAttributes;
		iVtObjectLineAttributes_c *lineAttributes;
		iVtObjectFillAttributes_c *fillAttributes;
		uint8_t format; //!< 0 => 1 bit/pixel, 1 => 4 bit/pixel, 2 => 8 bit/pixel
		uint8_t options; //!< Bit 0 transparent flag; Bit 1 line, font, fill colour usage
		Colour transparencyColour;  //!< Colour to be shown transparent.
		iVtObjectGraphicsContext_s(
				ObjectID ID,
				uint16_t viewportWidth,
				uint16_t viewportHeight,
				int16_t viewportX,
				int16_t viewportY,
				uint16_t canvasWidth,
				uint16_t canvasHeight,
				float viewportZoom,
				int16_t cursorX,
				int16_t cursorY,
				uint8_t foregroundColour,
				Colour backgroundColour,
				iVtObjectFontAttributes_c *fontAttributes,
				iVtObjectLineAttributes_c *lineAttributes,
				iVtObjectFillAttributes_c *fillAttributes,
				uint8_t format,
				uint8_t options,
				Colour transparencyColour)
		: iVtObject_s(ID)
		, viewportWidth(viewportWidth)
		, viewportHeight(viewportHeight)
		, viewportX(viewportX)
		, viewportY(viewportY)
		, canvasWidth(canvasWidth)
		, canvasHeight(canvasHeight)
		, viewportZoom(viewportZoom)
		, cursorX(cursorX)
		, cursorY(cursorY)
		, foregroundColour(foregroundColour)
		, backgroundColour(backgroundColour)
		, fontAttributes(fontAttributes)
		, lineAttributes(lineAttributes)
		, fillAttributes(fillAttributes)
		, format(format)
		, options(options)
		, transparencyColour(transparencyColour)
		{
		}
	};

	struct iVtObjectAuxiliaryInput2_s: iVtObjectObject_s {
		Colour backgroundColour;
		uint8_t functionAttributes;
		iVtObjectAuxiliaryInput2_s(
				ObjectID ID,
				Colour backgroundColour,
				uint8_t functionAttributes)
	    : iVtObject_s(ID)
		, iVtObjectObject_s(ID)
		, backgroundColour(backgroundColour)
		, functionAttributes(functionAttributes)
		{
		}


	};

	struct iVtObjectAuxiliaryFunction2_s: iVtObjectObject_s {
		Colour backgroundColour;
		uint8_t functionAttributes;
		iVtObjectAuxiliaryFunction2_s(
				ObjectID ID,
				Colour backgroundColour,
				uint8_t functionAttributes)
	    : iVtObject_s(ID)
		, iVtObjectObject_s(ID)
		, backgroundColour(backgroundColour)
		, functionAttributes(functionAttributes)
		{
		}


	};

	struct iVtObjectAuxiliaryControlDesignatorObjectPointer_s: iVtObject_s {
		uint8_t pointerType;
		iVtObject_c *value;
		iVtObjectAuxiliaryControlDesignatorObjectPointer_s(
				ObjectID ID,
				uint8_t pointerType,
				iVtObject_c *value)
		: iVtObject_s(ID)
		, pointerType(pointerType)
		, value(value)
		{
		}


	};

// METHODS
public:
  // Constructor
  iVtObject_c();

  ~iVtObject_c() override = default;

  //  Operation: getID
  uint16_t getID() const;

  //! *** API-function ***
  //! Per default, all objects are uploaded.
  //! This function inhibts objects from being uploaded
  //! to the VT. Only use this function if you know what
  //! you're doing and if there's no other way.
  //! This can be useful for omitting e.g. AuxFunction
  //! objects that should not be available due to configuration.
  void omitFromUpload();

  virtual void setOriginSKM(bool b_SKM);
  virtual void setOriginBTN(iVtObjectButton_c* p_btn);

  void setClientID (uint8_t ui8_clientID);

  /** return object type as described in the standard.
      please note that the upper byte may be used for proprietary objects.
   */
  virtual uint16_t getObjectType() const = 0;

protected:
  iVtObject_s* vtObject_a; //TODO: remove this later !!!

  iVtObjectButton_c* p_parentButtonObject;

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

#endif
