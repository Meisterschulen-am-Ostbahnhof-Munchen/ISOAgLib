/***************************************************************************
                          isoname_c.cpp - handles the ISo 64bit NAME field
                             -------------------
    begin                : Wed Jan 03 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
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
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
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
#include <IsoAgLib/typedef.h>
#include <cstring>
#include "isoname_c.h"

/* ************************************** */
/* import some namespaces for easy access */
/* ************************************** */
using __IsoAgLib::ISOName_c;


/**
  constructor which can read in initial data from uint8_t string
  @param rpb_src 64bit input data string
*/
ISOName_c::ISOName_c(const uint8_t* rpb_src)
: u_data(rpb_src)
{ // simply copy 8byte string
}
/**
  constructor which can read in initial data from uint8_t string
  @param rpb_src 64bit input data string
*/
ISOName_c::ISOName_c(const Flexible8ByteString_c* rpu_src)
: u_data(*rpu_src)
{}

/**
  constructor which format data string from series of input flags
  @param rb_selfConf true -> indicate sefl configuring ECU
  @param rui8_indGroup industry group of device (2 for agriculture)
  @param rui8_devClass device class of ECU (equivalent to DEVCLASS in DIN)
  @param rui8_devClassInst instance number of ECU with same devClass
        in the network (comparable to POS in DIN9684)
  @param rb_func function of the ECU (usual 25 for network interconnect)
  @param rui16_manufCode code of manufactor (11bit)
  @param rui32_serNo serial no of specific device (21bit)
  @param rb_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
  @param rb_funcInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
ISOName_c::ISOName_c(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rui8_devClass, uint8_t rui8_devClassInst,
      uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst, uint8_t rb_ecuInst)
{ // call the set function for the single flags
  set(rb_selfConf, rui8_indGroup, rui8_devClass, rui8_devClassInst, rb_func, rui16_manufCode, rui32_serNo,
      rb_funcInst, rb_ecuInst);
}
/**
  copy constructor for ISOName
  @param rrefc_src source ISOName_c instance
*/
ISOName_c::ISOName_c(const ISOName_c& rrefc_src)
: u_data( rrefc_src.u_data )
{ // simply copy data string
}
/**
  assign constructor for ISOName
  @param rrefc_src source ISOName_c object
*/
const ISOName_c& ISOName_c::operator=(const ISOName_c& rrefc_src)
{ // simply copy data string
  u_data = rrefc_src.u_data;
  return rrefc_src;
}
/**
  default destructor
*/
ISOName_c::~ISOName_c()
{ // do nothing
}

/**
  set data string with all flags with one call
  @param rb_selfConf true -> indicate sefl configuring ECU
  @param rui8_indGroup industry group of device (2 for agriculture)
  @param rui8_devClass device class of ECU (equivalent to DEVCLASS in DIN)
  @param rui8_devClassInst instance number of ECU with same devClass
        in the network (comparable to POS in DIN9684)
  @param rb_func function of the ECU (usual 25 for network interconnect)
  @param rui16_manufCode code of manufactor (11bit)
  @param rui32_serNo serial no of specific device (21bit)
  @param rb_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
  @param rb_funcInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
void ISOName_c::set(bool rb_selfConf, uint8_t rui8_indGroup, uint8_t rui8_devClass, uint8_t rui8_devClassInst,
      uint8_t rb_func, uint16_t rui16_manufCode, uint32_t rui32_serNo, uint8_t rb_funcInst, uint8_t rb_ecuInst)
{ // call the set function for the single flags
  setSelfConf(rb_selfConf);
  setIndGroup(rui8_indGroup);
  setDevClass(rui8_devClass);
  setDevClassInst(rui8_devClassInst);
  setFunc(rb_func);
  setManufCode(rui16_manufCode);
  setSerNo(rui32_serNo);
  setFuncInst(rb_funcInst);
  setEcuInst(rb_ecuInst);
}


ISOName_c::ecuType_t ISOName_c::getEcuType() const
{
  const uint8_t cui8_func = func();
  switch (cui8_func)
  {
    case 23: return ecuTypeNavigation;
    case 29: return ecuTypeVirtualTerminal;
    case 61: return ecuTypeFileServerOrPrinter;
    default:
      if (cui8_func >= 128)
      {
        if (indGroup() == 2) // Agricultural & Forestry
        {
          if (devClass() == 0) // Non-specific systems
          {
            switch (cui8_func)
            {
              case 130: return ecuTypeTaskControl;
              case 134: return ecuTypeTractorECU;
              default:  break;
            }
          }
        }
      }
      break;
  }
  return ecuTypeANYOTHER;
}


/**
  get self config mode
  @return self configuration adress state
*/
uint8_t ISOName_c::selfConf() const
{
  return ( u_data[7] >> 7);
}

/**
  get industry group code
  @return industry group of device
*/
uint8_t ISOName_c::indGroup() const
{
  return ((u_data[7] >> 4) & 0x7) ;
}

/**
  get device class instance number
  @return:device class instance number
*/
uint8_t ISOName_c::devClassInst() const
{
  return (u_data[7] & 0xF);
}

/**
  get device class code
  @return:device class
*/
uint8_t ISOName_c::devClass() const
{
  return (u_data[6] >> 1);
}

/**
  get function code
  @return function code
*/
uint8_t ISOName_c::func() const
{
  return u_data[5];
}

/**
  get function instance code
  @return function instance code
*/
uint8_t ISOName_c::funcInst() const
{
  return (u_data[4] >> 3);
}

/**
  get ECU instance code
  @return ECU instance code
*/
uint8_t ISOName_c::ecuInst() const
{
  return (u_data[4] & 0x7);
}

/**
  get manufactor code
  @return manufactor code
*/
uint16_t ISOName_c::manufCode() const
{
  return ((u_data[3] << 3) | (u_data[2] >> 5));
}


/**
  get serial number
  @return serial number
*/
uint32_t ISOName_c::serNo() const
{
  return ((static_cast<uint32_t>(u_data[2] & 0x1F) << 16) | (static_cast<uint32_t>(u_data[1]) << 8) | u_data[0]);
}


/**
  set the NAME data from 8 uint8_t string
  @param rpb_src pointer to 8byte source string
*/
void ISOName_c::inputString(const uint8_t* rpb_src)
{
  if (NULL != rpb_src) u_data.setDataFromString( rpb_src );
}
/**
  set the NAME data from 8 uint8_t string
  @param rpb_src pointer to 8byte source string
*/
void ISOName_c::inputUnion(const Flexible8ByteString_c* rpu_src)
{
  if (rpu_src == NULL) return;
  // when we reach here, the source pointer is valid
  u_data = *rpu_src;
}

/**
  set self config mode
  @param rb_selfConf true -> indicate sefl configuring ECU
*/
void ISOName_c::setSelfConf(bool rb_selfConf)
{
  u_data.setUint8Data( 7, ((u_data[7] & 0x7F) | (rb_selfConf << 7)) );
}
/**
  set industry group code
  @param rui8_indGroup industry group of device (2 for agriculture)
*/
void ISOName_c::setIndGroup(uint8_t rui8_indGroup)
{
  u_data.setUint8Data( 7, ((u_data[7] & 0x8F) | ((rui8_indGroup & 0x7) << 4)) );
}
/**
  set device class instance number
  @param rui8_devClassInst instance number of ECU with same devClass
        in the network (comparable to POS in DIN9684)
*/
void ISOName_c::setDevClassInst(uint8_t rui8_devClassInst)
{
  u_data.setUint8Data( 7, ((u_data[7] & 0xF0) | (rui8_devClassInst)) );
}
/**
  set device class code
  @param rui8_devClass device class of ECU (equivalent to DEVCLASS in DIN)
*/
void ISOName_c::setDevClass(uint8_t rui8_devClass)
{
  u_data.setUint8Data( 6, ((0 /* reserved bit set to zero!*/) | (rui8_devClass << 1)) );
/* old version, which would be right if the reserved bit would have been set somewhere else.
  pb_data[6] = ((pb_data[6] & 0x1) | (rui8_devClass << 1));
*/
}
/**
  set function code
  @param rb_func function of the ECU (usual 25 for network interconnect)
*/
void ISOName_c::setFunc(uint8_t rb_func)
{
  u_data.setUint8Data( 5, rb_func );
}
/**
  set function instance code
  @param rb_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
*/
void ISOName_c::setFuncInst(uint8_t rb_funcInst)
{
  u_data.setUint8Data( 4, ((u_data[4] & 0x7) | (rb_funcInst << 3)) );
}
/**
  set ECU instance code
  @param rb_funcInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
void ISOName_c::setEcuInst(uint8_t rb_ecuInst)
{
  u_data.setUint8Data( 4, ((u_data[4] & 0xF8) | (rb_ecuInst & 0x7)) );
}
/**
  set manufactor code
  @param rui16_manufCode code of manufactor (11bit)
*/
void ISOName_c::setManufCode(uint16_t rui16_manufCode)
{
  u_data.setUint8Data( 3, (rui16_manufCode >> 3) );
  u_data.setUint8Data( 2, ((u_data[2] & 0x1F) | ((rui16_manufCode & 0x7) << 5)) );
}

/**
  set serial number (Identity Number)
  @param rui32_serNo serial no of specific device (21bit)
*/
void ISOName_c::setSerNo(uint32_t rui32_serNo)
{
  u_data.setUint16Data( 0, uint16_t(rui32_serNo & 0xFFFFU) );
  u_data.setUint8Data( 2, ( (u_data[2] & 0xE0) | ((rui32_serNo >> 16) & 0x1F) ) );
}


/**
  check if this NAME has higher prio
  than the given NAME 8-uint8_t string
  @param rpb_compare
  @return 0 == equal; -1 == this has lower prio than par; +1 == this item has higher prio than par
*/
int8_t ISOName_c::higherPriThanPar(const Flexible8ByteString_c* rpu_compare) const
{
#if defined(DEBUG) && !(SYSTEM_A1)
if ( rpu_compare == NULL )
{ // calling function called this function with wrong parameter
  // - but in production version, we await, that the caller makes sure,
  //  that the parameters are correct.
  // So output suitable debug information in DEBUG mode and trigger than an abort() to get a clear indication on this failure.
  // In real production version, we would have to decide on WHAT to do for this case in such a lowlevel comparison function - what return value, .....
  INTERNAL_DEBUG_DEVICE
    << "ERRORR!! ISOName_c::higherPriThanPar() was called with parameter == NULL!!" << EXTERNAL_DEBUG_DEVICE_ENDL
    << "The this adress was " << (int)this << EXTERNAL_DEBUG_DEVICE_ENDL
    << "The program will be aborted now for explicit detection of this erroneous call. Fix the CALLING function - and not this function,"
    << " as this function makes never sense when called with NULL!!"
    << EXTERNAL_DEBUG_DEVICE_ENDL;
  INTERNAL_DEBUG_FLUSH;
  abort();
}
#endif

  // if one of the both comparison parameters have 0xFF in the byte 0..5, then only compare
  // device class, -instance and industry group
  #if defined(OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN) && SIZEOF_INT >= 4
  static const uint64_t cui64_lazyEvaluationIndicator = 0x000000FFFFFFFFFFULL;
  if ( ( (       u_data.uint64[0] & cui64_lazyEvaluationIndicator ) == cui64_lazyEvaluationIndicator )
    || ( ( rpu_compare->uint64[0] & cui64_lazyEvaluationIndicator ) == cui64_lazyEvaluationIndicator ) )
  #else
  static const uint8_t lazyEvaluationIndicator[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  if ( ( CNAMESPACE::memcmp( u_data.uint8, lazyEvaluationIndicator, 5 ) == 0 ) || ( CNAMESPACE::memcmp( rpu_compare->uint8, lazyEvaluationIndicator, 5 ) == 0 ) )
  #endif
  { // perform only lazy evaluation
    // compare device class instance and industry group
    if ( (u_data.uint8[7] & 0x7F) > (rpu_compare->uint8[7] & 0x7F) ) return -1;
    else if ( (u_data.uint8[7] & 0x7F) < (rpu_compare->uint8[7] & 0x7F) ) return +1;
    // compare device class
    if ( (u_data.uint8[6] >> 1) > (rpu_compare->uint8[6] >> 1) ) return -1;
    else if ( (u_data.uint8[6] >> 1) < (rpu_compare->uint8[6] >> 1) ) return +1;
    // check if one part has a NOT spefified FUNCTION ( == 0xFF as default value ), as then
    // both sides should be regarded as equal for lazy evaluation
    if ( ( u_data.uint8[5] == 0xFF ) || ( rpu_compare->uint8[5] == 0xFF ) ) return 0;
    // compare Function as both have a non-default value
    if ( (u_data.uint8[5]) > (rpu_compare->uint8[5]) ) return -1;
    else if ( (u_data.uint8[5]) < (rpu_compare->uint8[5]) ) return +1;
    // if still here -> both should be regarded as equal for this compare level
    return 0;
  }
  // we reach here only, when the full ISONAME has to be compared
  // -> Flexible8ByteString_c::compare returns +1 when VALUE of u_data is LARGER
  // ==> priority is HIGHER when VALUE IS LOWER --> multiply by -1
  return (-1 * u_data.compare( *rpu_compare ));
}
