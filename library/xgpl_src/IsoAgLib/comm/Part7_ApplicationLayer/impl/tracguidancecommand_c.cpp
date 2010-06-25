/*
  tracguidance_c.cpp: stores, updates and delivers all guidance data
    information from CanCustomer_c derived for CAN sending and
    receiving interaction; from BaseCommon_c derived for interaction
    with other IsoAgLib objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include "tracguidancecommand_c.h"
#include "tracguidance_c.h"


using namespace std;

namespace __IsoAgLib { // Begin Namespace __IsoAglib

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
/** C-style function, to get access to the unique TracGuidanceCommand_c singleton instance
  * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
  */
TracGuidanceCommand_c& getTracGuidanceCommandInstance( uint8_t aui8_instance )
{ // if > 1 singleton instance is used, no static reference can be used
  return TracGuidanceCommand_c::instance( aui8_instance );
};
#else
/** C-style function, to get access to the unique TracGuidanceCommand_c singleton instance */
TracGuidanceCommand_c& getTracGuidanceCommandInstance( void )
{
  static TracGuidanceCommand_c& c_lbsTracGuidanceCommand = TracGuidanceCommand_c::instance();
  return c_lbsTracGuidanceCommand;
};
#endif

    /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
      void TracGuidanceCommand_c::singletonInit()
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
  void TracGuidanceCommand_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (apc_isoName, getSingletonVecKey(), at_identMode);
  };

/** config the TracGuidance_c object after init -> set pointer to isoName and
    config send/receive of a moving msg type
    @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    @return true -> configuration was successfull
  */
bool TracGuidanceCommand_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
{
  //call config for handling which is base data independent
  //if something went wrong leave function before something is configured
  if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

  ///Set time Period for Scheduler_c --> periodic action only if in IMPLEMENT MODE!!
  if (at_identMode == IsoAgLib::IdentModeImplement) setTimePeriod( (uint16_t) 100);
  else  setTimePeriod( (uint16_t) 1000   );

  mui8_commanderSa = 0xFF;

  return true;
}

/** check if filter boxes shall be created - create only ISO filters based
    on active local idents which has already claimed an address
    --> avoid to much Filter Boxes
  */
void TracGuidanceCommand_c::checkCreateReceiveFilter( )
{
  IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
  CanIo_c &c_can = getCanInstance4Comm();

  if ( ( !checkFilterCreated() ) && ( c_isoMonitor.existActiveLocalIsoMember() ) )
  { // check if needed receive filters for ISO are active
    setFilterCreated();

    // create FilterBox_c for PGN GUIDANCE_MACHINE_STATUS, PF 254 - mask for DP, PF and PS
    // mask: (0x3FF00 << 8) filter: (GUIDANCE_MACHINE_STATUS << 8)
    c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FF00) << 8),
                      (static_cast<MASK_TYPE>(GUIDANCE_SYSTEM_CMD) << 8), true, Ident_c::ExtendedIdent);
  }
}

/** send a ISO11783 guidance command information PGN.
  * this is only called when sending ident is configured and it has already claimed an address
    @pre  function is only called in implement mode
    @see  BaseCommon_c::timeEvent()
  */
bool TracGuidanceCommand_c::timeEventImplMode( )
{ ///Timeperiod of 100ms is set in ::config
  isoSendMessageImplMode();

  return true;
}

/** HIDDEN constructor for a TracGuidance_c object instance which can optional
    set the configuration for send/receive for a guidance msg
    NEVER instantiate a variable of type TracGuidance_c within application
    only access TracGuidance_c via getTracGuidanceInstance() or getTracGuidanceInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
  */
TracGuidanceCommand_c::TracGuidanceCommand_c() {}


/** process a ISO11783 base information PGN
    @pre  sender of message is existent in monitor list
    @see  CanPkgExt_c::resolveSendingInformation()
  */
bool TracGuidanceCommand_c::processMsg()
{
  uint8_t ui8_sa = data().isoSa();

  switch (data().isoPgn() & 0x3FF00)
  {
    case GUIDANCE_SYSTEM_CMD:
      if ( checkMode(IsoAgLib::IdentModeTractor) && ( getCommander() == ui8_sa ) )
      {
        mmap_commanders[mui8_commanderSa].ui16_curvatureCmd = data().getUint16Data(0);
        mmap_commanders[mui8_commanderSa].t_curvatureCmdStatus = IsoAgLib::IsoSteerFlag_t( data().getUint8Data(2) & 0x3 );

        setUpdateTime( data().time() );
      }
      break;
  }
  return true;
}

/** send guidance command data
    @see  CanIo_c::operator<<
  */
void TracGuidanceCommand_c::isoSendMessageImplMode( )
{
  if ( ( NULL == getISOName() ) || ( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( *getISOName(), true ) ) )
    return;

  CanIo_c& c_can = getCanInstance4Comm();

  // retrieve the actual dynamic sender no of the member with the registered isoName
  data().setIdentType(Ident_c::ExtendedIdent);
  data().setIsoPri(3);
  data().setLen(8);

  data().setISONameForDA( getTracGuidanceInstance4Comm().getSelectedDataSourceISONameConst() );
  data().setISONameForSA( *getISOName() );
  data().setIsoPgn(GUIDANCE_SYSTEM_CMD);
  data().setUint16Data(0, mmap_commanders[mui8_commanderSa].ui16_curvatureCmd);
  uint8_t ui8_temp = 0;
  ui8_temp |= (mmap_commanders[mui8_commanderSa].t_curvatureCmdStatus);
  data().setUint8Data(2, ui8_temp);
  //reserved fields
  data().setUint32Data(3, 0xFFFFFFFFUL);
  data().setUint8Data(7, 0xFF);

  //CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
  // then it sends the data
  c_can << data();
}

/** get currently selected curvature commanders source address
    @param  ui8_sourceAddress  source address of curvature commander
    */
void TracGuidanceCommand_c::setCommander(uint8_t ui8_sourceAddress)
{
  // create new entry with key ui8_sourceAddress if it does not yet exist
  mmap_commanders[ui8_sourceAddress];
  mui8_commanderSa = ui8_sourceAddress;
}

/** get data of a curvature commander by source address
    @param  ui8_sourceAddress  source address
    @return                    curvature commander data or NULL if not exist
  */
CurvatureCommander_s* TracGuidanceCommand_c::getCommanderDataBySA( uint8_t ui8_sourceAddress )
{
  STL_NAMESPACE::map<uint8_t, CurvatureCommander_s>::iterator itr = mmap_commanders.find(ui8_sourceAddress);
  CurvatureCommander_s*  ps_commander = NULL;

  if ( itr != mmap_commanders.end() )
  {
    ps_commander = &itr->second;
  }
  else
  { // there exist no commander data for this source address
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
  }

  return ps_commander;
}

/** get data of a curvature commander by m_index
    @param ui8_index  source address
    @return curvature commander data or NULL if not exist
  */
CurvatureCommander_s* TracGuidanceCommand_c::getCommanderDataByIndex( uint8_t ui8_index)
{
  STL_NAMESPACE::map<uint8_t, CurvatureCommander_s>::iterator itr;
  CurvatureCommander_s*  ps_commander = NULL;
  unsigned int count = 0;

  if ( ui8_index > mmap_commanders.size() )
  {
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
    return ps_commander;
  }

  for(itr = mmap_commanders.begin(); (itr != mmap_commanders.end() && count <= ui8_index); ++itr)
  {
    if (count == ui8_index)
      ps_commander = &itr->second;

    count++;
  }

  return ps_commander;
}

///  Used for Debugging Tasks in Scheduler_c
const char*
TracGuidanceCommand_c::getTaskName() const
{   return "TracGuidanceCommand_c"; }

/** dummy implementation
    @todo SOON-824: add answering of requestPGN in case this object is configured for sending of these information
           - verify this also for the other TracFoo classes
  */
bool TracGuidanceCommand_c::processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/)
{
  return false;
}

}// End Namespace __IsoAglib
