/*
  ident_c.cpp: source file for Ident

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "ident_c.h"

namespace __IsoAgLib
{
  /** constructor which can set the values
    @param at_ident new ident setting
    @param ren_identType new ident type
        (Ident_c::S for 11bit ident or Ident_c::E for 29bit)
  */
  Ident_c::Ident_c(MASK_TYPE at_ident, identType_t ren_identType)
  {
    mt_ident = (ren_identType == StandardIdent)?(at_ident&0x7FF):at_ident;
    mt_type = ren_identType;
  }

  /** constructor which gets its values from other instance
    @param acrc_src source Ident_c instance
  */
  Ident_c::Ident_c(const Ident_c& acrc_src)
  {
    mt_ident = acrc_src.mt_ident;
    mt_type = acrc_src.mt_type;
  }

  /** destructor which has nothing to do */
  Ident_c::~Ident_c()
  {
  }

  /** compare this ident setting with another
    (use memory operation for max speed)
    @param acrc_src compared ident
    @return true -> referenced ident has same setting and type
  */
  bool Ident_c::operator==(const Ident_c& acrc_src) const
  {
    return ((ident()==acrc_src.ident())
            &&(identType()==acrc_src.identType()));
  }

  /** set this ident
    @param at_ident new ident setting
    @param ren_identType new ident type
    @param ren_identType new ident type: 11bit Ident_c::StandardIdent or 29bit Ident_c::ExtendedIdent
  */
  void Ident_c::set(MASK_TYPE at_ident, identType_t ren_identType)
  {
    mt_ident = (ren_identType == StandardIdent)?(at_ident&0x7FF):at_ident;
    mt_type = ren_identType;
  }

  /** set this ident with access to single byte
    @param ab_val new val for ident at wanted position
    @param ab_pos position in ident, where ident should be placed in
  */
  void Ident_c::setByte(uint8_t ab_val, uint8_t ab_pos )
  {
    mt_ident &= ~(MASK_TYPE(0xFF) << (ab_pos*8));
    mt_ident |= (MASK_TYPE(ab_val) << (ab_pos*8));
  }

  /**
    set specific uint16_t of this ident
    (position 0 is least significant position -> nearest to DLC field of
    CAN frame)
    @param aui16_val value for ident at wanted position for the telegram
    @param aui8_pos position [0..1] for wanted value for ident for the telegram (pos0==byte0, pos1==byte2)
  */
  void Ident_c::setWord(uint16_t aui16_val, uint8_t aui8_pos)
  {
    mt_ident &= ~(MASK_TYPE(0xFFFF) << (aui8_pos*16));
    mt_ident |= (MASK_TYPE(aui16_val) << (aui8_pos*16));
  }

  /** deliver amount of different bits from own ident to compared ident
    @param acrc_ident reference to compared ident
    @return amount of different bits
  */
  uint8_t Ident_c::bit_diff(const Ident_c& acrc_ident)const
  {
    uint8_t cnt = 0;
    // XOR delivers '1' where both values are different
    MASK_TYPE ui32_comp = mt_ident ^ acrc_ident.mt_ident;
      for(MASK_TYPE ui32_new = (ui32_comp & (ui32_comp-1)); ui32_new != ui32_comp;
        ui32_comp=ui32_new, ui32_new &= (ui32_new-1))cnt++;
      return cnt;
  }

  /**
    deliver amount of different bits from own ident to compared ident
    @param acrc_ident reference to compared ident
    @param at_mask
    @return amount of different bits
  */
  uint8_t Ident_c::bitDiffWithMask(const Ident_c& acrc_ident, MASK_TYPE at_mask, unsigned int& ui_lsbFromDiff ) const
  {
    unsigned int cnt = 0;
    ui_lsbFromDiff = 0;
    // XOR delivers '1' where both values are different
    MASK_TYPE ui32_comp = (mt_ident & at_mask) ^ (acrc_ident.mt_ident & at_mask);
    for ( MASK_TYPE testMask = 1; testMask <= 0x10000000; testMask <<= 1 )
    {
      if ( ui32_comp & testMask )
      { // the checked bit is set in ui32_comp
        cnt++;
      }
      else if ( 0 == cnt )
      { // the compared bit is not set in ui32_comp --> the lowest possible bit position in
        // ui32_comp is another position higher
        ui_lsbFromDiff++;
      }
      // break from compare loop, as soon as the test mask value is equal/larger than compare
      // value, as no higher bit than in testMask can be set in ui32_comp
      if ( testMask >= ui32_comp ) break;
    }
      return cnt;
  };

  /**
    deliver amount of different bits from own ident to compared ident
    @param acrc_ident reference to compared ident
    @return amount of different bits
  */
  uint8_t Ident_c::bitDiff(const Ident_c& acrc_ident, unsigned int& ui_lsbFromDiff) const
  {
    unsigned int cnt = 0;
    ui_lsbFromDiff = 0;
    // XOR delivers '1' where both values are different
    MASK_TYPE ui32_comp = mt_ident ^ acrc_ident.mt_ident;
    for ( MASK_TYPE testMask = 1; testMask <= 0x10000000; testMask <<= 1 )
    {
      if ( ui32_comp & testMask )
      { // the checked bit is set in ui32_comp
        cnt++;
      }
      else if ( 0 == cnt )
      { // the compared bit is not set in ui32_comp --> the lowest possible bit position in
        // ui32_comp is another position higher
        ui_lsbFromDiff++;
      }
      // break from compare loop, as soon as the test mask value is equal/larger than compare
      // value, as no higher bit than in testMask can be set in ui32_comp
      if ( testMask >= ui32_comp ) break;
    }
      return cnt;
  };

}
