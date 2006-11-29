/***************************************************************************
                          genericdata_c.cpp -
                             -------------------
    begin                : Tue Oct 31 2006
    copyright            : (C) 1999 - 2006 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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
 * - Instatiation of a variable with a datatype from interface nsetIdentamespace,  *
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


#include ""

  /** this method is clearing the vector
  */
  void GenericData_c::ClearVector()
  {
    /* if the vector is not empty */
    if (vec_data.size() > 0)
    {
      /* clearing the vector */
      vec_data.clear();
    }
  }


  /**
    set ident for the telegram
    @param rui32_ident ident for the telegram
  */
  void GenericData_c::setIdent(uint32_t rui32_ident)
  {
    c_data.setIdent(rui32_ident)
  }

  /**
  */
  void GenericData_c::setDataUi8( uint16_t rui16_pos, uint8_t rui8_data)
  {
    vec_data.insert( ui16_pos16, rui8_data );
    //oder push_back ????? was ist besser
    c_data.setDataUi8(rui16_pos, rui8_data);
  }

  /**
  */
  void GenericData_c::setDataI8( uint16_t rui16_pos, uint8_t ri8_data)
  {
    c_data.setDataI8(rui16_pos, ri8_data);

    switch ( rui16_pos )
    {
      case 0: uint16[0] = rui8_val;
              vec_data.push_back();
      break;
      case 2: uint16[1] = rui8_val;
      break;
      case 1:
      break;
      default:
//        getLibErrInstance().registerError(LibErr_c::Range, LibErr_c::Can);
      break;
    };
  }

  /**
  */
  void GenericData_c::setDataUi16( uint16_t rui16_pos, uint16_t rui16_data)
  {
    c_data.setDataUi16(rui16_pos, rui16_data);
  }
  /**
  */
  void GenericData_c::setDataI16( uint16_t rui16_pos, int16_t ri16_data)
  {
    c_data.setDataI16(rui16_pos, ri16_data);
  }
  /**
  */
  void GenericData_c::setDataUi32( uint16_t rui16_pos, uint32_t rui32_data)
  {
    c_data.setDataUi32(rui16_pos, rui32_data);
  }
  /**
  */
  void GenericData_c::setDataI32( uint16_t rui16_pos, int32_t ri32_data)
  {
    c_data.setDataI32(rui16_pos, ri32_data);
  }
  /**
  */
  void GenericData_c::setDataStream(uint16_t rui16_bytePos, const uint8_t* rpui8_data, uint16_t rui16_dataLength)
  {
    if (rpui8_data != NULL )
    {
      const unsigned int cui_useLen = ( (rui16_dataLength + rui16_bytePos) < 8 ) ? (rui16_dataLength + rui16_bytePos) : 8;

      CNAMESPACE::memcpy(uint8 + rui16_bytePos, rpui8_data, cui_useLen );
    }
  }
  /**
  */
  uint8_t GenericData_c::getDataUi8( uint16_t rui16_pos, uint8_t rui8_data)
  {
    return( c_data.setDataUi8(rui16_pos, rui8_data);
  }
  /**
  */
  int8_t GenericData_c::getDataI8( uint16_t rui16_pos, uint8_t ri8_data) const
  {
    return( c_data.setDataI8(rui16_pos, ri8_data);
  }
  /**
  */
  uint16_t GenericData_c::getDataUi16( uint16_t rui16_pos, uint16_t rui16_data) const
  {
    return( c_data.setDataUi16(rui16_pos, rui16_data);
  }
  /**
  */
  int16_t GenericData_c::getDataI16( uint16_t rui16_pos, int16_t ri16_data) const
  {
    return( c_data.setDataI16(rui16_pos, ri16_data);
  }
  /**
  */
  uint32_t GenericData_c::getDataUi32( uint16_t rui16_pos, uint32_t rui32_data) const
  {
    return( c_data.setDataUi32(rui16_pos, rui32_data);
  }
  /**
  */
  int32_t GenericData_c::getDataI32( uint16_t rui16_pos, int32_t ri32_data) const
  {
    return( c_data.setDataI32(rui16_pos, ri32_data) );
  }
  /**
  */
  const uint8_t* GenericData_c::getDataStream(uint16_t rui16_bytePos) const
  {
    if ()
    return(vec_data);
  }



