/***************************************************************************
                          ident_c.cpp  -  source file for Ident
                             -------------------
    begin                : Sun Dec 24 2000
    copyright            : (c) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
#include "ident_c.h"

namespace __IsoAgLib
{
  /** constructor which can set the values
    @param rt_ident new ident setting
    @param ren_identType new ident type
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
        (default: DEFAULT_IDENT_TYPE set in isoaglib_config)
  */
  Ident_c::Ident_c(MASK_TYPE rt_ident, identType_t ren_identType)
  {
    t_ident = (ren_identType == StandardIdent)?(rt_ident&0x7FF):rt_ident;
    data.type = ren_identType;
    data.empty = 0;
  }

  /** constructor which gets its values from other instance
    @param rrefc_src source Ident_c instance
  */
  Ident_c::Ident_c(const Ident_c& rrefc_src)
    {CNAMESPACE::memcpy(this, &rrefc_src, sizeof(Ident_c));}

  /** destructor which has nothing to do */
  Ident_c::~Ident_c()
  {
  }

  /** compare this ident setting with another
    (use memory operation for max speed)
    @param rrefc_src compared ident
    @return true -> referenced ident has same setting and type
  */
  bool Ident_c::operator==(const Ident_c& rrefc_src) const
  {
    return ((ident()==rrefc_src.ident())
            &&(identType()==rrefc_src.identType())
            &&(empty()==rrefc_src.empty()))?true:false;
  }

  /** set this ident
    @param rt_ident new ident setting
    @param ren_identType new ident type
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
        (default defined in isoaglib_config.h)
  */
  void Ident_c::set(MASK_TYPE rt_ident, identType_t ren_identType)
  {
    t_ident = (ren_identType == StandardIdent)?(rt_ident&0x7FF):rt_ident;
    data.type = ren_identType;
    data.empty = 0;
  }

  /** set this ident with access to single byte
    @param rb_val new val for ident at wanted position
    @param rb_pos position in ident, where ident should be placed in
    @param ren_identType new ident type
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
        (default defined in isoaglib_config.h)
  */
  void Ident_c::set(uint8_t rb_val, uint8_t rb_pos, identType_t ren_identType)
  {
#if defined( OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN )
    pb_ident[rb_pos] = rb_val;
#elif defined(  OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN )
    pb_ident[sizeof(MASK_TYPE) - 1 - rb_pos] = rb_val;
#else
    const uint bitCount = (rb_pos*8);
    MASK_TYPE clearMask = ~(0xFF << bitCount);
    t_ident &= clearMask;
    t_ident |= (MASK_TYPE(rb_val) << (rb_pos*8));
#endif
    data.type = ren_identType;
    data.empty = 0;
  }

  /** deliver amount of different bits from own ident to compared ident
    @param rrefc_ident reference to compared ident
    @return amount of different bits
  */
  uint8_t Ident_c::bit_diff(const Ident_c& rrefc_ident)const
  {
      uint8_t cnt = 0;
    // XOR delivers '1' where both values are different
    MASK_TYPE ui32_comp = t_ident ^ rrefc_ident.t_ident;
      for(MASK_TYPE ui32_new = (ui32_comp & (ui32_comp-1)); ui32_new != ui32_comp;
        ui32_comp=ui32_new, ui32_new &= (ui32_new-1))cnt++;
      return cnt;
  }
}
