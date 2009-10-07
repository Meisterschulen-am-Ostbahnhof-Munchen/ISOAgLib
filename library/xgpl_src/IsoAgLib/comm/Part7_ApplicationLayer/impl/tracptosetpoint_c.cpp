/***************************************************************************
            tracptosetpoint_c.cpp - working on pto set point data;
                                    stores, updates  and delivers all pto set point
                                    data informations from CanCustomer_c derived for
                                    CAN sending and receiving interaction;
                                    from Scheduler_Task_c derived for
                                    interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#if defined(USE_BASE) || defined(USE_TRACTOR_PTO)
  #include "tracpto_c.h"
#endif
#include "tracptosetpoint_c.h"


using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAgLib

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique TracPTOSetPoint_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
    */
  TracPTOSetPoint_c& getTracPtoSetPointInstance(uint8_t aui8_instance)
  { // if > 1 singleton instance is used, no static reference can be used
    return TracPTOSetPoint_c::instance(aui8_instance);
  };
  #else
  /** C-style function, to get access to the unique TracPTOSetPoint_c singleton instance */
  TracPTOSetPoint_c& getTracPtoSetPointInstance( void )
  {
    static TracPTOSetPoint_c& c_lbsTracPtoSetPoint = TracPTOSetPoint_c::instance();
    return c_lbsTracPtoSetPoint;
  };
  #endif

    /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
      void TracPTOSetPoint_c::singletonInit()
      { // singletonInit is called, AFTER the initializing instance() function has assigned a suitable
        // singleton vec key - this key value is NOT available at construction time!!!
        BaseCommon_c::singletonInitBase(SINGLETON_VEC_KEY);
      }


  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param apc_isoName optional pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  void TracPTOSetPoint_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (apc_isoName, getSingletonVecKey(), at_identMode);
  };

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
    CanIo_c &c_can = getCanInstance4Comm();

    if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
    { // check if needed receive filters for ISO are active
      setFilterCreated();
      // create FilterBox_c for PGN HITCH_PTO_COMMANDS , PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (HITCH_PTO_COMMANDS  << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFFF) << 8),
                        (static_cast<MASK_TYPE>(HITCH_PTO_COMMANDS ) << 8), true, Ident_c::ExtendedIdent);
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

    CanIo_c& c_can = getCanInstance4Comm();

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

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    c_can << data();

    // we do not need to set setUpdateTime() in basecommon_c because time relevant
    // settings are now done by the scheduler
  }


///  Used for Debugging Tasks in Scheduler_c
const char*
TracPTOSetPoint_c::getTaskName() const
{   return "TracPTOSetPoint_c"; }

/** dummy implementation
    @todo SOON-135: add answering of requestPGN in case this object is configured for sending of these information
           - verify this also for the other TracFoo classes
  */
bool TracPTOSetPoint_c::processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/)
{
  return false;
}

} // End Namespace __IsoAgLib
