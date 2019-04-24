/*
  traccommonrx_c.h: base class for receiving typical tractor information

  (C) Copyright 2016 - 2019 by OSB AG

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
#ifndef TRAC_COMMON_RX_H
#define TRAC_COMMON_RX_H

#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part7_ApplicationLayer/ibasetypes.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/scheduler/impl/scheduler_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <vector>

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif

namespace __IsoAgLib
{
  class CanPkgExt_c;


  class TractorCommonRx_c : public CanCustomer_c
  {
    MACRO_MULTITON_CONTRIBUTION();
  
  public:
    TractorCommonRx_c( uint16_t timeout, uint32_t pgn ) :
      mt_task( *this, 100, true ),
      mvec_msgEventHandlers(),
      mi32_lastMsgReceived( -1 ),
      mc_sender(),
      mui16_timeOut( timeout ),
      mui32_pgn( pgn )
    {}

    ~TractorCommonRx_c() {}

    void init();
    void close();

    /** register an event handler that gets called for any incoming PGN.
        Please look into the implementation to see for which PGNs it is
        actually called.
        Note: Double registration will be allowed, whereas deregistration
              will remove all occurances. */
    void registerMsgEventHandler (IsoAgLib::iMsgEventHandler_c &arc_msgEventHandler)
    { mvec_msgEventHandlers.push_back (&arc_msgEventHandler); }

    /** deregister all event handlers matching the parameter
        @param arc_msgEventHandler Reference to an implementation of the
                                   handler class of type MsgEventHandler_c */
    void deregisterMsgEventHandler (IsoAgLib::iMsgEventHandler_c &arc_msgEventHandler);

    /** Retrieve the last update time of the specified information type
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid time-age if message is received and still in the time-out range, i.e. "valid" */
    int32_t lastedTimeSinceUpdate() const { return( mi32_lastMsgReceived == -1 ) ? -1 : ( System_c::getTime() - mi32_lastMsgReceived ); }

    /** Retrieve the time of last update
        @return -1 is no msg received (or only outdated messages, i.e. already timed out.)
                >= 0 valid timestamp if message is received and still in the time-out range, i.e. "valid" */
    ecutime_t lastUpdateTime() const { return mi32_lastMsgReceived; }

    inline void updateReceived( ecutime_t updateTime, const IsoName_c& sender );

    /** check if a received message should be parsed */
    bool checkParseReceived(const IsoName_c& acrc_currentSender) const;

    /** get IsoName of data source */
    const IsoName_c& getSender() const { return mc_sender; }

    static const uint16_t TIMEOUT_SENDING_NODE_NMEA = 3000;
    static const uint16_t TIMEOUT_SENDING_NODE_J1939 = 5000;
    /** if this is not a cyclic message, use this timeout to disable the timeout: */
    static const uint16_t TIMEOUT_SENDING_NODE_NONE = 0;

    virtual void resetValues() = 0;
    virtual void setValues( const CanPkgExt_c& ) = 0;

  private:
    virtual void processMsg( const CanPkg_c& );

    void notifyOnEvent();

    bool initialized() const {
      return mt_task.initialized();
    }

    void setInitialized() {
      mt_task.setInitialized();
    }

    void setClosed() {
      mt_task.setClosed();
    }

  private:
    virtual void timeEvent();

    // not copyable.
    TractorCommonRx_c(TractorCommonRx_c const &);
    TractorCommonRx_c &operator=(TractorCommonRx_c const &);

    CLASS_SCHEDULER_TASK_PROXY(TractorCommonRx_c)
    typedef SchedulerTaskProxy_c Task_t;

  protected:
    Task_t mt_task;

    STL_NAMESPACE::vector<IsoAgLib::iMsgEventHandler_c*> mvec_msgEventHandlers;

  private:
    ecutime_t mi32_lastMsgReceived;
    IsoName_c mc_sender;
    const uint16_t mui16_timeOut;
    const uint32_t mui32_pgn;
  };


  inline void
  TractorCommonRx_c::updateReceived( ecutime_t updateTime, const IsoName_c& sender )
  {
    mi32_lastMsgReceived = updateTime;
    mc_sender = sender;
    
    notifyOnEvent();
  }


} // __IsoAgLib

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif
