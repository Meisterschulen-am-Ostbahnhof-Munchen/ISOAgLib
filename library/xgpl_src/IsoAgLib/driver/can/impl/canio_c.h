/*
  canio_c.h: master object for CAN communication; owns array of
    FilterBox_c and MsgObj_c instances

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef CAN_IO_H
#define CAN_IO_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/hal/hal_can.h>
#include <IsoAgLib/hal/hal_system.h>
#include "ident_c.h"
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  #include "msgobj_c.h"
#endif
#include "filterbox_c.h"

#include <list>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/* *************************************** */
/* ********** special defines ************ */
/* *************************************** */
#define MIN_MSG_LIMIT 0
#define MAX_MSG_LIMIT 1

/* *************************************** */
/* ********* class definition ************ */
/* *************************************** */

class CanIo_c;
/**
  Encapsulation of CAN IO with MsgObj_c instances in dynamic array.
  Initialization of CAN hardware, with default setting of one send object.

  An __IsoAgLib::CanCustomer_c  can create a FilterBox_c object with its own Filter/Mask
  combination, and with automatic direct processMsg() call on received CAN
  telegram. Direct access via pointer to __IsoAgLib::CanCustomer_c  instances on FilterBox
  insertion. Uses dynamic array of MsgObj_c, FilterBox_c and for receive
  notification dyn. array of pointer to FilterBox_c.

  @short Encapsulation of CAN IO with managing received data by dynamic arrays
  of FilterBox_c and MsgObj
  @author Dipl.-Inform. Achim Spangler
  @see FilterBox_c MsgObj
*/
class CanIo_c : public Subsystem_c
{
  MACRO_MULTITON_CONTRIBUTION();
 public: // changed from protected to work with the access from MsgObj_c
  /** define dynamic array of MsgObj_c instances for each hardware
    MsgObj_c one object instances in array
  */
  #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
    typedef STL_NAMESPACE::list<MsgObj_c,STL_NAMESPACE::__malloc_alloc_template<0> > ArrMsgObj;
    #else
    typedef STL_NAMESPACE::list<MsgObj_c> ArrMsgObj;
    #endif
    /** define dynamic array of FilterBox_c instances;
      if a __IsoAgLib::CanCustomer_c  creates one FilterBox_c definitions,
      one object instance is inserted in array
    */
  #endif //SYSTEM_WITH_ENHANCED_CAN_HAL



   typedef STL_NAMESPACE::vector<FilterBox_c> ArrFilterBox;


 public:

  /** Initialize the CAN hardware, and instantiate one msg object for
      sending of messages. Do configuration for BUS number, sending bitrate,
      CAN ident length, minx/max hardware/BIOS Msg Obj numbers by parameters;
      called by specified constructor or external functions;
      wrong BUS and msg obj numbers are rejected and cause set of Err_c:range

      possible errors:
          * Err_c::range on undefined BUS,  msgOb_nr or sendBufferSize,
          * Err_c::hwConfig on uninitialized BUS, undef. msgType or CAN-BIOS mem-err,
          * Err_c::busy on already used sending Msg-Obj

      @param aui8_busNumber optional number of the CAN bus
      @param aui16_bitrate  optional bitrate (default by define in isoaglib_config.h)
      @param aui8_minObjNr  optional minimum number for hardware CAN
                            message object (important for sharing CAN controller with
                            other tasks) (default by define in isoaglib_config.h)
      @param aui8_maxObjNr  optional maximum number for hardware CAN
                            message object (default by define in isoaglib_config.h)
      @return               true -> correct initialisation without errors
      @see    HAL::init_can
      @see    HAL::tCanObjConfig
      @see    HAL::configCanObj
      @see    Ident_c::t_identType
    */
  bool init(
    uint8_t aui8_busNumber,
    uint16_t aui16_bitrate,
    uint8_t aui8_minObjNr = 0,
    uint8_t aui8_maxObjNr = (CONFIG_CAN_OBJ_CNT-1) );

  /** check if this CanIo_c instance is configured so that it can be used to send */
  bool isReady2Send() const { return ( mui8_busNumber != 0xFF ); }

  /** every subsystem of IsoAgLib has explicit function for controlled shutdown */
  void close( void );

  ~CanIo_c() {}

  /** provide BUS number */
  uint8_t getBusNumber( void ) const { return mui8_busNumber; }

  #ifdef USE_CAN_MEASURE_BUSLOAD
  /** deliver actual BUS load in baud */
  uint32_t getProcessedThroughput() const;
  #endif

  /** wait until specified timeout or until next CAN message receive
   *  @return true -> there are CAN messages waiting for process. else: return due to timeout
   */
  static bool waitUntilCanReceiveOrTimeout( int32_t timeoutInterval )
  { return HAL::can_waitUntilCanReceiveOrTimeout( timeoutInterval );}

  /** deliver the numbers which can be placed at the moment in the send buffer
    @return number of msgs which fit into send buffer
  */
  uint8_t sendCanFreecnt();

  /** clear the send buffer */
  void sendCanClearbuf();

  /** test if a FilterBox_c definition already exist
    (version expecial for extended ident, chosen at compile time)
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param arc_filterpair filter mask type set
    @param apc_iter optional pointer Iterator to result FilterBox
    @return true -> same FilterBox_c already exist
  */
  bool existFilter(const __IsoAgLib::CanCustomer_c& ar_customer,
      const IsoAgLib::iMaskFilterType_c& arc_filterpair,
      ArrFilterBox::iterator* apc_iter = NULL);

  /** create a Filter Box with specified aui32_mask/aui32_filter
    on ui8_busNr of object; reconfig HW CAN MsgObj_c only if
    ab_reconfigImmediate == true -> useful for
    avoiding unneeded reconfiguration during
    sequence of FilterBox_c insertions;
    by ar_customer CanIo_c (FilterBox_c) can start direct processing
    of received data in dedicated customer object (no search);
    uses BIOS functions

    possible errors:
        * Err_c::badAlloc on not enough memory for new FilterBox
        instance or for new configured MsgObj_c's
    @see __IsoAgLib::CANCustomer
    @param ar_customer reference to __IsoAgLib::CanCustomer_c  which needs
         filtered messages (-> on received msg call ar_customer.processMsg())
    @param arc_filterpair mask filter combination
    @param ab_reconfigImmediate true -> all Filter objects are reconfigured
         to according CAN hardware MsgObj after creating this filter
    @return != NULL -> if inserting and wanted reconfiguration are performed without errors,
      a reference to the created FilterBox is returned
   @exception badAlloc
  */

  FilterBox_c* insertFilter(__IsoAgLib::CanCustomer_c& ar_customer,
                            const IsoAgLib::iMaskFilterType_c& arc_filterpair,
                            int ai_dlcForce,
                            bool ab_reconfigImmediate);

  /** reconfigure the MsgObj after insert/delete of FilterBox */
  bool reconfigureMsgObj();

  /** verify given BUS number and MsgObj number, if they are correct
    (mostly used by MsgObj_c to verify itself)
    @param aui8_busNr number of the BUS
    @param aui8_objNr number of the MsgObj
    @return true -> values are correct
  */
  bool verifyBusMsgobjNr(uint8_t aui8_busNr, uint8_t aui8_objNr);

  /** helper function to search all FilterBoxes for matching
    instance which maps to received CAN messages
    ( needed if the coordinated FilterBoxes don't match,
      especially important to process messages from global
      CAN MsgObj which doesn't have explicit connected FilterBox
      instances )
    @param aui32_ident Ident of received CAN message
    @return is another filterbox found?
  */
  bool canMsg2FilterBox( uint32_t aui32_ident, Ident_c::identType_t at_type, ArrFilterBox::iterator& ar_arrFilterBoxIter, bool ab_start );

  /** delete a FilerBox definition
    @param ar_customer reference to the processing class ( the same filter setting can be registered by different consuming classes )
    @param arc_filterpair filter mask combination
    @return true -> FilterBox_c found and deleted
  */
  bool deleteFilter(const __IsoAgLib::CanCustomer_c& ar_customer,
      const IsoAgLib::iMaskFilterType_c& arc_filterpair );

  bool deleteAllFiltersForCustomer (const __IsoAgLib::CanCustomer_c& ar_customer);


  /**
    initiate processing of all received msg
    check all active MsgObj_c for received CAN msg and
    initiate their processing
  */
  void processMsg( bool& stopCanProcessing );

  /** deliver count of CAN messages which were processed during last timeEvent() / processMsg() call
    * this helps Scheduler_c to decide about needed double retrigger of CanIo_c::processMsg()
    * at high CAN BUS load (important to avoid overflow of CAN buffers in HAL
    */
  uint8_t getProcessedMsgCnt( void ) const { return mui8_processedMsgCnt;}

  /** function for sending data out of CanPkg_c (uses BIOS function)
      possible errors:
          * Err_c::hwConfig on wrong configured CAN obj,
            not init BUS or no configured send obj
          * Err_c::range on undef BUS or BIOS send obj nr
          * Err_c::can_warn on physical CAN-BUS problems
          * Err_c::can_off on physical CAN-BUS off state
      @see CANPkg
      @see HAL::sendCanMsg
      @param acrc_src CanPkg_c which holds the to be sent data
      @return reference to this CanIo_c instance ==>
            needed by commands like "c_can_io << pkg_1 << pkg_2 ... << pkg_n;"
    */
  CanIo_c& operator<<(CanPkg_c& acrc_src);

  /** check for can send conflict error and stop send retry on error
      (thus avoid BUS OFF)
      @return true -> there was send error and send retry stopped
    */
  bool stopSendRetryOnErr();

  /** set the new maximum send delay
      @param ai32_maxSendDelay new maximum send delay in milli-seconds
   */
  void setMaxSendDelay (int32_t ai32_maxSendDelay);

  FilterBox_c& getFilterBoxInstance(int32_t ai32_fbIdx) {return m_arrFilterBox[ai32_fbIdx];};


  #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION && !defined(SYSTEM_WITH_ENHANCED_CAN_HAL)
  void printMsgObjInfo();
  #endif

  /** for precise time checks e.g. within isVtActive() the last timestamp of the last processed can package is accessible.
    @return time stamp of the last can package that has been received and processed successfully
  */
  int32_t getLastProcessedCanPkgTime() const { return mi32_lastProcessedCanPkgTime; }

 protected: // Protected methods
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** evaluate common bits of all defined filterBox
     instances and set it in mask -> build up global mask
   */
  void getCommonFilterMask();

  /** correct global masks after merge based on merged IDs of all
      resulting MsgObj_c instances
    */
  void getCommonFilterMaskAfterMerge();

  /** build unique MsgObj_c's based on given
      global mask (different FilterBox_c def's can result in one
      MsgObj_c if differences are hidden by global mask), return count of obj

      possible errors:
          * Err_c::badAlloc on not enough memory for created MsgObj_c instances
      @see MsgObj
      @see FilterBox
    */
  int16_t FilterBox2MsgObj();

  /** checks if count of preconfigured msgObj
      is allowed; shrink by merging the objects
      with smallest Bit distance
      -> all existing MsgObj_c instances can be
        configured in hardware after execution
      @see MsgObj
    */
  void CheckSetCntMsgObj();

#endif //SYSTEM_WITH_ENHANCED_CAN_HAL

 private: // private methods
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of CanIo_c within application
    * (set mui8_busNumber to 0xFF so that init() detects first call after constructor)
    */
  CanIo_c( void );

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** is the fist add after a reconfiguration */
  bool isFirstAddFilterBox() const
  { return (mi32_minChangedFilterBox == -1); }

  /** fast or full reconfiguration */
  bool isFullReconfigNecessary() const { return ((mi32_minChangedFilterBox < 0 )); }

  /** set full reconfiguration */
  void setFullReconfigNecessary() { mi32_minChangedFilterBox = -2; }

  /** set value */
  void setMinChangedFilterBox (int32_t ai32_value) { mi32_minChangedFilterBox = ai32_value; }

  /** get a filterbox index */
  int32_t getMinChangedFilterBox() const { return (mi32_minChangedFilterBox >= 0 )? mi32_minChangedFilterBox : 0; }

  /** init value */
  void initMinChangedFilterBox() { mi32_minChangedFilterBox = -1; }
#endif

  /** deliver min msg obj nr
      @return min msg obj nr
    */
  uint8_t minHALMsgObjNr()const{return mui8_minmsgObjLimit;}

  /** get offset for received messages
      @return offset for received messages
    */
 uint8_t minReceiveObjNr() const {return mui8_minReceiveObjNr;}

  /** set min msg obj nr
      @param ab_limit wanted min msg obj nr
    */
  void setMinHALMsgObjNr(uint8_t ab_limit){mui8_minmsgObjLimit = ab_limit;}

  /** search for a FilterBox with given mask and filter
      @param  arc_filterpair filter mask combination
      @return            located FilterBox or NULL if not exist
    */
  FilterBox_c* getFilterBox( const IsoAgLib::iMaskFilterType_c& arc_maskFilter ) const;

  void setCntFilter(size_t at_newSize ) { mt_filterBoxCnt = at_newSize;}
  size_t cntFilter() const { return mt_filterBoxCnt;}

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  void setCntMsgObj(size_t at_newSize ) { mt_msgObjCnt = at_newSize;}
  size_t cntMsgObj() const { return mt_msgObjCnt;}
  /** deliver max msg obj nr
      @return max msg obj nr
    */
  uint8_t maxHALMsgObjNr()const{return mui8_maxMsgObjLimit;}

  /** set max msg obj nr
      @param ab_limit wanted max msg obj nr
    */
  void setMaxHALMsgObjNr(uint8_t ab_limit){mui8_maxMsgObjLimit = ab_limit;}

  /** call the needed HAL function for setting the new global masks,
      without invalidating already open send and last msg obj.
    */
  bool registerChangedGlobalMasks(void);
#endif

  /** get offset for received messages
      @return offset for received messages
    */
  uint8_t updateMinReceiveObjNr();


  /** switch CAN bitrate (possible during runtime
      with automatic reconfiguring of CAN MsgObj)
      @param aui16_newSpeed
    */
  bool setBitrate(uint16_t aui16_newSpeed, bool ab_force = false);

  /** perform base init for CAN with set of speed and init of send object(s)
      @param aui16_bitrate wanted CAN bitrate
    */
  bool baseCanInit(uint16_t aui16_bitrate);

#if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
  void doDebug(uint8_t ui8_busNr, uint8_t ui8_sendObjNr);
#endif

  void setEndLastReconfigTime(){ mi32_endLastReconfigTime = HAL::getTime(); }

// Private attributes
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** dynamic array of MsgObj_c which manages each one a
      hardware CAN object
      @see MsgObj
    */
  ArrMsgObj marr_msgObj;

  /** temp MsgObj_c for inserting of new msgObj instances
      @see MsgObj
    */
  MsgObj_c mc_tempObj;

  /** MsgObj_c for last hardware CAN message object where all CAN messages are
      stored which don't match t one of the active CAN MsgObj_c ( typical number 15 )
      @see MsgObj
    */
  MsgObj_c mc_lastMsgObj;
  size_t mt_msgObjCnt;

  /** for the fast reconfiguration */
  int32_t mi32_minChangedFilterBox;
#endif


  /** dynamic array of FilterBox_c instances which
      represents the demanded filter boxes
      @see FilterBox
    */
  ArrFilterBox m_arrFilterBox;
  size_t mt_filterBoxCnt;

/** return a reference of a FilterBox instance */


 #if DEBUG_CAN_FILTERBOX_MSGOBJ_RELATION
   void printMyFilterBox();
 #endif
  /** temp filer box to avoid new/delete for each insert of a filterBox
      @see FilterBox
    */
  FilterBox_c mc_tempFilterBox;

  /** maximum send delay - value of < 0 indicates that no send-delay check is requested*/
  int32_t mi32_maxSendDelay;

  /**  timestamp of last CAN BUS integrity check  */
  int32_t mi32_lastCanCheck;

  /**  timestamp of last received and and processed CAN package  */
  int32_t mi32_lastProcessedCanPkgTime;

  /** CAN mui16_bitrate (in kBit/s) */
  uint16_t mui16_bitrate;


  int32_t mi32_endLastReconfigTime;

  /** global mask with standard 11bit type */
  Ident_c mc_maskStd;

  /** global mask with extended 29bit type */
  Ident_c mc_maskExt;

  /** global last msg mask */
  Ident_c mc_maskLastmsg;

  /** BUS Number for systems with more than one BUS
      (for each BUS one  CanIo_c instance)
    */
  uint8_t mui8_busNumber;

  /** min limit of allowed msgObj numbers for this CanIo_c instance
      (important for multithreading environments where different
      Processes must share one BUS)
    */
  uint8_t mui8_minmsgObjLimit;

  /** minimal obj nr that can be used for a receive msg obj which is dependent of identType_t */
  uint8_t mui8_minReceiveObjNr;

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  /** max limit of allowed msgObj numbers for this CanIo_c instance
      (important for multithreading environments where different
      Processes must share one BUS)
    */
  uint8_t mui8_maxMsgObjLimit;
#endif

  /** count of CAN messages which were processed during last timeEvent() / processMsg() call
    * this helps Scheduler_c to decide about needed double retrigger of CanIo_c::processMsg()
    * at high CAN BUS load (important to avoid overflow of CAN buffers in HAL
    */
  uint8_t mui8_processedMsgCnt;

  /** flag to avoid loop of CAN message processing, when timeEvent() is called during previous
   *  timeEvent triggered CAN processing -> when this flag is true, no further processing is performed
   */
  bool mb_runningCanProcess;

  friend CanIo_c &getCanInstance( uint8_t aui8_instance );
};


  /** C-style function, to get access to the unique CanIo_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  CanIo_c &getCanInstance( uint8_t aui8_instance = 0 );

}

#endif
