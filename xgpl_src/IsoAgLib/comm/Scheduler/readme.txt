/** @page SchedulerPage Central Scheduler of IsoAgLib
  * All periodic activities of the IsoAgLib are controlled by the class IsoAgLib::iScheduler_c.
  * This strategy is independent from any timers and timer interrupts of the target system,
  * so that the application designer can use all interrupts of the CPU for the application
  * specific stuff.
  *
  * @section SchedulerTimingRequirements Timing of Execution for Scheduled Activities
  * @subsection SchedulerWarnvoidParallelAPICalls Avoid Parallel Calls of IsoAgLib API Functions
  * <b>Important:</b><br>
  * Please don't call the function IsoAgLib::iScheduler_c::timeEvent() from interrupts, which could
  * be triggered during the procession of API functions which are called from the normal
  * application context. The API functions of the IsoAgLib can only guarantee that the
  * internal state of the library is consistent after the function returns. If an interrupt
  * function ( i.e. higher level function context ), calls another API function of the IsoAgLib
  * inbetween, this function can't work correct. Moreover it will probably lead to an undefined
  * state of the complete library.
  * <b>
  * If you ultimatively want to call API functions of the library from different execution levels
  * ( parallel tasks which could interrupt each other ), please implement some semaphor function
  * on your own.
  * </b>
  * @subsection SchedulerStopTimeEvent Valid Stop of Execution of Scheduled Events
  * If yout just want to stop the execution of the periodic scheduled activities, to
  * perform some very important work, you can call
  * IsoAgLib::iScheduler_c::forceExecStop(). The IsoAgLib will then return from the timeEvent() triggered
  * functions to the earliest possible time - while leaving everything in a valid state.
  * This is provided by a central timestamp, which tells all timeEvent functions within the IsoAgLib
  * when the execution must be returned back to the caller. The call of forceExecStop() simply resets
  * this timestamp, so that the nearest point for valid stop of execution will be used.
  *
  * @subsection SchedulerDefineExecutionEnd Define Time of Return from Scheduled Event Execution
  * But this call of IsoAgLib::iScheduler_c::forceExecStop() shouldn't be the standard way to control the end
  * of execution in the IsoAgLib. The preferred way for strictly scheduled return from execution in the
  * IsoAgLib::iScheduler_c::timeEvent() action cycle is the use of the optional argument ri32_demandedExecEnd
  * in IsoAgLib::iScheduler_c::timeEvent( int32_t ).
  * The given timestamp will be stored in the above mentioned timestamp variable, which will be checked
  * by each scheduled function on each execution point, where the IsoAgLib will stay in a valid state.
  * These decision points are distributed with a fine granularity, as the time check is implemented with a
  * quick access on a global variable.
  * If no execution end is specified, all periodic actions are performed, and all received CAN messages
  * are processed.
  *
  * @section SchedulerActivityOverview Overview on Scheduled Activities
  * The IsoAgLib is designed to deliver several internal background services, which can be controlled
  * and stimulated by API function calls. This allows the application developer to concentrate
  * on the specific topics, while all standard activities are realized as automatic as possible.
  *
  * @subsection SchedulerCanProcess Processing of Received CAN Messages
  * All received CAN messages, which should be stored by the target specific HAL implementation in
  * ( circular ) puffers, are interpreted by the __IsoAgLib::CanIo_c::timeEvent() and the internal called
  * __IsoAgLib::CanIo_c::processMsg() function.
  * After delegating each message to the corresponding communication class, the individual reaction is performed.
  * - Update of monitor lists for ISO 11783 ( IsoAgLib::iIsoMonitor_c ) on receive of adress claims, alive messages, etc.
  * - Answer request for claimed adress for local ISO 11783 identities
  * - Update data structures in IsoAgLib::iTimePosGps_c, IsoAgLib::iTracGeneral_c, IsoAgLib::iTracMove_c and IsoAgLib::iTracPto_c
  *   where several important base data are stored
  * - Update current value for remote process data instances, so that the application can retrieve
  *   them at independent time ( @ref EnhancedUseWithHandlerCall "optionally: value change handler of application can be called " )
  * - Answer process data value requests for local process data ( therefore the measurement value must be
  *   update by application by call of the setMasterVal() member function of the process data instances )
  * - React on command for multi message protocol ( send first data after CTS, update send state on EndofMsgACK
  *   or ConnAbort )
  * - Call the registered handlers for ISO 11783 terminal events
  *
  * @subsection SystemMgmt_cMgmt Periodic Activities for Local Identities and the ISO Monitor List
  * All dead node entires are erased from the monitor lists, if they didn't send their alive message for more than three seconds.
  * The ISO monitor IsoAgLib::iIsoMonitor_c can optionally erase all nodes from the monitor list, which
  * didn't answer the last "request for claimed adress" call within the standard time interval.
  *
  * @subsection Base_c Periodic Send of Base Data
  * If IsoAgLib::iTimePosGps_c, IsoAgLib::iTracGeneral_c, IsoAgLib::iTracMove_c and IsoAgLib::iTracPto_c were
  * \ref BaseDataConfig "configured to send some base data types" , the timeEvent() call is used provide the periodic data send.
  * But as long as the default configuration isn't actively changed after the start of the IsoAgLib, no periodic
  * action is scheduled for IsoAgLib::iTimePosGps_c, IsoAgLib::iTracGeneral_c, IsoAgLib::iTracMove_c and IsoAgLib::iTracPto_c.
  *
  * @subsection SchedulerProcess Periodic Send of Process Data
  * If a remote system starts a measurement program, which requires the send of current data
  * on some individual events ( time or distance proprotional, etc. ), this is provided
  * by the scheduling function of the IsoAgLib. Thereby the application developer doesn't have
  * to cope with the possibly parallel measurement program requests from several other ECUs
  * for different process data types. All the application has to do for local process data
  * instances is to update the measurement value, and react on received setpoints.
  *
  * @subsection SchedulerMultiMessage Continue Data Stream Upload
  * A running data stream upload is feeded by the scheduled timeEvent() calls.
  * On each activation the current send state is evaluated and the possibly needed
  * send of next CAN messages is triggered.
  *
  * @subsection SchedulerIsoTerminal Spool Attribute Changes
  * The IsoAgLib tries to send the attribute change CAN messages immediately after the corresponding
  * change function is called. But as the procotol standard demands the wait for ACK of the last change
  * command, before the next can be sent, a circular puffer is used to spool the updates in FIFO order.
  */