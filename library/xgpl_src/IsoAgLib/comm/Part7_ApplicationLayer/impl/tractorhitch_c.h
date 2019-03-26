/*
  tractorhitch_c.h: common class for front and rear hitch.

  (C) Copyright 2016 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACTORHITCH_C_H
#define TRACTORHITCH_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorcommonrx_c.h>


namespace __IsoAgLib
{

  class TractorHitch_c : public TractorCommonRx_c
  {
  public:

  protected:
    TractorHitch_c( uint32_t pgn ) : TractorCommonRx_c( TractorCommonRx_c::TIMEOUT_SENDING_NODE_J1939, pgn ) {}

    /** @return hitch up position [0,0%;100.0%], res: 0,4%/bit, offset: 0 */
    uint8_t getPosition() const { return mui8_position; }
    
    /** @return nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
    uint8_t getNominalLowerLinkForce() const { return mui8_linkForce; }

    /** @return hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
    uint16_t getDraft() const { return mui16_draft; }

    /** @return code Note this is 6 bit only! */
    uint8_t getExitReasonCode() const { return mui8_exitReasonCode; }

    IsoAgLib::IsoHitchInWorkIndicationFlag_t getInWorkIndication() const { return mt_inWorkIndication; }

    IsoAgLib::IsoLimitFlag_t getPositionLimit() const { return mt_posLimitStatus; }

  private:
    virtual void setValues( const CanPkgExt_c& arc_data );
    virtual void resetValues();

  private:
    /** hitch up position [0,0%;100.0%], res: 0,4%/bit, offset: 0 */
    uint8_t mui8_position;
    
    /** nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
    uint8_t mui8_linkForce;

    /** hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
    uint16_t mui16_draft;

    uint8_t mui8_exitReasonCode; // 6 bit only!

    IsoAgLib::IsoHitchInWorkIndicationFlag_t mt_inWorkIndication;
    IsoAgLib::IsoLimitFlag_t mt_posLimitStatus;
  };



}
#endif
