/*
  filterbox_c.cpp: FilterBox_c permits free definition of mask/filter
    for receiving CAN telegrams by a CANCustomer (or derived) object

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "filterbox_c.h"
#include "canio_c.h"
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>
#include <IsoAgLib/util/iassert.h>

#if DEBUG_FILTERBOX || DEBUG_HEAP_USEAGE || DEBUG_CAN_BUFFER_FILLING
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
  #include <IsoAgLib/util/impl/util_funcs.h>
#endif


namespace __IsoAgLib {

FilterBox_c* FilterBox_c::mspc_currentlyProcessedFilterBox = NULL;
int FilterBox_c::msi_processMsgLoopIndex = -1; // not used if "mspc_currentlyProcessedFilterBox==NULL" though.
int FilterBox_c::msi_processMsgLoopSize = -1; // not used if "mspc_currentlyProcessedFilterBox==NULL" though.

FilterBox_c::FilterBox_c()
  : mc_maskFilterPair()
  , mvec_customer()
  , mui8_filterBoxNr(IdleState)
  , mui8_busNumber(IdleState)
  , mi32_fbVecIdx(-1)
{}

/**
  copy constructor which uses data of another FilterBox_c instance

  @param acrc_src reference to the source FilterBox_c instance for copying
   @exception badAlloc
*/
FilterBox_c::FilterBox_c(const FilterBox_c& acrc_src)
  : mc_maskFilterPair(acrc_src.mc_maskFilterPair)
  , mvec_customer(acrc_src.mvec_customer)
  , mui8_filterBoxNr(acrc_src.mui8_filterBoxNr)
  , mui8_busNumber(acrc_src.mui8_busNumber)
  , mi32_fbVecIdx(acrc_src.mi32_fbVecIdx)
{}

/**
  destructor of this FilterBox_c instance
*/
FilterBox_c::~FilterBox_c()
{
  clearData();
}

/**
  copy values of acrc_src FilterBox_c object to this instance

  possible errors:
      * badAlloc on not enough memory for copying buffed CAN msg from source

  @param acrc_src FilterBox_c instance with data to assign to this instance
  @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
*/

FilterBox_c& FilterBox_c::operator=(const FilterBox_c& acrc_src){
  if ( this != &acrc_src)
  {
    mc_maskFilterPair = acrc_src.mc_maskFilterPair;
    mvec_customer = acrc_src.mvec_customer;

    mui8_busNumber = acrc_src.mui8_busNumber;
    mui8_filterBoxNr = acrc_src.mui8_filterBoxNr;
    mi32_fbVecIdx = acrc_src.mi32_fbVecIdx;
  }
  return *this;
}


/** clear the data of this instance */
void FilterBox_c::clearData()
{
  mvec_customer.clear();
  mc_maskFilterPair.setEmpty();

  mui8_busNumber = IdleState;
  mui8_filterBoxNr = IdleState;
  mi32_fbVecIdx = InvalidIdx;
}

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
bool FilterBox_c::configCan(uint8_t aui8_busNumber, uint8_t aui8_FilterBoxNr)
{
  // store mui8_busNumber for later close of can
  mui8_busNumber = aui8_busNumber;
  mui8_filterBoxNr = aui8_FilterBoxNr;

  isoaglib_assert( ! mc_maskFilterPair.empty() );
  Ident_c c_mask = mc_maskFilterPair.getMaskIdent();
  Ident_c c_filter = mc_maskFilterPair.getFilterIdent();
  if( HAL::can_configMsgobjInit(aui8_busNumber, aui8_FilterBoxNr, c_filter, c_mask, 0) != HAL_NO_ERR ) {
    IsoAgLib::getILibErrInstance().registerFatal( IsoAgLib::iLibErr_c::HalCanConfig, aui8_busNumber );
    return false;
  }
  return true;
}
#endif

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
/**   close the BIOS filterbox object of this instance and close hardware CAN filterbox object */
void FilterBox_c::closeHAL()
{
  if ( mui8_busNumber != IdleState && mui8_filterBoxNr != IdleState )
  {
    if (HAL::can_configMsgobjClose(mui8_busNumber, mui8_filterBoxNr ) == HAL_RANGE_ERR)
    { // given BUS or filterBox number is wrong
      IsoAgLib::getILibErrInstance().registerFatal( IsoAgLib::iLibErr_c::HalCanConfig, 0 );
    }
    mui8_busNumber = mui8_filterBoxNr = IdleState;
  }
}
#endif

/* *************************************** */
/* ******* filter/mask managing ********** */
/* *************************************** */

/**
  set the mask (t_mask) and filter (t_filter) of this FilterBox
 @param acrc_mask mask for this Filer_Box (MASK_TYPE defined in isoaglib_config.h)
  @param arc_maskFilterPair mask filter combination
  @param apc_customer pointer to the CanCustomer_c instance, which creates this FilterBox_c instance
  @param ai8_dlcForce force the DLC to be exactly this long (0 to 8 bytes). use -1 for NO FORCING and accepting any length can-pkg
*/
void FilterBox_c::set (const IsoAgLib::iMaskFilterType_c& arc_maskFilterPair,
                       CanCustomer_c* apc_customer,
                       int8_t ai8_dlcForce)
{
  // actually "apc_customer" should've rather been a reference!
  isoaglib_assert (apc_customer);

  mc_maskFilterPair = arc_maskFilterPair;

  STL_NAMESPACE::vector<CustomerLen_s>::iterator pc_iter = mvec_customer.begin();
  for (; pc_iter != mvec_customer.end(); ++pc_iter)
  {
    if (apc_customer == pc_iter->pc_customer)
    { // overwrite the DLC of the one found!
      pc_iter->i8_dlcForce = ai8_dlcForce;
      break;
    }
  }
  if (pc_iter == mvec_customer.end())
  { // push back new
    mvec_customer.push_back (CustomerLen_s (apc_customer, ai8_dlcForce));
    /// Currently "msi_processMsgLoopIndex" is not being adapted,
    /// because if a Message triggers Customer A and B, and Customer A
    /// inserts a filter for that same message for Customer C,
    /// we do NOT want Customer C to be called immediately.
  }
};

bool FilterBox_c::equalCustomer( const __IsoAgLib::CanCustomer_c& ar_customer ) const

{
  STL_NAMESPACE::vector<CustomerLen_s>::const_iterator pc_iter;
  for(pc_iter = mvec_customer.begin(); pc_iter != mvec_customer.end(); ++pc_iter)
    if( &ar_customer == pc_iter->pc_customer)
      return true;

  return false;
}

/** delete CanCustomer_c  instance from array or set FilterBox_c to idle
    if CanCustomer_c  is the only customer for this FilterBox_c instance
    @param  ar_customer  CANCustomer to delete
    @return                true -> no more cancustomers exist, whole filterbox can be deleted
  */
bool
FilterBox_c::deleteFilter( const __IsoAgLib::CanCustomer_c& ar_customer)
{
  int deleteIndex = 0;
  for (STL_NAMESPACE::vector<CustomerLen_s>::iterator pc_iter = mvec_customer.begin();
       pc_iter != mvec_customer.end();)
  {
    if (&ar_customer == pc_iter->pc_customer)
    { // the to-be-deleted customer is found and now pointed by pc_iter
      mvec_customer.erase(pc_iter);
      // in case we're currently in the processing loop for this FilterBox
      if (mspc_currentlyProcessedFilterBox == this)
      { // we need to adapt the LoopSize and maybe LoopIndex
        if (deleteIndex <= msi_processMsgLoopIndex)
          --msi_processMsgLoopIndex;
        --msi_processMsgLoopSize;
      }
      break;
    }
    ++deleteIndex;
    ++pc_iter;
  }

  if ( mvec_customer.empty() )
  { // the last customer has been removed
    #ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    closeHAL();
    //if the connection to the HAL is disconnected clear all data of the now idle filterbox
    #endif

    clearData();

    return true;
  }
  else
  { // there is at least one other can customer -> this FilterBox_c is still in use
    return false;
  }
}

/* ************************************************** */
/* ***** insert/get/process buffered CanPkg_c ******** */
/* ************************************************** */

/**
  control the processing of a received message
  (MsgObj_c::processMsg inserted data directly in CANCustomer
   -> FilterBox_c::processMsg() initiates conversion of CAN string
      to data flags and starts processing in CanCustomer_c )

  possible errors:
      * precondition no valid CanCustomer_c  (or derived) is registered
  @return true -> FilterBox_c was able to inform registered CANCustomer
*/
bool FilterBox_c::processMsg()
{
  bool b_result = false;

  //! We need to FIRST get the number of entries and then DON'T USE iterators,
  //! because the number may increase and the iterators may get invalid in case
  //! a Filter is inserted IN THIS filterbox (pushed back!)
  mspc_currentlyProcessedFilterBox = this;
  msi_processMsgLoopSize = mvec_customer.size();
  for ( msi_processMsgLoopIndex = 0;
        msi_processMsgLoopIndex < msi_processMsgLoopSize;
        ++msi_processMsgLoopIndex )
  {
    const int8_t ci8_vecCustomerDlcForce = mvec_customer[msi_processMsgLoopIndex].i8_dlcForce;
    CanCustomer_c* pc_customer = mvec_customer[msi_processMsgLoopIndex].pc_customer;
    isoaglib_assert( pc_customer );

    // dont construct a new pkg for each iteration - thus it's "yet" static TODO
    CanPkg_c c_pkg;

#if defined SYSTEM_WITH_ENHANCED_CAN_HAL
    HAL::can_useMsgobjGet( mui8_busNumber, 0xFF, &c_pkg );
#else
    const int32_t ci32_fifoRet = HAL::fifo_useMsgObjGet( mui8_busNumber, &c_pkg );
    if ( ci32_fifoRet != HAL_NO_ERR )
    {
#if DEBUG_FILTERBOX
      INTERNAL_DEBUG_DEVICE
      << "Central Fifo - Reading problem on bus : " << int( mui8_busNumber ) << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      IsoAgLib::getILibErrInstance().registerNonFatal( IsoAgLib::iLibErr_c::HalCanBusWarn, mui8_busNumber );
      break; // so that "msi_processMsgLoopIndex = -1" will be done before returning.
    }
#if DEBUG_FILTERBOX
    INTERNAL_DEBUG_DEVICE
    << "FilterBox is consuming the message " << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
#endif // SYSTEM_WITH_ENHANCED_CAN_HAL


    /// Check DataLengthCode (DLC) if required
    if (( ci8_vecCustomerDlcForce < 0 ) || ( ci8_vecCustomerDlcForce == c_pkg.getLen() ) )
    {
      // either no dlc-check requested or dlc matches the check!
      b_result |= pc_customer->processMsg( c_pkg );
    }
    else
    { // dlc-check was requested but failed
#if DEBUG_FILTERBOX
      INTERNAL_DEBUG_DEVICE
      << "DLC_ERROR on identifier : " << c_pkg.ident()
      << " dlc: " << static_cast<int>( c_pkg.getLen() )
      << " expected: " <<  static_cast<int>( ci8_vecCustomerDlcForce )
      << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
    }

  }


  mspc_currentlyProcessedFilterBox = NULL; // indicate that we're not anymore in the loop!
  return b_result;
}

#if DEBUG_CAN_BUFFER_FILLING
/** some debug messages */
void FilterBox_c::doDebug(uint8_t aui8_busNumber)
{
  #if CAN_INSTANCE_CNT == 1
  static uint16_t sui16_maxBufferUseage = 0;
  uint16_t &r_maxCnt = sui16_maxBufferUseage;

  static uint16_t sui16_minBufferFree = 0xFFFF;
  uint16_t &r_minFree = sui16_minBufferFree;
  #elif CAN_INSTANCE_CNT == 2
  static uint16_t sui16_maxBufferUseage[2] = {0, 0};
  uint16_t &r_maxCnt = sui16_maxBufferUseage[aui8_busNumber];

  static uint16_t sui16_minBufferFree[2] = {0xFFFF,0xFFFF};
  uint16_t &r_minFree = sui16_minBufferFree[aui8_busNumber];
  #else
  static uint16_t sui16_maxBufferUseage[CAN_INSTANCE_CNT];
  uint16_t &r_maxCnt = sui16_maxBufferUseage[aui8_busNumber];

  static uint16_t sui16_minBufferFree[CAN_INSTANCE_CNT];
  uint16_t &r_minFree = sui16_minBufferFree[aui8_busNumber];
  if ( ( sui16_maxBufferUseage[0] == 0 ) && ( sui16_maxBufferUseage[1] == 0 ) )
  {
    for ( uint16_t ind = 0; ind < CAN_INSTANCE_CNT; ind++) sui16_minBufferFree[ind] = 0xFFFF;
  }
  #endif
  if ( HAL::can_stateMsgobjBuffercnt(aui8_busNumber, mui8_filterBoxNr) > r_maxCnt )
  { // new MAX detected -> update and print
    r_maxCnt = HAL::can_stateMsgobjBuffercnt(aui8_busNumber, mui8_filterBoxNr);
    INTERNAL_DEBUG_DEVICE << "\r\nNew Max buffer filling: " << r_maxCnt
      << " at Filterbox Nr: " << uint16_t(mui8_filterBoxNr)
      << " with Filter: " << mc_maskFilterPair.getFilter()
      << " at BUS: " << uint16_t(aui8_busNumber)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
  if ( HAL::can_stateMsgobjFreecnt(aui8_busNumber, mui8_filterBoxNr) < r_minFree )
  { // new MIN detected -> update and print
    r_minFree = HAL::can_stateMsgobjFreecnt(aui8_busNumber, mui8_filterBoxNr);
    INTERNAL_DEBUG_DEVICE << "\r\nNew Min buffer free: " << r_minFree
      << " at Filterbox Nr: " << uint16_t(mui8_filterBoxNr)
      << " with Filter: " << mc_maskFilterPair.getFilter()
      << " at BUS: " << uint16_t(aui8_busNumber)
      << INTERNAL_DEBUG_DEVICE_ENDL;
  }
}
#endif

} // End namespace __IsoAgLib
