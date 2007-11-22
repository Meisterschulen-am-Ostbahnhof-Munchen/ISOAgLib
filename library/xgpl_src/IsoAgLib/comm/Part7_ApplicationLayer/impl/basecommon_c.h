#define SINGLETON_C_DATA_DEF [+8]
/***************************************************************************
                          basecommon_c.h  - base class for basa data types
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate: 2005-01-11 10:55:57 +0100 (Di, 11 Jan 2005) $
    $LastChangedRevision: 805 $
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
#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#include <IsoAgLib/comm/Scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement//impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/ibasetypes.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{

  class BaseCommon_c : public Scheduler_Task_c
  {
  public:

    /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
    void singletonInitBase(SINGLETON_VEC_KEY_PARAMETER_DEF);

    /// General init() so that EVERY subsystem of IsoAgLib has its init() call.
    /// nothing done so far, but can be overwritten in the derived classes if needed
    /// do not mistake this init() with the virtual (!) init_base() function
    /// It's okay that this init() normally does NOTHING, as init_base() is being
    /// called from singleonInit()!
    void init() {}

    /** constructor */
    BaseCommon_c() : mui16_suppressMask(0),
		                 mt_identMode(IsoAgLib::IdentModeImplement),
                     mi32_lastMsgReceived(0),
                     mpc_isoName(),
                     mc_selectedDataSourceISOName()
                   {}
    /** destructor */
    ~BaseCommon_c() {};

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    void close( );

    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances
        possible errors:
          * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param apc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual void init_base (const IsoName_c*, int ai_singletonVecKey, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement);

    /** tractor object after init --> store isoName and mode
        this function was originally named "config", but to avoid warnings with the interface classes'
        "config" function this one is now named "config_base". It's simply not clean to name interface
        functions to virtual functions the same!
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement, uint16_t aui16_suppressMask = 0);

     /** deliver reference to data pkg
         @return reference to the member CanPkg, which encapsulates the CAN send structure
        */
    CanPkgExt_c& data() {return mc_data;};

    /**
      virtual function which delivers a pointer to the CANCustomer
      specific CanPkgExt_c instance
    */
    virtual CanPkgExt_c& dataBase();

    /** functions with actions, which must be performed periodically
        -> called periodically by Scheduler_c
        ==> sends base msg if configured in the needed rates
        possible errors:
          * dependant error in CanIo_c on CAN send problems
        @see CanPkg_c::getData
        @see CanPkgExt_c::getData
        @see CanIo_c::operator<<
        @return true -> all planned activities performed in allowed time
      */
    virtual bool timeEvent(void);

    /** send a PGN request */
    bool sendPgnRequest(uint32_t ui32_requestedPGN);

    /** check if preconditions for request for pgn are fullfilled
        @return  true -> the request for pgn can be send
      */
    virtual bool check4ReqForPgn(uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    virtual bool timeEventTracMode();

    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    virtual bool timeEventImplMode();

    /** Retrieve the last update time of the specified information type*/
    int32_t lastedTimeSinceUpdate() const { return (System_c::getTime() - mi32_lastMsgReceived);}

    /** Retrieve the time of last update */
    int32_t lastUpdateTime() const {return mi32_lastMsgReceived;}

    /** set last time of data msg [msec]*/
    void setUpdateTime(int32_t updateTime) {mi32_lastMsgReceived = updateTime;}

    /** check if a received message should be parsed */
    bool checkParseReceived(const IsoName_c& arc_currentSender) const;

    /** return if you currently are in implement mode or tractor mode*/
    bool checkMode(IsoAgLib::IdentMode_t at_identMode) const {return (mt_identMode == at_identMode);}

    /** check if iso filters have alread been created*/
    bool checkFilterCreated() const {return mb_filterCreated;}

    /** set mb_filterCreated*/
    void setFilterCreated() {mb_filterCreated = true;}

    /** clear mb_filterCreated*/
    void clearFilterCreated() {mb_filterCreated = false;}

    /** return sender of a msg*/
    const IsoName_c* getISOName() const {return mpc_isoName;}

    /** get IsoName of data source (e.g. tractor, terminal) from which commands are send exclusively */
    IsoName_c& getSelectedDataSourceISOName() {return mc_selectedDataSourceISOName;}

    /** get const IsoName of data source (e.g. tractor, terminal) from which commands are send exclusively */
    const IsoName_c& getSelectedDataSourceISONameConst() const {return mc_selectedDataSourceISOName;}

    /** get actual mode */
    IsoAgLib::IdentMode_t getMode() const {return mt_identMode;}

    /** set mode to implement or tractor*/
    void setMode(IsoAgLib::IdentMode_t at_identMode) {mt_identMode = at_identMode;}

    /** set IsoName of data source (e.g. tractor, terminal) which sends commands exclusively */
    void setSelectedDataSourceISOName(const IsoName_c& ac_dataSourceISOName){mc_selectedDataSourceISOName = ac_dataSourceISOName;}

    /** if a message is not send after 3 seconds it is expected that the sending node stopped sending */
    static const uint16_t TIMEOUT_SENDING_NODE = 3000;

    /// Using the singletonVecKey from mc_data (-->CanPkgExt_c)
    SINGLETON_PAR_DOT_DEF(mc_data)

  protected:
    /** flags that disable PGNs individually */
    uint16_t mui16_suppressMask;

  private:

    // private methods
    /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
    virtual void checkCreateReceiveFilter() = 0;

    /** set sender of a msg */
    void setISOName(const IsoName_c* isoName) {mpc_isoName = isoName;}


    // private attributes
    /** can be implement mode or tractor mode */
    IsoAgLib::IdentMode_t mt_identMode;

    /** flag to detect, if receive filters for ISO are created */
    bool mb_filterCreated;

    /** last time of data msg [msec] */
    int32_t mi32_lastMsgReceived;

    /** isoName which act as sender of a msg: either responses on behalf of implement or commands as tractor.
        This pointer is set in config function
      */
    const IsoName_c* mpc_isoName;

    /** IsoName of data source (e.g. tractor, terminal) from which commands are send exclusively */
    IsoName_c mc_selectedDataSourceISOName;

    /** temp data where received data is put */
    CanPkgExt_c mc_data;
  };

}// end namespace __IsoAgLib

#endif
