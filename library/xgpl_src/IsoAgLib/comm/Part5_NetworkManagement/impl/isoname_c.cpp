/*
  isoname_c.cpp: handles the ISO 64bit NAME field

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/hal/hal_typedef.h>
#include <cstring>
#include "isoname_c.h"
// necessary for convert operators
#include "../iisoname_c.h"
#include <IsoAgLib/util/iassert.h>

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
  // if NULL, mu_data will be 0ULL
}


IsoName_c::IsoName_c()
: mu_data() // initializes to 0ULL
{
  setUnspecified();
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
  @param acrc_src source IsoName_c instance
*/
IsoName_c::IsoName_c(const IsoName_c& acrc_src)
: mu_data( acrc_src.mu_data )
{ // simply copy data string
}

/** assign constructor for ISOName
  @param acrc_src source IsoName_c object
*/
const IsoName_c& IsoName_c::operator=(const IsoName_c& acrc_src)
{ // simply copy data string
  mu_data = acrc_src.mu_data;
  return acrc_src;
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
bool IsoName_c::selfConf() const
{
  return ( mu_data[7] >> 7) != 0;
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
  isoaglib_assert(apu_compare);

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
IsoName_c::isEqualRegardingNonInstFields (const IsoName_c& acrc_isoName) const
{
  return ( (devClass()  == acrc_isoName.devClass() )
        && (indGroup()  == acrc_isoName.indGroup() )
        && (func()      == acrc_isoName.func()     )
        && (serNo()     == acrc_isoName.serNo()    )
        && (selfConf()  == acrc_isoName.selfConf() )
        && (manufCode() == acrc_isoName.manufCode())
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
