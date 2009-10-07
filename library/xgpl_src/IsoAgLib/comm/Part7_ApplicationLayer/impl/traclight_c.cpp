/***************************************************************************
                          traclight_c - working on lighting data;
                                        stores, updates  and
                                        delivers all base data informations
                                        from CanCustomer_c derived for CAN
                                        sending and receiving interaction;
                                        from BaseCommon_c derived for
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

#include "traclight_c.h"
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>

using namespace std;

namespace __IsoAgLib {

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
    /** C-style function, to get access to the unique TracLight_c singleton instance
      * if more than one CAN BUS is used for IsoAgLib, an m_index must be given to select the wanted BUS
      */
    TracLight_c& getTracLightInstance( uint8_t aui8_instance )
    { // if > 1 singleton instance is used, no static reference can be used
      return TracLight_c::instance( aui8_instance );
    }
  #else
    /** C-style function, to get access to the unique TracLight_c singleton instance */
    TracLight_c& getTracLightInstance( void )
    {
      static TracLight_c& c_lbsTracLight = TracLight_c::instance();
      return c_lbsTracLight;
    }
  #endif


    /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
      void TracLight_c::singletonInit()
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
  void TracLight_c::init_base (const IsoName_c* apc_isoName, int /*ai_singletonVecKey*/, IsoAgLib::IdentMode_t at_identMode)
  {
    //call init for handling which is base data independent
    BaseCommon_c::init_base (apc_isoName, getSingletonVecKey(), at_identMode);
  };

  /** config the TracLight_c object after init -> set pointer to isoName, set implementMode,
      store pointer to isoName separately if we send as tractor
      @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  bool TracLight_c::config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask)
  {
    //store old mode to decide to register or unregister to request for pgn
    IsoAgLib::IdentMode_t t_oldMode = getMode();
    //call config for handling which is base data independent
    //if something went wrong leave function before something is configured
    if ( !BaseCommon_c::config_base (apc_isoName, at_identMode, aui16_suppressMask) ) return false;

    if ( checkMode(IsoAgLib::IdentModeTractor))
    {
      // set Time Period in ms for Scheduler_c
      setTimePeriod( (uint16_t) 1000   );
    }

    if ( at_identMode == IsoAgLib::IdentModeImplement )
    { // a change from Tractor mode to Implement mode occured
      // create FilterBox_c for REQUEST_PGN_MSG_PGN, LIGHTING_DATA_PGN
      getIsoRequestPgnInstance4Comm().registerPGN (*this, LIGHTING_DATA_PGN); // request for lighting information
    }

    if (t_oldMode == IsoAgLib::IdentModeImplement && at_identMode == IsoAgLib::IdentModeTractor)
    {  // a change from Implement mode to Tractor mode occured
      // unregister from request for pgn, because in implement mode no request should be answered
      getIsoRequestPgnInstance4Comm().unregisterPGN (*this, LIGHTING_DATA_PGN);
    }

    // set configure values
    mb_cmdWait4Response = false;
    mb_changeNeedBeSend = false;

    for (int i = 0; i < 10; i++)
      marr_timeStamp[i] = 0;

    m_index = 0;

    return true;
  };

  /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  void TracLight_c::checkCreateReceiveFilter()
  {
    IsoMonitor_c& c_isoMonitor = getIsoMonitorInstance4Comm();
    CanIo_c &c_can = getCanInstance4Comm();

    if ((!checkFilterCreated()) && (c_isoMonitor.existActiveLocalIsoMember()))
    { // check if needed receive filters for ISO are active
      setFilterCreated();

      // create FilterBox_c for PGN LIGHTING_DATA_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (LIGHTING_DATA_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFFF) << 8),
                        (static_cast<MASK_TYPE>(LIGHTING_DATA_PGN) << 8), false, Ident_c::ExtendedIdent);
      // create FilterBox_c for PGN LIGHTING_COMMAND_PGN, PF 254 - mask for DP, PF and PS
      // mask: (0x3FFFF << 8) filter: (LIGHTING_COMMAND_PGN << 8)
      c_can.insertFilter(*this, (static_cast<MASK_TYPE>(0x3FFFF) << 8),
                        (static_cast<MASK_TYPE>(LIGHTING_COMMAND_PGN) << 8), true, Ident_c::ExtendedIdent);

    }
  }

  bool TracLight_c::processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver)
  {
    // check if we are allowed to send a request for pgn
    if ( ! BaseCommon_c::check4ReqForPgn(aui32_pgn, apc_isoItemSender, apc_isoItemReceiver) ) return false;

    // call TracLight_c function to send lighting information
    // sendMessage checks if this item (identified by ISOName)
    // is configured to send lighting information
    sendMessage();
    return true;
  }

  /** find out if a particular implement has responded to a command from the tractor
      @param sa   source address of the sought implement
      @return     has responded to a command (true), else (false)
    */
  bool TracLight_c::find(const uint8_t sa) const
   {
      STL_NAMESPACE::map<uint8_t, lightBitData_t>::const_iterator ci = mmap_data.find(sa);
      if (ci != mmap_data.end())
        return true;
      else
        return false;
  }

  /** get the status of a light of a special implement
      possible errors:
        * iLibErr_c::Range m_index exceeds map size
      @param acui_index index of implement
      @param at_command which light
      @return status of the light of the implement
    */
  IsoAgLib::IsoActiveFlag_t TracLight_c::getByIndex(const unsigned int acui_index, const IsoAgLib::IsoCommandFlag_t at_command)
  {
    STL_NAMESPACE::map<uint8_t, lightBitData_t>::iterator itr;
    const lightBitData_t* pt_data = NULL;
    unsigned int count = 0;

    for(itr = mmap_data.begin(); (itr != mmap_data.end() && count <= acui_index); ++itr)
    {
      if (count == acui_index)
        pt_data = &itr->second;
      count++;
    }
    if (pt_data != NULL)
      return getInfo(*pt_data, at_command);
    else {
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return IsoAgLib::IsoError;
    }
  }

  /** get the status of a light of a special implement
      possible errors:
        * iLibErr_c::Range the implement with sa has not responded to command from the tractor
      @param sa         source address of implement
      @param t_command  which light
      @return           status of the light of the implement
    */
  IsoAgLib::IsoActiveFlag_t TracLight_c::getBySa(const uint8_t sa, const IsoAgLib::IsoCommandFlag_t t_command) const
  {
    const lightBitData_t* pt_data = NULL;
    STL_NAMESPACE::map<uint8_t, lightBitData_t>::const_iterator itr = mmap_data.find(sa);

    if (itr != mmap_data.end())
      pt_data = &itr->second;
    if (pt_data != NULL)
      return getInfo(*pt_data, t_command);
    else {
      getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Base );
      return IsoAgLib::IsoError;
    }
  }

  /** set a lighting command in tractor mode or set a response on a lighting command in implement mode
      @param t_command command to set
      @param t_val command value
    */
  void TracLight_c::setCommand(const IsoAgLib::IsoCommandFlag_t t_command, const IsoAgLib::IsoActiveFlag_t t_val)
  {
    switch (t_command) {
      case IsoAgLib::daytimeRunning:
        if ( (mt_cmd.daytimeRunning == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.daytimeRunning == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.daytimeRunning = t_val;
        break;
      case IsoAgLib::alternateHead:
        if (  (mt_cmd.alternateHead == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.alternateHead == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.alternateHead = t_val;
        break;
      case IsoAgLib::lowBeamHead:
        if (  (mt_cmd.lowBeamHead == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.lowBeamHead == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.lowBeamHead = t_val;
        break;
      case IsoAgLib::highBeamHead:
        if (  (mt_cmd.highBeamHead == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.highBeamHead == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.highBeamHead = t_val;
        break;
      case IsoAgLib::frontFog:
        if (  (mt_cmd.frontFog == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.frontFog == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.frontFog = t_val;
        break;
      case IsoAgLib::beacon:
        if (  (mt_cmd.beacon == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.beacon == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.beacon = t_val;
        break;
      case IsoAgLib::rightTurn:
        if (  (mt_cmd.rightTurn == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.rightTurn == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.rightTurn = t_val;
        break;
      case IsoAgLib::leftTurn:
        if (  (mt_cmd.leftTurn == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.leftTurn == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.leftTurn = t_val;
        break;
      case IsoAgLib::backUpLightAlarmHorn:
        if (  (mt_cmd.backUpLightAlarmHorn == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.backUpLightAlarmHorn == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.backUpLightAlarmHorn = t_val;
        break;
      case IsoAgLib::centerStop:
        if (  (mt_cmd.centerStop == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.centerStop == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.centerStop = t_val;
        break;
      case IsoAgLib::rightStop:
        if (  (mt_cmd.rightStop == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.rightStop == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.rightStop = t_val;
        break;
      case IsoAgLib::leftStop:
        if (  (mt_cmd.leftStop == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.leftStop == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.leftStop = t_val;
        break;
      case IsoAgLib::implClearance:
        if (  (mt_cmd.implClearance == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implClearance == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implClearance = t_val;
        break;
      case IsoAgLib::tracClearance:
        if (  (mt_cmd.tracClearance == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.tracClearance == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.tracClearance = t_val;
        break;
      case IsoAgLib::implMarker:
        if (  (mt_cmd.implMarker == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implMarker == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implMarker = t_val;
        break;
      case IsoAgLib::tracMarker:
        if (  (mt_cmd.tracMarker == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.tracMarker == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.tracMarker = t_val;
        break;
      case IsoAgLib::rearFog:
        if (  (mt_cmd.rearFog == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.rearFog == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.rearFog = t_val;
        break;
      case IsoAgLib::undersideWork:
        if (  (mt_cmd.undersideWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.undersideWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.undersideWork = t_val;
        break;
      case IsoAgLib::rearLowWork:
        if (  (mt_cmd.rearLowWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.rearLowWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.rearLowWork = t_val;
        break;
      case IsoAgLib::rearHighWork:
        if (  (mt_cmd.rearHighWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.rearHighWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.rearHighWork = t_val;
        break;
      case IsoAgLib::sideLowWork:
        if (  (mt_cmd.sideLowWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.sideLowWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.sideLowWork = t_val;
        break;
      case IsoAgLib::sideHighWork:
        if (  (mt_cmd.sideHighWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.sideHighWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.sideHighWork = t_val;
        break;
      case IsoAgLib::frontLowWork:
        if (  (mt_cmd.frontLowWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.frontLowWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.frontLowWork = t_val;
        break;
      case IsoAgLib::frontHighWork:
        if (  (mt_cmd.frontHighWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.frontHighWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.frontHighWork = t_val;
        break;
      case IsoAgLib::implOEMOpt2:
        if (  (mt_cmd.implOEMOpt2 == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implOEMOpt2 == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implOEMOpt2 = t_val;
        break;
      case IsoAgLib::implOEMOpt1:
        if (  (mt_cmd.implOEMOpt1 == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implOEMOpt1 == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implOEMOpt1 = t_val;
        break;
      case IsoAgLib::implRightForwardWork:
        if (  (mt_cmd.implRightForwardWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implRightForwardWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implRightForwardWork = t_val;
        break;
      case IsoAgLib::implLeftForwardWork:
        if (  (mt_cmd.implLeftForwardWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implLeftForwardWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implLeftForwardWork = t_val;
        break;
      case IsoAgLib::dataMsgReq:
        // this is the data message request flag which indicates if the lighting data message from all
        // implements should be send and NOT a flag for a light
        mt_cmd.dataMsgReq = t_val;
        break;
      case IsoAgLib::implRightFacingWork:
        if (  (mt_cmd.implRightFacingWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implRightFacingWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implRightFacingWork = t_val;
        break;
      case IsoAgLib::implLeftFacingWork:
        if (  (mt_cmd.implLeftFacingWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implLeftFacingWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implLeftFacingWork = t_val;
        break;
      case IsoAgLib::implRearWork:
        if (  (mt_cmd.implRearWork == IsoAgLib::IsoActive && t_val == IsoAgLib::IsoInactive)
            ||(mt_cmd.implRearWork == IsoAgLib::IsoInactive && t_val == IsoAgLib::IsoActive)
           )
          mb_changeNeedBeSend = true;
        mt_cmd.implRearWork = t_val;
        break;
    }
    if ( mb_changeNeedBeSend )
      sendMessage();
  }

  /** get command which the tractor send
      @param t_command  command from the tractor
      @return           value of the command which the tractor send
    */
  IsoAgLib::IsoActiveFlag_t TracLight_c::getCommand(const IsoAgLib::IsoCommandFlag_t t_command) const
  {
    return getInfo(mt_cmd, t_command);;
  }

  /** helper funktion to get the mode of a light of a specified implement
      @param at_data bit field where the lighting information is stored
      @param at_command light to find
      @return status of the light
    */
  IsoAgLib::IsoActiveFlag_t TracLight_c::getInfo(const lightBitData_t& at_data, const IsoAgLib::IsoCommandFlag_t at_command) const
  {
    switch (at_command)
    {
      case IsoAgLib::daytimeRunning:        return IsoAgLib::IsoActiveFlag_t(at_data.daytimeRunning);
      case IsoAgLib::alternateHead:         return IsoAgLib::IsoActiveFlag_t(at_data.alternateHead);
      case IsoAgLib::lowBeamHead:           return IsoAgLib::IsoActiveFlag_t(at_data.lowBeamHead);
      case IsoAgLib::highBeamHead:          return IsoAgLib::IsoActiveFlag_t(at_data.highBeamHead);
      case IsoAgLib::frontFog:              return IsoAgLib::IsoActiveFlag_t(at_data.frontFog);
      case IsoAgLib::beacon:                return IsoAgLib::IsoActiveFlag_t(at_data.beacon);
      case IsoAgLib::rightTurn:             return IsoAgLib::IsoActiveFlag_t(at_data.rightTurn);
      case IsoAgLib::leftTurn:              return IsoAgLib::IsoActiveFlag_t(at_data.leftTurn);
      case IsoAgLib::backUpLightAlarmHorn:  return IsoAgLib::IsoActiveFlag_t(at_data.backUpLightAlarmHorn);
      case IsoAgLib::centerStop:            return IsoAgLib::IsoActiveFlag_t(at_data.centerStop);
      case IsoAgLib::rightStop:             return IsoAgLib::IsoActiveFlag_t(at_data.rightStop);
      case IsoAgLib::leftStop:              return IsoAgLib::IsoActiveFlag_t(at_data.leftStop);
      case IsoAgLib::implClearance:         return IsoAgLib::IsoActiveFlag_t(at_data.implClearance);
      case IsoAgLib::tracClearance:         return IsoAgLib::IsoActiveFlag_t(at_data.tracClearance);
      case IsoAgLib::implMarker:            return IsoAgLib::IsoActiveFlag_t(at_data.implMarker);
      case IsoAgLib::tracMarker:            return IsoAgLib::IsoActiveFlag_t(at_data.tracMarker);
      case IsoAgLib::rearFog:               return IsoAgLib::IsoActiveFlag_t(at_data.rearFog);
      case IsoAgLib::undersideWork:         return IsoAgLib::IsoActiveFlag_t(at_data.undersideWork);
      case IsoAgLib::rearLowWork:           return IsoAgLib::IsoActiveFlag_t(at_data.rearLowWork);
      case IsoAgLib::rearHighWork:          return IsoAgLib::IsoActiveFlag_t(at_data.rearHighWork);
      case IsoAgLib::sideLowWork:           return IsoAgLib::IsoActiveFlag_t(at_data.sideLowWork);
      case IsoAgLib::sideHighWork:          return IsoAgLib::IsoActiveFlag_t(at_data.sideHighWork);
      case IsoAgLib::frontLowWork:          return IsoAgLib::IsoActiveFlag_t(at_data.frontLowWork);
      case IsoAgLib::frontHighWork:         return IsoAgLib::IsoActiveFlag_t(at_data.frontHighWork);
      case IsoAgLib::implOEMOpt2:           return IsoAgLib::IsoActiveFlag_t(at_data.implOEMOpt2);
      case IsoAgLib::implOEMOpt1:           return IsoAgLib::IsoActiveFlag_t(at_data.implOEMOpt1);
      case IsoAgLib::implRightForwardWork:  return IsoAgLib::IsoActiveFlag_t(at_data.implRightForwardWork);
      case IsoAgLib::implLeftForwardWork:   return IsoAgLib::IsoActiveFlag_t(at_data.implLeftForwardWork);
      case IsoAgLib::dataMsgReq:            return IsoAgLib::IsoActiveFlag_t(at_data.dataMsgReq);
      case IsoAgLib::implRightFacingWork:   return IsoAgLib::IsoActiveFlag_t(at_data.implRightFacingWork);
      case IsoAgLib::implLeftFacingWork:    return IsoAgLib::IsoActiveFlag_t(at_data.implLeftFacingWork);
      case IsoAgLib::implRearWork:          return IsoAgLib::IsoActiveFlag_t(at_data.implRearWork);
      //not necessarily essential, but it prevents the compiler from a warning
      default:                              return IsoAgLib::IsoNotAvailable;
    }
  }

  /** HIDDEN constructor for a TracLight_c object instance which can optional
      set the configuration for send/receive for a lighting msg type
      NEVER instantiate a variable of type TracLight_c within application
      only access TracLight_c via getTracLightInstance() or getTracLightInstance( int riLbsBusNr ) in case more than one ISO11783 BUS is used for IsoAgLib
    */
  TracLight_c::TracLight_c() {}


  /** process received msg and store updated value for later reading access;
      called by FilterBox_c::processMsg after receiving a msg
      possible errors:
        * iLibErr_c::BaseSenderConflict msg received from different member than before

      @pre  sender of message is existent in monitor list
      @see  CanPkgExt_c::resolveSendingInformation()
      @see FilterBox_c::processMsg
      @see CanIo_c::processMsg
      @return true -> message was processed; else the received CAN message will be served to other matching CanCustomer_c
    */
  bool TracLight_c::processMsg()
  { // an ISO11783 base information msg received
    // there is no need to check if sender exist in the monitor list because this is already done
    // in CanPkgExt_c -> resolveSendingInformation
    IsoName_c const& rcc_tempISOName = data().getISONameForSA();

    lightBitData_t* pt_data = NULL;

    switch (data().isoPgn() /*& 0x3FFFF*/) // don't need to &, we're interested in the whole PGN!
    {
      case LIGHTING_DATA_PGN:
        // lighting state information is sent by more than one sender -> store ALL messages with SA as key in STL_NAMESPACE::map
        pt_data = &(mmap_data[data().isoSa()]);
        if (pt_data != NULL)
          pt_data->dataMsgReq = IsoAgLib::IsoDontCare; //reserved field in lighting data
        break;
      case LIGHTING_COMMAND_PGN:
        // CMD is EXCLUSIVELY SENT BY ONE TRACTOR ECU!!! --> CHECK
        if ( checkParseReceived (rcc_tempISOName) )
        { // sender is allowed to send
          pt_data = &mt_cmd;
          mt_cmd.dataMsgReq = IsoAgLib::IsoDataReq_t( data().getUint8Data(7) & 3 );

          if (mt_cmd.dataMsgReq == IsoAgLib::IsoDataRequested)
            mb_cmdWait4Response = true;

          // set last time - use the array of send time stamps which is needed in tractor mode
          // in implement mode, the first item can be used to trace received tractor commands
          marr_timeStamp[0] = data().time();

          setSelectedDataSourceISOName (rcc_tempISOName);
          setUpdateTime( data().time() );
        } else
        { // there is a sender conflict
          getILibErrInstance().registerError( iLibErr_c::BaseSenderConflict, iLibErr_c::Base );
        }
        break;
    }

    if ( pt_data == NULL )
    { // preconditions for parsing of this message are NOT fullfilled --> exit function with false
      return false;
    }
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // from here on, we can safely process the message as all preconditions are fullfilled
    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    #if SIZEOF_INT == 2
      uint16_t ui16_temp = data().getUint16Data( 0 );
      pt_data->daytimeRunning =       IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 ) ;
      pt_data->alternateHead  =       IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 ) ;
      pt_data->lowBeamHead  =         IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 ) ;
      pt_data->highBeamHead  =        IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 ) ;
      pt_data->frontFog =             IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  8) & 3 ) ;
      pt_data->beacon  =              IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 ) ;
      pt_data->rightTurn  =           IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 ) ;
      pt_data->leftTurn  =            IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 ) ;
      ui16_temp = data().getUint16Data( 2 );
      pt_data->backUpLightAlarmHorn = IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 ) ;
      pt_data->centerStop  =          IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 ) ;
      pt_data->rightStop  =           IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 ) ;
      pt_data->leftStop  =            IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 ) ;
      pt_data->implClearance =        IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  8) & 3 ) ;
      pt_data->tracClearance  =       IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 ) ;
      pt_data->implMarker  =          IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 ) ;
      pt_data->tracMarker  =          IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 ) ;
      ui16_temp = data().getUint16Data( 4 );
      pt_data->rearFog =              IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 ) ;
      pt_data->undersideWork  =       IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 ) ;
      pt_data->rearLowWork  =         IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 ) ;
      pt_data->rearHighWork  =        IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 ) ;
      pt_data->sideLowWork =          IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  8) & 3 ) ;
      pt_data->sideHighWork  =        IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 ) ;
      pt_data->frontLowWork  =        IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 ) ;
      pt_data->frontHighWork  =       IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 ) ;
      ui16_temp = data().getUint16Data( 6 );
      pt_data->implOEMOpt2 =           IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  0) & 3 ) ;
      pt_data->implOEMOpt1  =          IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  2) & 3 ) ;
      pt_data->implRightForwardWork  =    IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  4) & 3 ) ;
      pt_data->implLeftForwardWork  =     IsoAgLib::IsoActiveFlag_t( (ui16_temp >>  6) & 3 ) ;
      // pt_data->dataMsgReq is treated separately
      pt_data->implRightFacingWork  =     IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 10) & 3 ) ;
      pt_data->implLeftFacingWork  =       IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 12) & 3 ) ;
      pt_data->implRearWork  =        IsoAgLib::IsoActiveFlag_t( (ui16_temp >> 14) & 3 ) ;
    #else
      // 32-Bit CPU
      uint32_t ui32_temp = data().getUint32Data( 0 );
      pt_data->daytimeRunning =       IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  0) & 3 ) ;
      pt_data->alternateHead =        IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  2) & 3 ) ;
      pt_data->lowBeamHead =          IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  4) & 3 ) ;
      pt_data->highBeamHead =         IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  6) & 3 ) ;
      pt_data->frontFog =             IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  8) & 3 ) ;
      pt_data->beacon =               IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 10) & 3 ) ;
      pt_data->rightTurn =            IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 12) & 3 ) ;
      pt_data->leftTurn =             IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 14) & 3 ) ;
      pt_data->backUpLightAlarmHorn = IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 16) & 3 ) ;
      pt_data->centerStop =           IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 18) & 3 ) ;
      pt_data->rightStop =            IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 20) & 3 ) ;
      pt_data->leftStop =             IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 22) & 3 ) ;
      pt_data->implClearance =        IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 24) & 3 ) ;
      pt_data->tracClearance =        IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 26) & 3 ) ;
      pt_data->implMarker =           IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 28) & 3 ) ;
      pt_data->tracMarker =           IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 30) & 3 ) ;
      ui32_temp = data().getUint32Data( 4 );
      pt_data->rearFog =              IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  0) & 3 ) ;
      pt_data->undersideWork =        IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  2) & 3 ) ;
      pt_data->rearLowWork =          IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  4) & 3 ) ;
      pt_data->rearHighWork =         IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  6) & 3 ) ;
      pt_data->sideLowWork =          IsoAgLib::IsoActiveFlag_t( (ui32_temp >>  8) & 3 ) ;
      pt_data->sideHighWork =         IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 10) & 3 ) ;
      pt_data->frontLowWork =         IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 12) & 3 ) ;
      pt_data->frontHighWork =        IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 14) & 3 ) ;
      pt_data->implOEMOpt2 =          IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 16) & 3 ) ;
      pt_data->implOEMOpt1 =          IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 18) & 3 ) ;
      pt_data->implRightForwardWork = IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 20) & 3 ) ;
      pt_data->implLeftForwardWork =  IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 22) & 3 ) ;
      //pt_data->dataMsgReq is treated separately
      pt_data->implRightFacingWork =  IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 26) & 3 ) ;
      pt_data->implLeftFacingWork =   IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 28) & 3 ) ;
      pt_data->implRearWork =         IsoAgLib::IsoActiveFlag_t( (ui32_temp >> 30) & 3 ) ;
    #endif

    if ( mb_cmdWait4Response )
      sendMessage();

    return true;
  }

  /** functions with actions, which must be performed periodically
      -> called periodically by Scheduler_c
      ==> sends msg if configured in the needed rates
      possible errors:
        * dependant error in CanIo_c on CAN send problems
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
      @return true -> all planned activities performed in allowed time
    */
  bool TracLight_c::timeEventTracMode( )
  {
    // directly call sendMessage() as this function performs the needed pre checks
    sendMessage();

    return true;
  }

  /** functions with actions, which must be performed periodically
      -> called periodically by Scheduler_c
      ==> sends msg if configured in the needed rates
      possible errors:
        * dependant error in CanIo_c on CAN send problems
      @pre  function is only called in implement mode
      @see  BaseCommon_c::timeEvent()
      @return true -> all planned activities performed in allowed time
    */
  bool TracLight_c::timeEventImplMode( )
  {
    // directly call sendMessage() as this function performs the needed pre checks
    sendMessage();

    return true;
  }

  /** send light update; there is a difference between implement and tractor mode
      @see  TracLight_c::processMsgRequestPGN
      @see  CanIo_c::operator<<
    */
  void TracLight_c::sendMessage()
  { // there is no need to check for address claim in tractor mode because this is already done in the timeEvent
    // function of base class BaseCommon_c

    CanIo_c& c_can = getCanInstance4Comm();
    const int32_t ci32_now = getLastRetriggerTime();

    if ( checkMode(IsoAgLib::IdentModeImplement) )
    { // precondition checks for sending as implement
      if (!mb_cmdWait4Response) return;

      // getISOName() might report NULL at least during init time
      if ( ( NULL == getISOName() ) || ( ! getIsoMonitorInstance4Comm().existIsoMemberISOName( *getISOName(), true ) ) )
        return;

      data().setIsoPgn(LIGHTING_DATA_PGN);
      //reset flag because msg will now be send
      mb_cmdWait4Response = false;
    }
    else
    { // tractor mode
      if ( ( ci32_now - marr_timeStamp[m_index] ) <= 1000 )
      { // WE ARE NOT ALLOWED TO SEND - EVEN IF REQUESTED
        return;
      }
      else if ( (!mb_changeNeedBeSend) && ( (ci32_now - marr_timeStamp[(m_index+9)%10]) < 900 ) ) // (m_index+9)%10 -> youngest entry in array marr_timeStamp[];
      { // to send requested (i.e. no change occured) or not yet time to repeat last command
        return;
      }
      mb_changeNeedBeSend = false;
      // now it's evident, that we have to send a command
      data().setIsoPgn(LIGHTING_COMMAND_PGN);
      setSelectedDataSourceISOName(*getISOName());
    }

    //set different priority for tractor and implemnt
    if (checkMode(IsoAgLib::IdentModeTractor))
      data().setIsoPri(3);
    else //implement mode
       data().setIsoPri(6);

    data().setIdentType(Ident_c::ExtendedIdent);
    data().setISONameForSA( *getISOName() );
    data().setLen(8);

    uint16_t ui16_temp = 0;
    ui16_temp = (mt_cmd.daytimeRunning <<  0) +
                (mt_cmd.alternateHead  <<  2) +
                (mt_cmd.lowBeamHead    <<  4) +
                (mt_cmd.highBeamHead   <<  6) +
                (mt_cmd.frontFog       <<  8) +
                (mt_cmd.beacon         << 10) +
                (mt_cmd.rightTurn      << 12) +
                (mt_cmd.leftTurn       << 14);
    data().setUint16Data(0, ui16_temp);
    ui16_temp = 0;
    ui16_temp = (mt_cmd.backUpLightAlarmHorn <<  0) +
                (mt_cmd.centerStop           <<  2) +
                (mt_cmd.rightStop            <<  4) +
                (mt_cmd.leftStop             <<  6) +
                (mt_cmd.implClearance        <<  8) +
                (mt_cmd.tracClearance        << 10) +
                (mt_cmd.implMarker           << 12) +
                (mt_cmd.tracMarker           << 14);
    data().setUint16Data(2, ui16_temp);
    ui16_temp = 0;
    ui16_temp = (mt_cmd.rearFog       <<  0) +
                (mt_cmd.undersideWork <<  2) +
                (mt_cmd.rearLowWork   <<  4) +
                (mt_cmd.rearHighWork  <<  6) +
                (mt_cmd.sideLowWork   <<  8) +
                (mt_cmd.sideHighWork  << 10) +
                (mt_cmd.frontLowWork  << 12) +
                (mt_cmd.frontHighWork << 14);
    data().setUint16Data(4, ui16_temp);

    if (checkMode(IsoAgLib::IdentModeTractor)) //tractor
    {
      ui16_temp = 0;
      ui16_temp = (mt_cmd.implOEMOpt2          <<  0) +
                  (mt_cmd.implOEMOpt1          <<  2) +
                  (mt_cmd.implRightForwardWork <<  4) +
                  (mt_cmd.implLeftForwardWork  <<  6) +
                  (mt_cmd.dataMsgReq           <<  8) +
                  (mt_cmd.implRightFacingWork  << 10) +
                  (mt_cmd.implLeftFacingWork   << 12) +
                  (mt_cmd.implRearWork         << 14);
      data().setUint16Data(6, ui16_temp);
      //overwrite the eldest time event with latest time event
      marr_timeStamp[m_index] = ci32_now;

      //set m_index to the eldest time event
      m_index = (m_index + 1) % 10;
    } else //implement
    {
      ui16_temp = 0;
      ui16_temp = (mt_cmd.implOEMOpt2          <<  0) +
                  (mt_cmd.implOEMOpt1          <<  2) +
                  (mt_cmd.implRightForwardWork <<  4) +
                  (mt_cmd.implLeftForwardWork  <<  6) +
                  (0                          <<  8) +    //reserved field in lighting data
                  (mt_cmd.implRightFacingWork  << 10) +
                  (mt_cmd.implLeftFacingWork   << 12) +
                  (mt_cmd.implRearWork         << 14);
      data().setUint16Data(6, ui16_temp);
    }

    // CanIo_c::operator<< retreives the information with the help of CanPkg_c::getData
    // then it sends the data
    c_can << data();
  }


///  Used for Debugging Tasks in Scheduler_c
const char*
TracLight_c::getTaskName() const
{   return "TracLight_c"; }

} // end of namespace __IsoAgLib
