/***************************************************************************
                                 ivttypes.h
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag.de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag.de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

#ifndef IVTTYPES_H
#define IVTTYPES_H

#include <IsoAgLib/typedef.h>
#include <stdlib.h>       // using abs()
#include <algorithm>     // using min() max()

#define BUTTON_HAS_BEEN_UNLATCHED 0
#define BUTTON_HAS_BEEN_PRESSED 1
#define BUTTON_HAS_BEEN_LATCHED 1
#define BUTTON_IS_STILL_HELD 2

#define KEY_HAS_BEEN_RELEASED 0
#define KEY_HAS_BEEN_PRESSED 1
#define KEY_IS_STILL_HELD 2


// Begin Namespace IsoAgLib
namespace IsoAgLib {

// Forward declarations
class iVtObject_c;
class iVtObjectWorkingSet_c;
class iVtObjectDataMask_c;
class iVtObjectAlarmMask_c;
class iVtObjectContainer_c;
class iVtObjectSoftKeyMask_c;
class iVtObjectKey_c;
class iVtObjectButton_c;
class iVtObjectInputBoolean_c;
class iVtObjectInputString_c;
class iVtObjectInputNumber_c;
class iVtObjectInputList_c;
class iVtObjectOutputString_c;
class iVtObjectOutputNumber_c;
class iVtObjectLine_c;
class iVtObjectMask_c;
class iVtObjectRectangle_c;
class iVtObjectEllipse_c;
class iVtObjectPolygon_c;
class iVtObjectMeter_c;
class iVtObjectLinearBarGraph_c;
class iVtObjectArchedBarGraph_c;
class iVtObjectPictureGraphic_c;
class iVtObjectNumberVariable_c;
class iVtObjectStringVariable_c;
class iVtObjectFontAttributes_c;
class iVtObjectLineAttributes_c;
class iVtObjectFillAttributes_c;
class iVtObjectInputAttributes_c;
class iVtObjectObjectPointer_c;
class iVtObjectMacro_c;

class iVtObjectAuxiliaryFuntion_c;
class iVtObjectAuxiliaryInput_c;

// +++ (attribute list) struct declarations
struct repeat_iVtObject_s {
  iVtObject_c* vtObject;
};

// +++ (attribute list) struct declarations
struct repeat_iVtObject_x_y_iVtObjectFontAttributes_row_col_s {
  iVtObject_c* vtObject;
  int16_t x;
  int16_t y;
  iVtObjectFontAttributes_c* vtObjectBlockFont;
  int16_t row;
  int16_t col;
};

struct repeat_event_iVtObjectMacro_s {
  uint8_t event;
  iVtObject_c* vtObjectMacro; //@todo specialize to ObjectMacro !!
};

struct repeat_x_y_s {
  int16_t x;
  int16_t y;
};

struct repeat_vtLanguage_s {
  uint8_t language[2];
};

struct repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s {
  const HUGE_MEM uint8_t* rawData;
  uint32_t numberOfBytesInRawData;
  uint16_t actualWidth;
  uint16_t actualHeight;
  uint8_t formatoptions; /* format is bit 8+7, options is bit 2-0 (NO RLE1/4/8 stuff here!! */
};

enum e_vtColour { BackgroundColour=0, BorderColour=1, NeedleColour=2, ArcAndTickColour=3, BarGraphColour=4, Colour=4 /*intentionally!*/, TargetLineColour=5, TransparencyColour=6, FontColour=7, LineColour=8, FillColour=9 };

/** struct of the data contained in the "VT Status Message" */
typedef struct vtState_s {
  uint32_t lastReceived; /* Timestamp of last reception */

  /* the following data is extracted from one "VT Status Message" */
  uint8_t  saOfActiveWorkingSetMaster;
  uint16_t dataAlarmMask;
  uint16_t softKeyMask;
  uint8_t  busyCodes;
  uint8_t  functionBusy;
} vtState_s;

/** Pair of coordinates representing a point location at the VT. */
class iVtPoint_c {
  public:
    iVtPoint_c( void ) : x(0), y(0) {}
    iVtPoint_c( int16_t x, int16_t y ) : x(x), y(y) {}
    int16_t getX( void ) const { return x; }
    int16_t getY( void ) const { return y; }
    void setX( int16_t i_x ) { x = i_x; }
    void setY( int16_t i_y ) { y = i_y; }

    //! Add another points coordinates to this point.
    iVtPoint_c& operator+=( const iVtPoint_c& a ) { x += a.x; y += a.y; return *this; }
    //! Subtract another points coordinates from this point.
    iVtPoint_c& operator-=( const iVtPoint_c& a ) { x -= a.x; y -= a.y; return *this; }
    //! Scale points coordinates by integer factor.
    iVtPoint_c& operator*=( int a ) { x *= a; y *= a; return *this; }
    //! Divide points coordinates by integer divisor.
    iVtPoint_c& operator/=( int a ) { x /= a; y /= a; return *this; }
    //! Absolute value of this points coordinates.
    iVtPoint_c abs( void ) const { return iVtPoint_c( int16_t(::abs(int(x))), int16_t(::abs(int(y))) ); }

    //! Don't use: This is just a workaround for problems using the VT server MACRO_processChangeAttributeAID
    int16_t& setX( void ) { return x; }
    //! Don't use: This is just a workaround for problems using the VT server MACRO_processChangeAttributeAID
    int16_t& setY( void ) { return y; }
  protected:
    int16_t x;
    int16_t y;
};

inline iVtPoint_c operator+( const iVtPoint_c& a, const iVtPoint_c& b ) {
  iVtPoint_c r(a); return r += b;
}
inline iVtPoint_c operator-( const iVtPoint_c& a, const iVtPoint_c& b ) {
  iVtPoint_c r(a); return r -= b;
}
inline iVtPoint_c operator*( const iVtPoint_c& a, int b ) { iVtPoint_c r(a); return r *= b; }
inline iVtPoint_c operator/( const iVtPoint_c& a, int b ) { iVtPoint_c r(a); return r /= b; }
inline iVtPoint_c abs( const iVtPoint_c& a ) { return a.abs(); }
inline iVtPoint_c min( const iVtPoint_c& a, const iVtPoint_c& b ) { 
  return iVtPoint_c( CNAMESPACE::min(a.getX(),b.getX()), CNAMESPACE::min(a.getY(),b.getY()) );
}
inline iVtPoint_c max( const iVtPoint_c& a, const iVtPoint_c& b ) {
  return iVtPoint_c( CNAMESPACE::max(a.getX(),b.getX()), CNAMESPACE::max(a.getY(),b.getY()) );
}

} // namespace IsoAgLib

#endif // IVTTYPES_H
