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
const CANPkg_c& BasePkg_c::operator=(const CANPkg_c& rrefc_right)
{
	const BasePkg_c& rrefc_mine = static_cast<const BasePkg_c&>(rrefc_right);
  #ifdef USE_ISO_11783
  /** int32_t value in Byte 3 to 6 */
  i32_val36 = rrefc_mine.i32_val36;
  i16_val45 = rrefc_mine.i16_val45;
  #endif

  /** uint8_t data string for BABO and SEND */
  std::memcpy(pb_internalData, rrefc_mine.pb_internalData, 2);

  i16_val12 = rrefc_mine.i16_val12;
  i16_val34 = rrefc_mine.i16_val34;
  i16_val56 = rrefc_mine.i16_val56;
  i16_val78 = rrefc_mine.i16_val78;

  /** array for uint8_t values */
  std::memcpy(pb_val, rrefc_mine.pb_val, 9);

	return CANPkg_c::operator=(rrefc_right);
}

/** overloaded virtual function to translate the string data into flag values */
void BasePkg_c::string2Flags()
{
  #ifdef USE_ISO_11783
  if (identType() == Ident_c::StandardIdent)
  {
  #endif
     // set BABO code from bit 4-7 in identifier
     setBabo(((ident() >> 4) & 0xF));
     // set BABO code from bit 4-7 in identifier
     setSend(ident() & 0xF);
     // check for type of base msg
     if ( (babo()== 0x4) || (babo()==0x5) || (babo()==0x6) || (babo()==0xC) )
     { // base msg type 1, 2, 3 or fuel
       // the data format of all msg types is equal for uint8_t 1-6
       // the difference in uint8_t 7 and 8 is not important,
       // because it's no error to transform the value from uint8_t 7,8
       // both to 16bit int16_t and two uint8_t values
      i16_val12 = static_cast<int16_t>(pb_data[0]) + (static_cast<int16_t>(pb_data[1]) << 8);
      i16_val34 = static_cast<int16_t>(pb_data[2]) + (static_cast<int16_t>(pb_data[3]) << 8);
      i16_val56 = static_cast<int16_t>(pb_data[4]) + (static_cast<int16_t>(pb_data[5]) << 8);

       // for Base Data 1 (BABO == 0x4) with 16bit int16_t in data bytes 7,8
      i16_val78 = static_cast<int16_t>(pb_data[6]) + (static_cast<int16_t>(pb_data[7]) << 8);

       // for Base Data 2 (BABO == 0x5) with 8bit hitch back/front in data bytes 7/8
       setVal7(pb_data[6]);
       setVal8(pb_data[7]);

			// for Base Data Fuel (BABO == 0xC) -> Byte3 with fuel temp
 			setVal3(pb_data[2]);
     }
     else if (babo()== 0xF)
     { // lower priority base msg with calendar in uint8_t 1-8
       CNAMESPACE::memmove(pb_val, pb_data, 8);
     }
  #ifdef USE_ISO_11783
  }
  else
  { // its a ISO11783 msg
    isoString2Flags();
  }
  #endif
}

/** overloaded virtual function to translate flag values to data string */
void BasePkg_c::flags2String()
{
	#ifdef USE_ISO_11783
	if (identType() == Ident_c::StandardIdent)
	{
	#endif
		setIdent((1 << 8) | (babo() << 4) | send(), Ident_c::StandardIdent);
		/** now set values according BABO */
		if ( (babo()== 0x4) || (babo()==0x5) || (babo()==0x6) )
		{ // base msg type 1, 2 or 3
			// base msg 1,2 are equal in uint8_t 1-6
			// for setting of uint8_t 7-8 the msg type (BABO) must be checked

			pb_data[0] = (i16_val12 & 0xFF);
			pb_data[1] = (i16_val12 >> 8);
			pb_data[2] = (i16_val34 & 0xFF);
			pb_data[3] = (i16_val34 >> 8);
			pb_data[4] = (i16_val56 & 0xFF);
			pb_data[5] = (i16_val56 >> 8);

			// both base msg has 8 uint8_t length
			setLen(8);

			if (babo() == 0x4)
			{ // is't a base msg 1 (BABO = 0x4), where uint8_t 7,8 hold 16bit int
				pb_data[6] = (i16_val78 & 0xFF);
				pb_data[7] = (i16_val78 >> 8);
			}
			else if (babo() == 0x5)
			{ // it's a base msg 2 (BABO = 0x5), where uint8_t 7,8 hold two 8bit values
				pb_data[6] = val7();
				pb_data[7] = val8();
			}
			else
			{ // place nominal link force in Byte7
				pb_data[6] = val7();
				// set len to 7
				setLen(7);
			}
		}
		else if (babo()== 0xC)
		{ // fuel consumption
			pb_data[0] = (i16_val12 & 0xFF);
			pb_data[1] = (i16_val12 >> 8);
			pb_data[2] = val3();
			setLen(3);
		}
		else if (babo()== 0xF)
		{ // lower priority base msg with calendar information
			// represented as 7 uint8_t string
			CNAMESPACE::memmove(pb_data, pb_val, 8);
		setLen(7);
		}
	#ifdef USE_ISO_11783
	}
	else
	{ // its a ISO11783 msg
		isoFlags2String();
	}
	#endif
};

#ifdef USE_ISO_11783
/**
  translate ISO11783 extended ident string data to flags
*/
void BasePkg_c::isoString2Flags()
{
  uint16_t ui16_temp;
  switch (isoPgn())
  {
    case 0xFEE6:
      // time - date
      // read in string of single bytes
       CNAMESPACE::memmove(pb_val, pb_data, 8);
      break;
    case GROUND_BASED_SPEED_DIST_PGN:
    case WHEEL_BASED_SPEED_DIST_PGN:
      ui16_temp = static_cast<uint16_t>(pb_data[0]) + (static_cast<int16_t>(pb_data[1]) << 8);
      i16_val12 = ui16_temp;
      i32_val36 = static_cast<int32_t>(pb_data[2]) + (static_cast<int32_t>(pb_data[3]) << 8) +
                  (static_cast<int32_t>(pb_data[4]) << 16) + (static_cast<int32_t>(pb_data[5]) << 24);
       setVal7(pb_data[6]);
       setVal8(pb_data[7]);
      break;
    case FRONT_HITCH_STATE_PGN:
    case BACK_HITCH_STATE_PGN:
       setVal1(pb_data[0]);
       setVal2(pb_data[1]);
       setVal3(pb_data[2]);
      ui16_temp = static_cast<uint16_t>(pb_data[3]) + (static_cast<uint16_t>(pb_data[4]) << 8);
      i16_val45 = ui16_temp;
      break;
    case FRONT_PTO_STATE_PGN:
    case BACK_PTO_STATE_PGN:
      ui16_temp = static_cast<uint16_t>(pb_data[0]) + (static_cast<uint16_t>(pb_data[1]) << 8);
      i16_val12 = ui16_temp;
      ui16_temp = static_cast<uint16_t>(pb_data[2]) + (static_cast<uint16_t>(pb_data[3]) << 8);
      i16_val34 = ui16_temp;
       setVal5(pb_data[4]);
      break;
  }
}
/**
  translate ISO11783 flag data to extended CAN string
*/
void BasePkg_c::isoFlags2String()
{
  switch (isoPgn())
  {
		case TIME_DATE_PGN:
      // time - date
      // read in string of single bytes
       CNAMESPACE::memmove(pb_data, pb_val, 8);
			 setLen( 8 );
      break;
    case GROUND_BASED_SPEED_DIST_PGN:
    case WHEEL_BASED_SPEED_DIST_PGN:
      pb_data[0] = (static_cast<uint16_t>(i16_val12) & 0xFF);
      pb_data[1] = (static_cast<uint16_t>(i16_val12) >> 8);

      pb_data[2] = (i32_val36 & 0xFF);
      pb_data[3] = ((i32_val36 >> 8) & 0xFF);
      pb_data[4] = ((i32_val36 >> 16) & 0xFF);
      pb_data[5] = ((i32_val36 >> 24) & 0xFF);

       pb_data[6] = val7();
       pb_data[7] = val8();
      setLen(8);
      break;
    case FRONT_HITCH_STATE_PGN:
    case BACK_HITCH_STATE_PGN:
       pb_data[0] = val1();
       pb_data[1] = val2();
       pb_data[2] = val3();

       pb_data[3] = (static_cast<uint16_t>(i16_val45) & 0xFF);
      pb_data[4] = (static_cast<uint16_t>(i16_val45) >> 8);
      setLen(8);
      break;
    case FRONT_PTO_STATE_PGN:
    case BACK_PTO_STATE_PGN:
      pb_data[0] = (static_cast<uint16_t>(i16_val12) & 0xFF);
      pb_data[1] = (static_cast<uint16_t>(i16_val12) >> 8);

      pb_data[2] = (static_cast<uint16_t>(i16_val34) & 0xFF);
      pb_data[3] = (static_cast<uint16_t>(i16_val34) >> 8);

      pb_data[4] = val5();
      setLen(8);
      break;
  } // switch
}
#endif // USE_ISO_11783
} // end of namespace __IsoAgLib

