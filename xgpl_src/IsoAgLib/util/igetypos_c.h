/***************************************************************************
                          igetypos_c.h - class iGetyPos_c combines device type
                                      ( GETY for DIN 9684 ) and instance
                                      number ( POS for DIN 9684 )
                             -------------------
    begin                 Sun Feb 23 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
 * Copyright (C) 2003 - 2004 Dipl.-Inform. Achim Spangler                 *
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
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#ifndef IGETYPOS_H
#define IGETYPOS_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/getypos_c.h"
#ifdef USE_ISO_11783
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>
#endif
// Begin Namespace IsoAgLib
namespace IsoAgLib {
class iGetyPos_c : public __IsoAgLib::GetyPos_c {
 public:
    /** constant for default parameters and initialization, where the device type is not yet spcified.
        the instantiation of this constant variable is located in the module cancustomer_c.cpp
      */
    static const iGetyPos_c GetyPosUnspecified;
    /** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
        the instantiation of this constant variable is located in the module cancustomer_c.cpp
      */
    static const iGetyPos_c GetyPosInitialProcessData;
  /** default constructor
    * @param rui16_gety optional initial GETY (device type)
    * @param rui16_pos optional initial POS (device number)
    */
  iGetyPos_c( uint16_t rui16_gety = 0x7F, uint16_t rui16_pos = 0xF )
  : GetyPos_c( rui16_gety, rui16_pos ){};
  iGetyPos_c( const __IsoAgLib::GetyPos_c& refc_src )
    : GetyPos_c( refc_src )
  {};
  #ifdef USE_ISO_11783
  /** default constructor
    * @param rui16_gety optional initial GETY (device type)
    * @param rui16_pos optional initial POS (device number)
    */
  iGetyPos_c(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rb_devClass, uint8_t rb_devClassInst,
      uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst = 0, uint8_t rb_ecuInst = 0)
  : GetyPos_c(rb_selfConf, rui8_indGroup, rb_devClass, rb_devClassInst,
      rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst) {};
  iGetyPos_c( const uint8_t* rpui8_dataName ) : GetyPos_c( rpui8_dataName ) {};
  #endif
  /** set GETY and POS with two seperate parameters */
  void set( uint16_t rui16_gety, uint16_t rui16_pos ) { GetyPos_c::set( rui16_gety, rui16_pos );};
  #ifdef USE_ISO_11783
  /** set GETY and POS with two seperate parameters */
  void set( const iISOName_c& rrefc_isoName ) { GetyPos_c::set( rrefc_isoName );};
  /** set GETY and POS with two seperate parameters */
  void set( const uint8_t* rpui8_dataName ) { GetyPos_c::set( rpui8_dataName );};
  /** provide pointer to second level compare NAME */
  iISOName_c& getName() { return static_cast<IsoAgLib::iISOName_c&>(GetyPos_c::getName());};
  /** provide pointer to second level compare NAME */
  const iISOName_c& getConstName() const { return static_cast<const IsoAgLib::iISOName_c&>(GetyPos_c::getConstName());};
  #endif
 /** deliver GETY (device type ) */
 uint8_t getGety( void ) const { return GetyPos_c::getGety();};
 /** deliver POS ( device type instance number ) */
 uint8_t getPos( void ) const { return GetyPos_c::getPos();};
 /** set GETY (device type ) */
 void setGety( uint16_t rui16_gety ) { GetyPos_c::setGety( rui16_gety );};
 /** set POS ( device type instance number ) */
 void setPos( uint16_t rui16_pos ) { GetyPos_c::setPos( rui16_pos );};
 /** assign value from another iGetyPos_c instance */
 const iGetyPos_c& operator=( const iGetyPos_c& refc_src ) { return static_cast<const iGetyPos_c&>( GetyPos_c::operator=( refc_src ) );};
 /** compare two iGetyPos_c values with operator== */
 bool operator==( const iGetyPos_c& refc_right ) const { return GetyPos_c::operator==( static_cast<const __IsoAgLib::GetyPos_c&>( refc_right ) );};
 /** compare two iGetyPos_c values with operator!= */
 bool operator!=( const iGetyPos_c& refc_right ) const { return GetyPos_c::operator!=( static_cast<const __IsoAgLib::GetyPos_c&>( refc_right ) );};
 /** compare two iGetyPos_c values with operator< */
 bool operator<( const iGetyPos_c& refc_right ) const { return GetyPos_c::operator<( static_cast<const __IsoAgLib::GetyPos_c&>( refc_right ) );};
 /** set this instance to indicator for unspecified value */
 void setUnspecified( void )  { GetyPos_c::setUnspecified();};
 /** check if this instance has specified value (different from default */
 bool isSpecified( void )      const { return GetyPos_c::isSpecified();};
 bool isUnspecified( void )    const { return GetyPos_c::isUnspecified();};
 bool isSpecifiedPos( void )   const { return GetyPos_c::isSpecifiedPos();};
 bool isUnspecifiedPos( void ) const { return GetyPos_c::isUnspecifiedPos();};
};
}

#endif
