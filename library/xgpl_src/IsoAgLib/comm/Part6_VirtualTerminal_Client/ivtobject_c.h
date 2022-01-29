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

#include "ivttypes.h"
#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/multiton.h>
#include <IsoAgLib/util/iassert.h>

const uint8_t OO_CAPACITY = 100;

namespace IsoAgLib {

class iVtObject_c : public ClientBase
{
protected:
  struct iVtObject_s {
    uint16_t ID;
    static uint16_t nextID;
    iVtObject_s(uint16_t oID = 0){
        ID = oID == 0 ? nextID++ : oID;
    }
  };


  struct iVtObjectObject_s : iVtObject_s {
	uint8_t numberOfObjectsToFollow_size;
    uint8_t numberOfObjectsToFollow;
    repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
    void Append(iVtObject_c* const vtObject, int16_t x, int16_t y);
    iVtObjectObject_s(uint16_t ID = 0, uint8_t size = OO_CAPACITY);
  };




  struct iVtObjectAlarmMask_s : iVtObjectObject_s {
    uint8_t backgroundColour;
    iVtObjectSoftKeyMask_c* softKeyMask;
    uint8_t priority;
    uint8_t acousticSignal;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
    iVtObjectAlarmMask_s(
    		uint16_t ID = 0,
    		uint8_t backgroundColour = 0,
			iVtObjectSoftKeyMask_c *softKeyMask = nullptr,
			uint8_t priority = 0,
            uint8_t acousticSignal = 0,
            uint8_t numberOfMacrosToFollow = 0,
			const repeat_event_iVtObjectMacro_s *macrosToFollow = nullptr)
    : iVtObjectObject_s(ID)
    , backgroundColour(backgroundColour)
    , softKeyMask(softKeyMask)
    , priority(priority)
    , acousticSignal(acousticSignal)
    , numberOfMacrosToFollow(numberOfMacrosToFollow)
    , macrosToFollow(macrosToFollow)
    {}
  };

  struct iVtObjectArchedBarGraph_s : iVtObject_s {
    uint16_t width;
    uint16_t height;

      iVtObjectArchedBarGraph_s(uint16_t width, uint16_t height, uint8_t colour, uint8_t targetLineColour,
                                uint8_t options, uint8_t startAngle, uint8_t endAngle, uint16_t barGraphWidth,
                                uint16_t minValue, uint16_t maxValue, iVtObject_c *variableReference, uint16_t value,
                                iVtObject_c *targetValueVariableReference, uint16_t targetValue,
                                uint8_t numberOfMacrosToFollow, const repeat_event_iVtObjectMacro_s *macrosToFollow)
              : width(width), height(height), colour(colour), targetLineColour(targetLineColour), options(options),
                startAngle(startAngle), endAngle(endAngle), barGraphWidth(barGraphWidth), minValue(minValue),
                maxValue(maxValue), variableReference(variableReference), value(value),
                targetValueVariableReference(targetValueVariableReference), targetValue(targetValue),
                numberOfMacrosToFollow(numberOfMacrosToFollow), macrosToFollow(macrosToFollow) {}

      uint8_t colour;
    uint8_t targetLineColour;
    uint8_t options;
    uint8_t startAngle;
    uint8_t endAngle;
    uint16_t barGraphWidth;
    uint16_t minValue;
    uint16_t maxValue;
    iVtObject_c* variableReference;
    uint16_t value;
    iVtObject_c* targetValueVariableReference;
    uint16_t targetValue;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };




  struct iVtObjectButton_s : iVtObjectObject_s {
    uint16_t width;
    uint16_t height;
    uint8_t backgroundColour;
    uint8_t borderColour;
    uint8_t keyCode;
    uint8_t options;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
    iVtObjectButton_s(
    		uint16_t ID = 0,
    	    uint16_t width = 80,
    	    uint16_t height = 30,
    	    uint8_t backgroundColour = 12,
    	    uint8_t borderColour = 8,
    	    uint8_t keyCode = 251,
    	    uint8_t options = 0,
    	    uint8_t numberOfMacrosToFollow = 0,
    	    const repeat_event_iVtObjectMacro_s* macrosToFollow = nullptr
    		)
    : iVtObjectObject_s(ID)
    , width(width)
    , height(height)
    , backgroundColour(backgroundColour)
    , borderColour(borderColour)
    , keyCode(keyCode)
    , options(options)
    , numberOfMacrosToFollow(numberOfMacrosToFollow)
    , macrosToFollow(macrosToFollow)
    {}
  };


  struct iVtObjectContainer_s : iVtObject_s {
    uint16_t width;
    uint16_t height;

      iVtObjectContainer_s(uint16_t width, uint16_t height, uint8_t hidden, uint8_t numberOfObjectsToFollow,
                           const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s *objectsToFollow,
                           uint8_t numberOfMacrosToFollow, const repeat_event_iVtObjectMacro_s *macrosToFollow) : width(
              width), height(height), hidden(hidden), numberOfObjectsToFollow(numberOfObjectsToFollow), objectsToFollow(
              objectsToFollow), numberOfMacrosToFollow(numberOfMacrosToFollow), macrosToFollow(macrosToFollow) {}

      uint8_t hidden;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectDataMask_s : iVtObject_s {
    uint8_t backgroundColour;
    iVtObjectSoftKeyMask_c* softKeyMask;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
    iVtObjectDataMask_s(
    		uint16_t ID = 0,
    	    uint8_t backgroundColour = 0,
    	    iVtObjectSoftKeyMask_c* softKeyMask = nullptr,
    	    uint8_t numberOfObjectsToFollow = 0,
    	    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow = nullptr,
    	    uint8_t numberOfMacrosToFollow = 0,
    	    const repeat_event_iVtObjectMacro_s* macrosToFollow = nullptr)
    :iVtObject_s(ID)
    , backgroundColour(backgroundColour)
    , softKeyMask(softKeyMask)
    , numberOfObjectsToFollow(numberOfObjectsToFollow)
    , objectsToFollow(objectsToFollow)
    , numberOfMacrosToFollow(numberOfMacrosToFollow)
    , macrosToFollow(macrosToFollow)
    {}
  };

  struct iVtObjectEllipse_s : iVtObject_s {
    iVtObjectLineAttributes_c* lineAttributes;
    uint16_t width;
    uint16_t height;
    uint8_t ellipseType;
    uint8_t startAngle;
    uint8_t endAngle;
    iVtObjectFillAttributes_c* fillAttributes;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectFillAttributes_s : iVtObject_s {
    uint8_t fillType;
    uint8_t fillColour;
    iVtObjectPictureGraphic_c* fillPatternObject;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectFontAttributes_s : iVtObject_s {
    uint8_t fontColour;
    uint8_t fontSize;
    uint8_t fontType; // always =0 ISO_LATIN_1
    uint8_t fontStyle;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
    explicit iVtObjectFontAttributes_s(
    	    uint16_t ID = 0,
    	    uint8_t fontColour = 0,
    	    uint8_t fontSize = 1,
    	    uint8_t fontType = 0, // always =0 ISO_LATIN_1
    	    uint8_t fontStyle = 0,
    	    uint8_t numberOfMacrosToFollow = 0,
    	    const repeat_event_iVtObjectMacro_s* macrosToFollow = nullptr)
    :iVtObject_s(ID)
    , fontColour(fontColour)
    , fontSize(fontSize)
    , fontType(fontType) // always =0 ISO_LATIN_1
    , fontStyle(fontStyle)
    , numberOfMacrosToFollow(numberOfMacrosToFollow)
    , macrosToFollow(macrosToFollow)
    {}
  };

  struct iVtObjectInputAttributes_s : iVtObject_s {
    uint8_t validationType;
    uint8_t length;
    char* validationString;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectInputBoolean_s : iVtObject_s {
    uint8_t backgroundColour;
    uint16_t width;
    iVtObject_c* foregroundColour;
    iVtObject_c* variableReference;
    uint8_t value;
    uint8_t enabled;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectInputList_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    iVtObject_c* variableReference;
    uint8_t value;
    uint8_t options;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_s* objectsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectInputNumber_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    uint8_t backgroundColour;
    iVtObject_c* fontAttributes;
    uint8_t options;
    iVtObject_c* variableReference;
    uint32_t value;
    uint32_t minValue;
    uint32_t maxValue;
    int32_t offset;
    float scale;
    uint8_t numberOfDecimals;
    uint8_t format;
    uint8_t horizontalJustification;
    uint8_t secondOptionsByte;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectInputString_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    uint8_t backgroundColour;
    iVtObjectFontAttributes_c* fontAttributes;
    iVtObjectInputAttributes_c* inputAttributes;
    uint8_t options;
    iVtObjectStringVariable_c* variableReference;
    uint8_t horizontalJustification;
    uint16_t length;
    char* value;
    uint8_t enabled;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectKey_s : iVtObject_s {
    uint8_t backgroundColour;
    uint8_t keyCode;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectLinearBarGraph_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    uint8_t colour;
    uint8_t targetLineColour;
    uint8_t options;
    uint8_t numberOfTicks;
    uint16_t minValue;
    uint16_t maxValue;
    iVtObject_c* variableReference;
    uint16_t value;
    iVtObject_c* targetValueVariableReference;
    uint16_t targetValue;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectLineAttributes_s : iVtObject_s {
    uint8_t lineColour;
    uint8_t lineWidth;
    uint16_t lineArt;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectLine_s : iVtObject_s {
    iVtObjectLineAttributes_c* lineAttributes;
    uint16_t width;
    uint16_t height;
    uint8_t lineDirection;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectMacro_s : iVtObject_s {
    uint16_t numBytesToFollow;
    const uint8_t* commandsToFollow;
  };

  struct iVtObjectMeter_s : iVtObject_s {
    uint16_t width;
    uint8_t needleColour;
    uint8_t borderColour;
    uint8_t arcAndTickColour;
    uint8_t options;
    uint8_t numberOfTicks;
    uint8_t startAngle;
    uint8_t endAngle;
    uint16_t minValue;
    uint16_t maxValue;
    iVtObject_c* variableReference;
    uint16_t value;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectNumberVariable_s : iVtObject_s {
    uint32_t value;
  };

  struct iVtObjectObjectPointer_s : iVtObject_s {
    iVtObject_c* value;
  };

  struct iVtObjectOutputList_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    iVtObject_c* variableReference;
    uint8_t value;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_s* objectsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectOutputNumber_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    uint8_t backgroundColour;
    iVtObjectFontAttributes_c* fontAttributes;
    uint8_t options;
    iVtObject_c* variableReference;
    uint32_t value;
    int32_t offset;
    float scale;
    uint8_t numberOfDecimals;
    uint8_t format;
    uint8_t horizontalJustification;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };


  struct iVtObjectOutputString_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    uint8_t backgroundColour;
    iVtObjectFontAttributes_c* fontAttributes;
    uint8_t options;
    iVtObjectStringVariable_c* variableReference;
    uint8_t horizontalJustification;
    uint16_t length;
    char* value; /* size length+1 (0-termination intern!) */
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
    iVtObjectOutputString_s(
    	    uint16_t ID = 0,
    	    uint16_t width = 50,
    	    uint16_t height= 20,
    	    uint8_t backgroundColour = 1,
    	    iVtObjectFontAttributes_c* fontAttributes = nullptr,
    	    uint8_t options = 0,
    	    iVtObjectStringVariable_c* variableReference = nullptr,
    	    uint8_t horizontalJustification = 0,
    	    uint16_t length = 0,
    	    char* value = nullptr, /* size length+1 (0-termination intern!) */
    	    uint8_t numberOfMacrosToFollow = 0,
    	    const repeat_event_iVtObjectMacro_s* macrosToFollow = nullptr
    		)
    :iVtObject_s(ID),
    width(width),
    height(height),
    backgroundColour(backgroundColour),
    fontAttributes(fontAttributes),
    options(options),
    variableReference(variableReference),
    horizontalJustification(horizontalJustification),
    length(length),
    value(value), /* size length+1 (0-termination intern!) */
    numberOfMacrosToFollow(numberOfMacrosToFollow),
    macrosToFollow(macrosToFollow)
	{}
  };

  struct iVtObjectPictureGraphic_s : iVtObject_s {
    uint16_t width;
    uint16_t actualWidth;
    uint16_t actualHeight;
    uint8_t format;
    uint8_t options;
    uint8_t transparencyColour;
    uint32_t numberOfBytesInRawData0;
    const HUGE_MEM uint8_t* rawData0;
    uint32_t numberOfBytesInRawData1;
    const HUGE_MEM uint8_t* rawData1;
    uint32_t numberOfBytesInRawData2;
    const HUGE_MEM uint8_t* rawData2;
    uint8_t numberOfFixedBitmapsToFollow;
    const repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s* fixedBitmapsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectColourMap_s : iVtObject_s {
    uint16_t numOfColourIdxToFollow;
    const HUGE_MEM uint8_t* colourMapArray;
  };

  struct iVtObjectPolygon_s : iVtObject_s {
    uint16_t width;
    uint16_t height;
    iVtObjectLineAttributes_c* lineAttributes;
    iVtObjectFillAttributes_c* fillAttributes;
    uint8_t polygonType;
    uint8_t numberOfPoints;
    const repeat_x_y_s* pointsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectRectangle_s : iVtObject_s {
    iVtObjectLineAttributes_c* lineAttributes;
    uint16_t width;
    uint16_t height;
    uint8_t lineSuppression;
    iVtObjectFillAttributes_c* fillAttributes;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectSoftKeyMask_s : iVtObject_s {
    uint8_t backgroundColour;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_s* objectsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
  };

  struct iVtObjectStringVariable_s : iVtObject_s {
    uint16_t length;
    char* value;
  };

  struct iVtObjectWorkingSet_s : iVtObject_s {
    uint8_t backgroundColour;
    uint8_t selectable;
    iVtObjectMask_c* activeMask; // data or alarm mask
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
    uint8_t numberOfMacrosToFollow;
    const repeat_event_iVtObjectMacro_s* macrosToFollow;
    uint8_t numberOfLanguagesToFollow;
    const repeat_vtLanguage_s* languagesToFollow;
    iVtObjectWorkingSet_s(
    	    uint16_t ID = 0,
    		uint8_t backgroundColour = 0,
    		uint8_t selectable = 1,
    		iVtObjectMask_c* activeMask = nullptr, // data or alarm mask
    		uint8_t numberOfObjectsToFollow = 0,
    		const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow = nullptr,
    		uint8_t numberOfMacrosToFollow = 0,
    		const repeat_event_iVtObjectMacro_s* macrosToFollow = nullptr,
    		uint8_t numberOfLanguagesToFollow = 0,
    		const repeat_vtLanguage_s* languagesToFollow = nullptr)
    : iVtObject_s(ID)
    , backgroundColour(backgroundColour)
    , selectable(selectable)
	, activeMask(activeMask)
	, numberOfObjectsToFollow(numberOfObjectsToFollow)
	, objectsToFollow(objectsToFollow)
	, numberOfMacrosToFollow(numberOfMacrosToFollow)
	, macrosToFollow(macrosToFollow)
	, numberOfLanguagesToFollow(numberOfLanguagesToFollow)
	, languagesToFollow(languagesToFollow)
    {}
  };

  struct iVtObjectAuxiliaryInput_s : iVtObject_s {
    uint8_t backgroundColour;
    uint8_t functionType;
    uint8_t inputId;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
  };

  struct iVtObjectAuxiliaryFunction_s : iVtObject_s {
    uint8_t backgroundColour;
    uint8_t functionType;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
  };

  struct iVtObjectGraphicsContext_s : iVtObject_s {
    uint16_t viewportWidth;
    uint16_t viewportHeight;
    int16_t  viewportX;       //!< Upper left corner of the viewport
    int16_t  viewportY;
    uint16_t canvasWidth;
    uint16_t canvasHeight;
    float    viewportZoom;    //!< Zoom value [-32.0F ... 32.0F]
    int16_t  cursorX;
    int16_t  cursorY;
    uint8_t  foregroundColour;
    uint8_t  backgroundColour;
    iVtObjectFontAttributes_c* fontAttributes;
    iVtObjectLineAttributes_c* lineAttributes;
    iVtObjectFillAttributes_c* fillAttributes;
    uint8_t  format;          //!< 0 => 1 bit/pixel, 1 => 4 bit/pixel, 2 => 8 bit/pixel
    uint8_t  options;         //!< Bit 0 transparent flag; Bit 1 line, font, fill colour usage
    uint8_t  transparencyColour;  //!< Colour to be shown transparent.
  };

  struct iVtObjectAuxiliaryInput2_s : iVtObject_s {
    uint8_t backgroundColour;
    uint8_t functionAttributes;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
  };

  struct iVtObjectAuxiliaryFunction2_s : iVtObject_s {
    uint8_t backgroundColour;
    uint8_t functionAttributes;
    uint8_t numberOfObjectsToFollow;
    const repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s* objectsToFollow;
  };

  struct iVtObjectAuxiliaryControlDesignatorObjectPointer_s : iVtObject_s {
    uint8_t pointerType;
    iVtObject_c* value;
  };

// METHODS
public:
  // Constructor
  iVtObject_c();

  virtual ~iVtObject_c() {}

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
  iVtObject_s* vtObject_a;

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
