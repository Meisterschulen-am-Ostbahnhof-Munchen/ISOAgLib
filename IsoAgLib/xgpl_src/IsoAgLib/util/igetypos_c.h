/***************************************************************************
                          igetypos_c.h - class iDevKey_c combines device class
                                      ( GETY_POS for DIN 9684 ) and instance
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
#ifndef IDEVCLASSPOS_H
#define IDEVCLASSPOS_H


/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/getypos_c.h"
#ifdef USE_ISO_11783
#include <IsoAgLib/comm/SystemMgmt/ISO11783/iisoname_c.h>
#endif
// Begin Namespace IsoAgLib
namespace IsoAgLib {
class iDevKey_c : public __IsoAgLib::DevKey_c {
 public:
    /** constant for default parameters and initialization, where the device type is not yet spcified.
        the instantiation of this constant variable is located in the module cancustomer_c.cpp
      */
    static const iDevKey_c DevKeyUnspecified;
    /** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
        the instantiation of this constant variable is located in the module cancustomer_c.cpp
      */
    static const iDevKey_c DevKeyInitialProcessData;
  /** default constructor
    * @param rui16_devClass optional initial DEVCLASS (device type)
    * @param rui16_pos optional initial device class instance
    */
  #ifdef USE_ISO_11783
  iDevKey_c( uint8_t rui8_devClass = 0x7F, uint8_t rui8_pos = 0xF )
  : DevKey_c( rui8_devClass, rui8_pos ){};
  #else
  iDevKey_c( uint16_t rui16_devClass = 0x7F, uint16_t rui16_pos = 0xF )
  : DevKey_c( rui16_devClass, rui16_pos ){};
  #endif
  iDevKey_c( const __IsoAgLib::DevKey_c& refc_src )
    : DevKey_c( refc_src )
  {};
  #ifdef USE_ISO_11783
  /** default constructor
    * @param rui16_devClass optional initial DEVCLASS (device type)
    * @param rui16_pos optional initial device class instance
    */
  iDevKey_c(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rui8_devClass, uint8_t rui8_devClassInst,
      uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst = 0, uint8_t rb_ecuInst = 0)
  : DevKey_c(rb_selfConf, rui8_indGroup, rui8_devClass, rui8_devClassInst,
      rb_func, rui16_manufCode, rui32_serNo, rb_funcInst, rb_ecuInst) {};
  iDevKey_c( const uint8_t* rpui8_dataName ) : DevKey_c( rpui8_dataName ) {};
  #endif
  /** set device class & instance with two seperate parameters */
  #ifdef USE_ISO_11783
  void set( uint8_t rui8_devClass, uint8_t rui8_pos ) { DevKey_c::set( rui8_devClass, rui8_pos );};
  #else
  void set( uint16_t rui16_devClass, uint16_t rui16_pos ) { DevKey_c::set( rui16_devClass, rui16_pos );};
  #endif
  #ifdef USE_ISO_11783
  /** set device class & instance with two seperate parameters */
  void set( const iISOName_c& rrefc_isoName ) { DevKey_c::set( rrefc_isoName );};
  /** set device class & instance with two seperate parameters */
  void set( const uint8_t* rpui8_dataName ) { DevKey_c::set( rpui8_dataName );};
  /** provide pointer to second level compare NAME */
  iISOName_c& getName() { return static_cast<IsoAgLib::iISOName_c&>(DevKey_c::getName());};
  /** provide pointer to second level compare NAME */
  const iISOName_c& getConstName() const { return static_cast<const IsoAgLib::iISOName_c&>(DevKey_c::getConstName());};
  #endif
 /** deliver DEVCLASS (device type ) */
 uint8_t getDevClass( void ) const { return DevKey_c::getDevClass();};
 /** deliver _instance_ ( device type instance number ) */
 uint8_t getDevClassInst( void ) const { return DevKey_c::getDevClassInst();};
 /** set DEVCLASS (device type ) */
#ifdef USE_ISO_11783
 void setDevClass( uint8_t rui8_devClass ) { DevKey_c::setDevClass( rui8_devClass );};
#else
 void setDevClass( uint16_t rui16_devClass ) { DevKey_c::setDevClass( rui16_devClass );};
#endif
 /** set device class instance */
#ifdef USE_ISO_11783
 void setDevClassInst( uint8_t rui8_pos ) { DevKey_c::setDevClassInst( rui8_pos );};
#else
 void setDevClassInst( uint16_t rui16_pos ) { DevKey_c::setDevClassInst( rui16_pos );};
#endif
 /** assign value from another iDevKey_c instance */
 const iDevKey_c& operator=( const iDevKey_c& refc_src ) { return static_cast<const iDevKey_c&>( DevKey_c::operator=( refc_src ) );};
 /** compare two iDevKey_c values with operator== */
 bool operator==( const iDevKey_c& refc_right ) const { return DevKey_c::operator==( static_cast<const __IsoAgLib::DevKey_c&>( refc_right ) );};
 /** compare two iDevKey_c values with operator!= */
 bool operator!=( const iDevKey_c& refc_right ) const { return DevKey_c::operator!=( static_cast<const __IsoAgLib::DevKey_c&>( refc_right ) );};
 /** compare two iDevKey_c values with operator< */
 bool operator<( const iDevKey_c& refc_right ) const { return DevKey_c::operator<( static_cast<const __IsoAgLib::DevKey_c&>( refc_right ) );};
 /** set this instance to indicator for unspecified value */
 void setUnspecified( void )  { DevKey_c::setUnspecified();};
 /** check if this instance has specified value (different from default */
 bool isSpecified( void )      const { return DevKey_c::isSpecified();};
 bool isUnspecified( void )    const { return DevKey_c::isUnspecified();};
 bool isSpecifiedDevClassInst( void )   const { return DevKey_c::isSpecifiedDevClassInst();};
 bool isUnspecifiedDevClassInst( void ) const { return DevKey_c::isUnspecifiedDevClassInst();};
};
}

#endif
