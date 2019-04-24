/*
  tractorpto_c.h: common class for front and rear PTO.

  (C) Copyright 2017 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACTORPTO_C_H
#define TRACTORPTO_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorcommonrx_c.h>


namespace __IsoAgLib
{

  class TractorPto_c : public TractorCommonRx_c
  {
  public:

  protected:
    TractorPto_c( uint32_t pgn ) : TractorCommonRx_c( TractorCommonRx_c::TIMEOUT_SENDING_NODE_J1939, pgn ) {}

    /** get output shaft speed
      @return actual rpm speed of PTO [1/8RPM]
     */
    uint16_t getSpeed() const { return mui16_speed8DigitPerRpm; }

    /** get measured value of the set point of the rotational speed of the power take-off (PTO) output shaft
        @return measured value of the set point [1/8RPM]
      */
    uint16_t getSpeedSetPoint() const { return mui16_speedSetPoint8DigitPerRpm; }

    /** deliver explicit information whether PTO is engaged
      * @return IsoActive -> PTO is engaged
      */
    IsoAgLib::IsoActiveFlag_t getEngagement() const { return mt_ptoEngaged; }

    /** deliver std RPM of PTO (1000 n/min; 540 n/min)
      * @return IsoActive == 1000 n/min; false == 540 n/min
      */
    IsoAgLib::IsoActiveFlag_t getMode1000() const { return mt_pto1000; }

    /** deliver economy mode of PTO
      * @return IsoActive -> PTO is in economy mode
      */
    IsoAgLib::IsoActiveFlag_t getEconomyMode() const { return mt_ptoEconomy; }

    /** get reported tractor ECU's status of engagement
        @return reported status
      */
    IsoAgLib::IsoReqFlag_t getEngagementReqStatus() const { return mt_ptoEngagementReqStatus; }

    /** get reported tractor ECU's status of mode
         @return reported status
      */
    IsoAgLib::IsoReqFlag_t getModeReqStatus() const { return mt_ptoModeReqStatus; }

    /** get reported tractor ECU's status of economy mode
        @return reported status
      */
    IsoAgLib::IsoReqFlag_t getEconomyModeReqStatus() const { return mt_ptoEconomyModeReqStatus; }

    /** get present limit status of pto shaft speed
        @return present limit status
      */
    IsoAgLib::IsoLimitFlag_t getShaftSpeedLimitStatus() const { return mt_ptoShaftSpeedLimitStatus; }

  private:
    virtual void setValues( const CanPkgExt_c& arc_data );
    virtual void resetValues();

  private:
    /** pto [1/8RPM] */
    uint16_t mui16_speed8DigitPerRpm;
	
    /** measured value of the set point of the rotational speed of the power take-off (PTO) output shaft [1/8RPM] */
    uint16_t mui16_speedSetPoint8DigitPerRpm;
	
    /** PTO engaged */
    IsoAgLib::IsoActiveFlag_t mt_ptoEngaged;
	
    /** mode of PTO: true == 1000 n/min; false == 540 n/min */
    IsoAgLib::IsoActiveFlag_t mt_pto1000;
	
    /** economy mode of PTO */
    IsoAgLib::IsoActiveFlag_t mt_ptoEconomy;
	
    /** report tractor ECU's status of engagement */
    IsoAgLib::IsoReqFlag_t mt_ptoEngagementReqStatus;
	
    /** report tractor ECU's status of mode */
    IsoAgLib::IsoReqFlag_t mt_ptoModeReqStatus;
	
    /** report tractor ECU's status of economy mode */
    IsoAgLib::IsoReqFlag_t mt_ptoEconomyModeReqStatus;
	
    /** present limit status of pto shaft speed */
    IsoAgLib::IsoLimitFlag_t mt_ptoShaftSpeedLimitStatus;
  };



}
#endif
