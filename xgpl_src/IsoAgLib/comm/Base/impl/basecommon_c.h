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

#include <IsoAgLib/util/impl/elementbase_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/comm/Base/ibasetypes.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{

  class BaseCommon_c : public ElementBase_c
  {
  public:

    /** initialize directly after the singleton instance is created.
        this is called from singleton.h and should NOT be called from the user again.
        users please use init(...) instead.
      */
    void singletonInit();

    /// General init() so that EVERY subsystem of IsoAgLib has its init() call.
    /// nothing done so far, but can be overwritten in the derived classes if needed
    /// do not mistake this init() with the virtual (!) init_base() function
    void init() {}

    /** constructor */
    BaseCommon_c() : t_identMode(IsoAgLib::IdentModeImplement),
                     i32_lastMsgReceived(0),
                     pc_isoName(),
                     c_selectedDataSourceISOName()
                   {}
    /** destructor */
    ~BaseCommon_c() {};

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    void close( );

    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances
        possible errors:
          * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param rpc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual void init_base (const ISOName_c*, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement);

    /** tractor object after init --> store isoName and mode
        this function was originally named "config", but to avoid warnings with the interface classes'
        "config" function this one is now named "config_base". It's simply not clean to name interface
        functions to virtual functions the same!
        @param rpc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement);

     /** deliver reference to data pkg
         @return reference to the member CanPkg, which encapsulates the CAN send structure
        */
    CANPkgExt_c& data() {return c_data;};

    /**
      virtual function which delivers a pointer to the CANCustomer
      specific CANPkgExt_c instance
    */
    virtual CANPkgExt_c& dataBase();

    /** functions with actions, which must be performed periodically
        -> called periodically by Scheduler_c
        ==> sends base msg if configured in the needed rates
        possible errors:
          * dependant error in CANIO_c on CAN send problems
        @see CANPkg_c::getData
        @see CANPkgExt_c::getData
        @see CANIO_c::operator<<
        @return true -> all planned activities performed in allowed time
      */
    virtual bool timeEvent(void);

    /** send a PGN request */
    bool sendPgnRequest(uint32_t ui32_requestedPGN);

    /** check if preconditions for request for pgn are fullfilled
        @return  true -> the request for pgn can be send
      */
    virtual bool check4ReqForPgn(uint32_t rui32_pgn, ISOItem_c* rpc_isoItemSender, ISOItem_c* rpc_isoItemReceiver);

    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    virtual bool timeEventTracMode();

    /** send a ISO11783 base information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
      */
    virtual bool timeEventImplMode();

    /** Retrieve the last update time of the specified information type*/
    int32_t lastedTimeSinceUpdate() const { return (System_c::getTime() - i32_lastMsgReceived);}

    /** Retrieve the time of last update */
    int32_t lastUpdateTime() const {return i32_lastMsgReceived;}

    /** set last time of data msg [msec]*/
    void setUpdateTime(int32_t updateTime) {i32_lastMsgReceived = updateTime;}

    /** check if a received message should be parsed */
    bool checkParseReceived(const ISOName_c& rrefc_currentSender) const;

    /** return if you currently are in implement mode or tractor mode*/
    bool checkMode(IsoAgLib::IdentMode_t rt_identMode) const {return (t_identMode == rt_identMode);}

    /** check if iso filters have alread been created*/
    bool checkFilterCreated() const {return b_filterCreated;}

    /** set b_filterCreated*/
    void setFilterCreated() {b_filterCreated = true;}

    /** clear b_filterCreated*/
    void clearFilterCreated() {b_filterCreated = false;}

    /** return sender of a msg*/
    const ISOName_c* getISOName() const {return pc_isoName;}

    /** get evkey of data source (e.g. tractor, terminal) from which commands are send exclusively */
    ISOName_c& getSelectedDataSourceISOName() {return c_selectedDataSourceISOName;}

    /** get Devkey of data source (e.g. tractor, terminal) from which commands are send exclusively */
    const ISOName_c& getSelectedDataSourceISONameConst() const {return c_selectedDataSourceISOName;}

    /** get actual mode */
    IsoAgLib::IdentMode_t getMode() const {return t_identMode;}

    /** set mode to implement or tractor*/
    void setMode(IsoAgLib::IdentMode_t rt_identMode) {t_identMode = rt_identMode;}

    /** set Devkey of data source (e.g. tractor, terminal) which sends commands exclusively */
    void setSelectedDataSourceISOName(const ISOName_c& rc_dataSourceISOName){c_selectedDataSourceISOName = rc_dataSourceISOName;}

    /** if a message is not send after 3 seconds it is expected that the sending node stopped sending */
    static const uint16_t TIMEOUT_SENDING_NODE = 3000;

  private:

    // private methods
    /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
    virtual void checkCreateReceiveFilter() = 0;

    /** set sender of a msg*/
    void setISOName(const ISOName_c* isoName) {pc_isoName = isoName;}


    // private attributes
    /** can be implement mode or tractor mode*/
    IsoAgLib::IdentMode_t t_identMode;

    /** flag to detect, if receive filters for ISO are created */
    bool b_filterCreated;

    /** last time of data msg [msec] */
    int32_t i32_lastMsgReceived;

    /** isoName which act as sender of a msg: either responses on behalf of implement or commands as tractor.
        This pointer is set in config function
      */
    const ISOName_c* pc_isoName;

    /** Devkey of data source (e.g. tractor, terminal) from which commands are send exclusively */
    ISOName_c c_selectedDataSourceISOName;

    /** temp data where received data is put */
    CANPkgExt_c c_data;
  };

}// end namespace __IsoAgLib

#endif
