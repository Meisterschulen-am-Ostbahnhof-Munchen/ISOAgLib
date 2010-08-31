/*
  basecommon_c.h: base class for basa data types

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef BASE_COMMON_H
#define BASE_COMMON_H

#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/ibasetypes.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib
{

  class BaseCommon_c : public CanCustomer_c
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
    BaseCommon_c() :
      mui16_suppressMask(0),
      mt_identMode(IsoAgLib::IdentModeImplement),
      mb_filterCreated(false),
      mi32_lastMsgReceived(0),
      mpc_isoName(NULL),
      mc_selectedDataSourceISOName(),
      mc_data(),
      mt_task(*this),
      mt_handler(*this)
    {}

    /** destructor */
    ~BaseCommon_c() {}

    /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
    virtual void close( );

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
    CanPkgExt_c& data() {return mc_data;}

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
    virtual bool timeEvent();

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
    bool checkParseReceived(const IsoName_c& acrc_currentSender) const;

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
    RegisterPgn_s getRegisterPgn() {
      return RegisterPgn_s(&mt_handler SINGLETON_VEC_KEY_WITH_COMMA);
    }

    UnregisterPgn_s getUnregisterPgn(){
      return UnregisterPgn_s(&mt_handler SINGLETON_VEC_KEY_WITH_COMMA);
    }

    void setTimePeriod(uint16_t aui16_timePeriod) {
      return mt_task.setTimePeriod(aui16_timePeriod);
    }

    bool checkAlreadyClosed() const {
      return mt_task.checkAlreadyClosed();
    }

    bool changeTimePeriodAndResortTask(uint16_t aui16_newTimePeriod ) {
      return getSchedulerInstance().changeTimePeriodAndResortTask(
          &mt_task,
          aui16_newTimePeriod);
    }

    bool changeRetriggerTimeAndResort(
        int32_t ai32_newRetriggerTime,
        int16_t ai16_newTimePeriod = -1) {
      return getSchedulerInstance().changeRetriggerTimeAndResort(
          &mt_task,
          ai32_newRetriggerTime,
          ai16_newTimePeriod);
    }

    int16_t getAvailableExecTime() {
      return mt_task.getAvailableExecTime();
    }

    int32_t getLastRetriggerTime() const {
      return mt_task.getLastRetriggerTime();
    }

    void setAlreadyClosed() {
      mt_task.setAlreadyClosed();
    }

    void clearAlreadyClosed() {
      mt_task.clearAlreadyClosed();
    }

    /** flags that disable PGNs individually */
    uint16_t mui16_suppressMask;

  private:
    // BaseCommon_c shall not be copyable.
    BaseCommon_c(BaseCommon_c const &arc_from);
    BaseCommon_c &operator=(BaseCommon_c const &arc_from);

    class SchedulerTaskProxy_c : public Scheduler_Task_c {
    public:
      typedef BaseCommon_c Owner_t;

      SchedulerTaskProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

      virtual ~SchedulerTaskProxy_c() {}

      using Scheduler_Task_c::setTimePeriod;

    private:
      virtual bool timeEvent() {
        return mrt_owner.timeEvent();
      }

      virtual void close() {
        mrt_owner.close();
      }

      virtual int32_t getTimeToNextTrigger(retriggerType_en e_retriggerType = StandardRetrigger) const {
        return mrt_owner.getTimeToNextTrigger(e_retriggerType);
      }

      virtual const char *getTaskName() const {
        return mrt_owner.getTaskName();
      }

      virtual void updateEarlierAndLatestInterval() {
        mrt_owner.updateEarlierAndLatestInterval();
      }

      virtual uint16_t getForcedMinExecTime() const {
        return mrt_owner.getForcedMinExecTime();
      }

      // SchedulerTaskProxy_c shall not be copyable. Otherwise the
      // reference to the containing object would become invalid.
      SchedulerTaskProxy_c(SchedulerTaskProxy_c const &);

      SchedulerTaskProxy_c &operator=(SchedulerTaskProxy_c const &);

      Owner_t &mrt_owner;
    };
    typedef SchedulerTaskProxy_c Task_t;
    class IsoRequestPgnHandlerProxy_c : public IsoRequestPgnHandler_c {
    public:
      typedef BaseCommon_c Owner_t;

      IsoRequestPgnHandlerProxy_c(Owner_t &art_owner) : mrt_owner(art_owner) {}

      virtual ~IsoRequestPgnHandlerProxy_c() {}

    private:
      virtual bool processMsgRequestPGN(
          uint32_t aui32_pgn,
          IsoItem_c *apc_isoItemSender,
          IsoItem_c *apc_isoItemReceiver)
      {
        return mrt_owner.processMsgRequestPGN(
            aui32_pgn,
            apc_isoItemSender,
            apc_isoItemReceiver);
      }

      // IsoRequestPgnHandlerProxy_c shall not be copyable. Otherwise
      // the reference to the containing object would become invalid.
      IsoRequestPgnHandlerProxy_c(IsoRequestPgnHandlerProxy_c const &);

      IsoRequestPgnHandlerProxy_c &operator=(IsoRequestPgnHandlerProxy_c const &);

      Owner_t &mrt_owner;
    };
    typedef IsoRequestPgnHandlerProxy_c Handler_t;

    // private methods
    /** check if filter boxes shall be created - create only ISO filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
    virtual void checkCreateReceiveFilter() = 0;

    /** set sender of a msg */
    void setISOName(const IsoName_c* isoName) {mpc_isoName = isoName;}

    virtual void updateEarlierAndLatestInterval() {
      mt_task.updateEarlierAndLatestIntervalDefault();
    }

    virtual uint16_t getForcedMinExecTime() const {
      return mt_task.getForcedMinExecTimeDefault();
    }

    virtual int32_t getTimeToNextTrigger(retriggerType_en e_retriggerType = StandardRetrigger) const {
      return mt_task.getTimeToNextTriggerDefault(e_retriggerType);
    }

    virtual char const *getTaskName() const = 0;

    virtual bool processMsgRequestPGN(
        uint32_t aui32_pgn,
        IsoItem_c *apc_isoItemSender,
        IsoItem_c *apc_isoItemReceiver) = 0;

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

    Task_t mt_task;
    Handler_t mt_handler;
  };

}// end namespace __IsoAgLib

#endif
