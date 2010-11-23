/*
  tracptosetpoint_c.cpp: working on pto set point data; stores,
    updates and delivers all pto set point data informations from
    CanCustomer_c derived for CAN sending and receiving interaction;
    from Scheduler_Task_c derived for interaction with other IsoAgLib
    objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/comm/impl/isobus_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#if defined(USE_BASE) || defined(USE_TRACTOR_PTO)
  #include "tracpto_c.h"
#endif
#include "tracptosetpoint_c.h"


namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  /** C-style function, to get access to the unique TracPTOSetPoint_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracPTOSetPoint_c &getTracPtoSetPointInstance(uint8_t aui8_instance)
  {
    MACRO_MULTITON_GET_INSTANCE_BODY(TracPTOSetPoint_c, PRT_INSTANCE_CNT, aui8_instance);
  }

  /** config the TracPTOSetPoint_c object after init -> set pointer to isoName and
      config send/receive of different base msg types
      @param apc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracPTOSetPoint_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

    ///Set time Period for Scheduler_c --> periodic action only if in IMPLEMENT MODE!!
    if (at_identMode == IsoAgLib::IdentModeImplement) setTimePeriod( (uint16_t) 100);
    else  setTimePeriod( (uint16_t) 1000   );

    //set default values
    mui8_frontHitchPosCmd     = 0;
    mui8_rearHitchPosCmd      = 0;
    mui16_frontPtoSetPointCmd = 0;
    mui16_rearPtoSetPointCmd  = 0;
    mt_frontPtoEngagement =  IsoAgLib::IsoNotAvailable;
    mt_rearPtoEngagement =   IsoAgLib::IsoNotAvailable;
    mt_frontPtoMode =        IsoAgLib::IsoNotAvailable;
    mt_rearPtoMode =         IsoAgLib::IsoNotAvailable;
    mt_frontPtoEconomyMode = IsoAgLib::IsoNotAvailable;
    mt_rearPtoEconomyMode =  IsoAgLib::IsoNotAvailable;

    return true;
  };

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracPTOSetPoint_c::checkCreateReceiveFilter( )
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    IsoBus_c &c_can = getIsoBusInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();

      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFFF) << 8),
                        (static_cast<MASK_TYPE>(HITCH_PTO_COMMANDS ) << 8), true);
    }
  }

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
    */
  bool TracPTOSetPoint_c::processMsg()
  {
    switch (data().isoPgn() /*& 0x3FFFF*/) // don't need to &, as we're interested in the whole PGN
    {
      case HITCH_PTO_COMMANDS:
        if ( checkMode(IsoAgLib::IdentModeTractor) )
        {
          mui8_frontHitchPosCmd =     data().getUint8Data(0);
          mui8_rearHitchPosCmd =      data().getUint8Data(1);
          mui16_frontPtoSetPointCmd = data().getUint16Data(2);
          mui16_rearPtoSetPointCmd =  data().getUint16Data(4);
          mt_frontPtoEngagement =  IsoAgLib::IsoActiveFlag_t( (data().getUint8Data(6) >> 6) & 3);
          mt_rearPtoEngagement =   IsoAgLib::IsoActiveFlag_t( (data().getUint8Data(6) >> 4) & 3);
          mt_frontPtoMode =        IsoAgLib::IsoActiveFlag_t( (data().getUint8Data(7) >> 6) & 3);
          mt_rearPtoMode =         IsoAgLib::IsoActiveFlag_t( (data().getUint8Data(7) >> 4) & 3);
          mt_frontPtoEconomyMode = IsoAgLib::IsoActiveFlag_t( (data().getUint8Data(7) >> 2) & 3);
          mt_rearPtoEconomyMode =  IsoAgLib::IsoActiveFlag_t(  data().getUint8Data(7)       & 3);

          // we do not need to set setUpdateTime() in basecommon_c because time relevant
          // verifications are now done by the scheduler
        }
        break;
    }
    return true;
  }

  /** send a ISO11783 pto information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in implement mode
      @see  BaseCommon_c::timeEvent()
    */
  bool TracPTOSetPoint_c::timeEventImplMode( )
  {
    /// check if we are in implement mode and have a pointer to the sending isoname
    /// stored Pto information sending ISO member has claimed address
    sendPtoMsg();

    return true;
  }

  /** send hitch and pto command
      @see  CanIo_c::operator<<
    */
  void TracPTOSetPoint_c::sendPtoMsg()
  { // as BaseCommon_c timeEvent() checks only for adr claimed state in TractorMode, we have to perform those checks here,
    // as we reach this function mostly for ImplementMode, where getISOName() might report NULL at least during init time
    if ( ( NULL == getISOName() ) || ( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( *getISOName(), true ) ) )
      return;

    #if defined(USE_BASE) || defined(USE_TRACTOR_PTO)
    TracPTO_c& c_tracpto = getTracPtoInstance4Comm();
    //do not send message if no tractor is available to process the command
    if ( ! c_tracpto.getSelectedDataSourceISONameConst().isSpecified() ) return;
    #endif

    IsoBus_c& c_can = getIsoBusInstance4Comm();

    data().setISONameForSA( *getISOName() );
    data().setIdentType(Ident_c::ExtendedIdent);
    data().setIsoPri(3);
    data().setLen(8);
    data().setIsoPgn(HITCH_PTO_COMMANDS);

    data().setUint8Data(0, mui8_frontHitchPosCmd);
    data().setUint8Data(1, mui8_rearHitchPosCmd);
    data().setUint16Data(2, mui16_frontPtoSetPointCmd);
    data().setUint16Data(4, mui16_rearPtoSetPointCmd);
    uint8_t ui8_val6 = 0;
    ui8_val6 |= (mt_frontPtoEngagement  << 6);
    ui8_val6 |= (mt_rearPtoEngagement   << 4);
    data().setUint8Data(6, ui8_val6);
    uint8_t ui8_val7 = 0;
    ui8_val7 |= (mt_frontPtoMode        << 6);
    ui8_val7 |= (mt_rearPtoMode         << 4);
    ui8_val7 |= (mt_frontPtoEconomyMode << 2);
    ui8_val7 |= (mt_frontPtoEconomyMode     );
    data().setUint8Data(7, ui8_val7);

    c_can << data();

    // we do not need to set setUpdateTime() in basecommon_c because time relevant
    // settings are now done by the scheduler
  }


#if DEBUG_SCHEDULER
const char*
TracPTOSetPoint_c::getTaskName() const
{ return "TracPTOSetPoint_c"; }
#endif

/** dummy implementation
    @todo SOON-824: add answering of requestPGN in case this object is configured for sending of these information
           - verify this also for the other TracFoo classes
  */
bool TracPTOSetPoint_c::processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/)
{
  return false;
}

} // End Namespace __IsoAgLib
