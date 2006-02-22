/***************************************************************************
                          basepkg_c.cpp - header file for CAN telgram
                                        encapsulating object, which presents
                                        the base msg informations through
                                        easy flag access; manages base msg
                                        type 1,2 and lower priority
                                        msg like calendar
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
#include "basepkg_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>

namespace __IsoAgLib {

/** default constructor which has nothing to do */
BasePkg_c::BasePkg_c(){
}
/** default destructor which has nothing to do */
BasePkg_c::~BasePkg_c(){
}
/**
	assign operator to insert informations from one CANPkg_c into another
	@see __IsoAgLib::FilterBox_c::operator>>
	@see CANPkgExt_c::operator=
	@see CANPkgExt_c::getData
	@param rrefc_right reference to the source CANPkg_c on the right
	@return reference to the source CANPkg_c to enable assign chains like
			"pkg1 = pkg2 = pkg3 = pkg4;"
*/
/*const CANPkg_c& BasePkg_c::operator=(const CANPkg_c& rrefc_right)
{
	const BasePkg_c& rrefc_mine = static_cast<const BasePkg_c&>(rrefc_right);
  #ifdef USE_ISO_11783
  // int32_t value in Byte 3 to 6
  i32_val36 = rrefc_mine.i32_val36;
  i16_val45 = rrefc_mine.i16_val45;
  #endif

  // uint8_t data string for BABO and SEND
  std::memcpy(pb_internalData, rrefc_mine.pb_internalData, 2);

  i16_val12 = rrefc_mine.i16_val12;
  i16_val34 = rrefc_mine.i16_val34;
  i16_val56 = rrefc_mine.i16_val56;
  i16_val78 = rrefc_mine.i16_val78;

  // array for uint8_t values
  std::memcpy(pb_val, rrefc_mine.pb_val, 9);

	return CANPkg_c::operator=(rrefc_right);
}
*/
/** overloaded virtual function to translate the string data into flag values */
/*void BasePkg_c::string2Flags()
{
  #ifdef USE_ISO_11783
  if (identType() == Ident_c::StandardIdent)
  {
  }
  else
  { // its a ISO11783 msg
  //  isoString2Flags();
  }
  #endif
}
*/

/** overloaded virtual function to translate flag values to data string */
/*void BasePkg_c::flags2String()
{
	#ifdef USE_ISO_11783
	if (identType() == Ident_c::StandardIdent)
	{
	}
	else
	{ // its a ISO11783 msg
	//	isoFlags2String();
	}
	#endif
};
*/
#ifdef USE_ISO_11783
/**
  translate ISO11783 extended ident string data to flags
*/
/*void BasePkg_c::isoString2Flags()
{
}*/
/**
  translate ISO11783 flag data to extended CAN string
*/
/*void BasePkg_c::isoFlags2String()
{
}*/
#endif // USE_ISO_11783
} // end of namespace __IsoAgLib

