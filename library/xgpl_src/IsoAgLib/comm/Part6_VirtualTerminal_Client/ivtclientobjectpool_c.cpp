/*
  ivtclientobjectpool_c.cpp: Class for managing an object pool with
    softkey/button activation event handling

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
#include "ivtclientobjectpool_c.h"
#include <IsoAgLib/util/iassert.h>

namespace
{

uint8_t const carrui8_normalized[] = {
  16, 231, 34, 37, 124, 127, 142, 188, 145, 21, 46, 51, 196, 201, 226, 19 };

IsoAgLib::Colour normalizeDepth2Color(IsoAgLib::Colour ui8_color) {
	//TODO
  return ui8_color < 16 ? (IsoAgLib::Colour)carrui8_normalized[ui8_color] : ui8_color;
}

}


namespace IsoAgLib
{

Colour iVtClientObjectPool_c::convertColourDefault(
    Colour colorValue,
    uint8_t colorDepth,
    iVtObject_c * /*obj*/,
    e_vtColour /*whichColour*/)
{
  unsigned const cu_fittingDepth =
    colorValue < 2 ? 0 :
    colorValue < 16 ? 1 :
    2;

  if (colorDepth >= cu_fittingDepth)
    return colorValue; // don't convert

  Colour const colorValue1 = (1 == cu_fittingDepth) ?
    normalizeDepth2Color(colorValue) : colorValue;

  isoaglib_assert(NAVY < colorValue1);
  if (colorValue1 < 232) {
    unsigned const cu_r0 = colorValue1 - 16u;
    unsigned const cu_z0 = cu_r0 % 6u;
    unsigned const cu_r1 = cu_r0 / 6u;
    unsigned const cu_z1 = cu_r1 % 6u;
    unsigned const cu_r2 = cu_r1 / 6u;
    unsigned const cu_z2 = cu_r2 % 6u;
    return (Colour)(7u < cu_z0 + cu_z1 + cu_z2 ? 1 : 0);
  } else {
    // Convert proprietary colors to black:
    return BLACK;
  }
}

    iVtObject_c *const *const *iVtClientObjectPool_c::getIVtObjects() const { return iVtObjects->all_items; }

    uint16_t iVtClientObjectPool_c::getNumObjects() const { return iVtObjects->Count(); }

    uint16_t iVtClientObjectPool_c::getNumObjectsLang() const { return 0; }

    iVtClientObjectPool_c::ObjectPoolVersion_en iVtClientObjectPool_c::getVersion() const { return version; }

    void iVtClientObjectPool_c::overrideVersion(iVtClientObjectPool_c::ObjectPoolVersion_en override) { version = override; }

    uint16_t iVtClientObjectPool_c::getDimension() const { return dimension; }

    uint16_t iVtClientObjectPool_c::getSkWidth() const { return skWidth; }

    uint16_t iVtClientObjectPool_c::getSkHeight() const { return skHeight; }

    uint8_t iVtClientObjectPool_c::getNumLang() const { return 0; }

    bool iVtClientObjectPool_c::multiLanguage() const { return getNumLang() > 0; }

    void iVtClientObjectPool_c::Append(iVtObject_c *const c) {iVtObjects->Append(c);}

    iVtObjectWorkingSet_c &iVtClientObjectPool_c::getWorkingSetObject() const { return *(iVtObjectWorkingSet_c*)(**iVtObjects->all_items); }

    iVtClientObjectPool_c::iVtClientObjectPool_c(iVtClientObjectPool_c::ObjectPoolSettings_s a_objectPoolSettings)
            : iVtObjects (new iVtObject_cList())
            , version(a_objectPoolSettings.version)
            , dimension (a_objectPoolSettings.dimension)
            , skWidth (a_objectPoolSettings.skWidth)
            , skHeight (a_objectPoolSettings.skHeight)
            , b_initAllObjects (false)
    		, ai_multitonInst(multitonSTD)
    {

    }

    multiton iVtClientObjectPool_c::getAiMultitonInst() const {
        return ai_multitonInst;
    }

    void iVtClientObjectPool_c::setAiMultitonInst(multiton aiMultitonInst) {
        ai_multitonInst = aiMultitonInst;
    }

}
