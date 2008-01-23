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
// necessary for convert operators
#include "../iisoname_c.h"

#if defined(DEBUG)
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


namespace __IsoAgLib {


/** constant for default parameters and initialization, where the device type is not yet spcified.
 */
const IsoName_c&
IsoName_c::IsoNameUnspecified()
{
  static IsoName_c sc_isoNameUnspecified( 0x7F, 0xF );
  return sc_isoNameUnspecified;
}

/** constant for not yet spcified process data ident -> <device class, device class instance> := <0x0,0xF>
  */
const IsoName_c&
IsoName_c::IsoNameInitialProcessData()
{
  static IsoName_c sc_isoNameInitialProcessData( 0x00, 0xF );
  return sc_isoNameInitialProcessData;
}


/** constructor which can read in initial data from uint8_t string
  @param apb_src 64bit input data string
*/
IsoName_c::IsoName_c(const uint8_t* apb_src)
: mu_data(apb_src)
{ // simply copy 8byte string
}

/** constructor which can read in initial data from uint8_t string
  @param apu_src  64bit input data string
*/
IsoName_c::IsoName_c(const Flexible8ByteString_c* apu_src)
: mu_data(*apu_src)
{}

/** constructor which format data string from series of input flags
  @param ab_selfConf true -> indicate sefl configuring ECU
  @param aui8_indGroup industry group of device (2 for agriculture)
  @param aui8_devClass device class of ECU
  @param aui8_devClassInst instance number of ECU with same devClass in the network
  @param ab_func function of the ECU (usual 25 for network interconnect)
  @param aui16_manufCode code of manufactor (11bit)
  @param aui32_serNo serial no of specific device (21bit)
  @param ab_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
  @param ab_ecuInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
IsoName_c::IsoName_c(bool ab_selfConf, uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst,
      uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t ab_funcInst, uint8_t ab_ecuInst)
{ // call the set function for the single flags
  set(ab_selfConf, aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo,
      ab_funcInst, ab_ecuInst);
}

/** copy constructor for ISOName
  @param arcc_src source IsoName_c instance
*/
IsoName_c::IsoName_c(const IsoName_c& arcc_src)
: mu_data( arcc_src.mu_data )
{ // simply copy data string
}

/** assign constructor for ISOName
  @param arcc_src source IsoName_c object
*/
const IsoName_c& IsoName_c::operator=(const IsoName_c& arcc_src)
{ // simply copy data string
  mu_data = arcc_src.mu_data;
  return arcc_src;
}

/** default destructor */
IsoName_c::~IsoName_c() {}

/** set data string with all flags with one call
  @param ab_selfConf true -> indicate sefl configuring ECU
  @param aui8_indGroup industry group of device (2 for agriculture)
  @param aui8_devClass device class of ECU
  @param aui8_devClassInst instance number of ECU with same devClass in the network
  @param ab_func function of the ECU (usual 25 for network interconnect)
  @param aui16_manufCode code of manufactor (11bit)
  @param aui32_serNo serial no of specific device (21bit)
  @param ab_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
  @param ab_funcInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
void IsoName_c::set(bool ab_selfConf, uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst,
      uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t ab_funcInst, uint8_t ab_ecuInst)
{ // call the set function for the single flags
  setSelfConf(ab_selfConf);
  setIndGroup(aui8_indGroup);
  setDevClass(aui8_devClass);
  setDevClassInst(aui8_devClassInst);
  setFunc(ab_func);
  setManufCode(aui16_manufCode);
  setSerNo(aui32_serNo);
  setFuncInst(ab_funcInst);
  setEcuInst(ab_ecuInst);
}

/** set device class & instance with two seperate parameters */
void IsoName_c::set( uint8_t aui8_devClass, uint8_t aui8_pos ) { setDevClass( aui8_devClass); setDevClassInst( aui8_pos ); }

IsoName_c::ecuType_t IsoName_c::getEcuType() const
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

/** get self config mode
  @return self configuration adress state
*/
uint8_t IsoName_c::selfConf() const
{
  return ( mu_data[7] >> 7);
}

/** get industry group code
  @return industry group of device
*/
uint8_t IsoName_c::indGroup() const
{
  return ((mu_data[7] >> 4) & 0x7) ;
}

/** get device class instance number
  @return:device class instance number
*/
uint8_t IsoName_c::devClassInst() const
{
  return (mu_data[7] & 0xF);
}

/** get device class code
  @return:device class
*/
uint8_t IsoName_c::devClass() const
{
  return (mu_data[6] >> 1);
}

/** get function code
  @return function code
*/
uint8_t IsoName_c::func() const
{
  return mu_data[5];
}

/** get function instance code
  @return function instance code
*/
uint8_t IsoName_c::funcInst() const
{
  return (mu_data[4] >> 3);
}

/** get ECU instance code
  @return ECU instance code
*/
uint8_t IsoName_c::ecuInst() const
{
  return (mu_data[4] & 0x7);
}

/** get manufactor code
  @return manufactor code
*/
uint16_t IsoName_c::manufCode() const
{
  return ((mu_data[3] << 3) | (mu_data[2] >> 5));
}

/** get serial number
  @return serial number
*/
uint32_t IsoName_c::serNo() const
{
  return ((static_cast<uint32_t>(mu_data[2] & 0x1F) << 16) | (static_cast<uint32_t>(mu_data[1]) << 8) | mu_data[0]);
}

/** set the NAME data from 8 uint8_t string
  @param apb_src pointer to 8byte source string
*/
void IsoName_c::inputString(const uint8_t* apb_src)
{
  if (NULL != apb_src) mu_data.setDataFromString( apb_src );
}

/** set the NAME data from 8 uint8_t string
  @param apu_src pointer to 8byte source string
*/
void IsoName_c::inputUnion(const Flexible8ByteString_c* apu_src)
{
  if (apu_src == NULL) return;
  // when we reach here, the source pointer is valid
  mu_data = *apu_src;
}

/** set self config mode
  @param ab_selfConf true -> indicate sefl configuring ECU
*/
void IsoName_c::setSelfConf(bool ab_selfConf)
{
  mu_data.setUint8Data( 7, ((mu_data[7] & 0x7F) | (ab_selfConf << 7)) );
}

/** set industry group code
  @param aui8_indGroup industry group of device (2 for agriculture)
*/
void IsoName_c::setIndGroup(uint8_t aui8_indGroup)
{
  mu_data.setUint8Data( 7, ((mu_data[7] & 0x8F) | ((aui8_indGroup & 0x7) << 4)) );
}

/** set device class instance number
  @param aui8_devClassInst instance number of ECU with same devClass in the network
*/
void IsoName_c::setDevClassInst(uint8_t aui8_devClassInst)
{
  mu_data.setUint8Data( 7, ((mu_data[7] & 0xF0) | (aui8_devClassInst)) );
}

/** set device class code
  @param aui8_devClass device class of ECU
*/
void IsoName_c::setDevClass(uint8_t aui8_devClass)
{
  mu_data.setUint8Data( 6, ((0 /* reserved bit set to zero!*/) | (aui8_devClass << 1)) );
/* old version, which would be right if the reserved bit would have been set somewhere else.
  pb_data[6] = ((pb_data[6] & 0x1) | (aui8_devClass << 1));
*/
}

/** set function code
  @param ab_func function of the ECU (usual 25 for network interconnect)
*/
void IsoName_c::setFunc(uint8_t ab_func)
{
  mu_data.setUint8Data( 5, ab_func );
}

/** set function instance code
  @param ab_funcInst instance number of ECU with same function and device class
      (default 0 - normally)
*/
void IsoName_c::setFuncInst(uint8_t ab_funcInst)
{
  mu_data.setUint8Data( 4, ((mu_data[4] & 0x7) | (ab_funcInst << 3)) );
}

/** set ECU instance code
  @param ab_ecuInst instance number of ECU with same function, device class and function instance
      (default 0 - normally)
*/
void IsoName_c::setEcuInst(uint8_t ab_ecuInst)
{
  mu_data.setUint8Data( 4, ((mu_data[4] & 0xF8) | (ab_ecuInst & 0x7)) );
}

/** set manufactor code
  @param aui16_manufCode code of manufactor (11bit)
*/
void IsoName_c::setManufCode(uint16_t aui16_manufCode)
{
  mu_data.setUint8Data( 3, (aui16_manufCode >> 3) );
  mu_data.setUint8Data( 2, ((mu_data[2] & 0x1F) | ((aui16_manufCode & 0x7) << 5)) );
}

/** set serial number (Identity Number)
  @param aui32_serNo serial no of specific device (21bit)
*/
void IsoName_c::setSerNo(uint32_t aui32_serNo)
{
  mu_data.setUint16Data( 0, uint16_t(aui32_serNo & 0xFFFFU) );
  mu_data.setUint8Data( 2, ( (mu_data[2] & 0xE0) | ((aui32_serNo >> 16) & 0x1F) ) );
}

/** check if this NAME has higher prio
  than the given NAME 8-uint8_t string
  @param apu_compare
  @return 0 == equal; -1 == this has lower prio than par; +1 == this item has higher prio than par
*/
int8_t IsoName_c::higherPriThanPar(const Flexible8ByteString_c* apu_compare) const
{
#if defined(DEBUG) && !defined(SYSTEM_A1) && !defined(SYSTEM_A5) && defined(SYSTEM_PC)
  if ( apu_compare == NULL )
  { // calling function called this function with wrong parameter
    // - but in production version, we await, that the caller makes sure,
    //  that the parameters are correct.
    // So output suitable debug information in DEBUG mode and trigger than an abort() to get a clear indication on this failure.
    // In real production version, we would have to decide on WHAT to do for this case in such a lowlevel comparison function - what return value, .....
    INTERNAL_DEBUG_DEVICE
      << "ERRORR!! IsoName_c::higherPriThanPar() was called with parameter == NULL!!" << INTERNAL_DEBUG_DEVICE_ENDL
      << "The this adress was " << this << INTERNAL_DEBUG_DEVICE_ENDL
      << "The program will be aborted now for explicit detection of this erroneous call. Fix the CALLING function - and not this function,"
      << " as this function makes never sense when called with NULL!!"
      << INTERNAL_DEBUG_DEVICE_ENDL;
    INTERNAL_DEBUG_FLUSH;
    abort();
  }
#endif

  // if one of the both comparison parameters have 0xFF in the byte 0..5, then only compare
  // device class, -instance and industry group
  #if defined(OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN) && SIZEOF_INT >= 4
  static const uint64_t cui64_lazyEvaluationIndicator = 0x000000FFFFFFFFFFULL;
  if ( ( (       mu_data.uint64[0] & cui64_lazyEvaluationIndicator ) == cui64_lazyEvaluationIndicator )
    || ( ( apu_compare->uint64[0] & cui64_lazyEvaluationIndicator ) == cui64_lazyEvaluationIndicator ) )
  #else
  static const uint8_t lazyEvaluationIndicator[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  if ( ( CNAMESPACE::memcmp( mu_data.uint8, lazyEvaluationIndicator, 5 ) == 0 ) || ( CNAMESPACE::memcmp( apu_compare->uint8, lazyEvaluationIndicator, 5 ) == 0 ) )
  #endif
  { // perform only lazy evaluation
    // compare device class instance and industry group
    if ( (mu_data.uint8[7] & 0x7F) > (apu_compare->uint8[7] & 0x7F) ) return -1;
    else if ( (mu_data.uint8[7] & 0x7F) < (apu_compare->uint8[7] & 0x7F) ) return +1;
    // compare device class
    if ( (mu_data.uint8[6] >> 1) > (apu_compare->uint8[6] >> 1) ) return -1;
    else if ( (mu_data.uint8[6] >> 1) < (apu_compare->uint8[6] >> 1) ) return +1;
    // check if one part has a NOT spefified FUNCTION ( == 0xFF as default value ), as then
    // both sides should be regarded as equal for lazy evaluation
    if ( ( mu_data.uint8[5] == 0xFF ) || ( apu_compare->uint8[5] == 0xFF ) ) return 0;
    // compare Function as both have a non-default value
    if ( (mu_data.uint8[5]) > (apu_compare->uint8[5]) ) return -1;
    else if ( (mu_data.uint8[5]) < (apu_compare->uint8[5]) ) return +1;
    // if still here -> both should be regarded as equal for this compare level
    return 0;
  }
  // we reach here only, when the full ISONAME has to be compared
  // -> Flexible8ByteString_c::compare returns +1 when VALUE of mu_data is LARGER
  // ==> priority is HIGHER when VALUE IS LOWER --> multiply by -1
  return (-1 * mu_data.compare( *apu_compare ));
}


/** Check if all Non-Instance fields of both ISONames match
  @return true if equal, false if one non-inst field differs!
*/
bool
IsoName_c::isEqualRegardingNonInstFields (const IsoName_c& arcc_isoName) const
{
  return ( (devClass()  == arcc_isoName.devClass() )
        && (indGroup()  == arcc_isoName.indGroup() )
        && (func()      == arcc_isoName.func()     )
        && (serNo()     == arcc_isoName.serNo()    )
        && (selfConf()  == arcc_isoName.selfConf() )
        && (manufCode() == arcc_isoName.manufCode())
         );
}

/** convert function - avoids lots of explicit static_casts */
IsoAgLib::iIsoName_c& IsoName_c::toIisoName_c()
{
  return static_cast<IsoAgLib::iIsoName_c&>(*this);
  //return static_cast<IsoAgLib::iIsoName_c&>(*static_cast<IsoAgLib::iIsoName_c*>((void *)(this)));
  // doing it the ugly way, as some compiler do a recursive call of this operator at the "correct"
  // "return static_cast<IsoAgLib::iIsoName_c&>(*this);" statement - some do not.
  // with the above way we can get absolutely sure that there are no recursive uses of this operator
  // as those would overflow the stack and hence cause a system-crash
}

/** convert function - avoids lots of explicit static_casts */
const IsoAgLib::iIsoName_c& IsoName_c::toConstIisoName_c() const
{
  return static_cast<const IsoAgLib::iIsoName_c&>(*this);
//  return static_cast<const IsoAgLib::iIsoName_c&>(*static_cast<const IsoAgLib::iIsoName_c*>((void *)(this)));
  // doing it the ugly way, as some compiler do a recursive call of this operator at the "correct"
  // "return static_cast<const IsoAgLib::iIsoName_c&>(*this);" statement - some do not.
  // with the above way we can get absolutely sure that there are no recursive uses of this operator
  // as those would overflow the stack and hence cause a system-crash
}

} // namespace __IsoAgLib
